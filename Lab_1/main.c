// ----------------------------------------------------------------------------
// Laboratorio Nº1: Salidas.
// ----------------------------------------------------------------------------
#include "SysTick.h"
#include "Salidas.h"
#define SYSTICK_FREQ_HZ 1000	//Frecuencia del System Tick
// ----- main() ---------------------------------------------------------------
int main(void)
{
	// System Tick Init
	SysTick_Init (SYSTICK_FREQ_HZ);
	/* Conf. Salidas */
	__HAL_RCC_GPIOD_CLK_ENABLE(); // Enable GPIOD Peripheral clock
	Out_Init(GPIOD, LED_ROJO_A|LED_AMARILLO_A);
	/*Todos los leds Off*/
	HAL_GPIO_WritePin(GPIOD, LED_ROJO_A|LED_AMARILLO_A, GPIO_PIN_RESET);
	while (1) // Loop infinito.
	{
	// Escriba su codigo aqui
	}

}

// ----------------------------------------------------------------------------
void SysTickHook (void) // Para colgarse del System Tick
{
	// Escriba su codigo aqui
}
