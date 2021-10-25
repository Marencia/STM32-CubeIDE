/**
 ************************************************************************************************************************************************************
 * Proyecto: "Implementacion de un filtro FIR de audio por DSP".
 * Autor del proyecto: Ezequiel Porto.
 * Basado en el proyecto "Implementacion de un filtro de audio" de Diego Sebastian Comas.
 *
 * Archivo: "MiAudio.c"
 * Fecha: 30/09/2019
 * Comentario: Funciones para manejar audio.
 ******************************************************************************
 */

// ----------------------------------------------------------------------------
#include "MiAudio.h" // Archivo de Audio
// ----------------------------------------------------------------------------

// ---------DEFINICION DE VARIABLES GLOBALES:
extern Tipo_Estado_Buffer Estado_BUFFER;
extern Tipo_Estado_Filtro modo_FILTRO;
extern uint32_t nMuestras; // La cantidad de muestras del archivo de audio!
extern uint32_t nRestantes; // La cantidad de datos que aun no fueros reproducidos.
extern uint16_t Audio_File[];
extern uint16_t Audio_Buffer[4*BLOCK_SIZE];

// ----Variables usadas por el CODEC DE AUDIO y sus rutinas de interrupcion:
extern I2S_HandleTypeDef       hAudioOutI2s;
extern I2S_HandleTypeDef       hAudioInI2s;
__IO uint32_t TimeRecBase = 0;  /* Time Recording base variable */


arm_fir_instance_f32 fStruct;	// puntos a una instancia de la estructura del filtro FIR.
float32_t auxBuffer[BLOCK_SIZE]; // Buffer auxiliar que contiene las muestras a filtrar casteadas a float.
								 // Tamaño de auxBuffer=Audio_Buffer/2 Bytes (definido en main.c)
float32_t outBuffer[BLOCK_SIZE]; // Buffer que contiene las muestras filtradas (float).
// ----------------------------------------------------------------------------
// ------ Funciones de Audio ------------
// ----------------------------------------------------------------------------
// Funcion: "Procesar_Buffer"
// Si modo_FILTRO==OFF, llama a funcion de copia audio original a RAM
// Else si modo_FILTRO==ON llama a funcion de filtro y copia del audio filtrado a RAM.
/* --Parametros de entrada:
      pAudio: Puntero al comienzo de un bloque de datos de ROM.
  	  pBuffer: Puntero al comienzo de un bloque del BUFFER (Audio_Buffer) de RAM.
  	  nDatos: Nº de muestras de 16 bits a procesar.   */
void Procesar_Buffer(uint16_t* pAudio, uint16_t* pBuffer, uint32_t nDatos) {
	switch(modo_FILTRO)
	{
	case OFF: // Transfiere muestras de ROM a RAM sin filtrar
		Copiar_Buffer(pAudio, pBuffer, nDatos);
		break;
	case ON: // Transfiere muestras de ROM a RAM previamente filtradas
		Filtrar_Buffer(pAudio, pBuffer, nDatos);
	}
}
// -------------------------------------------------------------------
// Funcion: "Copiar_Buffer"
// Transfiere un bloque de muestras de ROM al buffer de RAM sin filtrar
/* --Parametros de entrada:
      pAudio: Puntero al comienzo de un bloque de datos de ROM (Audio_File).
  	  pBuffer: Puntero al comienzo de un bloque del BUFFER (Audio_Buffer) de RAM.
  	  nDatos: Nº de muestras de 16 bits a copiar.   */
void Copiar_Buffer(uint16_t* pAudio, uint16_t* pBuffer, uint32_t nDatos) {
	if (nRestantes>=nDatos) 	// Quedan datos suficientes para llenar un bloque completo del buffer (RAM)
	{
		ROM2BUFFER(pAudio, pBuffer, nDatos); // Copia un bloque completo de audio
		// Calcula cuantos datos restan:
		nRestantes-=nDatos; // Recalcula cuantas muestras quedan en ROM.
	}
	else // No quedan datos suficientes para llenar un bloque completo del buffer (RAM)!
	{    // Entonces, copia muestras restantes, luego completa con las que faltan para llenar el bloque
	     // y reinicia el ciclo (Si nMuestras no es divisible por BLOCK_SIZE, el puntero se irá desfasando.)
		if (nRestantes>0) // Todavia queda una porcion de muestras (menor que un bloque)
		{
			ROM2BUFFER(pAudio, pBuffer, nRestantes); // Completa una parte del bloque con la muestras restantes
			nDatos-=nRestantes; // Calcula nº muestras que faltan para llenar el bloque
		}
		pAudio=Audio_File; // Inicializa puntero al comienzo del archivo de audio.
		// Toma una porcion de muestras al comienzo del audio para completar el bloque con las muestras q faltan.
		// Si nMuestras no es divisible por BLOCK_SIZE, el puntero se irá desfasando.
		// Previamente, el puntero del buffer de RAM se desplaza lo suficiente (nRestantes) para copiar la
		// porcion faltante.
		ROM2BUFFER(pAudio, pBuffer+nRestantes*sizeof(uint16_t), nDatos);
		nRestantes=nMuestras-nDatos; // Recalcula nº de datos restantes
	}
}
// ----------------------------------------------------------------------------
/* Funcion: "ROM2BUFFER".
 * Copia datos de Audio (de ROM) a BUFFER (en RAM). Debe duplicar cada muestra para
 * generar los dos canales (L+R). */
/* --Parametros de entrada:
      pAudio: Puntero al comienzo de un bloque de datos de ROM.
  	  pBuffer: Puntero al comienzo de un bloque del BUFFER (Audio_Buffer) de RAM.
  	  nDatos: Nº de muestras de 16 bits a copiar.   */
void ROM2BUFFER(uint16_t* pAudio, uint16_t* pBuffer, uint32_t nDatos) {
	// Escriba su código aqui!!!

	for(int i=0; i<=(nDatos-1); i++)
	{
		*pBuffer=*pAudio;
		pBuffer++;
		*pBuffer=*pAudio;
		pBuffer++;
		pAudio++;

	}
	// ...
}
// ------------------------------------------------------------------------
// Función: "Filtrar_Buffer"
// (1) Convierte y copia un bloque de muestras de ROM (uint16_t) al buffer auxBuffer (float32_t),
// (2) luego llama a la funcion del filtro y ésta copia el bloque filtrado a RAM (outBuffer).
// (3) Por último re-convierte a uint16_t y copia el bloque filtrado al buffer de audio (Audio_Buffer)
/* --Parámetros de entrada:
      pAudio: Puntero al comienzo de un bloque de datos de ROM.
  	  pBuffer: Puntero al comienzo de un bloque del BUFFER (Audio_Buffer) de RAM.
  	  nDatos: N� de muestras de 16 bits a Filtrar.   */
void Filtrar_Buffer(uint16_t* pAudio, uint16_t* pBuffer, uint32_t nDatos) {
	if (nRestantes>=nDatos) // Quedan datos suficientes para llenar un bloque completo del buffer auxBuffer.
	{
		UINT16toFLOAT32(pAudio, auxBuffer, nDatos); /* Convierte el tipo de datos de audio a float32_t. */
		FIRFilter(auxBuffer, outBuffer, nDatos); 	// Aplica filtro al bloque completo
		FLOAT32toUINT16(outBuffer, pBuffer, nDatos);// Convierte y copia salida del filtro al Audio_Buffer
		nRestantes-=nDatos; // re-calculo nº de muestras restantes a filtrar.
	}
	else // No quedan datos suficientes para llenar un bloque completo!
	{    // Entonces, copia muestras restantes, luego completa con las que faltan para llenar el bloque
		if (nRestantes>0) // Todavia queda una porcion de muestras (menor que un bloque)
		{
			UINT16toFLOAT32(pAudio, auxBuffer, nRestantes); // Convierte las muestras restantes.
		}
		pAudio=Audio_File;	// Re-inicializa el puntero al comienzo del audio.
		// Toma una porcion de muestras del comienzo del audio y las convierte a float32_t para
		// completar el bloque con las muestras q faltan.
		// Luego las filtra y las convierte a uint16_t y las copia a Audio_Buffer.
		// Si nMuestras no es divisible por BLOCK_SIZE, el puntero se irá desfasando.
		UINT16toFLOAT32(pAudio, auxBuffer+nRestantes, nDatos-nRestantes); // Completa el bloque con las muestras
																		  // convertidas que le faltan.
		FIRFilter(auxBuffer, outBuffer, nDatos); 	 // Aplica filtro al bloque completo
		FLOAT32toUINT16(outBuffer, pBuffer, nDatos); // Convierte y copia salida del filtro al Audio_Buffer
		nRestantes=nMuestras-(nDatos-nRestantes);    // Recalcula nº de datos restantes
	}
}
// ----------------------------------------------------------------------------
/* Función: "UINT16toFLOAT32".
 * Convierte y copia un bloque de datos de ROM (uint16_t) a un buffer de RAM (auxBuffer) de tipo float32_t. */
/* --Parametros de entrada:
      pAudio: Puntero al comienzo de un bloque de datos de ROM.
  	  pAuxBuffer: Puntero al comienzo de un bloque del BUFFER auxiliar (auxBuffer) de RAM.
  	  nDatos: Nº de muestras de 16 bits a convertir y copiar. */
void UINT16toFLOAT32(uint16_t* pAudio, float32_t* pAuxBuffer, uint32_t nDatos) {
	// Escriba su código aqui!!!
	// ...

	for(int i=0; i<=(nDatos-1); i++)
		{
			*pAuxBuffer=*pAudio;//
			pAuxBuffer++;
			pAudio++;

		}

	// ...
}
// ----------------------------------------------------------------------------
/* Función: "FLOAT32toUINT16".
 * Convierte y copia un bloque de datos de un buffer (outBuffer) de RAM de tipo float32_t
 * a un buffer de RAM (Audio_Buffer) de tipo uint16_t.
 * DUPLICA EN EL BUFFER DE SALIDA LOS DATOS PARA CONVERTIRLO A STEREO!!!
 * --Parametros de entrada:
  	  outBuffer: Puntero al comienzo de un bloque del BUFFER de salida del filtro en RAM (outBuffer).
      pBuffer: Puntero al comienzo de un bloque del buffer de datos en RAM (Audio_Buffer).
  	  nDatos: N� de muestras de 32 bits a convertir y copiar.   */
void FLOAT32toUINT16(float32_t* outBuffer, uint16_t* pBuffer, uint32_t nDatos){
	// Escriba su código aqui!!!
	// ...

	for(int i=0; i<=(nDatos-1); i++)
		{
			*pBuffer=*outBuffer;
			pBuffer++;
			*pBuffer=*outBuffer;
			pBuffer++;
			outBuffer++;

		}

	// ...
}
// ----------------------------------------------------------------------------
/* Función: "FIRFilter"
 * Aplica un filtrado FIR a la secuencia de audio previamente convertida a float32.
 * Las salidas del filtro (float32_t) se DEBEN volver a transformar en uint16 y
 *  se DEBEN duplicar para alimentar ambos canales L+R del Audio DAC.
 * --Parametros de entrada:
   	  pAuxBuffer: Puntero al comienzo de un bloque del BUFFER auxiliar (auxBuffer) de RAM.
  	  pBuffer: Puntero al comienzo de un bloque del BUFFER (Audio_Buffer) de RAM.
  	  nDatos: Nº de muestras de 16 bits a copiar.   */
void FIRFilter(float32_t* pAuxBuffer, float32_t* outBuffer, uint32_t nDatos) {
	arm_fir_f32(&fStruct, pAuxBuffer, outBuffer, nDatos);
}
// ----------------------------------------------------------------------------
/* Función: "FIR_Init"
 * Inicializa el filtro y crea una estructura S que contiene todos sus parametros.
 * N_etapas indica el numero de coef. &firCoeffs[0] es un puntero al arreglo donde
 * se encuentran los coeficientes. &firState[0] es un puntero al vector de estados
 * del filtro y BLOCK_SIZE es el numero de muestras que procesa por vez. */
void FIR_Init(void)
{
	// ----Variables usadas por el filtro FIR
	static uint8_t N_etapas;
	static float32_t *firState;
	// Estos coefs. tienen que estar invertidos en tiempo.
	static const float32_t firCoeffs[] = {0.20301697342587313,
			0.040944384243841649,
			0.043815531416821839,
			0.045999808763215751,
			0.047151258571339477,
			0.047654437435629096,
			0.047151258571339477,
			0.045999808763215751,
			0.043815531406820839,
			0.040944384243841649,
			0.20301697342587313
	};
	N_etapas=sizeof(firCoeffs)/sizeof(float32_t); // Calcula Nº de Etapas
	firState = malloc((BLOCK_SIZE + N_etapas - 1) * sizeof(float32_t)); // Buffer de entradas y salidas anteriores del filtro.
	arm_fir_init_f32(&fStruct, N_etapas, (float32_t *) firCoeffs, firState, BLOCK_SIZE);
}
// ----------------------------------------------------------------------------
// Función: "BSP_AUDIO_OUT_HalfTransfer_CallBack"
// Interrupción generada por el Codec de Audio cuando ha TRANSFERIDO la 1º mitad de los datos.
// Guarda el estado.
void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{
	Estado_BUFFER=MITAD; // Flag indicando que transfirió la 1º mitad del buffer.
}
// ----------------------------------------------------------------------------
// Función: "BSP_AUDIO_OUT_TransferComplete_CallBack"
// Interrupción generada por el Codec de Audio cuando termina de TRANSFERIR la 2º mitad de los datos.
// Guarda el estado.
void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
	Estado_BUFFER=TODO; // Flag indicando que se transfirió la 2º mitad del buffer.
	// BSP_AUDIO_OUT_ChangeBuffer: Ordena envío de nuevas muestras al reproductor, via DMA.
	// Se indica la cantidad de muestras de 16 bits a reproducir (Todo el buffer de RAM).
	BSP_AUDIO_OUT_ChangeBuffer((uint16_t*) &Audio_Buffer[0], 4*BLOCK_SIZE);
}
// ----------------------------------------------------------------------------
// Función: "BSP_AUDIO_OUT_Error_CallBack"
void BSP_AUDIO_OUT_Error_CallBack(void)
// Manages the DMA FIFO error interrupt.
{
  /* Stop the program with an infinite loop */
  while (1)
  {
  }
}
// ----------------------------------------------------------------------------
/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/
// ----------------------------------------------------------------------------
void NMI_Handler(void)
/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
{
}
// ----------------------------------------------------------------------------
void HardFault_Handler(void)
/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}
// ----------------------------------------------------------------------------
void MemManage_Handler(void)
/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}
// ----------------------------------------------------------------------------
void BusFault_Handler(void)
/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}
// ----------------------------------------------------------------------------
void UsageFault_Handler(void)
/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
// ----------------------------------------------------------------------------
void SVC_Handler(void)
/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
{
}
// ----------------------------------------------------------------------------
void DebugMon_Handler(void)
/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
{
}
// ----------------------------------------------------------------------------
void PendSV_Handler(void)
/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
{
}
// ----------------------------------------------------------------------------
/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f40xx.s).                                              */
/******************************************************************************/
// ----------------------------------------------------------------------------
void EXTI0_IRQHandler(void)
/**
  * @brief  This function handles External line 0 interrupt request.
  * @param  None
  * @retval None
  */
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}
// ----------------------------------------------------------------------------
void EXTI1_IRQHandler(void)
/**
  * @brief  This function handles External line 1 interrupt request.
  * @param  None
  * @retval None
  */
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}
// ----------------------------------------------------------------------------
void I2S3_IRQHandler(void)
/**
  * @brief  This function handles main I2S interrupt.
  * @param  None
  * @retval 0 if correct communication, else wrong communication
  */
{
  HAL_DMA_IRQHandler(hAudioOutI2s.hdmatx);
}
// ----------------------------------------------------------------------------
void I2S2_IRQHandler(void)
/**
  * @brief  This function handles DMA Stream interrupt request.
  * @param  None
  * @retval None
  */
{
  HAL_DMA_IRQHandler(hAudioInI2s.hdmarx);
}
// ----------------------------------------------------------------------------
void TIM4_IRQHandler(void)
/**
  * @brief  This function handles TIM4 global interrupt request.
  * @param  None
  * @retval None
  */
{
  //HAL_TIM_IRQHandler(&hTimLed);
}
// ----------------------------------------------------------------------------
void OTG_FS_IRQHandler(void)
/**
  * @brief  This function handles USB-On-The-Go FS global interrupt request.
  * @param  None
  * @retval None
  */
{
  //HAL_HCD_IRQHandler(&hHCD);
}
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
