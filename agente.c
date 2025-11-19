/**********************************************************************************************
 * Autores:Santiago Henrnandez Morales, Jose Jesus Cepeda Vargas,Andres Eduardo Meneses Rincon,Karol Dayan Torres Vides
 * Fecha:18/11/2025
 * Docente:John Franco Corredor
 * Materia: Sistemas Operativos
 * Descripcion:en los siguientes renglones
 *  Cliente (Agente de Reserva) que se conecta al Controlador de
 *  reservas. El agente:
 *    - Se ejecuta con: ./agente -s nombreAgente -a fileSolicitud -p pipeRecibe
 *    - Se registra con el controlador enviando su nombre y el pipe
 *      donde va a recibir las respuestas.
 *    - Recibe la hora actual de simulacion.
 *    - Lee el archivo CSV (hoja de calculo) de solicitudes, valida cada una y envia
 *      solicitudes de reserva.
 *    - Espera respuestas (aceptada / reprogramada / negada) e imprime
 *      el resultado.
 *    - Espera 2 segundos entre las solicitudes.
 *    - Al terminar el archivo, avisa al controlador que el agente
 *      termina.
 *****************************************************************************************************/

//se declara el archivo de cabecera
#include "moduloAgente.h"

int main(int argc, char *argv[])
{//se declara la funcion principal la cual va a ejecutar el programa
    char nombreAgente[MAX_NOMBRE];//representa el nombre del agente
    char fileSolicitud[256];//representa el archivo de solicitudes
    char pipeRecibe[MAX_PIPE];//representa el pipe del controlador
    char pipeAgente[MAX_PIPE];

    int  fdCtrl = -1;   // Pipe hacia el controlador (escritura) 
    int  fdResp = -1;   // Pipe propio del agente (lectura)     

    FILE *fsol = NULL;//se inicializa la variable fsol a un valor nulo

    Mensaje msg;//se declara la variable msg de tipo mensaje
    Mensaje resp;// se declara la variable resp de tipo mensaje

    int horaActualSim = 0;  //se inicializa la variable hora actual de simulacion a 0


    parse_args(argc, argv, nombreAgente, fileSolicitud, pipeRecibe);//se llama la funcion parse_args la cual recibe los argumentos que se van a utilizar en el programa

    printf(" Agente de Reserva \n");//se imprime el nombre del programa
    printf("Nombre del agente     : %s\n", nombreAgente);//se imprime el nombre del agente
    printf("Archivo de solicitudes: %s\n", fileSolicitud);//se imprime el archivo de solicitudes
    printf("Pipe hacia controlador: %s\n\n", pipeRecibe);//se imprime el pipe hacia el controlador

    //se construye el nombre del pipe
    snprintf(pipeAgente, sizeof(pipeAgente), "pipeResp_%s", nombreAgente);

    //se crea el pipe del agente en caso de que no exista
    if (mkfifo(pipeAgente, 0666) == -1)//si mkfifo es igual a -1 se ejecuta lo siguiente
    {
        if (errno != EEXIST)//si errno es diferente de EEXIST se ejecuta lo siguiente
        {
            perror("Error al crear pipe del agente (mkfifo)");//se imprime un error al intentar crear el pipe del agente
            exit(EXIT_FAILURE);//se sale del programa
        }
    }

    //se abre el pipe hacia el controlador
    fdCtrl = open(pipeRecibe, O_WRONLY);//se abre el pipe hacia el controlador
    if (fdCtrl == -1)//si fdCtrl es igual a-1 se ejecuta lo siguiente
    {
        perror("Error al abrir pipeRecibe (O_WRONLY)");//se imprime un error al intentar abrir el pipe hacia el controlador
        unlink(pipeAgente);//se elimina el pipe del agente
        exit(EXIT_FAILURE);//se sale del programa
    }

    /* Abrir pipe del agente (lectura de respuestas del controlador) */
    fdResp = open(pipeAgente, O_RDWR);//se abre el pipe del agente
    if (fdResp == -1)//se verifica si fdResp es igual a -1 si es asi se ejecuta lo siguiente
    {
        perror("Error al abrir pipeAgente (O_RDWR)");//se imprime un error al intentar abrir el pipe del agente
        close(fdCtrl);//se cierra el pipe hacia el controlador
        unlink(pipeAgente);//se elimina el pipe del agente
        exit(EXIT_FAILURE);//se sale del programa
    }

    //invocacion y registro usando el controlador

    memset(&msg, 0, sizeof(Mensaje));//se inicializa la variable msg a 0
    msg.tipo = TIPO_REGISTRO;//se asigna a msg.tipo el valor de tipo registro
    strncpy(msg.nombreAgente, nombreAgente, MAX_NOMBRE - 1);//se copia el nombre del agente a la variable msg.nombreAgente
    strncpy(msg.pipeAgente,  pipeAgente,  MAX_PIPE   - 1);//se copia el pipe del agente a la variable msg.pipeAgente

    /* Enviar mensaje de registro */
    if (write(fdCtrl, &msg, sizeof(Mensaje)) != sizeof(Mensaje))//se verifica si write es diferente a sizeof(mensaje) si es asi se ejecuta lo siguiente
    {
        perror("Error al enviar mensaje de registro");//se imprime un error al intentar enviar el mensaje de registro
        close(fdCtrl);//se cierra el pipe hacia el controlador
        close(fdResp);//se cierra el pipe del agente
        unlink(pipeAgente);//se elimina el pipe del agente
        exit(EXIT_FAILURE);//se sale del programa
    }


    if (read(fdResp, &resp, sizeof(Mensaje)) != sizeof(Mensaje))//se verifica si read es diferente a sizeof(mensaje) si es asi se ejecuta lo siguiente
    {
        perror("Error al leer respuesta de registro");//se imprime un error al intentar leer la respuesta de registro
        close(fdCtrl);//se cierra el pipe hacia el controlador
        close(fdResp);//se cierra el pipe del agente
        unlink(pipeAgente);//se elimina el pipe del agente
        exit(EXIT_FAILURE);//se sale del programa
    }

    if (resp.tipo != TIPO_RESP_REGISTRO)// si el tipo de la respuesta es diferente a tipo de la respuesta del registro entonces se ejecuta lo siguiente
    {
        fprintf(stderr,"[Agente %s] Respuesta inesperada al registro (tipo=%d)\n",nombreAgente, resp.tipo);//se imprime un error al intentar leer la respuesta de registro
        close(fdCtrl);//se cierra el pipe hacie el controlador
        close(fdResp);//se cierra el pipe del agente
        unlink(pipeAgente);//se elimina el pipe del agente
        exit(EXIT_FAILURE);//se sale del programa
    }

    horaActualSim = resp.horaSimulacion;//se asigna el valor de la hora de simulacion a la variable horaActualSim
    printf("[Agente %s] Registrado correctamente. Hora actual de simulacion: %d\n\n",//se imprime el nombre del agente y la hora actual de simulacion
           nombreAgente, horaActualSim);

    /* Lectura del archivo y solicitudes de reserva*/

    fsol = fopen(fileSolicitud, "r");//se abre el archivo de solicitudes
    if (!fsol)//se verifica si fsol es nulo si es asi se ejecuta lo siguiente
    {
        perror("Error al abrir fileSolicitud");//se imprime un error al intentar abrir el archivo de soliticudes
        close(fdCtrl);//se cierra el pipe hacia controlador
        close(fdResp);//se cierra el pipe del agente
        unlink(pipeAgente);//se elimina el pipe del agente
        exit(EXIT_FAILURE);//se sale del programa
    }

    char linea[256];//se declara un arreglo de 256 posiciones llamado linea
    char familia[32];//se declara un arreglo de 32 posiciones llamado familia
    int  hora;//se declara una variable de tipo int con nombre hora
    int  personas;//se declara una variable de tipo int con nombre personas

    while (fgets(linea, sizeof(linea), fsol))//mientras fgets sea diferente de nulo se ejecuta lo siguiente
    {
       //se saltan las lineas vacias o comentarios
        if (linea[0] == '\n' || linea[0] == '\r' || linea[0] == '#')//si la linea es igual a \n o \r o # se ejecuta lo siguiente
            continue;//se continua

        /* Formato esperado:Familia,hora,personas */
        if (sscanf(linea, " %31[^,],%d,%d", familia, &hora, &personas) != 3) //si scanf es diferente a 3 se ejecuta lo siguiente
        {
            fprintf(stderr,"[Agente %s] Linea invalida en %s: %s",nombreAgente, fileSolicitud, linea);//se imprime un error al intentar leer la linea
            continue;//se continua
        }

        /* Validar rango de la hora de reserva */
        if (hora < HORA_MIN || hora > HORA_MAX)//se verifica si la hora es menor a la hora minima o la hora myor a la hora maxima si es asi se ejecuta lo siguiente
        {
            fprintf(stderr,"[Agente %s] Hora %d fuera del rango [%d,%d]. Se ignora la solicitud de la familia %s.\n",nombreAgente, hora, HORA_MIN, HORA_MAX, familia);//se imprime un error al intentar leer la hora
            continue;//se continua
        }

        //se valida que la hora se menor a la hora actual de simulacion
        if (hora < horaActualSim)
        {
            printf("[Agente %s] Solicitud EX-TEMPORANEA para familia %s a la hora %d (hora actual de simulacion: %d). NO se envia al controlador.\n",nombreAgente, familia, hora, horaActualSim);//se imprime un error al intentar leer la hora
            continue;//se continua
        }

        if (personas <= 0)//si la cantidad de personas es menor o igual a 0 entonces se ejecuta lo siguiente
        {
            fprintf(stderr,"[Agente %s] Numero de personas no valido (%d) para familia %s. Se ignora.\n",nombreAgente, personas, familia); //se imprime un error al intentar leer la cantidad de personas
            continue;//se continua
        }

        //se envia la solicitud y se espera respuesta

        memset(&msg, 0, sizeof(Mensaje));//se inicializa la variable msg a 0
        msg.tipo = TIPO_SOLICITUD;//se asigna a msg.tipo el valor del tipo de solicitud
        strncpy(msg.nombreAgente, nombreAgente, MAX_NOMBRE - 1);//se copia el nombre del agente a la variable msg.nombreAgente
        strncpy(msg.pipeAgente,  pipeAgente,  MAX_PIPE   - 1);//se copia el pipe del agente a la variable msg.pipeAgente

        strncpy(msg.solicitud.familia, familia, sizeof(msg.solicitud.familia) - 1);//se copia la familia a la variable msg.solicitud.familia
        msg.solicitud.horaInicio = hora;//se asigna a msg.solicitud.horaInicio el valor de la hora
        msg.solicitud.personas   = personas;//se asigna a msg.solicitud.personas el valor de la cantidad de personas

        if (write(fdCtrl, &msg, sizeof(Mensaje)) != sizeof(Mensaje))//si write es diferente a sizeof(mensaje) se ejecuta lo siguiente 
        {
            perror("[Agente] Error al escribir solicitud en pipeRecibe");//se genera un error al intentar escribir la solicitud en el pipeRecibe
            break;//se rompe el ciclo
        }

        printf("[Agente %s] Solicitud enviada: familia=%s, hora=%d, personas=%d\n",nombreAgente, msg.solicitud.familia,msg.solicitud.horaInicio, msg.solicitud.personas);//se imprime que la solicitud fue enviada

        //se espera la respuesta del controlador
        if (read(fdResp, &resp, sizeof(Mensaje)) != sizeof(Mensaje))//se verifica si read es diferente a sizeof(mensaje) si es asi se ejecuta lo siguiente
        {
            perror("[Agente] Error al leer respuesta de solicitud");//se imprime un error al intentar leer la respuesta a la solicitud
            break;//se rompe el ciclo
        }

        if (resp.tipo != TIPO_RESP_SOLICITUD)//si el tipo de respuesta es diferente a tipo de la respuesta de la solicitud entonces se ejecuta lo siguiente
        {
            fprintf(stderr,"[Agente %s] Mensaje inesperado recibido (tipo=%d) al esperar respuesta de solicitud.\n",nombreAgente, resp.tipo);//se imprime un error al intentar leer la respuesta de la solicitud
            break;//se rompe el ciclo
        }

        if (resp.horaSimulacion >= 0)//si la hora de simulacion es mayor o igual a 0 entonces se ejecuta lo siguiente
        {
            horaActualSim = resp.horaSimulacion;//se asigna el valor de la hora de simulacion a la variable horaActualSim
        }

        //se imprime si fue aceptada,negada o reprogramada

        if (resp.resultado == RESP_ACEPTADA)//si la respuesta es igual a respuesta aceptada entonces se ejecuta lo siguiente
        {
            printf("[Agente %s] RESERVA ACEPTADA para familia %s. ""Hora inicio: %d, hora fin: %d. Personas: %d\n",nombreAgente,resp.reserva.familia,resp.reserva.horaInicio,resp.reserva.horaFin,resp.reserva.personas);//se imprime que la reserva fue aceptada
        }
        else if (resp.resultado == RESP_REPROGRAMADA)//si la respuesta es igual a respuesta reprogramada entonces se ejecuta lo siguiente
        {
            printf("[Agente %s] RESERVA REPROGRAMADA para familia %s. Nueva hora inicio: %d, nueva hora fin: %d. Personas: %d\n",nombreAgente,resp.reserva.familia,resp.reserva.horaInicio,resp.reserva.horaFin,resp.reserva.personas);//se imprime que la reserva fue reprogramada
        }
        else if (resp.resultado == RESP_NEGADA)//si la respuesta es igual a respuesta negada entonces se ejecuta lo siguiene
        {
            printf("[Agente %s] RESERVA NEGADA para familia %s a la hora %d (personas=%d).\n",nombreAgente,familia,hora,personas);//se imprime que la reserva fue negada
        }
        else //de lo contrario se ejecuta lo siguiente
        {
            printf("[Agente %s] Respuesta desconocida para familia %s.\n",nombreAgente, familia);//se imprime que la respuesta es deconocida para la familia
        }

        //se espera dos segundos antes de enviar la siguiente solicitud
        sleep(2);
    }

    printf("\n[Agente %s] Fin de archivo de solicitudes.\n", nombreAgente);//se imprime que el archivo de solicitudes termino

    //se envia mensaje de fin controlador
    memset(&msg, 0, sizeof(Mensaje));//se inicializa la variable msg a 0
    msg.tipo = TIPO_FIN;//se asigna a msg.tipo el valor de tipo fin
    strncpy(msg.nombreAgente, nombreAgente, MAX_NOMBRE - 1);//se copia el nombre del agente a la variable msg.nombreAgente
    strncpy(msg.pipeAgente,  pipeAgente,  MAX_PIPE   - 1);//se copia el pipe del agente a la variable msg.pipeAgente

    if (write(fdCtrl, &msg, sizeof(Mensaje)) != sizeof(Mensaje))//si write es diferente a sizeof(mensaje) entonces se ejecuta lo siguiente
    {
        perror("[Agente] Error al enviar mensaje de finalizacion");//se imprime un error al intentar enviar el mensaje de finalizacion

    }

    printf("Agente %s termina.\n", nombreAgente);//se imprime que el agente termino

    //se cierran los diferentes recursos utilizados en el programa
    fclose(fsol);//se cierra el archivo de solicitudes
    close(fdCtrl);//se cierra el pipe hacia el controlador
    close(fdResp);//se cierra el pipe del agente

    //se elimina el pipe
    if (unlink(pipeAgente) == -1)//si unlink es igual a -1 entonces se ejecuta lo siguiente
    {

        perror("Advertencia: error al eliminar pipeAgente (unlink)");//se imprime un error al intentar eliminar el pipe del agente
    }

    return 0;//se retorna 0 para indicar que el programa se ejecuto correctamente
}