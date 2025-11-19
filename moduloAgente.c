/***************************
 * Autores:Santiago Henrnandez Morales, Jose Jesus Cepeda Vargas,Andres Eduardo Meneses Rincon,Karol Dayan Torres Vides
 * Fecha:18/11/2025
 * Docente:John Franco Corredor
 * Materia:Ssitemas opeaivos
 * Descripcion:Este codigo contiene la implementacion de las funciones de moduloAgente.h
 * 
 * 
 * 
 */

//se declara el archivo de cabecera
#include "moduloAgente.h"

void uso(const char *prog)//se declara la funcion uso la cual recibe un puntero de tipo cahr
{
    fprintf(stderr,"Uso: %s -s nombreAgente -a fileSolicitud -p pipeRecibe\n",prog);//se indica el uso correcto del programa
    exit(EXIT_FAILURE);//se sale del programa
}


/* Parseo de argumentos de linea de comandos */
void parse_args(int argc, char *argv[],char *nombreAgente,char *fileSolicitud,char *pipeRecibe)//se declara la funcion parse_args la cual recibe un entero,un arreglo de punteros de tipo char.
{
    int opt;//se declara la variable opt de tipo int
    int s_ok = 0, a_ok = 0, p_ok = 0;//se declaran tres variable int incializadas en 0

    while ((opt = getopt(argc, argv, "s:a:p:")) != -1)//mientras que opt sea diferente a -1 entoces se ejecutara lo siguiente
    {
        switch (opt)//opt es una variable que se utiliza para almacenar el valor de la opcion que se ingresa en la terminal
        {
            case 's'://en caso que sea s se ejecuta lo siguiente
                strncpy(nombreAgente, optarg, MAX_NOMBRE - 1);//se copia el nombre del agente a la variable nombreAgente
                nombreAgente[MAX_NOMBRE - 1] = '\0';//se asigna el valor de nulo a la variable nombreAgente
                s_ok = 1;//se asigna el valor de 1 a la variable s_ok
                break;//se rompe el switch
            case 'a'://en caso que sea a se ejecuta lo siguiente
                strncpy(fileSolicitud, optarg, 255);//se copia el archivo de solicitud a la variable fileSolicitud
                fileSolicitud[255] = '\0';//se asigna el valor vacio a la variable fileSolicitud
                a_ok = 1;//se asigna a la variable a_ok el valor de 1
                break;//se rompe el switch
            case 'p'://se verifica si la opcion es p si es asi se ejecuta lo siguiente
                strncpy(pipeRecibe, optarg, MAX_PIPE - 1);//se copia el pipe que recibe a la variable pipeRecibe
                pipeRecibe[MAX_PIPE - 1] = '\0';//se asigna el valor de vacio a la variablepipeRecibe
                p_ok = 1;//se asigna a p_ok el valor de 1
                break;//se rompe el switch
            default://si no se cumple ninguna de las anteriores condiciones entonces se ejecuta lo siguiente
                uso(argv[0]);//se ejecuta la funcion uso con argv[0] como argumento
        }
    }

    if (!s_ok || !a_ok || !p_ok)//si las tres variables declaradas anteriormente tienen valores diferentes de 0 entonces se ejecuta lo siguiente
    {
        uso(argv[0]);//se ejecuta la funcion uso con el argumento argv[0]
    }
}