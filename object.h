#ifndef OBJECT_H
#define OBJECT_H

#ifndef RASPI
    /*  Librerias de allegro  */
    #include <allegro5/allegro.h>
    #include <allegro5/allegro_image.h>
    #include <allegro5/allegro_primitives.h>
    #include <allegro5/allegro_color.h>
    #include <allegro5/allegro_audio.h>
    #include <allegro5/allegro_acodec.h>
    #include <allegro5/allegro_font.h> 
    #include <allegro5/allegro_ttf.h> 
#endif

    typedef struct
    {
        float* x;                //puntero a FLOAT (puntero a  primer elemento de un arreglo de floats)
        int y;                  //coordenada y
        float speed;            //velocidad (en cada vuelta, el objeto se movera "speed" veces a su derecha, acepta negaativos).
        int separation;         //separación entre objetos del mismo tipo en pantalla
        int screen_rep;         //cantidad maxima de veces que se muestra en pantalla al mismo tiempo
        int cant_squares;       //variable que guarda el valor de la cantidad de cuadrados que ocupa en cada vehiculo
#ifndef RASPI
        ALLEGRO_BITMAP* image;
#endif        
    }object_t;
    
    typedef struct          //estructura para seleccionar opciones [cuadradito amarillo]
    {
        int x_start;        //donde comienza coordenada x
        int y_start;        //donde termina coordenada x
        int x_end;          //donde empieza coordenada y
        int y_end;          //donde termina coordenada y
        int pressed;        //si se apreto o no
    } selection_t;

    enum ON_OFF {OFF = 0, ON = 1};
    enum MENUS {GAME = 20, MAIN_MENU, NICKNAME_MENU, HALL_OF_FAME_MENU, CHOOSE_FROG_MENU, PAUSE_MENU, GAMEOVER, WIN};
    
    #define OBJ_MAX 10      //cant maxima de objets
    enum MYOBJECTS          //macros asociadas a los objetos
    {
        FROG, CAR1, CAR2, CAR3, CAR4, TRUCK, TORTU1, TORTU2, TRONCO1, TRONCO2, FROG2
    };
    //   0      1     2     3    4      5       6       7       8       9      10



#endif 
