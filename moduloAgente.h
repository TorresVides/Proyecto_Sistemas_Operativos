/***************************
 * Autores:Santiago Henrnandez Morales, Jose Jesus Cepeda Vargas,Andres Eduardo Meneses Rincon,Karol Dayan Torres Vides
 * Fecha:18/11/2025
 * Docente:John Franco Corredor
 * Materia:Sistemas Operativos
 * Descripcion:Este programa contiene las constantes, estructuras y funciones que se van a utilizar en el programa moduloAgente
 * 
 * 
 * 
 */

//se define el archivo de cabecera
#ifndef MODULO_AGENTE_H
//se define el archivo para que solo se incluya una vez
#define MODULO_AGENTE_H

//se declaran las bibliotecas que se van a utilizar en el programa
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>




/* Rango de horas permitido para las reservas */
#define HORA_MIN 7
#define HORA_MAX 19

/* Nombres máximos */
#define MAX_NOMBRE 32
#define MAX_PIPE   64

/* Tipos de mensaje entre Agente y Controlador */
#define TIPO_REGISTRO          1   
/* Agente -> Controlador */
#define TIPO_RESP_REGISTRO     2   
/* Controlador -> Agente */
#define TIPO_SOLICITUD         3   
/* Agente -> Controlador */
#define TIPO_RESP_SOLICITUD    4   
/* Controlador -> Agente */
#define TIPO_FIN               5   
/* Agente -> Controlador: agente terminó */

/* Resultado de una solicitud de reserva */
#define RESP_ACEPTADA          0
#define RESP_REPROGRAMADA      1
#define RESP_NEGADA            2



/*  Estructuras compartidas con el Controlador  */

typedef struct
{
    char familia[32];   // nombre de la familia
    int  horaInicio;    // hora solicitada de inicio
    int  personas;      // número de personas
} MensajeSolicitud;

typedef struct
{
    char familia[32];   // nombre de la familia
    int  horaInicio;    // hora en la que entra
    int  horaFin;       // hora en la que sale
    int  personas;      // número de personas
    int  estado;        // 0 = reservado, 1 = en parque, 2 = finalizado
} Reserva;


typedef struct
{
    int   tipo;   //representa el tipo de mensaje                            
    char  nombreAgente[MAX_NOMBRE];          // nombre del agente
    char  pipeAgente[MAX_PIPE];              // pipe para respuestas


    MensajeSolicitud solicitud;//representa los datos de la solicitud

    Reserva reserva;//se declara una variable reserva de tipo Reserva

    int   resultado;//se declara una variable resultado de tipo int

    int   horaSimulacion;//se declara una variable hora simulacion de tipo int
} Mensaje;//nombre de la estructura


void parse_args(int argc, char *argv[],char *nombreAgente,char *fileSolicitud,char *pipeRecibe);//se declara la funcion parse_args

//se utiliza para indicar el fin de la definicion de el codigo que se encuentra dentro de #ifndef
#endif