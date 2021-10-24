// ----------------------------------------------------------------------------
// Laboratorio Nº6: Fotometro.
// ----------------------------------------------------------------------------

#include "ICTimer.h"
// Agregue sus "#includes" aqui
#include "SysTick.h"
#include "Salidas.h"
#include "Entradas.h"
//Definicion de constantes
#define SYSTICK_FREQ_HZ 1000
#define MODULO 4294967295
void Error_Handler(void);
// ----------------------------------------------------------------------------
/* Variables globales */
uint32_t Frecuencia = 0; // en Hz
uint8_t BUFDISP[4];
uint32_t N_CICLOS;
uint32_t N_ANTERIOR;
uint32_t F_555;
uint8_t Vector_Promedio[4];
// ----- main() ---------------------------------------------------------------
int main(void)
{
	// INICIALIZACION
	SysTick_Init (SYSTICK_FREQ_HZ);
	/* Peripherals clock enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_TIM2_CLK_ENABLE();
	Init_ICTimer(); //Inicializo el ictimer para que entre a la función

	/*Salidas*/
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	Out_Init(GPIOD, SEGMENTO_a|SEGMENTO_b|SEGMENTO_c|SEGMENTO_d|SEGMENTO_e|SEGMENTO_f|SEGMENTO_g|SEGMENTO_PD);
	Out_Init(GPIOC, SEL_MUX_0|SEL_MUX_1);

	for (;;)
	{
	// Escriba su codigo aqui
		HAL_Delay(500);

		BUFDISP[0]=(F_555 % 10);
		BUFDISP[1]=((F_555/10)%10);
		BUFDISP[2]=((F_555/100)%10);
		BUFDISP[3]=(F_555/1000);

	} 
}
// ----------------------------------------------------------------------------
void SysTickHook (void) // Para colgarse del System Tick
{
	//Variables
		static uint8_t CONTADOR=0, DIGITO=0;

	//Conversion BCD -> 7 segmentos
	const uint32_t BIN7SEG[] = {
			SEGMENTO_a|SEGMENTO_b|SEGMENTO_c|SEGMENTO_d|SEGMENTO_e|SEGMENTO_f,	// Cero
			SEGMENTO_b|SEGMENTO_c,												// Uno
			SEGMENTO_a|SEGMENTO_b|SEGMENTO_g|SEGMENTO_e|SEGMENTO_d,				// Dos
			SEGMENTO_a|SEGMENTO_b|SEGMENTO_c|SEGMENTO_d|SEGMENTO_g,				// Tres
			SEGMENTO_f|SEGMENTO_b|SEGMENTO_g|SEGMENTO_c,						// Cuatro
			SEGMENTO_a|SEGMENTO_f|SEGMENTO_g|SEGMENTO_c|SEGMENTO_d,				// Cinco
			SEGMENTO_f|SEGMENTO_e|SEGMENTO_d|SEGMENTO_c|SEGMENTO_g,				// Seis
			SEGMENTO_a|SEGMENTO_b|SEGMENTO_c,									// Siete
			SEGMENTO_a|SEGMENTO_b|SEGMENTO_c|SEGMENTO_d|SEGMENTO_e|SEGMENTO_f|SEGMENTO_g,	// Ocho
			SEGMENTO_a|SEGMENTO_b|SEGMENTO_c|SEGMENTO_f|SEGMENTO_g};			// Nueve

		// Escriba su codigo aqui
		//Multilplexado
		if (CONTADOR!=5){
			CONTADOR++;
		}
		else{
			CONTADOR=0;
			DIGITO++;
			if (DIGITO==4){
				DIGITO=0;
			}
		}

		SelDig(DIGITO);
		HAL_GPIO_WritePin(GPIOD, BIN7SEG[8], GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, BIN7SEG[BUFDISP[DIGITO]], GPIO_PIN_SET);
}

void ICTimerHook (uint32_t CapturedValue)
{
// Utilice la Constante FTIM2 para calcular la frecuencia!!!	


	if (N_ANTERIOR>CapturedValue) {
		N_CICLOS= MODULO-N_ANTERIOR+CapturedValue;
	}
	else{
		if (N_ANTERIOR!=CapturedValue){
		N_CICLOS=(CapturedValue-N_ANTERIOR);
	}
		else{
			F_555=0;
		}
	}
	N_ANTERIOR=CapturedValue;

//	if (i!=4){
//		Vector_Promedio[i]=CapturedValue;
//		i++;
//	}
//	else{
//		Vector_Promedio[0]
//		i=0;
//	}
	F_555=FTIM2/N_CICLOS;

}

void Error_Handler(void)
{
    while(1)
    {
    	//Stay here on Error
    }
}
