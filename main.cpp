/**
* @file main.cpp
* @author Sandoval, Raña
* @date 13/04/2023
* @section TP 4
* @brief    Se trata del detector de fuego trabajado en las últimas clases con ciertas modificaciones.
*           Se modularizó en carpetas y se descartó en uso de pines de entrada. Se utiliza un teclado
*           matricial.
* @details
* En la carpeta Build está el archivo mbed_config.h donde están definidas las constantes que usa mbed. 
* En mbed-os, están las librerías provistas por mbed con las funciones necesarias para
* desarrollar los programas. Por ejemplo, se encuentra el archivo mbed.h y los correspondientes
* a Doxygen.
*
*La carpeta modules contiene las librerias utilizadas en el programa.
* Luego, hay 6 archivos:
*       .gitignore es un archivo que indica a Git que archivos o carpetas ignorar
*       .mbed indica el modelo del dispositivo a trabajar
*       arm_book_lib.h Es una librería armada por los autores del libro para trabajar con los ejemplos
*       compile_commands.json es un archivo json con los comandos para compilar
*       main.cpp es el archivo principal del proyecto en C++ 
*       mbed-os.lib es la dirección al repositorio donde se encuentra la carpeta mbed-os
*/


//=====[Libraries]=============================================================

#include "smart_home_system.h"

//=====[Main function, the program entry point after power on or reset]========

/**
* La función main se puede ver como el sistema "Smart Home System" en donde están definidas
* las funciones smartHomeSystemInit y smartHomeSystemUpdate.
*
* Esas funciones están declaradas en smart_home_system.h y definidas en smart_home_system.cpp.
* Ahí, se incluye a las bibliotecas siren.h, user_interface.h, fire_alarm.h, pc_serial_com.h
* y event_log.h. Esas bibliotecas son subsistemas que incluyen otras bibliotecas.
*/
int main()
{
    smartHomeSystemInit();
    /** smartHomeSystemInit es una funcion que inicializa la comunicación con el teclado matricial,
        los sensores y las alarmas
        */
    while (true) {
        smartHomeSystemUpdate();
    }
}