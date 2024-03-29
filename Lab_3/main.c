// ----------------------------------------------------------------------------
// Laboratorio Nº3: Display Multiplexado.
//Primero tengo que crear una variable para que cada cinco milisegundos cambie de digito
//Ese cambio de digito tiene que ser otra variable que se resetee desp del 3
//El cambio de digito lo ejecuto con el SelDig
//Cuando caigo en cada display, le cargo el numero con BIN7SEG
// ----------------------------------------------------------------------------
// Agregue sus "#includes" aqui
#include "SysTick.h"
#include "Salidas.h"
#include "Entradas.h"
// ----------------------------------------------------------------------------
// Variables globales
#define SYSTICK_FREQ_HZ 1000
uint8_t cont_sw1;

// ----- main() ---------------------------------------------------------------
int main(void)
{
	//System Tick Init
	SysTick_Init (SYSTICK_FREQ_HZ);

	/*Conf. Entradas*/
	__HAL_RCC_GPIOA_CLK_ENABLE();
	In_Init(GPIOA, SW1|SW2);

	/* Conf. Salidas */
	__HAL_RCC_GPIOD_CLK_ENABLE(); // Enable GPIOD Peripheral clock
	__HAL_RCC_GPIOC_CLK_ENABLE();
	Out_Init(GPIOD, SEGMENTO_a|SEGMENTO_b|SEGMENTO_c|SEGMENTO_d|SEGMENTO_e|SEGMENTO_f|SEGMENTO_g|SEGMENTO_PD);
	Out_Init(GPIOC, SEL_MUX_0|SEL_MUX_1);


	while (1) // Loop infinito.
	{
		if (!HAL_GPIO_ReadPin (GPIOA, SW1)) {
		HAL_Delay(20);
		if (!HAL_GPIO_ReadPin (GPIOA, SW1)){
			cont_sw1++;}
		while (!HAL_GPIO_ReadPin (GPIOA, SW1));
		}
	}	

}

// ----------------------------------------------------------------------------
void SysTickHook (void) // Para colgarse del System Tick
{
	static uint8_t UNIDAD=0, DECENA=0, DECIMA=0, CENTESIMA=0;
	static uint8_t CONTADOR=0, DIGITO=0;
	static uint8_t CONT_MILESIMAS=0;
	static uint8_t BUFDISP[4];
	static uint8_t cont_sw2;

	// Tabla de Conversion BCD -> 7 segmentos
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
			SEGMENTO_a|SEGMENTO_b|SEGMENTO_c|SEGMENTO_f|SEGMENTO_g				// Nueve

	};
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

	// Calculo de digitos
if (cont_sw1==1){
	if (CONT_MILESIMAS!=10){
		CONT_MILESIMAS++;

	}
	else{
		CONT_MILESIMAS=0;
		CENTESIMA++;
		BUFDISP[0]=CENTESIMA;
		if (CENTESIMA==10){
			CENTESIMA=0;
			BUFDISP[0]=CENTESIMA;
			DECIMA++;
			BUFDISP[1]=DECIMA;
			if (DECIMA==10){
				DECIMA=0;
				BUFDISP[1]=DECIMA;
				UNIDAD++;
				BUFDISP[2]=UNIDAD;
				if (UNIDAD==10){
					UNIDAD=0;
					BUFDISP[2]=UNIDAD;
					DECENA++;
					BUFDISP[3]=DECENA;
					if (DECENA==6){
						DECENA=0;
						BUFDISP[3]=DECENA;
					}
				}
			}
		}

	}
	HAL_GPIO_WritePin(GPIOD, BIN7SEG[BUFDISP[DIGITO]], GPIO_PIN_SET);
}
else {
	if ((cont_sw1==2)||(cont_sw2==0)){
		cont_sw1=0;
			HAL_GPIO_WritePin(GPIOD, BIN7SEG[BUFDISP[DIGITO]], GPIO_PIN_SET);
	}
	if (!HAL_GPIO_ReadPin (GPIOA, SW2)){

			UNIDAD=0;
			DECENA=0;
			DECIMA=0;
			CENTESIMA=0;
			BUFDISP[0]=0;
			BUFDISP[1]=0;
			BUFDISP[2]=0;
			BUFDISP[3]=0;

			cont_sw2=0;
	}
}

}

//Dejar el refresco fuera de los condicionales





