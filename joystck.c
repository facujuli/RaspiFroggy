/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <unistd.h>
#include "disdrv.h"
#include "joydrv.h"
#include "joystck.h"
#include "object.h"
#include "sim.h"
#include "dispRaspi.h"
#include "backEnd.h"
#include "winCond.h"


#define THRESHOLD 40	//Límite a partir del cual se mueve el LED encendido


void* joystick(void* simPtrPtr)
{
    usleep(200000);		//sleep para no tener problemas con la inicialización del otro thread
	world_t** simPtr = (world_t**)simPtrPtr;
    world_t *sim = *simPtr;

    joy_init();         //inicializa el joystick

while(sim->running)
{
	joyinfo_t coord = {0,0,J_NOPRESS};							//coordenadas medidas del joystick
    dcoord_t ps;
	sim->key_pressed = 0;		// 0: PLAY, 1: EXIT,  2: RESTART

    //EL siguiente bloque se encarga de la lectura del joystick, 
    //como tambien de borrar el led que prendio previamente la rana
    //se corre mientras no se presione el switch
	int i= 0;
	do
	{
		i++;
		coord = joy_read();	//Guarda las coordenadas medidas
		ps.x =  sim->objetos[FROG].x[0];
        ps.y = sim->objetos[FROG].y;

		if(coord.sw != J_NOPRESS && sim->menu_status == PAUSE_MENU)
		{
			if(sim->key_pressed == 0)
			{
				sim->menu_status = GAME;
			}
			else if(sim->key_pressed == 1)
			{
				sim->menu_status = MAIN_MENU;
				gameOver(sim);
				resetFrog(sim);
			}
			if(sim->key_pressed== 2)
			{
				sim->menu_status = GAME;
			}
			usleep(200000);
		}
		else if(coord.sw != J_NOPRESS && sim->menu_status == MAIN_MENU)
		{
			if(sim->key_pressed == 0 || sim->key_pressed == 2)
			{
				sim->menu_status = GAME;    //Se presiono play
			}
			else if(sim->key_pressed == 1)
			{
				sim->running = 0;
				sim->menu_status = 0;
			}
			usleep(200000);
		}
        //printf("X: %f   Y:   %f\n", (float)sim->objetos[FROG].x[0], (float)sim->objetos[FROG].y);
		//Establece la próxima posición según las coordenadas medidas
		if(coord.x > THRESHOLD && sim->objetos[FROG].x[0] < DISP_MAX_X)
		{
            printf("derecha\n");
			if(sim->menu_status == GAME)
			{
				disp_write(ps, D_OFF);		
				sim->objetos[FROG].x[0]++;
			}		
            usleep(500000);
		}
		if(coord.x < -THRESHOLD && sim->objetos[FROG].x[0] > DISP_MIN)
		{
            printf("izquierda\n");
			if(sim->menu_status == GAME)
			{
				disp_write(ps, D_OFF);		
				sim->objetos[FROG].x[0]--;
			}
            usleep(500000);
		}
		if(coord.y > THRESHOLD && sim->objetos[FROG].y > DISP_MIN)
		{
            printf("arriba\n");
			if(sim->menu_status == GAME)
			{
          	  	disp_write(ps, D_OFF);		
				sim->objetos[FROG].y--;
			}
			else if((sim->menu_status == MAIN_MENU || sim->menu_status == PAUSE_MENU )&& sim->key_pressed != 2)
			{
				sim->key_pressed++;		//si estoy en algun menu cambio la opcion
			}
            usleep(500000);
		}
		if(coord.y < -THRESHOLD && sim->objetos[FROG].y < DISP_MAX_Y)
		{
            printf("abajo\n");
			if(sim->menu_status == GAME)
			{
				disp_write(ps, D_OFF);		
				sim->objetos[FROG].y++;
			}
			else if((sim->menu_status == MAIN_MENU || sim->menu_status == PAUSE_MENU ) && sim->key_pressed != 0)
			{
				sim->key_pressed--;
			}
            usleep(500000);
		}

	} while(sim->running );	//termina si se presiona el switch


}
return NULL;
}