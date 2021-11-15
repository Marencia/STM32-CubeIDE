/* Programa para controlar un porton corredizo */
/* Integración de la maquina de estados creada por Yakindu*/
/**********************************************************/
//Agregue todos los includes necesarios
//para los perifericos que use.
#include "Control.h"
#include "sc_timer_service.h"
#include "SysTick.h"
#include "Salidas.h"
#include "Entradas.h"
#include <math.h>
#include "ConversorxInts.h"
#include "ICTimer.h"

Control ls; // Estructura de datos de State Machine
#define SYSTICK_FREQ_HZ 1000
#define MAX_TIMERS 4 // Lugar para los Timers de la StateChart (por defecto)
#define MODULO 4294967295
static sc_timer_t timers[MAX_TIMERS];
static sc_timer_service_t timer_service;
/* Variables globales */
uint8_t UpdateTimerFlag=false;
uint32_t N_CICLOS;
uint32_t N_ANTERIOR;
uint32_t F_555;

void Error_Handler(void);

int main(void)
{
	/* Inicialice sus Perifericos AQUI*/
	/* Conf. Salidas */
	__HAL_RCC_GPIOD_CLK_ENABLE(); // Enable GPIOD Peripheral clock
	Out_Init(GPIOD, LED_ROJO_A|LED_AMARILLO_A|LED_VERDE_A);
	/*Conf. Entradas*/
	__HAL_RCC_GPIOA_CLK_ENABLE();
	In_Init(GPIOA, SW1|SW2|SW3);
	/*Todos los leds Off*/
	//HAL_GPIO_WritePin(GPIOD, LED_ROJO_A|LED_AMARILLO_A|LED_VERDE_A, GPIO_PIN_RESET);

	/*Sensor */
		__HAL_RCC_TIM2_CLK_ENABLE();
		Init_ICTimer();

	/*Inicialización del servicio de timers de la StateChart*/
	sc_timer_service_init(&timer_service, timers, MAX_TIMERS,
						  (sc_raise_time_event_fp) &control_raise_time_event);
	/* System Tick Init */
	SysTick_Init (SYSTICK_FREQ_HZ);
	/*Inicializa la maquina de estados*/
	control_init(&ls);
	/*Ingresa a la maquina de estados*/
	control_enter(&ls);
	while (1) // Loop infinito.
		{
		if (UpdateTimerFlag)	// TRUE?->Entonces se ejecuta un nuevo ciclo de la SC
		{
		// Manejo de Eventos de Entrada
			// Ej: Estado del SW1->Evento
			if ((GPIOA->IDR & SW1)==0)  // Estado de SW1?
				control_eSW1_raise_press (&ls);
			else
				control_eSW1_raise_no_press (&ls);
			// Continúe aquí
			if ((GPIOA->IDR & SW2)==0)  // Estado de SW2?
				control_eSW2_raise_press (&ls);
			else
				control_eSW2_raise_no_press (&ls);

			if ((GPIOA->IDR & SW3)==0)  // Estado de SW3?
				control_eSW3_raise_press (&ls);

			else
				control_eSW3_raise_no_press (&ls);

			if (F_555<600){
				control_esensor_raise_on (&ls);
			}
			else{
				control_esensor_raise_off (&ls);
			}

			sc_timer_service_proceed(&timer_service, 1/*ms*/); // Incrementa el timer de la SC.
			control_run_cycle(&ls);	// Se ejecuta un ciclo de la StateChart.
			UpdateTimerFlag=false;
		}
		//__WFI(); // Wait for Interrupt (se va a dormir)
		}
}

// ----------------------------------------------------------------------------
void SysTickHook (void) // Para colgarse del System Tick
{
	/*Sincronizacion del timer del micro con los timers de la maquina de estados*/
		UpdateTimerFlag=true;
}


//Definicion de funciones de acciones (operaciones)
void control_aledR_on(const Control* handle)
{
	HAL_GPIO_WritePin(GPIOD, LED_ROJO_A, GPIO_PIN_SET);
}
// Continue aqui
void control_aledR_off(const Control* handle)
{
	HAL_GPIO_WritePin(GPIOD, LED_ROJO_A, GPIO_PIN_RESET);
}

void control_aledV_on(const Control* handle)
{
	HAL_GPIO_WritePin(GPIOD, LED_VERDE_A, GPIO_PIN_SET);
}

void control_aledV_off(const Control* handle)
{
	HAL_GPIO_WritePin(GPIOD, LED_VERDE_A, GPIO_PIN_RESET);
}

void control_aledA_on(const Control* handle)
{
	HAL_GPIO_WritePin(GPIOD, LED_AMARILLO_A, GPIO_PIN_SET);
}

void control_aledA_off(const Control* handle)
{
	HAL_GPIO_WritePin(GPIOD, LED_AMARILLO_A, GPIO_PIN_RESET);
}

//void control_aled_off(const Control* handle, uint16_t mask)
//{
//	HAL_GPIO_WritePin(GPIOD, mask, GPIO_PIN_RESET);
//}
//! callback implementation for the setting up time events
void control_set_timer(Control* handle, const sc_eventid evid, const sc_integer time_ms, const sc_boolean periodic)
{
    sc_timer_set(&timer_service, handle, evid, time_ms, periodic);
}

//! callback implementation for canceling time events.
void control_unset_timer(Control* handle, const sc_eventid evid)
{
    sc_timer_unset(&timer_service, evid);
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

	F_555=FTIM2/N_CICLOS;

}

void Error_Handler(void)
{
    while(1)
    {
    	//Stay here on Error
    }
}

