/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   winCond.h
 * Author: sanmar
 *
 * Created on April 29, 2023, 11:06 AM
 */

#ifndef WINCOND_H
#define WINCOND_H

    #define CANT_DE_VIDAS 3

    #include "sim.h" 

    void rePositionFrog(world_t* sim);
    void resetFrog(world_t* sim);
    void gameOver(world_t* sim);

#endif /* WINCOND_H */

