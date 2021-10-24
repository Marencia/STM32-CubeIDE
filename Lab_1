// ----------------------------------------------------------------------------
// Laboratorio Nº2: EntradasSalidas.
// ----------------------------------------------------------------------------
// Agregue sus Includes aqui.
#include "SysTick.h"
#include "Salidas.h"
#include "Entradas.h"
//...
#define SYSTICK_FREQ_HZ 1000
 int cont;
// ----- main() ---------------------------------------------------------------
int main(void)
{
	// System Tick Init
	SysTick_Init (SYSTICK_FREQ_HZ);
	/* Conf. Salidas */
	__HAL_RCC_GPIOD_CLK_ENABLE(); // Enable GPIOD Peripheral clock
	Out_Init(GPIOD, LED_ROJO_A|LED_AMARILLO_A|LED_VERDE_A);
	/*Conf. Entradas*/
	__HAL_RCC_GPIOA_CLK_ENABLE();
	In_Init(GPIOA, SW1|SW2|SW3);
	/*Todos los leds Off*/
	HAL_GPIO_WritePin(GPIOD, LED_ROJO_A|LED_AMARILLO_A|LED_VERDE_A, GPIO_PIN_RESET);

	while (1) // Loop infinito.
	{
		if (!HAL_GPIO_ReadPin (GPIOA, SW1)) {
			HAL_Delay(20);
			if (!HAL_GPIO_ReadPin (GPIOA, SW1)){
				HAL_GPIO_TogglePin(GPIOD, LED_ROJO_A);}
			while (!HAL_GPIO_ReadPin (GPIOA, SW1));
			}

		if (!HAL_GPIO_ReadPin (GPIOA, SW2)){
			HAL_Delay(20);
			if (!HAL_GPIO_ReadPin (GPIOA, SW2)){
				HAL_GPIO_TogglePin(GPIOD, LED_AMARILLO_A);}
			while (!HAL_GPIO_ReadPin (GPIOA, SW2));
			}

		if (!HAL_GPIO_ReadPin (GPIOA, SW3)){
			HAL_Delay(20);
			if (!HAL_GPIO_ReadPin (GPIOA, SW3)){
				HAL_GPIO_TogglePin(GPIOD, LED_VERDE_A);}
			while (!HAL_GPIO_ReadPin (GPIOA, SW3));
			}

	}
}

// ----------------------------------------------------------------------------
void SysTickHook (void) // Para colgarse del System Tick
{
	// Escriba su codigo aqui

//
}
