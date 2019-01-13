#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include "../includes/game.h"


int main(int argc, char** argv) {
    if(argc != 2) {
        fprintf(stderr, "Unesite nivo 1, 2 ili 3 kao argument komandne linije\n");
        exit(1);
    }
    level = atoi(argv[1]);
    if(level < 1 || level > 3){
        fprintf(stderr, "Postoje nivoi samo od 1 do 3\n");
        exit(1);
    }
    obst = fopen("obstacles.txt", "r");
    if(obst == NULL) {
        fprintf(stderr, "Neuspelo otvaranje drugog fajla\n");
        exit(1);
    }
    glutInit(&argc, argv);
    initialize();
    
    initialValues();
    //generateRandomsforObstacles();
    generateFirstObstacles();
    
    glutMainLoop();
    fclose(obst);
    return 0;
}
