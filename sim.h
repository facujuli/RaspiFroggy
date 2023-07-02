/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   sim.h
 * Author: sanmar
 *
 * Created on March 25, 2023, 4:46 PM
 */

#ifndef SIM_H
#define SIM_H

    #include "object.h"

    typedef struct
    {
        short int running;
        short int redraw;
        int key_pressed;
        int menu_status;
        float rotation_flag;
        
        int onBoard;   //applies to frog only
       
        int nivel;
        short int lives;
        int points;
        int lily[6];
        
#ifndef RASPI
        /* allegro stuff */
        ALLEGRO_DISPLAY *display;
        ALLEGRO_TIMER *timer;
        ALLEGRO_EVENT_QUEUE* event_queue;
        ALLEGRO_EVENT event;
        ALLEGRO_KEYBOARD_STATE ks;    
        ALLEGRO_SAMPLE* sound_effects[5];
        ALLEGRO_AUDIO_STREAM* music;
        ALLEGRO_FONT* font;
        ALLEGRO_FONT* font2; 
        ALLEGRO_FONT* font3; 
        
        selection_t select_option;
        char nickName[5];
        
        ALLEGRO_BITMAP *menu_bitmaps[5];
        ALLEGRO_BITMAP* imagenLily;
#endif     

    object_t objetos[OBJ_MAX];

    }world_t;

#endif /* SIM_H */

