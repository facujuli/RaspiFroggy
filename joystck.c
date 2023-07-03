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


#define THRESHOLD 40	//Límite a partir del cual se mueve el LED encendido


void* joystick(void* simPtrPtr)
{
    usleep(200000);		//sleep para no tener problemas con la inicialización del otro thread
	world_t** simPtr = (world_t**)simPtrPtr;
    world_t *sim = *simPtr;

    joy_init();         //inicializa el joystick

while(sim->running)
{
	dcoord_t pos = {DISP_MAX_X>>1 , DISP_MAX_Y>>1};	//pos es la posición actual, empieza en el centro de la matriz
	dcoord_t npos = pos;							//npos es la próxima posición
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
			sim->menu_status = 0;
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
			else if(sim->menu_status == PAUSE_MENU && sim->key_pressed != 2)
			{
				sim->key_pressed++;
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
			else if(sim->menu_status == PAUSE_MENU && sim->key_pressed != 0)
			{
				sim->key_pressed--;
			}
            usleep(500000);
		}

	} while(coord.sw == J_NOPRESS && sim->running );	//termina si se presiona el switch


}
}