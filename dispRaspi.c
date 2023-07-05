#include "joydrv.h"
#include<stdio.h>
#include "object.h"
#include "sim.h"
#include "disdrv.h"
#include "backEnd.h"
#include "winCond.h"
#include "joystck.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>



static void dibujar_objeto(int macro_type, world_t* sim);
static void pausa(world_t* sim);
static void letterToMat(int map[5][3], char letter);
static void imprimirChar(int x, int y);
static void imprimirWord(int x_start, int y_start, int cant_let, char word[4]);
static void imprimirSentencia(char oracion[32], int size, int x, int y, int rep);
static void saludar(void);

joyinfo_t coord = {0,0,J_NOPRESS};

void* display(void* simPtrPtr)
{ 
    world_t** simPtr = (world_t**)simPtrPtr;
    world_t *sim = *simPtr;
    disp_init();
    disp_clear();
    disp_update();

    saludar();

    sim->menu_status = MAIN_MENU;

    int titilar = 0;
    sim->key_pressed = 0;
    int prev = sim->key_pressed;
    sim->redraw=0;
    int preLevel = sim->nivel;
    int puntaje = sim->points;
    while(sim->running)
    {
        while (sim->menu_status == MAIN_MENU)       //MENU PRINCIPAL
        {   
            char word[4];
            char game[4] = {'G', 'A', 'M', 'E'};
            disp_clear();

            if(sim->key_pressed == GAMEOVER)
            {
                word[0] = 'O';
                word[1] = 'V';
                word[2] = 'E';
                word[3] = 'R';
                imprimirWord(1, 0, 4, game);
                imprimirWord(1, 7, 4, word);
                usleep(1000000);
                
                //Transforma puntaje a string
                int len = 0;
                int s;
                int punteador = puntaje;
                char oracion[] = {' ','P','u','n','t','o','s',':',' ','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0',};
                char puntos[11];
                while(punteador != 0 || len == 0)
                {
                    punteador = punteador/10;
                    len++;      //cuenta cuantos caracteres tiene el numero

                }

                if(len > 10)
                {
                    len = 10;       //evita que el numero tenga mas de 10 caracteres
                }
                snprintf(puntos, len + 1, "%d", puntaje);
                for(s = 0; s < len; s++)
                {
                    oracion[9 + s] = puntos[s];     //concatena string de puntaje a string oracion
                }
                imprimirSentencia(oracion, 9 + len, 0, 5, 2);
                sim->key_pressed = 0;
                usleep(1000000);
                disp_clear();
            }
            else if( sim->key_pressed == WIN)
            {
                word[0] = 'U';
                word[1] = 'I';
                word[2] = 'N';
                imprimirWord(2, 4, 1, word);
                imprimirWord(4, 4, 3, word);
                sim->key_pressed = 0;
                usleep(2000000);
                disp_clear();
            }

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
                word[0] = 'F';
                word[1] = 'A';
                word[2] = 'C';
                word[3] = 'U';
            }
            

            imprimirWord(1, 0, 4, word);
            imprimirWord(1, 7, 4, game);

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
            if(sim->menu_status == GAME)        //evita alterar el puntaje si justo perdió
            {
                puntaje = sim->points;      //guarda copia del puntaje ya que cuando perdes pasa a valr 0.
            }
            if(preLevel != sim->nivel && sim->nivel!= 0)
            {
                disp_clear();
                char word[4] = {'L', 'E', 'V', 'L'};
                imprimirWord(1, 0, 4, word);
                word[0] = sim->nivel + '0';
                imprimirWord(7, 7, 1, word);
                usleep(2000000);
                preLevel = sim->nivel;
            }                       
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
                
            usleep(130000);

            
            if(coord.sw != J_NOPRESS)
            {
                sim->menu_status = PAUSE_MENU;
                usleep(200000);
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

        imprimirWord(1, 6, 4, word);

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

        int j;
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

    if(letter >= 'a' && letter <= 'z')
    {
        letter -= ('a'- 'A');
    }
    else if(letter >= '0' && letter <= '9')
    {
        letter += 0;
    } 
    else if( (letter < 'A' || letter > 'Z' ) && letter != ':' )
    {
        return;
    }

    if(letter == '1')
    {
        map[0][1] = 1;
        map[1][1] = 1;
        map[2][1] = 1;
        map[3][1] = 1;
        map[4][1] = 1;

    }
    else if(letter == '2')
    {
        map[0][0] = 1;
        map[2][0] = 1;
        map[3][0] = 1;
        map[4][0] = 1;

        map[0][1] = 1;
        map[2][1] = 1;
        map[4][1] = 1;

        map[0][2] = 1;
        map[1][2] = 1;
        map[2][2] = 1;
        map[4][2] = 1;
    }
    else if(letter == '3')
    {
        map[0][0] = 1;
        map[2][0] = 1;
        map[4][0] = 1;

        map[0][1] = 1;
        map[2][1] = 1;
        map[4][1] = 1;

        map[0][2] = 1;
        map[1][2] = 1;
        map[2][2] = 1;
        map[3][2] = 1;
        map[4][2] = 1;
    }
    else if(letter == '4')
    {
        map[0][0] = 1;
        map[1][0] = 1;
        map[2][0] = 1;

        map[2][1] = 1;

        map[0][2] = 1;
        map[1][2] = 1;
        map[2][2] = 1;
        map[3][2] = 1;
        map[4][2] = 1;
    }
    if(letter == '7')
    {
        map[0][0] = 1;

        map[0][1] = 1;

        map[0][2] = 1;
        map[1][2] = 1;
        map[2][2] = 1;
        map[3][2] = 1;
        map[4][2] = 1;

    }
    else if(letter == '8' || letter == 'B')
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
        map[1][2] = 1;
        map[2][2] = 1;
        map[3][2] = 1;
        map[4][2] = 1;
    }
    else if(letter == '9')
    {
        map[0][0] = 1;
        map[1][0] = 1;
        map[2][0] = 1;
        map[4][0] = 1;

        map[0][1] = 1;
        map[2][1] = 1;
        map[4][1] = 1;

        map[0][2] = 1;
        map[1][2] = 1;
        map[2][2] = 1;
        map[3][2] = 1;
        map[4][2] = 1;
    }
    else if(letter == ':')
    {
        map[1][1] = 1;
        map[3][1] = 1;
    }
    else if(letter == 'A')
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
    else if(letter == 'G' || letter == '6')
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
    else if(letter == 'H')
    {
        map[0][0] = 1;
        map[1][0] = 1;
        map[2][0] = 1;
        map[3][0] = 1;
        map[4][0] = 1;

        map[2][1] = 1;

        map[0][2] = 1;
        map[1][2] = 1;
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
    else if(letter == 'J')
    {
        map[4][0] = 1;
        
        map[4][1] = 1;

        map[0][2] = 1;
        map[1][2] = 1;
        map[2][2] = 1;
        map[3][2] = 1;
        map[4][2] = 1;
    }
    else if(letter == 'K')
    {
        map[0][0] = 1;
        map[1][0] = 1;
        map[2][0] = 1;
        map[3][0] = 1;
        map[4][0] = 1;

        map[2][1] = 1;

        map[0][2] = 1;
        map[1][2] = 1;
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
    else if(letter == 'O' || letter == '0' || letter == 'D')
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
    else if(letter == 'Q')
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
        map[1][2] = 1;
        map[2][2] = 1;
        map[3][2] = 1;
        map[4][2] = 1;
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
    else if(letter == 'S' || letter == '5')
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
    else if(letter == 'V')
    {
        map[0][0] = 1;
        map[1][0] = 1;
        map[2][0] = 1;
        map[3][0] = 1;

        map[4][1] = 1;

        map[0][2] = 1;
        map[1][2] = 1;
        map[2][2] = 1;
        map[3][2] = 1;
    }
    else if(letter == 'W')
    {
        map[0][0] = 1;
        map[1][0] = 1;
        map[2][0] = 1;
        map[3][0] = 1;
        map[4][0] = 1;

        map[3][1] = 1;

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
    else if(letter == 'Z')
    {
        map[0][0] = 1;
        map[3][0] = 1;
        map[4][0] = 1;

        map[0][1] = 1;
        map[0][1] = 1;
        map[2][1] = 1;
        map[4][1] = 1;

        map[0][2] = 1;
        map[1][2] = 1;
        map[4][2] = 1;
    }

    return;
}

static void imprimirChar(int x, int y)
{
    dcoord_t pos = {x,y};
    disp_write(pos,D_ON);
    disp_update();
    return;
}

static void imprimirWord(int x_start, int y_start, int cant_let, char word[4])
{
    int i,j;
    int len;
    int map[5][3];

    if(cant_let > 4)
    {
        cant_let = 4;       //entran maximo cuatro letras
    }
    for(len = 0; len < cant_let; len++)
    {
        letterToMat(map, word[len]);
        for (i = 0; i < 5; i++)
        {
            for(j = 0; j < 3; j++)
            {
                if(map[i][j])
                {
                    imprimirChar(x_start + j, i + y_start);       // en la fila 0 arranca
                }
            }
        }
        x_start += j + 1;
    }
    return;
}

static void imprimirSentencia(char oracion[32], int size, int x, int y, int rep)
{
    if(size > 32)       //32 es la palabra mas grande q tiene permitido msotrar
    {
        size = 32;
    }
    int i, j;
    char c;
    for(i = 0; i < size*rep - 4; i++)       //el -4 evita que la ultima palabra termine cortado ( 4 es la cant maxima d caracteres en pantalla)
        {
            disp_clear();
            imprimirWord(x, y, 4, oracion);
            usleep(200000);
            c = oracion[0];
            for(j = 0; j < size-1; j++)
            {
                oracion[j] = oracion[j+1];
            }
            oracion[size - 1] = c;
        }
}

static void saludar(void)
{
    printf("Ingrese su nombre:\n");

    char ingrese[] = {'i','n','g','r','e','s','e',' ','s','u',' ','n','o','m','b','r','e',' '};
    imprimirSentencia(ingrese, sizeof(ingrese) + 1, 0, 5, 1);

    char saludo[32];    // 12 CARACTERES PARA "BIENVENIDO", 20 PARA EL NOMBRE DEL PJ
    saludo[0] = ' ';
    saludo[1] = 'B';
    saludo[2] = 'I';    
    saludo[3] = 'E';    
    saludo[4] = 'N';    
    saludo[5] = 'V';  
    saludo[6] = 'E';    
    saludo[7] = 'N';    
    saludo[8] = 'I';    
    saludo[9] = 'D';    
    saludo[10] = 'O';
    saludo[11] = ' ';        

    
    char input = 0;
    int len = 0;
    while(input != '\n' && len < 20)
    {
        input = getchar();
        saludo[12 + len] = input;
        len++;
        
    }
    len+=12; //len ahora vale la longitud total
    imprimirSentencia(saludo, len, 1, 5, 1);
    return;
}