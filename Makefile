######################################################################
# Autores:Santiago Henrnandez Morales, Jose Jesus Cepeda Vargas,Andres Eduardo Meneses Rincon,Karol Dayan Torres Vides
# Fecha:18/11/2025
# Docente:John Franco Corredor
#Materia: Sistemas Operativos
#Descripcion:Automatiza la compilacion del programa
######################################################################

# se define el compilador que se va a usar (gcc)
GCC      = gcc
#para poder saber si hay un error, ej (variables no inicializadas, funciones sin usar, etc)
CFLAGS  = -Wall
#bandera para el uso de posix
POSIX = -lpthread
# se define los programa posibles a compilar
PROGRAMAS = controlador agente

MODC = moduloControlador
MODA = moduloAgente

# compila el programa de acuerdo al que se necesita
controlador:
	$(GCC) $(CFLAGS) $@.c $(MODC).c -o $@ $(POSIX)

agente:
	$(GCC) $(CFLAGS) $@.c $(MODA).c -o $@

#limpieza de los programas
clean:
	$(RM) $(PROGRAMAS)