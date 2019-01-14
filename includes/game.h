#include <stdio.h>
#ifndef GAME_H
#define GAME_H

#define TIMER 40
#define MAX_OBSTICLES 6

extern float rotationPar;
extern float translationPar;
extern int animation_ongoing;
extern int level;
extern FILE* obst;

typedef struct {
    float y_coord;
    int num_of_obstacles_in_row;
    float x_coords[MAX_OBSTICLES];
    float x_vectors[MAX_OBSTICLES];
} Obstacle;

typedef struct {
    float x_coord;
    float y_coord;
} ExtraLife;

void on_keyboard(unsigned char key, int x, int y);
void on_reshape(int width, int height);
void on_display(void);
void on_timer(int value);


void initialize(void);
void detectCollisionWithRoad();
void detecCollisionWihtObstacles(int i);
void detectCollisionWithExtraLife();

void setLight();

void drawBall(void);
void drawObstacle(void);
void drawRoad(void);
void drawExtraLife();

void initialValues();
void generateRandomsforObstacles();
void generateRandomsForObstaclesinLine(int i);
void generateRadnomsForExtraLife();
void generateFirstObstacles();
void generateObstacles(int i);

void writeMessage();
void writeScore();
void writeFinishMessage();
#endif
