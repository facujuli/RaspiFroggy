/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   backEnd.h
 * Author: sanmar
 *
 * Created on April 29, 2023, 3:42 PM
 */

#ifndef BACKEND_H
#define BACKEND_H

    #include "sim.h"

    #define VEL(v) ((v)*(3.0/60)/20.0)
    #define MIN_SEP_CAR 2       //separacion minima que deben tener los coches entre si
    #define MIN_SEP_TRUCK 3     //separacion minima que deben tener los camiones entre si
    #define MIN_SEP_BOAT 5      //separacion minima que deben tener los botes entre si

    void free_memory(world_t *sim, int flag);
    void initialize_objects(world_t* sim);
    void move_objects(world_t* sim);
    void evaluate_sep(world_t* sim, int macro_type);
    int collide(world_t* sim);
    float round_if(float n, float speed);
    void resetPositions(world_t* sim);



#endif /* BACKEND_H */

