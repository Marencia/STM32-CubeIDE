/*
 * Nombre: Salidas.h
 * Autor: Eduardo L. Blotta
 * Comentarios: Manejo de Salidas Digitales en los GPIO
 * Año 2021
 */
//--------------------------------------
#include "CommonIO.h"
void Out_Init(GPIO_TypeDef *GPIOx, uint32_t mask);
void SelDig(uint8_t Digito);


/*
 * Nombre: Entradas.h
 * Autor: Eduardo L. Blotta
 * Comentarios: Manejo de Entradas Digitales en GPIO
 * Año 2021
 */
#include "CommonIO.h"

void In_Init(GPIO_TypeDef  *GPIOx, uint32_t mask);


