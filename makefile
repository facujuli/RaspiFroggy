# Cómo compilar ejemplos:

main: main.c backEnd.o dispRaspi.o joystck.o winCond.o

backEnd.o: backEnd.c backEnd.h

dispRaspi.o: dispRaspi.c dispRaspi.h

joystck.o: joystck.c joystck.h

winCond.o: winCond.c winCond.h

deuna: main.c backEnd.c backEnd.h winCond.c winCond.h joystck.c joystck.h dispRaspi.c dispRaspi.h
	gcc -o juegazo main.c backEnd.c winCond.c joystck.c dispRaspi.c disdrv.o joydrv.o -lpthread -Wall -D RASPI

