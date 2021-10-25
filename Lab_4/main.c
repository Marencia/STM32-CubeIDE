// ----------------------------------------------------------------------------
// Laboratorio Nº4: Temperatura.

// ----------------------------------------------------------------------------
#include "SysTick.h"
#include "Salidas.h"
#include "Entradas.h"
#include <math.h>
#include "ConversorxInts.h"
// Agregue sus "#includes" aqui
// ----------------------------------------------------------------------------
// Definicion de Constantes NTC
#define B (float) 4050		// [K]
#define To (float) 273		// [K]
#define Ro (float) 34.71	// [Kohms]
#define SYSTICK_FREQ_HZ 1000
void Error_Handler(void);
// ----------------------------------------------------------------------------
/* Variables globales */
uint16_t UltimaConversion;
float V_NTC;
float R_NTC;
float T;
float Tc;
uint8_t BUFDISP[4];

int main(void)
{
	/* Inicializacion Conversor A/D (ADC1 - Channel 2 - PA2) */
	/* Peripherals clock enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__ADC1_CLK_ENABLE();
	SysTick_Init (SYSTICK_FREQ_HZ);

	/* Salidas */
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	Out_Init(GPIOD, SEGMENTO_a|SEGMENTO_b|SEGMENTO_c|SEGMENTO_d|SEGMENTO_e|SEGMENTO_f|SEGMENTO_g|SEGMENTO_PD);
	Out_Init(GPIOC, SEL_MUX_0|SEL_MUX_1);

	/* Inicializacion Entrada Analogica NTC (ADC1 - Channel 2 - PA2) */
	ADC1_NTC_Init();
	/* Habilita Interrupciones ADC1 */
	if (HAL_ADC_Start_IT(&hADC1) != HAL_OK)
	{
		/* Start Conversation Error */
		Error_Handler();
	}
	
	for (;;) // Lazo infinito.
	{
	// Escriba su codigo aqui
		HAL_Delay(1000);
		V_NTC=3*(float)UltimaConversion/4095;
		R_NTC=V_NTC*10/(5-V_NTC);//el Kohm
		T=1/((log(R_NTC/Ro)/B)+1/To); //en kelvin
		Tc=T-273;

		//Tc=12.3;
		BUFDISP[3]=(int)(Tc/10); //DECENA 1
		BUFDISP[2]=(int)(Tc-(int)(Tc/10)*10);//UNIDAD 2
		BUFDISP[1]=(int)((Tc*10)-((int)(Tc)*10));//DECIMA 3
		BUFDISP[0]=10;//Grados
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
			SEGMENTO_a|SEGMENTO_b|SEGMENTO_c|SEGMENTO_f|SEGMENTO_g,				// Nueve
			SEGMENTO_a|SEGMENTO_f|SEGMENTO_g|SEGMENTO_b,						//grado
			};						
	//-----------------------------------------------------------------------------
	// Escriba su codigo aqui

	//Multilplexado
	if (CONTADOR!=5){
		CONTADOR++;
	}
	else{
		CONTADOR=0;
		DIGITO++;
		if (DIGITO==2){
			HAL_GPIO_WritePin(GPIOD, SEGMENTO_PD, GPIO_PIN_SET);
		}
		else{
			HAL_GPIO_WritePin(GPIOD, SEGMENTO_PD, GPIO_PIN_RESET);
		}
		if (DIGITO==4){
			DIGITO=0;
		}
	}

	SelDig(DIGITO);
	HAL_GPIO_WritePin(GPIOD, BIN7SEG[8], GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, BIN7SEG[BUFDISP[DIGITO]], GPIO_PIN_SET);

}

void ADCIRQHook (uint16_t ConversorAD)
{
	UltimaConversion=ConversorAD;
}

void Error_Handler(void)
{
  /* Trap for error*/
  while(1)
  {
  }
}
