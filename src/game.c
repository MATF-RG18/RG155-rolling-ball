#include "../includes/game.h"
#include <math.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

//postavljaju se inicijalne vrednosti za parametre
float animationPar = 0;
float translationPar = 0;
int animation_ongoing = 0;
int timer_id = 0;
int score = 0;
int level = 0;
FILE* obst = NULL;

//pravi se niz struktura
Obstacle obstacles[5];

//postavljamo inicijalne y koordinate gornje ivice svake trake
double linesPoints[5] = {3.425, 2.4, 1.375, 0.35, -0.675};

//postavljaju se inicijalne vrednosti za y koordinate svakog reda prepreka
void initialValues() {
    int i;
    obstacles[0].y_coord = 2.4;
    for(i = 1; i < 5; i++)
        obstacles[i].y_coord = obstacles[i-1].y_coord + 1.5;
}

//generisemo prvih 5 linija prepreka
void generateFirstObstacles() {
    int i, j, k, warning;
    FILE* f = fopen("firstObstacles.txt", "r");
    if(f == NULL) {
        fprintf(stderr, "Neuspesno otvaranje prvog fajla\n");
        exit(1);
    }
    srand(time(NULL));
    for(i = 0; i < 5; i++) {
        //uzimamo nasumican broj prepreka u redu u zavisnosti od nivoa
        obstacles[i].num_of_obstacles_in_row = rand() % 4 + level;
        fscanf(f, "%f", &obstacles[i].x_coords[0]);
        for(j = 1; j < obstacles[i].num_of_obstacles_in_row; j++) {
            float tmp;
            fscanf(f, "%f", &tmp);
            for(k = 0; k < j; k++){
                float res = obstacles[i].x_coords[k] - tmp;
                if(res < 0)
                    res = -res;
                //ako se dve prepreke preklapaju postavljamo warning na 1 i ponovo trazimo nasumicnu x koordinatu za istu prepreku
                
                if(res < 0.3){
                    warning = 1;
                }
            }
            if(warning == 0)
                obstacles[i].x_coords[j] = tmp;
            else
                j--;
            warning = 0;
        }
    }
    fclose(f);
}

//pravimo nasumican broj prepreka i postavljamo ih na nasumicne x koordinate tako da se ne preklapaju
void generateRandomsforObstacles() {
    srand(time(NULL));
    int i, j, k, warning = 0;
    float tmp;
    //generisemo nasumican broj prepreka izmedju 3 i 6
    for(i = 0; i < 5; i++) {
        obstacles[i].num_of_obstacles_in_row = rand() % 4 + 3;
        
        //za svaku prepreku generisemo nasumcnu x koordinatu i proveravamo da li su udaljene jedna od druge za 2r
        obstacles[i].x_coords[0] = (float)(rand() % 351 - 175) / 100;
        for(j = 1; j < obstacles[i].num_of_obstacles_in_row; j++) {
            tmp = (float)(rand() % 351 - 175) / 100;
            for(k = 0; k < j; k++){
                float res = obstacles[i].x_coords[k] - tmp;
                if(res < 0)
                    res = -res;
                //ako se dve prepreke preklapaju postavljamo warning na 1 i ponovo trazimo nasumicnu x koordinatu za istu prepreku
                
                if(res < 0.3){
                    warning = 1;
                }
            }
            if(warning == 0)
                obstacles[i].x_coords[j] = tmp;
            else
                j--;
            warning = 0;
        }
    }
}

void on_keyboard(unsigned char key, int x, int y) {
    
    (void)x;
    (void)y;
    switch (key) {
        //esc izlazak iz igrice
        case 27:
            exit(0);
            break;
        //pauza
        case 'p':
        case 'P':
            animation_ongoing = 0;
            break;
            
        //pocetak igre i nastavak posle pauze
        case 'o':
        case 'O':
            if(!animation_ongoing) {
                animation_ongoing = 1;
                glutTimerFunc(TIMER, on_timer, timer_id);
            }
            break;
        //pomeranje u levo
        case 'a':
            translationPar += 0.1;
            break;
        //pomeranje u desno
        case 'd':
            translationPar -= 0.1;
            break;
    }
}

void on_display(void) {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //postavlja se kamera
    gluLookAt(0, -1, 3, 0, 0, 0, 0, 1, 0);
    
    setLight();
    
    //crtava se put i sfera
    drawRoad();
    //crtaju se prepreke
    drawObstacle();
    //crta se glavna sfera
    drawBall();
    writeScore();
    glutSwapBuffers();
}

//resavamo koliziju sa ivicama puta
void detectCollisionWithRoad() {
    if(translationPar < -1.7)
        translationPar = -1.7;
    else if(translationPar > 1.7)
        translationPar = 1.7;
}

//funkcija iscrtavanja glavne sfere
void drawBall(void) {
    
    //postavljnje materijala za glavnu sferu
    GLfloat ambientCoeffs[] = { 0.0215, 0.1745, 0.0215, 1 };
    GLfloat diffuseCoeffs[] = { 0.07568, 0.61424, 0.07568, 1 };
    GLfloat specularCoeffs[] = { 0.633, 0.727811, 0.633, 1 };
    GLfloat shininess = 100;
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambientCoeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseCoeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularCoeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    
    //iscrtavamo loptu
    glPushMatrix();
        glTranslatef(0, -1, 0);
        detectCollisionWithRoad();
        glTranslatef(-translationPar, 0, 0);
        glRotatef(-animationPar, 1, 0, 0);
        glutSolidSphere(0.2, 20, 20);
    glPopMatrix();
    
}

//funkcija koja iscrtava prepreke
void drawObstacle(void) {
    
    //postavljanje materijala za prepreke
    GLfloat ambientCoeffs[] = { 0.1745, 0.01175, 0.01175, 1 };
    GLfloat diffuseCoeffs[] = { 0.61424, 0.04136, 0.04136, 1 };
    GLfloat specularCoeffs[] = { 0.727811, 0.626959, 0.626959, 1 };
    GLfloat shininess = 10;
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambientCoeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseCoeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularCoeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    int i, j;
    for(i = 0; i < 5; i++) {
        //umanjujemo y koordinatu, simuliramo kretanje
        obstacles[i].y_coord -= 0.045;
        //ako je linija prepreka izasla van ekrana, transliramo je i postavljamo nove nasumicne prepreke za tu liniju
        if(obstacles[i].y_coord < -1.8) {
            obstacles[i].y_coord = 5.7;
            //generateRandomsForObstaclesinLine(i);
            generateObstacles(i);
        }
        //ako su prepreke blizu lopte po y osi detektujemo kolizije
        if(obstacles[i].y_coord <= -0.8 && obstacles[i].y_coord >= -1.1) 
            detecCollisionWihtObstacles(i);
        
        //iscrtavaju se prepreke
        for(j = 0; j < obstacles[i].num_of_obstacles_in_row; j++) {
            glPushMatrix();
                glTranslatef(obstacles[i].x_coords[j], obstacles[i].y_coord, 0);
                glRotatef(-20, 1, 0, 0);
                glutSolidCone(0.15, 0.8, 20, 20);
            glPopMatrix();
        }
    }
    
}

//detektujemo kolizije sa preprekama
void detecCollisionWihtObstacles(int i) {
    int j;
    float ballPosition = -translationPar, res;
    for(j = 0; j < obstacles[i].num_of_obstacles_in_row; j++){
        res = obstacles[i].x_coords[j] - ballPosition;
        if(res < 0)
            res = -res;
        //ako je doslo do kolizije zavrsavamo sa igrom
        if(res < 0.28){
            animation_ongoing = 0;
            timer_id = -1;
            writeMessage();
        }
    }
}

//funkcija koja ispisuje poruku za kraj igre
void writeMessage() {
    glDisable(GL_LIGHTING);
    char *s = "GAME OVER";
    int i, n = strlen(s);
    glPushMatrix();
        glColor3f(0, 0, 0);
        glRasterPos3f(-0.2, 0, 1);
        
        for(i = 0; i < n; i++){
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, s[i]);
        }
    glPopMatrix();
    glEnable(GL_LIGHTING);
}
//ispisujemo poruku na kraju predjenog nivoa
void writeFinishMessage() {
    glDisable(GL_LIGHTING);
    char *s = "CESTITAMO, PRESLI STE NIVO";
    int i, n = strlen(s);
    glPushMatrix();
        glColor3f(0, 0, 0);
        glRasterPos3f(-0.4, 0, 1);
        
        for(i = 0; i < n; i++){
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, s[i]);
        }
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

//generisemo prepreke
void generateObstacles(int i) {
    int j, k, warning;
    //uzimamo nasumican broj prepreka u redu u zavisnosti od nivoa
        obstacles[i].num_of_obstacles_in_row = rand() % 4 + level;
        if(EOF == fscanf(obst, "%f", &obstacles[i].x_coords[0])) {
            writeFinishMessage();
            animation_ongoing = 0;
            timer_id = -1;
        }
        for(j = 1; j < obstacles[i].num_of_obstacles_in_row; j++) {
            float tmp;
            if(EOF == fscanf(obst, "%f", &tmp)){
                writeFinishMessage();
                animation_ongoing = 0;
                timer_id = -1;
                break;
            }
            for(k = 0; k < j; k++){
                float res = obstacles[i].x_coords[k] - tmp;
                if(res < 0)
                    res = -res;
                //ako se dve prepreke preklapaju postavljamo warning na 1 i ponovo trazimo nasumicnu x koordinatu za istu prepreku
                
                if(res < 0.3){
                    warning = 1;
                }
            }
            if(warning == 0)
                obstacles[i].x_coords[j] = tmp;
            else
                j--;
            warning = 0;
        }
}

//generise se nasumican broj prepreka i njihova pozicija nakon sto izadju iz ekrana
void generateRandomsForObstaclesinLine(int i) {
    
    int j, k, warning = 0;
    float tmp;
    obstacles[i].num_of_obstacles_in_row = rand() % 4 + 3;
        
    //za svaku prepreku generisemo nasumcnu x koordinatu i proveravamo da li su udaljene jedna od druge za 2r
    obstacles[i].x_coords[0] = (float)(rand() % 351 - 175) / 100;
    for(j = 1; j < obstacles[i].num_of_obstacles_in_row; j++) {
        tmp = (float)(rand() % 351 - 175) / 100;
        for(k = 0; k < j; k++){
            float res = obstacles[i].x_coords[k] - tmp;
            if(res < 0)
                res = -res;
                
            if(res < 0.3){
                warning = 1;
            }
        }
        if(warning == 0)
            obstacles[i].x_coords[j] = tmp;
        else
            j--;
        warning = 0;
        }
}

//funkcija iscrtavanja puta
void drawRoad(void) {
    //postavljanje materijala za put
    GLfloat ambientCoeffs[] = { 0.3, 0.3, 0.3, 1 };
    GLfloat diffuseCoeffs[] = { 0.45, 0.45, 0.45, 1 };
    GLfloat specularCoeffs[] = { 0.55, 0.55, 0.55, 1 };
    GLfloat shininess = 10;
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambientCoeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseCoeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularCoeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    //iscrtavamo put
    glPushMatrix();
    glBegin(GL_POLYGON);
        glVertex3f(2, -3, -0.1);
        glVertex3f(-2, -3, -0.1);
        glVertex3f(-2, 3, -0.1);
        glVertex3f(2, 3, -0.1);
    glEnd();
    glPopMatrix();
    
    glDisable(GL_LIGHTING);
    
    glColor3f(1, 1, 1);
    
    //crtamo isprekidanu belu liniju po sredini puta, kada jedna traka izadje iz ekrana transliramo je
    //na pocetak
    int i;
    for(i = 0; i < 5; i++) {
        if(linesPoints[i] - 0.01 < -1.7)
            linesPoints[i] = 3.425;
        else
            linesPoints[i] -= 0.045;
        glBegin(GL_POLYGON);
            glVertex3f(0.1, linesPoints[i] - 0.7, 0);
            glVertex3f(-0.1, linesPoints[i] - 0.7, 0);
            glVertex3f(-0.1, linesPoints[i], 0);
            glVertex3f(0.1, linesPoints[i], 0);
        glEnd();
    }
    glEnable(GL_LIGHTING);
}

//postavljanje osvetljenja
void setLight(){
    GLfloat lightPosition[] = { 0, -2, 3, 0 };
    GLfloat lightAmbient[] = { 0.1, 0.1, 0.1, 1 };
    GLfloat lightDiffuse[] = { 0.7, 0.7, 0.7, 1 };
    GLfloat lightSpecular[] = { 0.9, 0.9, 0.9, 1 };

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
}

//ispisujemo skor
void writeScore() {
    glDisable(GL_LIGHTING);
    score++;
    char *s = "Score: ";
    int i, n = strlen(s);
    glPushMatrix();
        glColor3f(1, 1, 1);
        glRasterPos3f(-1.7, 1, 1);
        
        for(i = 0; i < n; i++){
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, s[i]);
        }
    glPopMatrix();
    
    char sc[12];
    for(i = 0; i < 12; i++)
        sc[i] = '\0';
    //ispisujemo score - 2 da bi ispisani score krenuo od 0
    sprintf(sc, "%d", score - 2);
    n = strlen(sc);
    glPushMatrix();
        glColor3f(1, 1, 1);
        glRasterPos3f(-1.45, 1, 1);
        for(i = 0; i < 12; i++){
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, sc[i]);
        }
    glPopMatrix();
    
    
    glEnable(GL_LIGHTING);
}

void on_timer(int value) {
    if(value != timer_id)
        return;
    
    //uvecavamo animacioni parametar u svakom pozivu on_timer funkcije
    animationPar += 20;
    if(animationPar >= 2880)
        animationPar = 0;

    glutPostRedisplay();
    
    if(animation_ongoing)
        glutTimerFunc(TIMER, on_timer, timer_id);
}

//glut inicijalizacija 

void initialize(void) {
    
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    //postavljanje velicine prozora
    glutInitWindowSize(1000, 1000);
    //postavljanje pozicije prozora
    glutInitWindowPosition(400, 500);
    glutCreateWindow("Little ball game");
    
    glutKeyboardFunc(on_keyboard);
    
    glutReshapeFunc(on_reshape);
    glutDisplayFunc(on_display);
    
    glClearColor(0, 0, 0, 0);
    glEnable(GL_DEPTH_TEST);
    
}

void on_reshape(int width, int height) {
    
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float) width / height, 1, 40);
}
