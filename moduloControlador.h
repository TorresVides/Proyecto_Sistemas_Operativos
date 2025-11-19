/******************************************
 * Autores:Santiago Henrnandez Morales, Jose Jesus Cepeda Vargas,Andres Eduardo Meneses Rincon,Karol Dayan Torres Vides
 * Fecha:
 * Docente:John Franco Corredor
 * Materia:Sitemas operativos
 * Descripcion:En este programa se declaran las constantes, estructuras y funciones que se van a utilizar en el programa de moduloControlador
 * 
 * 
 * 
 *************************************************************************/


#ifndef MODULO_CONTROLADOR_H //se define el archivo de cabecera
//se deine el archivo para que solo se incluya una vez
#define MODULO_CONTROLADOR_H

//se declaran las bibliotecas que se van a utilizar en el programa
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

//se define la hora minima,hora maxima y la maxima cantidad de reservas
#define HORA_MIN 7
#define HORA_MAX 19
#define MAX_RESERVAS 1000

//se define el tamaño maximo del nombre y del pipe
#define MAX_NOMBRE 32
#define MAX_PIPE   64

//se define el codigo de mensaje para registrar agente,la respuesta al registro de agente,la solicitud de reserva,la respuesta a la solicitud y el mensaje que el agente termino
#define TIPO_REGISTRO 1
#define TIPO_RESP_REGISTRO 2
#define TIPO_SOLICITUD 3
#define TIPO_RESP_SOLICITUD 4
#define TIPO_FIN 5

//se define solicitud aceptada,solicitud reprogramada y solicitud negada
#define RESP_ACEPTADA 0
#define RESP_REPROGRAMADA 1
#define RESP_NEGADA  2


/*Variables globales compartidas*/
extern pthread_mutex_t mutex;//indica que existe un mutex definido en otro archivo
extern int escribir;//representa el indice del pipe que se utiliza para escribir

/*Estructuras*/
typedef struct //se declara una estructura llamada reserva
{
    char familia[32];//nombre de la familia
    int horaInicio;//hora en la que entra
    int horaFin;//hora en la que sale
    int personas;//numero de personas
    int estado; // 0 = reservado, 1 = en el parque, 2 = finalizado
}Reserva;

//estructrua estadisticas del dia
typedef struct //se declara una estructura llamada estadisticas
{
    int ocupacionHoras[25];//se declara un arreglo de 25 posiciones
    int aceptadas;//solicitudes aceptadas
    int reprogramadas;//solicitudes reprogramadas
    int negadas;//solicitudes negadas
    int negadasHora;//solicitudes negadas porque ya paso la hora
}Estadisticas;

//formato de mensaje que manda el agente
typedef struct//se declara una estructura llamada mensaje solicitud
{
    char familia[32];//se declara un arreglo de 32 posiciones llamado familia que representa el nombre de la familia
    int  horaInicio;//representa la hora de inicio de la reserva
    int  personas;//se declara una variable llamada personas que representa el numero de personas
} MensajeSolicitud;

typedef struct//se declara una estructura llamada mensaje
{
    int   tipo; //se declara una variable que representa el tipo de mensaje                            
    char  nombreAgente[MAX_NOMBRE];//se declara un arreglo llamado nombreAgente que representa el nombre del agente
    char  pipeAgente[MAX_PIPE];//se declara un arreglo llama pipe que representa el pipe del agente

    MensajeSolicitud solicitud;// datos de la solicitud
    Reserva          reserva;// datos de la reserva en la respuesta

    int   resultado;//se declara una variable que representa el resultado de la solicitud
    int   horaSimulacion; //hora simulacion
} Mensaje;

typedef struct {//se declara una estructura llamada controlador args
    int fdPipe;//representa el pipe principal que se utiliza para leer las solicitudes
    int *horaActual;//representa un puntero que apunta a la hora actual
    int horaFin;//representa la hora de finalizacion de simulacion
    int aforoMax;//representa el aforo maximo que se permite en el parque
    Reserva *res;//representa un puntero que apunta a Reserva
    int *numReservas;//representa un puntero que apunta a la cantidad de reservas
    Estadisticas *estadisticas;//representa un puntero que apunta a estadisticas
} ControladorArgs;

typedef struct {//se declara una estructura llamada reloj args
    int *horaActual;//representa un puntero que apunta a la hora actual
    int horaFin;//representa la hora de finalizacion de simulacion
    int segHoras;//representa los segundos por hora
    Reserva *res;       // arreglo de reservas
    int *numReservas;   // cantidad de reservas llenas en el arreglo
} RelojArgs;


/*prototipos de las funciones*/
int procesarSolicitud(MensajeSolicitud *msg, int horaActual, int horaFin, int aforoMax, Reserva res[], int *numReservas, Estadisticas *estadisticas);//procesa las solicitudes de reserva y retorna aceptada,negada o reprogramada.
void* hiloReloj(void *arg);//esta funcion se encarga de simular el avance del tiempo
void* hiloAtencion(void *arg);//esta funcion se encarga de procesar las solicitudes de reserva

//endif se utiliza para indicar el fin de la definicion de el codigo que se encuentra dentro de #ifndef
#endif