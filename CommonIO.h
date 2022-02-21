/*****************************************************************************
* Archivo: CommonIO.h
* Autor archivo: Eduardo L. Blotta
* Fecha: 2021
* Comentario: Archivo de declaraciones de entradas y salidas: "CommonIO.h".
******************************************************************************/
// ----------------------------------------------------------------------------
#include "stm32f4xx.h"
// ----------------------------------------------------------------------------
// -Definicion de algunas entradas y salidas en PLACA DISCOVERY:
// STM32F4DISCOVERY (device name: STM32F407VG)

//----- Switches en Port A-----
#define SW1 GPIO_PIN_2//FILA 1
#define SW2 GPIO_PIN_4 //no lo voy a usar para el tp final
#define SW3 GPIO_PIN_5//FILA 2

//Grupo A de LEDs en Port D
#define LED_ROJO_A      GPIO_PIN_8 //FILA 3
#define LED_AMARILLO_A 	GPIO_PIN_11 //FILA 4
#define LED_VERDE_A		GPIO_PIN_9 //COLUMNA 1

//--------------------------------------
//Grupo B de LEDs en Port D
#define LED_ROJO_B      GPIO_PIN_13 //COLUMNA 2
#define LED_AMARILLO_B 	GPIO_PIN_15 //COLUMNA 3
#define LED_VERDE_B		GPIO_PIN_12 //COLUMNA 4

//--------------------------------------
//Entradas al Mux en Port C
#define SEL_MUX_0      GPIO_PIN_0
#define SEL_MUX_1      GPIO_PIN_2

//--------------------------------------
//Definicion de segmentos
//--------------------------------------
/*		  	a (p14)
*          --------
*		  |        |
*		  |        |
*		 f|(p15)   |b (p8)
*		  |        |
*		  | g(p12) |
*          --------
*		  |        |
*		  |        |
*		 e|(p13)   |c (p11)
*		  |        |
*		  | d (p9) |	--
*          --------	   |pd| (p10)
*					    --
*/
//--------------------------------------
#define SEGMENTO_a      GPIO_PIN_14
#define SEGMENTO_b      GPIO_PIN_8
#define SEGMENTO_c 		GPIO_PIN_11
#define SEGMENTO_d		GPIO_PIN_9
#define SEGMENTO_e      GPIO_PIN_13
#define SEGMENTO_f 		GPIO_PIN_15
#define SEGMENTO_g		GPIO_PIN_12
#define SEGMENTO_PD		GPIO_PIN_10

//--------------------------------------
//Leds placa Discovery en Port D
#define LED_Green	GPIO_PIN_12
#define LED_Orange	GPIO_PIN_13
#define LED_Red		GPIO_PIN_14
#define LED_Blue	GPIO_PIN_15
//--------------------------------------
//Boton de usuario (azul)
//placa Discovery en Port A
//--------------------------------------
#define Blue_button	GPIO_PIN_0
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
