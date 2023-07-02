/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "backEnd.h"
#include "winCond.h"

#ifndef RASPI
    #include "display_settings.h"
#endif
#include <stdlib.h>
#include "stdio.h"

void free_memory(world_t *sim, int flag)
{
    int i;
    if(flag)
    {
        free(sim->objetos[FROG].x);
    }
    for ( i = 1; i < OBJ_MAX; i++)
    {
        free(sim->objetos[i].x);  //libero la memoria solicitada con mmalloc en la funcion initialize_objects
        printf("Libere\tcorrectamente\tlas\tx\tde :\t%d\n", i);
    }
}

void initialize_objects(world_t* sim)
{
    srand(time(0));

    sim->lily[0] = 0;
    sim->lily[1] = 0;
    sim->lily[2] = 0;
    sim->lily[3] = 0;
    sim->lily[4] = 0;
    sim->lily[5] = 0;

    float speed[OBJ_MAX] = {VEL(5), VEL(5), VEL(5), -VEL(9), -VEL(5), VEL(5), -VEL(5), VEL(5), -VEL(5), VEL(5)};     //Arreglo que contendrá en cada columna las respectivas velocidades de los objetos.
                                                                                                                    //cada fila representará un nivel distinto
    int separation[OBJ_MAX] = { 0, 4, 4, 9, 4, 5, 6, 6, 6, 7 };                                                     //Arreglo que contendrá en cada columna las respectivas separaciones 
                                                                                                                    //q posseen entre si los objetos
    int screen_rep[OBJ_MAX] = { 0, 3, 3, 1, 4, 3, 3, 3, 3, 3 };                                                     //Arreglo asociada que guarda cuantas veces aparece el objeto por pantalla, en el nivel
                                                                                                                    //si screen_rep[m][j] = 0, significa que en el nivel m, el objeto j no aparece.
    int coor_y[OBJ_MAX] = {0, 13, 12, 11, 10, 9, 7, 6, 5, 4};
    
    int squares_cant[OBJ_MAX] = {1,1,1,1,1,2,3,3,3,3};
    
    int j;
    if(sim->nivel >= 1)
    {
        for(j = 1; j < OBJ_MAX; j++)
        {
            speed[j] = speed[j] + (0.05)*(sim->nivel)*speed[j]; 
            if(j <= TRUCK && j != CAR3)
            {
                screen_rep[j] = rand() %5;
            }            
            else if(j == CAR3)
            {
                screen_rep[3] = rand() % 1 + 1;
            }
            else if(j <= TRONCO2 && j < TRUCK)
            {
                screen_rep[j] = rand() % 1 + 2;
            }
        }
    }

    int i;
    for(i = 1; i < OBJ_MAX; i++)
    {
        sim->objetos[i].separation = separation[i];
        sim->objetos[i].y = coor_y[i];
        sim->objetos[i].speed = 50*speed[i];
        sim->objetos[i].cant_squares = squares_cant[i];
        sim->objetos[i].screen_rep = screen_rep[i];
        sim->objetos[i].x = (float*) malloc((sim->objetos[i].screen_rep) * sizeof(float)); // creo una especie de arreglo que contendra la coordenada en X de cada copia de este objeto.
        sim->objetos[i].x[0] =  (rand()%17);            //se designa la coordenada x de la primer variacion del objeto de manera aleatoria con un numero <= 16;

        int k;
        
        for(k = 1; k < sim->objetos[i].screen_rep; k++)
        {
            sim->objetos[i].x [k] = (int) (sim->objetos[i].x [k-1] + sim->objetos[i].separation) % 17; //inicializo los valores de cada coordenada x del objeto, separadas homogeneamente
            
        }
        
        evaluate_sep(sim, i);

       
        #ifndef RASPI
        /********************************************************************************************/
            if(load_image(sim, 0, i))
            {
                printf("Fallé inicializando el bitmap del objeto[ %d ] en el nivel\n", i);
            }
        /********************************************************************************************/
        #endif
    }
    sim->objetos[FROG].x = (float*) malloc(sizeof(float));
    sim->objetos[FROG].x [0] = 8;
    sim->objetos[FROG].speed = 0;
    sim->onBoard = 0;
    
    #ifndef RASPI
    /***************************************/
        load_image(sim, 0, GAME);
        
        int i_menus;
        for(i_menus = MAIN_MENU; i_menus <= PAUSE_MENU; ++i_menus )
            load_image(sim, 0, i_menus);
    /***************************************/
    #endif

    printf("Se inicilizaron todos los objetos correctamente\n");
    
    return;
}

void move_objects(world_t* sim)
{
    if(!(sim->objetos[FROG].speed))
        sim->onBoard = 0;
        
    if( (sim->objetos[FROG].y <= sim->objetos[TORTU1].y) && (sim->objetos[FROG].y >= sim->objetos[TRONCO2].y)   )   //Si la rana se halla en la zona de agua
    {
        if(collide(sim) && !(sim->onBoard))
        {
            int i;
            for(i = TORTU1; i < OBJ_MAX; i++)
            {
                if(sim->objetos[i].y == sim->objetos[FROG].y)
                {  
                    sim->onBoard = 1;
                    sim->objetos[FROG].speed = sim->objetos[i].speed;   //la rana se mueve con la velocidad de el bote
                }
            }
        }
    }
    else
    {
        sim->objetos[FROG].x[0] = (int) (0.5 + sim->objetos[FROG].x[0]);  //La rana no se halla en un bote su coordenada x debe ser un numero entero entre 0 y 15. 
        sim->objetos[FROG].speed = 0;
    }
    
    int i,k;
    for ( i = 1; i < OBJ_MAX; i++)
    {
        for(k = 0; k < sim->objetos[i].screen_rep; k++)
        {
            sim->objetos[i].x [k] += (sim->objetos[i].speed);     //aumento la posicion en x speed veces

            if(sim->objetos[i].speed > 0 && sim->objetos[i].x [k]> 16)
            {
                sim->objetos[i].x [k] = (-1)*(sim->objetos[i].cant_squares);  //si salio de la pantalla por el lado derecho, lo corro su ancho a la izquierda del mapa
            }
            else if( sim->objetos[i].speed < 0 && sim->objetos[i]. x [k] < (-1)*(sim->objetos[i].cant_squares))
            {
                sim->objetos[i].x [k] = 16 + sim->objetos[i].cant_squares; // si salio de la pantalla por el lado izq, lo corro su ancho para el lado derecho
            }
        }
    }
    sim->objetos[FROG].x[0] += sim->objetos[FROG].speed;
    int j;

    if(sim->objetos[FROG].y == 3)
    {
        for(j = 0; j < 6; j++)
        {
            if(sim->objetos[FROG].x[0] == (2 + 2*j))
            {
                sim->lily[j] = 1;
                sim->lives++;
                sim->points += 20;
                rePositionFrog(sim);
            }
        }
    }

    if(sim->lily[0] && sim->lily[1] && sim->lily[2] && sim->lily[3] && sim->lily[4] && sim->lily[5])
    {
        sim->nivel++;
        sim->lily[0] = 0;
        sim->lily[1] = 0;
        sim->lily[2] = 0;
        sim->lily[3] = 0;
        sim->lily[4] = 0;
        sim->lily[5] = 0;
        sim->points +=100;
        sim->lives++;
        rePositionFrog(sim);
        sim->lives = CANT_DE_VIDAS;
        //ya junte 5 ranas paso de nivel
        //llamar a initialize objetcs si todo anda bien, asi se ve reflejado el cambio
        // (deberian cambiar los vehiculos y aumentr 5% las velocidades progresivamente en cad nivel)
        //recordar llamar a free memory asi las coordenadas en x se liberan de cada objeto(inclusive la rana) y se pueden volver a asignar
    }
    
    

    if(collide(sim) && (sim->objetos[FROG].y >= sim->objetos[TRUCK].y) )
    {
        rePositionFrog(sim);
        if(sim->lives <= 0 )
        {
            gameOver(sim);
            resetFrog(sim);
        }
    }
    else if( (!collide(sim)) && (sim->objetos[FROG].y >= sim->objetos[TRONCO2].y) && (sim->objetos[FROG].y <= sim->objetos[TORTU1].y) && !(sim->onBoard))
    {
        rePositionFrog(sim);
        if(sim->lives <= 0 )
        {
            gameOver(sim);
            resetFrog(sim);
        }
    }
}

void evaluate_sep(world_t* sim, int macro_type)
{
    int k, min_sep, modulo, flag = 0;
    
    if( macro_type < TRUCK)
    {
        //esta variable contiene la separacion minima que establecimos deben tener los vehiculos
        min_sep = MIN_SEP_CAR;      
    }
    else if(macro_type == TRUCK)
    {
        //esta variable contiene la separacion minima que establecimos deben tener los vehiculos
        min_sep = MIN_SEP_TRUCK;    
    }
    else
    {
        //esta variable contiene la separacion minima que establecimos deben tener los vehiculos
        min_sep = MIN_SEP_BOAT; 
    }
    
    //recorre todas las variaciones del vehiculo
    for(k = 1; k < sim->objetos[macro_type].screen_rep; k++)        
    {
        //evalua separacion de vehiculos entre si
        modulo = sim->objetos[macro_type].x[k] - sim->objetos[macro_type].x[k - 1];     
        if(modulo < 0)
        {
            modulo *= -1;
        }
        
        if(modulo < min_sep)
        {
            flag = 1;
        }
    }
    
    //comparo el primero con el ultimo
    modulo = sim->objetos[macro_type].x[k-1] - sim->objetos[macro_type].x[0];     
    
    if(modulo < 0)
    {
        modulo *= -1;
    }
 
    if(modulo < min_sep)
    {
        flag = 1;
    }
 
    if(flag)
    {
        sim->objetos[macro_type].x[0] = 0;
        
        for(k = 1; k < sim->objetos[macro_type].screen_rep; k++)
        {
             //inicializo los valores de cada coordenada x del objeto, separadas homogeneamente
            sim->objetos[macro_type].x [k] = (int) (sim->objetos[macro_type].x [k-1] + sim->objetos[macro_type].separation) % 17;
            
        }
    }
}

int collide(world_t* sim)
{

    int macro_type, i, k, value = 0;
    float n;        //coordenada x del vehiculo

    for (macro_type = 1; macro_type < OBJ_MAX; macro_type++)
    {
        if(sim->objetos[macro_type].y == sim->objetos[FROG].y)      //si estan en la misma linea
        {
            for(k = 0; k < sim->objetos[macro_type].screen_rep; k++)
            {
                for(i = 0; i < sim->objetos[macro_type].cant_squares; i++ )    //evaluo choque
                {
                    
                    n = round_if(sim->objetos[macro_type].x[k] + i, sim->objetos[macro_type].speed);  //evalua la coordenada x del vehiculo, y en base a su velocidad,
                                                                                                //decide si aanzarlo un bloque en el bitmap o no
                    if((int)sim->objetos[FROG].x[0] == (int)n)
                    {
                        value++;      //hubo colisión
                    }
                }
            }
        }
    }
    return value;
}

float round_if(float n, float speed)
{
    if(speed > 0)       //si la velocidad es mayor a 0
    {
        if(n >= (int)n + 0.5)       //redondeo al entero proximo mayor
        {
            return ( (int)n+1 );
        }
        else
        {
            return n;
        }
    }
    else if(speed < 0)
    {
        
        if(n <= (int)n + 0.5)       
        {
            return ( (int)n );  
        }
        else
        {
            return n + 1;
        }
    }
}

void resetPositions(world_t* sim)
{
    srand(time(0));
    
    float speed[OBJ_MAX] = {VEL(1), VEL(1), VEL(1), -VEL(5), -VEL(1), VEL(1), -VEL(1), VEL(1), VEL(1), VEL(1)};                                                                                                                 
    int separation[OBJ_MAX] = { 0, 4, 4, 9, 4, 5, 6, 6, 6, 7 };                                                                                              
    int screen_rep[OBJ_MAX] = { 0, 3, 3, 1, 4, 3, 3, 3, 3, 3 };                                                                                                              
    int coor_y[OBJ_MAX] = {0, 13, 12, 11, 10, 9, 7, 6, 5, 4};
    int squares_cant[OBJ_MAX] = {1,1,1,1,1,2,3,3,3,3};
    
    int j;
    if(sim->nivel >= 1)
    {
        for(j = 1; j < OBJ_MAX; j++)
        {
            speed[j] = speed[j] + (0.05)*(sim->nivel)*speed[j]; 
            if(j <= TRUCK && j != CAR3)
            {
                screen_rep[j] = rand() %5;
            }            
            else if(j == CAR3)
            {
                screen_rep[3] = rand() % 1 + 1;
            }
            else if(j <= TRONCO2 && j < TRUCK)
            {
                screen_rep[j] = rand() % 1 + 2;
            }
        }
    }
    
    int i;
    for(i = 1; i < OBJ_MAX; i++)
    {
        sim->objetos[i].separation = separation[i];
        sim->objetos[i].y = coor_y[i];
        sim->objetos[i].speed = speed[i];
        sim->objetos[i].cant_squares = squares_cant[i];
        sim->objetos[i].screen_rep = screen_rep[i];
        sim->objetos[i].x[0] =  (rand()%17);            //se designa la coordenada x de la primer variacion del objeto de manera aleatoria con un numero <= 16;

        int k;
        for(k = 1; k < sim->objetos[i].screen_rep; k++)
        {
            sim->objetos[i].x [k] = (int) (sim->objetos[i].x [k-1] + sim->objetos[i].separation) % 17; //inicializo los valores de cada coordenada x del objeto, separadas homogeneamente
            
        }
    }
}

