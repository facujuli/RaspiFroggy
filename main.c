/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: Ignacio Sammartino
 *
 * Created on March 25, 2023, 11:42 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include "dispRaspi.h"
#include "sim.h"
#include "joystck.h"


#ifndef RASPI

    #include "sound.h"
    #include "make.h"
    #include "display_settings.h"
    #include "keyboard_settings.h"
#endif

    #include "backEnd.h"

int main(void)
{
    world_t sim;
    sim.redraw = 3;
    
    world_t*  simPtr = &sim;
    world_t** simPtrPtr = &simPtr;
    sim.nivel = 0;
    
#ifndef RASPI
    if( allegro_init(simPtr) )
    {
        printf("Fallé realizando las inicializaciones de Allegro.\n");
        return 1;
    }
    
    al_register_event_source(sim.event_queue, al_get_display_event_source(sim.display) );
    al_register_event_source(sim.event_queue, al_get_timer_event_source(sim.timer) );
    al_register_event_source(sim.event_queue, al_get_mouse_event_source() );
    al_register_event_source(sim.event_queue, al_get_keyboard_event_source() );
    
    al_clear_to_color(al_map_rgb(0,255,255));
    al_start_timer(sim.timer);
#endif
    
    object_t frog;
    frog.x = NULL;
    frog.y = 14;
    frog.speed = 0;
    frog.screen_rep = 0;
    frog.cant_squares = 1;
#ifndef RASPI
    frog.image = NULL;
#endif
    
    sim.objetos[0] =  frog;
 
    sim.running = ON;
    sim.menu_status = MAIN_MENU;
    
    initialize_objects(&sim);

    pthread_t t1,t2  ;
    pthread_create(&t2, NULL, &joystick, (void*)simPtrPtr);
    pthread_create(&t1, NULL, &display, (void*)simPtrPtr);
    //pthread_create(&t1, NULL, &keyboard, (void*)simPtrPtr);
    pthread_join(t2,NULL);
    pthread_join(t1,NULL); 
   // pthread_join(t1,NULL);  
    

    free_memory(&sim, 1);
#ifndef RASPI
    //Programs begins to shuts down
    load_sound_effects(simPtr, 1); 
    destroy_allegro(&sim);
    printf("Destrui todo lo de allegro correctamente\n");
#endif
    printf("puntos: %d\n", sim.points);
    printf("fin del programa\n");
    return 0;
    
}

