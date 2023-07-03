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
static void pausa(world_t* sim);
static void letterToMat(int map[5][3], char letter);

joyinfo_t coord = {0,0,J_NOPRESS};

void* display(void* simPtrPtr)
{ 
    world_t** simPtr = (world_t**)simPtrPtr;
    world_t *sim = *simPtr;
    disp_init();
    disp_clear();
    disp_update();
    sim->menu_status = MAIN_MENU;

    int titilar = 0;
    sim->key_pressed = 0;
    int prev = sim->key_pressed;
    sim->redraw=0;

while(sim->running)
{
    while (sim->menu_status == MAIN_MENU)       //MENU PRINCIPAL
    {
        disp_clear();

        char word[4];
        char game[4] = {'G', 'A', 'M', 'E'};

       if(sim->key_pressed == 0 )   // 0: PLAY, 1: EXIT,  2: RESTART
        {
            word[0] = 'P';
            word[1] = 'L';
            word[2] = 'A';
            word[3] = 'Y';
        }
        else if(sim->key_pressed == 1)
        {
            word[0] = 'E';
            word[1] = 'X';
            word[2] = 'I';
            word[3] = 'T';
        }
        else if(sim->key_pressed == 2)
        {
            word[0] = 'P';
            word[1] = 'U';
            word[2] = 'T';
            word[3] = 'O';
        }

        //por defecto las letras arrancan desde la fila 14
        int index2let = 1;
        int i,j;
        int map[5][3];

        dcoord_t pos;

        int len = 0;
        for(len = 0; len < 4; len++)
        {
            letterToMat(map, word[len]);

            for (j = 0; j < 3; j++)
            {
                for(i = 0; i < 5; i++)
                {
                    pos.y = 0 +i;
                    pos.x = index2let +j;
                    if(map[i][j])
                    {
                        disp_write(pos, D_ON);
                    }
                    disp_update();	
                }
            }
            index2let += j + 1;
        }
        index2let = 1; 
        for(len = 0; len < 4; len++)
        {
            letterToMat(map, game[len]);

            for (j = 0; j < 3; j++)
            {
                for(i = 0; i < 5; i++)
                {
                    pos.y = 7 +i;
                    pos.x = index2let +j;
                    if(map[i][j])
                    {
                        disp_write(pos, D_ON);
                    }
                    disp_update();	
                }
            }
            index2let += j + 1;
        }

        while (prev == sim->key_pressed)        //hasta que no hagas click o cambies de opcion, no repinto el mapa
        {
            if(sim->menu_status!= MAIN_MENU)
            {
                break;
            }
        }
        prev = sim->key_pressed;
    }

    usleep(450000/2);
    

    while(sim->menu_status == GAME && !sim->redraw)//sim->running == ON)  Si se esta jugando y no se esta reasignando cosas
    {                                       
        coord = joy_read();
        int i;
        dcoord_t ps;

        disp_clear();
        for(i = 1; i < OBJ_MAX; i++)
        {
            if(sim->lives>0)
            {
                dibujar_objeto(i, sim);
            }
        }
        for(i = 0; i < 16; i++)
        {
            ps.x = i;
            ps.y = 3;
            disp_write(ps, D_ON);
            if(i == 2 || i == 4 || i == 6 || i == 8 || i == 10 || i == 12)
            {
                disp_write(ps, sim->lily[i/2 -1]);// pinto el lilypad
            }
            
        }
        if(sim->lives>= 1)
        {
            ps.y = 0;
            ps.x = 15;
            disp_write(ps, D_ON);
            ps.x = 14;
            disp_write(ps, D_ON);
            ps.y = 1;
            ps.x = 15;
            disp_write(ps, D_ON);
            ps.x = 14;
            disp_write(ps, D_ON);
        }
        if(sim->lives>= 2)
        {
            ps.y = 0;
            ps.x = 12;
            disp_write(ps, D_ON);
            ps.x = 11;
            disp_write(ps, D_ON);
            ps.y = 1;
            ps.x = 12;
            disp_write(ps, D_ON);
            ps.x = 11;
            disp_write(ps, D_ON);
        }
        if(sim->lives>= 3)
        {
            ps.y = 0;
            ps.x = 9;
            disp_write(ps, D_ON);
            ps.x = 8;
            disp_write(ps, D_ON);
            ps.y = 1;
            ps.x = 9;
            disp_write(ps, D_ON);
            ps.x = 8;
            disp_write(ps, D_ON);
        }

        titilar++;
        ps.x = sim->objetos[FROG].x[0]; 
        ps.y = sim->objetos[FROG].y;

        disp_write(ps, titilar%2);		//prende el led si titilar es par.
        
            
        disp_update();			//actualiza el display 
        
        if(sim->lives >0)
        {
            move_objects(sim);
        }
            
        usleep(200000);

        
        if(coord.sw != J_NOPRESS)
        {
            sim->menu_status = PAUSE_MENU;
        }
        
    }
    //YA sali del game, tas en pausa
    if(sim->menu_status == PAUSE_MENU)
    {
        pausa(sim);
        usleep(200000);
    }
}
    disp_clear();
    disp_update();
    return NULL;
}

void dibujar_objeto(int macro_type, world_t* sim)		
{
    int j,k,l;
    dcoord_t pos;
    for(l = 0; l < 16; l++)
    {
        if(macro_type > TRUCK)
        {
            pos.x = l;
            pos.y = sim->objetos[macro_type].y;
            
            disp_write(pos, D_ON);		//prende el led
        }
    }
    
    for(j = 0 ; j < sim->objetos[macro_type].screen_rep; j++)
    {
        int x = (int)round_if(sim->objetos[macro_type].x[j], sim->objetos[macro_type].speed);
        for (k = 0; k < sim->objetos[macro_type].cant_squares; k++)
        { 
            if(x + k >= -0.5 && x + k < 15.5 && macro_type <= TRUCK)
            {
                pos.x =  x + k;
                pos.y = sim->objetos[macro_type].y;

                disp_write(pos, D_ON);		//prende el led
            }
            else if(x + k >= -0.5 && x + k < 15.5 && macro_type > TRUCK)
            {
                pos.x =  x + k;
                pos.y = sim->objetos[macro_type].y;

                disp_write(pos, D_OFF);		//apaga el led
            }
        }
    }
}


static void pausa(world_t* sim)
{
    int prev = sim->key_pressed;

    while (sim->menu_status == PAUSE_MENU)
    {   
            
        disp_clear();           
        dcoord_t pos;
        
        //por defecto las letras arrancan desde la fila 14
        int index2let = 1;
        int i,j;
        int map[5][3];

        char word[4];

        if(sim->key_pressed == 0)   // 0: PLAY, 1: EXIT,  2: RESTART
        {
            word[0] = 'P';
            word[1] = 'L';
            word[2] = 'A';
            word[3] = 'Y';
        }
        else if(sim->key_pressed == 1)
        {
            word[0] = 'M';
            word[1] = 'E';
            word[2] = 'N';
            word[3] = 'U';
        }
        else if(sim->key_pressed == 2)
        {
            word[0] = 'R';
            word[1] = 'S';
            word[2] = 'E';
            word[3] = 'T';
        }


        int len = 0;
        for(len = 0; len < 4; len++)
        {
            letterToMat(map, word[len]);

            for (j = 0; j < 3; j++)
            {
                for(i = 0; i < 5; i++)
                {
                    pos.y = 6 +i;
                    pos.x = index2let +j;
                    if(map[i][j])
                    {
                        disp_write(pos, D_ON);
                    }
                    disp_update();	
                }
            }
            index2let += j + 1;
        }

        pos.y = 0;
        pos.x = 7;
        disp_write(pos, D_ON);
        pos.y = 15;
        disp_write(pos, D_ON);

        pos.y = 1;
        pos.x = 6;
        disp_write(pos, D_ON);
        pos.y = 14;
        disp_write(pos, D_ON);

        pos.y = 1;
        pos.x = 8;
        disp_write(pos, D_ON);
        pos.y = 14;
        disp_write(pos, D_ON);


        for(j = 0; j < 15; j++)
        {
            pos.y = 4;
            pos.x = j;
            disp_write(pos, D_ON);
            pos.y = 12;
            disp_write(pos, D_ON);
        }
        disp_update();

        while (prev == sim->key_pressed)        //hasta que no hagas click o cambies de opcion, no repinto el mapa
        {
            if(sim->menu_status!= PAUSE_MENU)
            {
                break;
            }
        }
        prev = sim->key_pressed;
        
    }
    return;
}

static void letterToMat(int map[5][3], char letter)
{
    int i,j;
    for(i = 0; i < 5; i++)
    {
        for(j = 0; j < 3; j++)
        {
            map[i][j] = 0;
        }
    }

    if(letter == 'A')
    {
        map[1][0] = 1;
        map[2][0] = 1;
        map[3][0] = 1;
        map[4][0] = 1;

        map[0][1] = 1;
        map[2][1] = 1;

        map[1][2] = 1;
        map[2][2] = 1;
        map[3][2] = 1;
        map[4][2] = 1;
    }
    else if(letter == 'C')
    {
        map[0][0] = 1;
        map[1][0] = 1;
        map[2][0] = 1;
        map[3][0] = 1;
        map[4][0] = 1;

        map[0][1] = 1;
        map[4][1] = 1;

        map[0][2] = 1;
        map[4][2] = 1;
    }
    else if(letter == 'E')
    {
        map[0][0] = 1;
        map[1][0] = 1;
        map[2][0] = 1;
        map[3][0] = 1;
        map[4][0] = 1;

        map[0][1] = 1;
        map[2][1] = 1;
        map[4][1] = 1;

        map[0][2] = 1;
        map[2][2] = 1;
        map[4][2] = 1;
    }
    else if(letter == 'F')
    {
        map[0][0] = 1;
        map[1][0] = 1;
        map[2][0] = 1;
        map[3][0] = 1;
        map[4][0] = 1;

        map[0][1] = 1;
        map[2][1] = 1;

        map[0][2] = 1;
    }
    else if(letter == 'G')
    {
        map[0][0] = 1;
        map[1][0] = 1;
        map[2][0] = 1;
        map[3][0] = 1;
        map[4][0] = 1;

        map[0][1] = 1;
        map[2][1] = 1;
        map[4][1] = 1;

        map[0][2] = 1;
        map[2][2] = 1;
        map[3][2] = 1;
        map[4][2] = 1;
    }
    else if(letter == 'I')
    {
        map[0][1] = 1;
        map[2][1] = 1;
        map[3][1] = 1;
        map[4][1] = 1;
    }
    else if(letter == 'M')
    {
        map[0][0] = 1;
        map[1][0] = 1;
        map[2][0] = 1;
        map[3][0] = 1;
        map[4][0] = 1;

        map[1][1] = 1;

        map[0][2] = 1;
        map[1][2] = 1;
        map[2][2] = 1;
        map[3][2] = 1;
        map[4][2] = 1;
    }
    else if(letter == 'N')
    {
        map[0][0] = 1;
        map[1][0] = 1;
        map[2][0] = 1;
        map[3][0] = 1;
        map[4][0] = 1;

        map[2][1] = 1;
        map[3][1] = 1;

        map[0][2] = 1;
        map[1][2] = 1;
        map[2][2] = 1;
        map[3][2] = 1;
        map[4][2] = 1;
    }
    else if(letter == 'L')
    {
        map[0][0] = 1;
        map[1][0] = 1;
        map[2][0] = 1;
        map[3][0] = 1;
        map[4][0] = 1;

        map[4][1] = 1;

        map[4][2] = 1;
    }
    else if(letter == 'O')
    {
        map[0][0] = 1;
        map[1][0] = 1;
        map[2][0] = 1;
        map[3][0] = 1;
        map[4][0] = 1;

        map[0][1] = 1;
        map[4][1] = 1;

        map[0][2] = 1;
        map[1][2] = 1;
        map[2][2] = 1;
        map[3][2] = 1;
        map[4][2] = 1;
    }
    else if(letter == 'P')
    {
        map[0][0] = 1;
        map[1][0] = 1;
        map[2][0] = 1;
        map[3][0] = 1;
        map[4][0] = 1;

        map[0][1] = 1;
        map[2][1] = 1;

        map[0][2] = 1;
        map[1][2] = 1;
        map[2][2] = 1;
    }
    else if(letter == 'R')
    {
        map[0][0] = 1;
        map[1][0] = 1;
        map[2][0] = 1;
        map[3][0] = 1;
        map[4][0] = 1;

        map[0][1] = 1;
        map[2][1] = 1;
        map[3][1] = 1;

        map[0][2] = 1;
        map[1][2] = 1;
        map[4][2] = 1;
    }
    else if(letter == 'S')
    {
        map[0][0] = 1;
        map[1][0] = 1;
        map[2][0] = 1;
        map[4][0] = 1;

        map[0][1] = 1;
        map[2][1] = 1;
        map[4][1] = 1;

        map[0][2] = 1;
        map[2][2] = 1;
        map[3][2] = 1;
        map[4][2] = 1;
    }
    else if(letter == 'T')
    {
        map[0][0] = 1;

        map[0][1] = 1;
        map[1][1] = 1;
        map[2][1] = 1;
        map[3][1] = 1;
        map[4][1] = 1;

        map[0][2] = 1;
    }
    else if(letter == 'U')
    {
        map[0][0] = 1;
        map[1][0] = 1;
        map[2][0] = 1;
        map[3][0] = 1;
        map[4][0] = 1;

        map[4][1] = 1;

        map[0][2] = 1;
        map[1][2] = 1;
        map[2][2] = 1;
        map[3][2] = 1;
        map[4][2] = 1;
    }
    else if(letter == 'X')
    {
        map[0][0] = 1;
        map[4][0] = 1;

        map[1][1] = 1;
        map[2][1] = 1;
        map[3][1] = 1;

        map[0][2] = 1;
        map[4][2] = 1;
    }
    else if(letter == 'Y')
    {
        map[0][0] = 1;
        map[1][0] = 1;
        map[2][0] = 1;

        map[2][1] = 1;
        map[3][1] = 1;
        map[4][1] = 1;

        map[0][2] = 1;
        map[1][2] = 1;
        map[2][2] = 1;
    }

    return;
}