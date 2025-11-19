/*******************************************
 * Autores:Santiago Henrnandez Morales, Jose Jesus Cepeda Vargas,Andres Eduardo Meneses Rincon,Karol Dayan Torres Vides
 * Fecha:18/11/2025
 * Docente:John Franco Corredor
 * Materia:Sitemas operativos
 * Descripcion:Este programa se encarga de Recibe las solicitudes de los agentes de reserva y las autoriza o rechaza dependiendo de la cantidad de personas que ya han reservado en las     *horas solicitadas.
 * 
 * 
 * 
 *****************************************/

//Se declara el archivo de cabecera
#include "moduloControlador.h"

int main(int argc, char *argv[]){//se declara la funcion principal la cual va a ejecutar el programa
    //verificar cantidad de parametros correctos
    if (argc != 11)//si el numero de argumentos es diferente de 11 entonces se ejecuta lo siguiente
    {
        printf("Cantidad de parametos insuficientes\n");//se indica que la cantidad de parametros es insuficiente
        printf("/ejecutable -i(hora inicio) -f(hora fin) -s(seg horas) -t(total) -p(pipe recibe)");//se indica el formato correcto de los parametros
        exit(0);//se sale del programa
    }

    //i hora inicio
    int horaInicio = atoi(argv[2]);
    //f hora fin
    int horaFin = atoi(argv[4]);
    //s seg Horas
    int segHoras = atoi(argv[6]);
    //t total (aforo total)
    int aforo = atoi(argv[8]);
    //nombre del pipe
    char *nomPipe = argv[10];

    //verificar hora inicio
    if (horaInicio < HORA_MIN || horaInicio > HORA_MAX)//si la hora de inicio es menor a la hora minima o la hora de inicio mayor a la hora maxima
    {
        printf("Hora inicio no valida, rango (7 <= i <= 19)\n");//se indica que la hora de inicio no es valida
        exit(0);//se sale del programa
    }

    //verificar hora fin
    if (horaFin < HORA_MIN || horaFin > HORA_MAX)//si la hora fin es menor a la hora minima o la hora fin es mayor a la hora maxima
    {
        printf("Hora fin no valida, rango (7 <= i <= 19)\n");//se indica que la hora fin no es valida
        exit(0);//se sale del programa
    }

    //validar segHoras
    if (segHoras <= 0)//si los segundos por hora son menores o iguales a cero entonces se ejecuta lo siguiente
    {
        printf("-s debe ser mayor a 0");//se indica que los segundos por hora deben ser mayores a cero
        exit(0);//se sale del programa
    }

    //verificar aforo mayor a 0
    if (aforo <= 0)//si el aforo es menor o igual a cero entonces se ejecuta lo siguiente
    {
        printf("Aforo debe ser mayor a 0\n");//se indica que el aforo debe ser mayor a cero
        exit(0);//se sale del programa
    }



    //horas para la simulacion
    int horaActual = horaInicio;//se declara la variable hora actual y se le asigna el valor de hora de inicio
    int horaFinSim = horaFin;//se declara la variable hora fin de simulacion y se le asigna el valor de hora fin
    int aforoMax = aforo;//se declara la variable aforo maximo y se le asigna el valor de aforo

    //arreglo de reservas
    Reserva reserva[MAX_RESERVAS];//se declara el arreglo de reservas
    int numReservas = 0;//se inicializa la variable numReservas a 0

    Estadisticas estadisticas;//se declara la variable estadisticas de tipo Estadisticas

    //incializar estadisticas en 0
    estadisticas.aceptadas = 0;//se inicializa la variable de estadisticas aceptadas a 0
    estadisticas.negadas = 0;//se inicializa la variable de estadisticas negadas a 0
    estadisticas.negadasHora = 0;//se inicializa la variable de estadisticas negadas por hora a 0
    for (int i = 0; i < 25; i++)//se genera un ciclo for el cual termina cuando i es igual a 25
    {
        estadisticas.ocupacionHoras[i] = 0;//se inicializa la variable de estadisticas ocupacion horas a 0 en cualquier posicion del arreglo
    }
    estadisticas.reprogramadas = 0;//se inicializa la variable de estadisticas reprogramadas a 0


    //creacion de mkfifo
    //no se usa pipe(), porque funciona con padre e hijo
    //usamos mkfifo porque funciona con procesos independientes
    if (mkfifo(nomPipe, 0666) == -1)//si la creacion de mkfifo es igual a -1 entonces se ejecuta lo siguiente
    {
        printf("Error en la creacion de mkfifo\n");//se indica que hubo un error en la creacion de mkfifo
        exit(0);//se sale del programa
    }

    printf("Creando fifo y abriendo para modo lectura\n");//se indica que se esta creando el fifo y abriendo para modo lectura
    //abrir el pipe para lectura
    int fdPipe = open(nomPipe, O_RDONLY );//se abre el pipr para lectura

    //abre el fifo para en modo escritura para evitar un bloqueo
    escribir = open(nomPipe, O_WRONLY);//se abre el fifo para modo escritura

    if (fdPipe == -1)//si el pipe es igual a -1 entonces se ejecuta lo siguiente
    {
        printf("Error en la lectura de mkfifo \n");//se indica que hubo un error en la lectura de mkfifo
        exit(0);//se sale del programa
    }

    printf("controlador listo, se esta esperando la solicitud del pipe: %s \n", nomPipe);//se indica que el controlador esta listo y se esta esperando la solicitud del pipe



    pthread_t tReloj, tAtencion;//se declaran las variables tReloj, tAtencion de tipo pthread_t

    // inicializar mutex (opcional porque ya lo hiciste con INITIALIZER)
    pthread_mutex_init(&mutex, NULL);//se inicializa el mutex

    // preparar argumentos para el hilo del reloj
    RelojArgs argsReloj;//se declara la variable argsReloj que es de tipo RelojArgs
    argsReloj.horaActual = &horaActual;//se le asigna el valor de hora actual a la variable argsReloj.horaActual
    argsReloj.horaFin    = horaFinSim;//se le asigna el valor de hora fin de simulacion a la variable argsReloj.horaFin
    argsReloj.segHoras   = segHoras;//se le asigna el valor de segundos por hora a la variable argsReloj.segHoras
    argsReloj.res         = reserva;//se le asigna el valor de reserva a la variable argsReloj.res
    argsReloj.numReservas = &numReservas;//se le asigna el valor de numReservas a la variable argsRelog.numReservas
    // preparar argumentos para el hilo de atencion
    ControladorArgs argsCtrl;
    argsCtrl.fdPipe       = fdPipe;//se le asigna el valor de fdPipe a la variable argsCtrl.fdPipe
    argsCtrl.horaActual   = &horaActual;//se le asigna el valor de hora actual a la variable argsCtrl.horaActual
    argsCtrl.horaFin      = horaFinSim;//se le asigna el valor de hora fin de simulacion a la variable argsCtrl.horaFin
    argsCtrl.aforoMax     = aforoMax;//se le asigna el valor de aforo maximo a la variable argsCtrl.aforoMax
    argsCtrl.res          = reserva;//se le asigna el valor de reserva a la variable argsCtrl.res
    argsCtrl.numReservas  = &numReservas;//se le asigna el valor de numReservas a la variable argsCtrl.numReservas
    argsCtrl.estadisticas = &estadisticas;//se le asigna el valor de estadisticas a la variable argsCtrl.estadisticas.

    //creacion de hilos
    pthread_create(&tReloj, NULL, hiloReloj, &argsReloj);//se crea el hilo del reloj
    pthread_create(&tAtencion, NULL, hiloAtencion, &argsCtrl);//se crea el hilo de atencion

    //espera a que terminen
    pthread_join(tReloj, NULL);//se espera a que termine el hilo de reloj
    pthread_join(tAtencion, NULL);//se espera a que termine el hilo de atencion


    //si read devuelve 0
    printf("no hay mas solicitudes\n");//se indica que no hay mas solicitudes

    //cerra fifo y borrarlo

    close(fdPipe);//se cierra el pipe
    unlink(nomPipe);//se borra el pipe

    //resumen del dia
    printf("Resumen del dia\n");//se imprime el resumen del dia
    printf("Aceptadas: %d\n", estadisticas.aceptadas);//se imprime la cantidad de reservas aceptadas
    printf("Reprogramadas: %d\n", estadisticas.reprogramadas);//se imprime la cantidad de reservas reprogramadas
    printf("Negadas: %d\n", estadisticas.negadas);//se imprime la cantidad de reservas que se negaron
    printf("Negadas hora: %d\n", estadisticas.negadasHora);//se imprimen la cantidad de reservas que se negaron por hora


    return 0;//se retorna 0 para indicar que el programa se ejecuto correctamente

}