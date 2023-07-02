#include "joydrv.h"
#include<stdio.h>
#include "object.h"
#include "sim.h"
#include "disdrv.h"
#include "backEnd.h"
#include "winCond.h"
#include "joystck.h"
#include <unistd.h>


static void dibujar_objeto(int macro_type, world_t* sim);
static void borrarLinea(int i, world_t* sim);

joyinfo_t coord = {0,0,J_NOPRESS};

void* display(void* simPtrPtr)
{ 
    world_t** simPtr = (world_t**)simPtrPtr;
    world_t *sim = *simPtr;
    disp_init();
    disp_clear();
    disp_update();
    
    int titilar = 0;

    

while(sim->running == ON)
{
    coord = joy_read();
    int i;

    for(i = 1; i < OBJ_MAX; i++)
    {
        borrarLinea(i, sim);      
        dibujar_objeto(i, sim);
    }

    titilar++;
    dcoord_t ps = {sim->objetos[FROG].x[0], sim->objetos[FROG].y};

    disp_write(ps, titilar%2);		//prende el led si titilar es par.
    
        
    disp_update();			//actualiza el display 
    
    //move_objects(sim);
    usleep(250000);

    if(coord.sw != J_NOPRESS)
    {
        sim->running = 0;
        disp_clear();
        disp_update();
    }
    
}
}

void dibujar_objeto(int macro_type, world_t* sim)		
{
    int j,k;
    for(j = 0 ; j < sim->objetos[macro_type].screen_rep; j++)
    {
        int x = (int)round_if(sim->objetos[macro_type].x[j], sim->objetos[macro_type].speed);
        for (k = 0; k < sim->objetos[macro_type].cant_squares; k++)
        { 
            if(x + k >= -0.5 && x + k < 15.5)
            {
                dcoord_t pos={ x + k, sim->objetos[macro_type].y };
                disp_write(pos, D_ON);		//prende el led
            }
        }
    }
}


void borrarLinea(int i, world_t* sim)		
{
	int u;	

	for(u=0;u<16;u++)				//apaga todos los leds de la linea i
	{
	dcoord_t pos2 = { u ,sim->objetos[i].y};
				
	disp_write(pos2, D_OFF);	
    	
	}

	return;
}