/*******************************************************************************************************************
 * Autores:Santiago Henrnandez Morales, Jose Jesus Cepeda Vargas,Andres Eduardo Meneses Rincon,Karol Dayan Torres Vides
 * Fecha:18/11/2025
 * Docente:John Franco Corredor
 * Materia:Sitemas operativos
 * Descripcion:Este programa contiene la implementacion de las funciones de moduloControlador.h 
 * 
 * 
 * 
 *****************************************************************************************************/
//se declara el archivo de cabecera
#include "moduloControlador.h"

/*definicion de las variables globales*/
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int escribir = -1;//se inicializa la variable escribir a -1

/*logica de las funciones*/

void* hiloReloj(void *arg) {//se declara la funcion hiloReloj la cual recibe un puntero de tipo void
    RelojArgs *p = (RelojArgs*) arg;//se declara la variable p de tipo RelojArgs y se le asigna el valor de arg

    int *horaActual  = p->horaActual;//se declara la variable horaActual de tipo int y se le asigna el valor de p->horaActual
    int horaFin      = p->horaFin;//se declara la variable horaFin de tipo int y se le asigna el valor de p->horaFin
    int segHoras     = p->segHoras;//se declara la variable segHoras de tipo int y se le asigna el valor de p->segHoras
    Reserva *res     = p->res;//se declara la variable res de tipo int y se le asigna el valor de p->res
    int *numReservas = p->numReservas;//se declara la variable numReservas de tipo int y se le asigna el valor de p->numReservas

    while (*horaActual < horaFin) {//mientras la hora actual sea menor a la hora fin se seguira ehecutando el ciclo while
        sleep(segHoras);//se espera la cantidad de segundos que se le asigno a segHoras

        pthread_mutex_lock(&mutex);//se bloquea el mutex

        (*horaActual)++;//se incrementa la hora actual en uno
        int h = *horaActual;//se declara la variable h la cual se le asigna el valor de la hora actual

        printf("Avanza la hora %d\n", h);//se imprime la hora actual

        //muestra quien entra en esa hora
        int hayEntradas = 0;//se inicializa la variable hay entradas a 0
        printf("  Entradas en la hora %d:\n", h);//se imprime la hora actual
        for (int i = 0; i < *numReservas; i++) {//se genera un ciclo for el cual termina cuando i es igual a numReservas
            if (res[i].horaInicio == h) {//si la hora de inicio es igual a la hora actual entonces se ejecuta lo siguiente
                printf("    Entra familia %s (%d personas)\n",res[i].familia, res[i].personas);//se imprime la familia y la cantidad de peronas que entran al parque
                hayEntradas = 1;//se asign el valor de 1 a la variable hayEntradas.
            }
        }
        if (!hayEntradas) {//se verifica si hay entradas
            printf("    Ninguna familia entra\n");//se imprime que ninguna familia entra
        }

        //muestra quien sale en esa hora
        int haySalidas = 0;//se inicializa la variable hay salidas a 0
        printf("  Salidas en la hora %d:\n", h);//se imprime la hora actual
        for (int i = 0; i < *numReservas; i++) {//se genera un ciclo for el cual termina cuando i es igual a numReservas.
            if (res[i].horaFin == h) {//si la hora fin es igual a la hora actual entonces se ejecuta lo siguiente
                printf("    Sale familia %s (%d personas)\n",res[i].familia, res[i].personas);//se imprime la familia y la cantidad de personas que salen del parque
                haySalidas = 1;//se asigna el valor de 1 a la variable haySalidas
            }
        }
        if (!haySalidas) {//si no hay salidas entonces se ejecuta lo siguiente
            printf("    Ninguna familia sale\n");//se indica que ninguna familia sale
        }




        pthread_mutex_unlock(&mutex);//se desbloquea el mutex
    }

    pthread_mutex_lock(&mutex);//se bloquea el mutes
    if (escribir != -1)//si escribir es diferente de -1 entonces se ejecuta lo siguiente
    {
        close(escribir);//se cierra el pipe
        escribir = -1;//se asigna el valor de -1 a la variable escribir

    }
    pthread_mutex_unlock(&mutex);//se desbloquea el mutex
    return NULL;//se retorna NULL
}






void* hiloAtencion(void *arg) {//se declara la funcion hiloAtencion la cual recibe un puntero de tipo Void
    ControladorArgs *p = (ControladorArgs*) arg;//se declara la variable p de tipo ControladorArgs y se le asigna el valor de arg

    int fdPipe      = p->fdPipe;//se declara la variable fdPipe de tipo int y se le asigna el valor de p->fdPipe
    int horaFin     = p->horaFin;//se declara la variable horaFin de tipo int y se le asigna el valor de p->horaFin
    int aforoMax    = p->aforoMax;//se declara la variable aforoMax de tipo int y se le asigna el valor de p->aforoMax
    int *horaActual = p->horaActual;//se declara la variable horaActual de tipo int y se le asigna el valor de p->horaActual
    Reserva *res    = p->res;//se declara la variable res de tipo Reserva y se le asigna el valor de p->res
    int *numReservas = p->numReservas;//se declara la variable numReservas de tipo int y se le asigna el valor de p->numReservas
    Estadisticas *estadisticas = p->estadisticas;//se declara la variable estadisticas de tipo Estadisticas y se le asigna el valor de p->estadisticas

    Mensaje msg;      // leer mensaje completo
    Mensaje resp;     // para responder al agente
    int leidos;

    while ((leidos = read(fdPipe, &msg, sizeof(Mensaje))) > 0) {//se genera un ciclo while el cual termina cuando leidos es mayor a 0

        //caso 1: registro de un agente
        if (msg.tipo == TIPO_REGISTRO) {//se verifica si el tipo de mensaje es igual a tipo del registro
            printf("[Controlador] Registro de agente: %s, pipeAgente=%s\n",msg.nombreAgente, msg.pipeAgente);//se imprime el nombre del agente y el pipe del agente

            memset(&resp, 0, sizeof(Mensaje));//se inicializa la variable resp a 0
            resp.tipo = TIPO_RESP_REGISTRO;//se asigna el valor de tipo de la respuesta del registro a la variable resp.tipo
            strncpy(resp.nombreAgente, msg.nombreAgente, MAX_NOMBRE - 1);//se copia el nombre del agente a la variable resp.nombreAgente
            strncpy(resp.pipeAgente,  msg.pipeAgente,  MAX_PIPE - 1);//se copia el pipe del agente a la variable resp.pipeAgente
            resp.horaSimulacion = *horaActual;//se asigna el valor de la hora actual a la variable resp.horaSimulacion

            // abrir pipe parea responder al agente
            int fdResp = open(msg.pipeAgente, O_WRONLY);//se abre el pipe para responder al agente
            if (fdResp == -1) {//se verifica si el pipe es igual a -1
                perror("[Controlador] Error al abrir pipeAgente para resp_registro");//se imprime el error al abrir el pipe del agente para la respuesta del registro
            } else {//de lo contrario se ejecuta lo siguiente
                write(fdResp, &resp, sizeof(Mensaje));//se escribe la respuesta en el pipe del agente
                close(fdResp);//se cierra el pipe del agente
            }
        }
        //caso 2:solicitud de reserva
        else if (msg.tipo == TIPO_SOLICITUD) {//se verifica si el tipo de mensaje es igual a tipo de la solicitud
            printf("[Controlador] Solicitud de %s: familia=%s, hora=%d, personas=%d\n",msg.nombreAgente,msg.solicitud.familia,msg.solicitud.horaInicio,msg.solicitud.personas);//se omprime el nombre del agente,la hora, la familia y la cantidad de personas

            pthread_mutex_lock(&mutex);//se bloquea el mutex
            int r = procesarSolicitud(&msg.solicitud,*horaActual, horaFin, aforoMax,res, numReservas, estadisticas);//se declara la variable r de tipo int y se le asigna el valor de procesarSolicitud
            pthread_mutex_unlock(&mutex);//se desbloquea el mutex

            memset(&resp, 0, sizeof(Mensaje));//se inicializa la variable resp a 0
            resp.tipo = TIPO_RESP_SOLICITUD;//se asigna el valor de tipo de la respuesta de la solicitud a la variable resp.tipo
            strncpy(resp.nombreAgente, msg.nombreAgente, MAX_NOMBRE - 1);//se copia el nombre del agente a la variable resp.nombreAgente
            strncpy(resp.pipeAgente,  msg.pipeAgente,  MAX_PIPE - 1);//se copia el pipe del agente a la variable resp.pipeAgente
            resp.horaSimulacion = *horaActual;//se asigna el valor de la hora actual a la variable resp.horaSimulacion

            if (r == 1) {//se verifica si r equivale a 1 si es asi se ejeuta lo siguiente
                resp.resultado = RESP_ACEPTADA;//se asigna el valor de respuesta aceptada a la variable resp.resultado

                resp.reserva = res[*numReservas - 1];//se asigna el valor de la reserva a la variable resp.reserva
                printf("[Controlador] aceptada\n");//se imprime que la reserva fue aceptada
            } else if (r == 2) {//se verifica si r equivale a 2 si es asi se ejecuta lo siguiente
                resp.resultado = RESP_REPROGRAMADA;//se asigna el valor de respuesta reprogramada a la variable resp.resultado
                resp.reserva = res[*numReservas - 1];//se asigna el valor de la reserva a la variable resp.reserva
                printf("[Controlador] reprogramada\n");//se imprime que la reserva fue reprogramada
            } else {//de lo contrario se ejecuta lo siguiente
                resp.resultado = RESP_NEGADA;//se asigna el valor de respuesta negada a la variable resp.resultado
                strncpy(resp.reserva.familia, msg.solicitud.familia,sizeof(resp.reserva.familia) - 1);//se copia la familia a la variable resp.reserva.familia
                resp.reserva.horaInicio = msg.solicitud.horaInicio;//se asigna el valor de la hora de inicio a la variable resp.reserva.horaInicio
                resp.reserva.personas   = msg.solicitud.personas;//se asigna el valor de la cantidad de personas a la variable resp.reserva.personas
                printf("[Controlador] negada\n");//se imprime que la reserva fue negada
            }

            //enviar la respuesta al agente
            int fdResp = open(msg.pipeAgente, O_WRONLY);
            if (fdResp == -1) {//si el pipe es igual a -1 se ejecuta lo siguiente
                perror("[Controlador] Error al abrir pipeAgente para RESP_SOLICITUD");//se imprime un error
            } else {//de lo contrario se ejecuta lo siguiente
                write(fdResp, &resp, sizeof(Mensaje));//se escribe la respuesta en el pipe del agente
                close(fdResp);//se cierra el pipe del agente
            }
        }
        // Caso 3: agente avisa que termino
        else if (msg.tipo == TIPO_FIN) {//se verifica si el tipo de mensaje es igual a tipo fin
            printf("[Controlador]Agente %s termino. PipeAgente=%s\n",msg.nombreAgente, msg.pipeAgente);//se imprime el nombre del agente y el pipe del agente

        }
        else {//si no se cumple ninguna de las anteriores condiciones s ejecuta lo siguiente
            printf("[Controlador] Mensaje no aceptado: %d\n", msg.tipo);//se imprime el mensaje de no aceptado
        }
    }

    return NULL;//se retorna un nulo
}





int procesarSolicitud(MensajeSolicitud *msg, int horaActual, int horaFin, int aforoMax, Reserva res[], int *numReservas, Estadisticas *estadisticas){//se declara la funcion procesar solicitud

    int horaSolicitada = msg->horaInicio;//se declara la variable hora solititada y se le asigna el valor de msg->horaInicio
    int personas = msg->personas; //se declara la variable personas y se le asigna el valor de msg->personas

    //verificar cantidad de reservas
    if (*numReservas >= MAX_RESERVAS)//se verifica si el numero de reservas es mayor o igual a la maxima cantidad de reservas
    {
        printf("Cantidad de reservas alcanzo el maximo\n");//se imprime que la cantidad de reservas alcanzo el maximo
        estadisticas->negadas++;//se incrementa la cantidad de reservas negadas en uno
        return 0;//se retorna 0
    }

    //verificar aforo
    if (personas > aforoMax)//si la cantidad de personas es mayor al aforo maximo se ejecuta lo siguiente
    {
        printf("Solicitud negada debido a que no hay espacio (%d) para el total de personas en la reserva (%d) \n", personas, aforoMax);//se indica que la solicitud fue negada debido a que no hay espacio
        estadisticas->negadas++;//se incrementa la cantidad de reservas negadas en uno
        return 0;//se retorna 0
    }


    //si hay cupo en la hora pedida y la siguiente
    if (horaSolicitada + 1 > horaFin)//si la hora solicitada mas uno es mayor que la hora fin entonces se ejecuta lo siguiente
    {
        printf("No es posible agregarlo a la reserva porque el parque cierra: %d y esta ingresando: %d \n", HORA_MAX, horaSolicitada);//se indica que no es posible agregarlo a la reserva porque el parque cierra
        estadisticas->negadas++;//se incrementa la cantidad de reservas negadas en uno
        return 0;//se retorna 0
    }

    //revisar si en esa hora y la sigueinte hay hoera todavia hay aforo en el parque 
    if (horaSolicitada >= horaActual && estadisticas->ocupacionHoras[horaSolicitada] + personas <= aforoMax && estadisticas->ocupacionHoras[horaSolicitada + 1] + personas <= aforoMax)//si la hora solicitada es mayor igual a la hora actual y la ocupacion de la hora solicitada mas la cantidad de personas es menor o igual al aforo maximo y la ocupacion de la hora solicitada mas uno mas la cantidad de personas es menor o igual al aforo maximo se ejecuta lo siguiente
    {
        //actualizar ocupacion
        estadisticas->ocupacionHoras[horaSolicitada] += personas;//se incrementa la ocupacion de la hora solicitada en la cantidad de personas
        estadisticas->ocupacionHoras[horaSolicitada + 1] += personas;//se incrementa la ocupacion de la hora solicitada mas uno en la cantidad de personas
        //crear la reserva
        Reserva *r = &res[*numReservas];//se declara la variable r de tipo reserva y se le asigna el valor de res[numReservas]
        //asignar valores
        strcpy(r->familia, msg->familia);//se copia la familia a la variable r->familia
        r->horaInicio = horaSolicitada;//se asigna el valor de la hora solicitada a la variable r->horaInicio
        r->horaFin = horaSolicitada + 2;//se asigna el valor de la hora solicitada mas dos a la variable r->horaFin
        r->personas = msg->personas;//se asgina el valor de la cantidad de personas a la variable r->personas
        r->estado = 0; //reservado
        //sumar uno a numreseras
        (*numReservas)++;//se incrementa la cantidad de reservas en uno
        //sumar uno a reservas aceptadas
        estadisticas->aceptadas++;//se incrementa la cantidad de reservas aceptadas en uno

        printf("Reserva aceptada: familia: %s, hora inicio %d, hora fin %d \n", r->familia, r->horaInicio, r->horaFin);//se imprime que la reserva fue aceptada
        return 1;//se retorna 1
    }else{//de lo contrario se ejecuta lo siguiente
        //si no hay cupo en la hora solcitada buscar otras

        int inicioBusqueda;//se declara la variable inicioBusqueda
        //si la hora pedida ya paso 
        if (horaSolicitada < horaActual)//se verifica si la hora solicitada es menor a la hora actual
        {
            inicioBusqueda = horaActual;//se asigna el valor de la hora actual a la variable inicioBusqueda
        }else{//se lo contrario se ejecuta lo siguiente
            inicioBusqueda = horaSolicitada;//se le asigna el valor de la hora solicitada a la variable inicioBusqueda
        }

        int h;//se declara la variable h
        int encontrado = 0;//se inicializa la variable encontrado a 0
        //buscar 2 horas con cupo entre inicioBusqueda y horaFin - 1
        for (h = inicioBusqueda; h <= horaFin - 1; h++)//se genera un ciclo for el cual termina cuando h es igual a horaFin -1
        {
            if (estadisticas->ocupacionHoras[h] + personas <= aforoMax && estadisticas->ocupacionHoras[h + 1] + personas <= aforoMax)//se verifica si la ocupacion de la hora h mas la cantidad de personas es menor o igual al aforo maximo y la ocupacion de la hora h mas uno mas la cantidad de personas es menor o igual al aforo maximo
            {
                encontrado = 1;//se asigna el valor de 1 a la variable encontrado
                break;//se rompe el ciclo
            }

        }

        if (encontrado)//si se encontro se ejecuta lo siguiente
        {
            estadisticas->ocupacionHoras[h]     += personas;//se incrementa la ocupacion de la hora h en la cantidad de personas
            estadisticas->ocupacionHoras[h + 1] += personas;//se incrementa la ocupacion de la hora h mas uno en la cantidad de personas

            //crear la reserva y asignar vsalores
            Reserva *r = &res[*numReservas];//se declara la variable r de tipo reserva la cual se le asigna el valor de res[numReservas]
            strcpy(r->familia, msg->familia);//se copia la familia a la variable r->familia
            r->horaInicio = h;//se asigna el valor de h a la variable r->horaInicio
            r->horaFin    = h + 2;//se asigna el valor de h mas dos a la variable r->horaFin
            r->personas   = msg->personas;//se asigna el valor de la cantidad de personas a la variable r->personas
            r->estado     = 0; //reservado

            (*numReservas)++;//se incrementa la cantidad de reservas en uno
            estadisticas->reprogramadas++;//se incrementa la cantidad de reservas reprogramadas en uno

            if (horaSolicitada < horaActual)
            {
                printf("Reserva programada por extemporanea: familia: %s, hora solicitada: %d, hora inicio: %d, hora fin: %d \n", r->familia, horaSolicitada, r->horaInicio, r->horaFin);//se indica que la reserva fue programada por extemporanea
            }else{
                printf("Reserva reprogramada: familia: %s, hora solicitada: %d, hora inicio: %d, hora fin: %d \n", r->familia, horaSolicitada, r->horaInicio, r->horaFin);//se indica que la reserva fue reprogramada

            }
            return 2;//se retorna 2
        }else{//se lo contrario se ejecuta lo siguiente
            //no se encontro ningun hueco libre de 2 horas
            if (horaSolicitada < horaActual)//se verifica si la hora solicitada es menor a la hora actual
            {
                printf("Rserva negada por extemporanea: hora pedida %d, actual %d, sin huecos libres\n", horaSolicitada, horaActual);//se indica que la reserva fue negada por extemporanea
                estadisticas->negadasHora++;//se incrementa la cantidad de reservas negadas por hora en uno
            }else{//de lo contrario se ejecuta lo siguiente
                printf("Reserva negada: no hay 2 bloques disponibles en el dia \n");//se indica que la reserva fue negada
                estadisticas->negadas++;//se incrementa la cantidad de reservas negadas en uno
            }
            return 0;//se retorna 0
        }   
    }


}