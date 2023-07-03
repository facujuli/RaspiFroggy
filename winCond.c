/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "winCond.h"
#include "backEnd.h"

#ifndef RASPI
    #include "Text.h"
    #include "display_settings.h"
    #include "make.h"
#endif

void rePositionFrog(world_t* sim)
{ 
    sim->objetos[FROG].x[0] = 8;                
    sim->objetos[FROG].y = 14;
    sim->lives--;    
    return; 
}

void resetFrog(world_t* sim)
{
    sim->objetos[FROG].x[0] = 8;                
    sim->objetos[FROG].y = 14;
    sim->lives = CANT_DE_VIDAS;
    sim->points = 0;
}

void gameOver(world_t* sim)
{
    #ifndef RASPI
        selection_t SELECT_RETURN =  {3*CUADRADITO_SIZE,14*CUADRADITO_SIZE, 7*CUADRADITO_SIZE,15*CUADRADITO_SIZE, OFF };
        sim->menu_status = HALL_OF_FAME_MENU;
        sim->select_option = SELECT_RETURN; 
        writeNick(sim);
        memset(sim->nickName, '\0', sizeof(sim->nickName));
        resetBitmaps(sim);
    #endif
    //sim->running = !sim->running;
    sim->nivel = 0;

    //Lo siguiente habria que hacerlo tambien en allegro, lo comento con raspi solo para que no haya problema 
    //de comunicacion entre threads. Habria que asegurarse que mientras se libere la memoria nadei acceda
    // ni a las posiciones de los autos ni de las ranas. EN la rpi ya lo tengo asegurado eso.
    #ifdef RASPI
        free_memory(sim, 0);
        initialize_objects(sim);
    #endif


}






