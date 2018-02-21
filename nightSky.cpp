/////////////////////////////////////////////////////
// nightSky.cpp
// Author:  Lloyd Poor
// Nickname : none
// Email: lspoor@syr.edu
// Assignment: Homework 1
// Environment/Compiler: XCode
// Date: February 5, 2018
// Refrences: None other than Textbook/Exercises and Lecture Notes/Class work
//
// This program draws a person with several interaction which change the person and the environment.
//
//  Interaction:
//  Arrow keys will move the moon
//  S/s will scale the moon up and down respectively
//  m will roate the moon one step at a time M will animate it to be automated
//  K/k will rotate the sky right and left
//  g will rotate the galaxies one step at a time G will animate it to be automated
//  p will change the view from parallel to perspective, I did not program this animation with
//      perspective in mind after I saw its odd effects on transitions, I don't suggest its use
//  r will reset the moon's position and phase to its default, being the origin and half
//  The ring around the blue planet when you press the 't' ket
//  +/- will reduce and add the amount of stars visible in the sky
//  The mouse have many functionalities depending on which window and where you are using it
//      in the Display window you can click and drag the moon to change its position as well as
//      right clicking to see the menu option such as change day to night, see wired and filled modes
//      and more. In the control panel it will simple act as a selector for the button objects.
/////////////////////////////////////////////////////

#include <cmath>
#include <iostream>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#define PI 3.14159265

using namespace std;

// Globals.
static int id1, id2; // Window identifiers.
static GLsizei width1, height1, width2, height2;
static long font = (long)GLUT_BITMAP_8_BY_13;
static float moonRadius; //Simple declaration of moon radius size, initialized later
static float Angle = 0; // Angle to rotate the moon.
static bool moonRotate = 0; //Boolean used to animate the moon and keep it in rotation
static float moonX = 0, moonY = 0; //Position for the moon used for the transition of the moon and the moving of the moon with the mouse.
static float Scale = 1; //Scale factor for the moon
static int radioSelect = 3; //This is the variable which is used to choose which radio button to show
static bool isDay = 0;  //Simple boolean to tell whether to show the day or night background
static float galaxyAngle = 0;   //The angle variable to spin the galaxies
static bool galaxyRotate = 0;   //A boolean used to keep glaxies in motion for animation
static int animationPeriod = 50;    //The period between each antimation refresh
static float skyRotate = 0; //The angle by which the entire pciture (i.e. the sky) is rotated on.
static bool perspective = 1;    //The boolean to change perspective views.
static bool isWire = 0; //Boolean to show the wire nature of the moon and other planets

#define numStars 250    //A definition of the number of stars possible to show


void writeBitmapString(void *font, const char *string)
{
    const char *c;
    for (c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
}

void drawSolidCircle(float x, float y, float z, float r){
    float t;
     
    //glColor3f(0.0,0.0,0.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(x, y, z);
    for(int i = 0; i <= 40; ++i){
        t = (2 * PI * i)/ 40;
        glVertex3f(x + cos(t) * r, y + sin(t) * r, z);
    }
    glEnd();
}
void drawRadioButton(float x, float y){ //Simple line loop circle created at a given x,y
    glColor3f(0.0, 0.0, 0.0);
    
    float r = 6, t = 0;
    glBegin(GL_LINE_LOOP);
    for(int i = 0; i < 40; ++i)
    {
        //glColor3ub(rand()%256, rand()%256, rand()%256);
        glVertex3f(x + r * cos(t), y + r * sin(t), 0.0);
        t += 2 * PI / 40;
    }
    glEnd();
}
void fillRadio(int bnum){ //depending on which number is given this function will fill one
    switch(bnum){           // and only one of the radio buttons to match the phase of the moon
        case 0:
            break;
        case 1: //New;
            drawSolidCircle(-(width1/2)+20, (height1/2)-28, 0.0, 4);
            glutSetWindow(id1);
            glutPostRedisplay();
            break;
        case 2: //Cresent
            drawSolidCircle(-(width1/2)+20, (height1/2)-44 ,0.0, 4);
            glutSetWindow(id1);
            glutPostRedisplay();
            break;
        case 3: //Half
            drawSolidCircle(-(width1/2)+20, (height1/2)-60, 0.0, 4);
            glutSetWindow(id1);
            glutPostRedisplay();
            break;
        case 4: //Gibbous
            drawSolidCircle(-(width1/2)+20, (height1/2)-76, 0.0, 4);
            glutSetWindow(id1);
            glutPostRedisplay();
            break;
        case 5: //Full
            drawSolidCircle(-(width1/2)+20, (height1/2)-92, 0.0, 4);
            glutSetWindow(id1);
            glutPostRedisplay();
            break;
        default:
            break;
            
    }
}
void checkRadio(){  //Based on the angle of the moon's rotation will reset the radio button selection
    if(Angle == 270) radioSelect = 1;       //variable accordingly
    if(Angle == 210 || Angle == 330) radioSelect = 2;
    if(Angle == 0 || Angle == 180) radioSelect = 3;
    if(Angle == 30 || Angle == 150) radioSelect = 4;
    if(Angle == 90) radioSelect = 5;
}

static bool isChecked[] = {0,0,0,0,0};  // Boolean arry allows us to keep track of multiple
void drawX(float x, float y){           // T/F values of the check boxes without having to name many
    int d = 5;                          // varibales
    glBegin(GL_LINES);
    glVertex3f(x + d, y + d, 0.0);      //Draws an x given a center point, which should also be the center of the box
    glVertex3f(x - d, y - d, 0.0);
    
    glVertex3f(x - d, y + d, 0.0);
    glVertex3f(x + d, y - d, 0.0);
    glEnd();
}
void drawCheckBox(float x, float y){    //Draws a box given a center point
    glColor3f(0.0, 0.0, 0.0);
    int d = 5;
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_POLYGON);
    glVertex3f(x + d, y + d, 0.0);
    glVertex3f(x - d, y + d, 0.0);
    glVertex3f(x - d, y - d, 0.0);
    glVertex3f(x + d, y - d, 0.0);
    glEnd();
}
void checkBoxes(){      //Checks a box's checked status if it is true it will mark that box with an X
    if(isChecked[0]){
        drawX(-(width1/2)+20, (height1/2)-140);
        glutSetWindow(id1);
        glutPostRedisplay();
    }
    if(isChecked[1]){
        drawX(-(width1/2)+20, (height1/2)-156);
        glutSetWindow(id1);
        glutPostRedisplay();
    }
    if(isChecked[2]){
        drawX(-(width1/2)+20, (height1/2)-172);
        glutSetWindow(id1);
        glutPostRedisplay();
    }
    if(isChecked[3]){
        drawX(-(width1/2)+20, (height1/2)-188);
        glutSetWindow(id1);
        glutPostRedisplay();
    }
    if(isChecked[4]){
        drawX(-(width1/2)+20, (height1/2)-204);
        glutSetWindow(id1);
        glutPostRedisplay();
    }
}

// Drawing routine for first window.
void drawScene1(void)       //The positions of the buttons are relative to the window size
{                           // They will always be in the upper left had corner and evenly spaced from
    // Choose window.       // one another not matter the size, obeviously if the user makes the
    glutSetWindow(id1);     // window too small the text and buttons will not be visible.
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos3f(-(width1/2)+16, (height1/2)-16 ,0.0);
    writeBitmapString((void*)font, "Moon Phases:");
    
    drawRadioButton(-(width1/2)+20, (height1/2)-28);
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos3f(-(width1/2)+32, (height1/2)-32 ,0.0);
    writeBitmapString((void*)font, "New");
    
    drawRadioButton(-(width1/2)+20, (height1/2)-44);
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos3f(-(width1/2)+32, (height1/2)-48 ,0.0);
    writeBitmapString((void*)font, "Cresent");
    
    drawRadioButton(-(width1/2)+20, (height1/2)-60);
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos3f(-(width1/2)+32, (height1/2)-64 ,0.0);
    writeBitmapString((void*)font, "Half");
    
    drawRadioButton(-(width1/2)+20, (height1/2)-76);
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos3f(-(width1/2)+32, (height1/2)-80 ,0.0);
    writeBitmapString((void*)font, "Gibbous");
    
    drawRadioButton(-(width1/2)+20, (height1/2)-92);
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos3f(-(width1/2)+32, (height1/2)-96 ,0.0);
    writeBitmapString((void*)font, "Full");
    
    fillRadio(radioSelect); //makes sure the readio button status variable is properly assigned
    checkRadio();           //before filling the button on the control panel
    
    
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos3f(-(width1/2)+16, (height1/2)-128 ,0.0);
    writeBitmapString((void*)font, "Stallar Objects:");
    
    drawCheckBox(-(width1/2)+20, (height1/2)-140);
    glRasterPos3f(-(width1/2)+32, (height1/2)-144 ,0.0);
    writeBitmapString((void*)font, "Stars");
    
    drawCheckBox(-(width1/2)+20, (height1/2)-156);
    glRasterPos3f(-(width1/2)+32, (height1/2)-160 ,0.0);
    writeBitmapString((void*)font, "Galaxies");
    
    drawCheckBox(-(width1/2)+20, (height1/2)-172);
    glRasterPos3f(-(width1/2)+32, (height1/2)-176 ,0.0);
    writeBitmapString((void*)font, "Little Dipper");
    
    drawCheckBox(-(width1/2)+20, (height1/2)-188);
    glRasterPos3f(-(width1/2)+32, (height1/2)-192 ,0.0);
    writeBitmapString((void*)font, "Big Dipper");
    
    drawCheckBox(-(width1/2)+20, (height1/2)-204);
    glRasterPos3f(-(width1/2)+32, (height1/2)-208 ,0.0);
    writeBitmapString((void*)font, "Planet");
    
    checkBoxes();
    
    glutSwapBuffers();
}

void drawMoon(){

    double eqn0[4] = {1.0, 0.0, 0.0, 0.0}; // Data for clipping plane 0. Clips the right side
    double eqn1[4] = {-1.0, 0.0, 0.0, 0.0}; // Data for clipping plane 1. Clips the left side
    double eqn2[4] = {1.0, 0.0, 0.0, 47.0};
    double eqn3[4] = {1.0, 0.0, 0.0, -47.0};
    moonRadius = 50;
    
    glClipPlane(GL_CLIP_PLANE0, eqn0);
    glClipPlane(GL_CLIP_PLANE1, eqn1);
    glClipPlane(GL_CLIP_PLANE2, eqn2);
    glClipPlane(GL_CLIP_PLANE3, eqn3);
    
    glPushMatrix(); //Making sure the translations,rotations,and scaling doesn't effect anything else
    
    glTranslatef(moonX, moonY, -150.0);
    glRotatef(Angle, 0.0, 1.0, 0.0);
    glScalef(Scale, Scale, Scale);
    
    glEnable(GL_CLIP_PLANE0);
    glColor3f(0.1, 0.1, 0.2);
    //glRotatef(90, 1.0, 0.0, 0.0);
    glutSolidSphere(moonRadius, 32, 20);
    glDisable(GL_CLIP_PLANE0);
    
    glEnable(GL_CLIP_PLANE1);
    glEnable(GL_CLIP_PLANE2);       //Clips the moon, leaving a hole
    glColor3f(0.8, 0.8, 0.9);
    //glRotatef(90, 1.0, 0.0, 0.0);
    glutSolidSphere(moonRadius, 32, 20);
    glDisable(GL_CLIP_PLANE2);
    glDisable(GL_CLIP_PLANE1);
    
    glEnable(GL_CLIP_PLANE3);           //Creates a sphere outside of the moon and
    //glPushMatrix();                   //clips it to fit into the hole left on the moon
    glColor3f(0.6, 0.6, 0.7);           // leaving a concave "crater" shape in the moon.
    glTranslatef(-94.0, 0.0, 0.0);
    glutSolidSphere(moonRadius, 32, 20);
    //glPopMatrix();
    glDisable(GL_CLIP_PLANE3);
    
    glPopMatrix();
    
}

static float starX[numStars];   //A list of randomly assigned x coords for stars
static float starY[numStars];   //A list of randomly assigned y coords for stars
static int showStars = 200;     //200 of the 250 possible stars will be seen
void starPos(){
    for(int i = 0; i < showStars; i++){
        starX[i] = rand() % ((width2/2) - (-(width2/2)) + 1) + (-(width2/2));
        starY[i] = rand() % ((height2/2) - (-(height2/2)) + 1) + (-(height2/2));
    }
}
void drawStars(int num){
    int x,y;
    //glPushMatrix();
    for(int i = 0; i < num; i++){
        drawSolidCircle(starX[i], starY[i], -198.0, 3); //Simple circle to symbolize stars
    }
    //glPopMatrix();
}

void drawGalaxy(int arms){
    float t; // Angle parameter.
    
    glPushMatrix();
    glLineWidth(5);
    //glColor3f(0.9, 0.9, 1.0);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0x1111);       //Creates a stippling effect to emulate stars in a galaxy

    glRotatef(galaxyAngle, 0.0, 0.0, 1.0);  //How the galaxies rotate
    
    for(int i = 0; i < 360.0; i += 360.0/arms){ //Evenly separated the arms of the galaxy
        glPushMatrix();
        glRotatef(i, 0.0, 0.0, 1.0);
        glBegin(GL_LINE_STRIP);
        for(t = 0 * PI; t <= 2 * PI; t += (PI/20.0))    // Creates a spiral similar to a helix, only
            glVertex3f(10 * t * cos(t),10 * t * sin(t), 0); // It moves inwards instead of in the z direction
        glEnd();
        glPopMatrix();
    }
    glDisable(GL_LINE_STIPPLE);
    
    glPopMatrix();
    
}

static float ringAngle = 45;
static float orbitAngle = 0;
static bool isOrbit = 0;
void drawPlanet(){
    float angle, N = 40;
    glColor3f(0.0, 0.8, 0.8);
    glutSolidSphere(40, 32, 20);
    
    glPushMatrix();
    glRotatef(ringAngle, 1.0, 1.0, 1.0);
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_TRIANGLE_STRIP);
    for(int i = 0; i <= N; ++i)
    {
        angle = 2 * PI * i / N;
        glVertex3f(cos(angle) * 45.0,sin(angle) * 45.0, 0.0);
        glVertex3f(cos(angle) * 50.0,sin(angle) * 50.0, 0.0);
    }
    glEnd();
    glPopMatrix();
}


// Drawing routine for second window.
void drawScene2(void)
{
    // Choose window.
    glutSetWindow(id2);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(perspective) //Change between perspective and parrallel views
        glOrtho(-(width2/2), width2/2, -(height2/2), height2/2, -1.0, 200.0);
    else glFrustum(-(width2/2), width2/2, -(height2/2), height2/2, 20, 200.0);
    
    if (isWire) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  //Change between wired and filled views
    else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    glEnable(GL_DEPTH_TEST);
    
    glRotatef(skyRotate, 0.0, 0.0, 1.0);    //Will rotate everything on screen because it hasn't been
                                            //removed from the matrix
    if(isDay){
        glBegin(GL_POLYGON);                //Simple background the size of the window.
        glColor3f(1.0, 0.1, 0.2);
        glVertex3f(-width2, -height2, -199.0);
        glVertex3f(width2, -height2, -199.0);
        glColor3f(1.0, 1.0, 0.2);
        glVertex3f(width2, height2, -199.0);
        glVertex3f(-width2, height2, -199.0);
        glEnd();
    }
    
    drawMoon();     //calls the draw moon fuction, most functionality to it is in the function
    
    if(isChecked[0]){   //Will only draw stars if the button is selected
        //starPos();
        glColor3f(0.980, 0.980, 0.824); //A slight yellow color
        drawStars(showStars);
    }
    
    if(isChecked[1]){       //Will only show galaxies if the button is pressed
        glPushMatrix();         //Push and pop respectively to keep relative translations
        glColor3f(0.867, 0.627, 0.867);
        glTranslatef(200.0, 180.0, -197.0);
        //glRotatef(galaxyAngle, 0.0, 0.0, 1.0);
        drawGalaxy(4);
        glPopMatrix();
        
        glPushMatrix();
        glColor3f(0.847, 0.749, 0.847);
        glTranslatef(-90.0, -50.0, -197.0);
        glScalef(0.5, 0.5, 0.5);
        drawGalaxy(3);
        glPopMatrix();
        
        glPushMatrix();
        glColor3f(0.902, 0.902, 0.980);
        glTranslatef(260.0, -280.0, -197.0);
        glScalef(4.0, 4.0, 0.0);
        drawGalaxy(6);
        glPopMatrix();
        
        glPushMatrix();
        glColor3f(0.9, 0.9, 1.0);
        glTranslatef(-210.0, 220.0, -197.0);
        glScalef(2.0, 2.0, 0.0);
        drawGalaxy(5);
        glPopMatrix();
    }
    
    if(isChecked[2]){       //Will only show little dipper if button is pressed
        glLineWidth(2.0);
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINE_STRIP);
        glVertex3f(110, 130, -150);     //Lines making up the constilation
        glVertex3f(80, 140, -150);
        glVertex3f(50, 145, -150);
        glVertex3f(20, 135, -150);
        glVertex3f(-10, 115, -150);
        glVertex3f(-15, 130, -150);
        glVertex3f(15, 148, -150);
        glVertex3f(20, 135, -150);
        glEnd();
        
        drawSolidCircle(110, 130, -150, 3);         //stars to hilight the intersections
        drawSolidCircle(80, 140, -150, 3.0);
        drawSolidCircle(50, 145, -150, 3.0);
        drawSolidCircle(20, 135, -150, 3.0);
        drawSolidCircle(-10, 115, -150, 3.0);
        drawSolidCircle(-15, 130, -150, 3.0);
        drawSolidCircle(15, 148, -150, 3.0);
    }
    
    if(isChecked[3]){           //Will only show big dipper if button is pressed (same as little dipper)
        glLineWidth(2.0);
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINE_STRIP);
        glVertex3f(-110, 30, -150);
        glVertex3f(-95, 20, -150);
        glVertex3f(-90, 10, -150);
        glVertex3f(-83, -5, -150);
        glVertex3f(-88, -15, -150);
        glVertex3f(-68, -30, -150);
        glVertex3f(-60, -22, -150);
        glEnd();
        
        drawSolidCircle(-110, 30, -150, 3.0);
        drawSolidCircle(-95, 20, -150, 3.0);
        drawSolidCircle(-90, 10, -150, 3.0);
        drawSolidCircle(-83, -5, -150, 3.0);
        drawSolidCircle(-88, -15, -150, 3.0);
        drawSolidCircle(-68, -30, -150, 3.0);
        drawSolidCircle(-60, -22, -150, 3.0);
        
    }
    
    if(isChecked[4]){
        glPushMatrix();
        glRotatef(orbitAngle, 0.0, 0.0, 1.0);
        glTranslatef(100.0, 100.0, -100.0);
        drawPlanet();
        glPopMatrix();
    }
    
    glDisable(GL_DEPTH_TEST);
    
    glutSwapBuffers();
}

// Initialization routine for first window.
void setup1(void)
{
    // Black background.
    glClearColor(1.0, 1.0, 1.0, 0.0);
}

// Initialization routine for second window.
void setup2(void)
{
    glClearColor(0.2, 0.0, 0.6, 0.0);
}

// Reshape routine for first window.
void resize1(int w, int h)
{
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-(w/2), w/2, -(h/2), h/2, -1.0, 1.0);
    
    width1 = w;     //Global width and height of window 1
    height1 = h;
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Reshape routine for second window.
void resize2(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(perspective)
        glOrtho(-(w/2), w/2, -(h/2), h/2, -1.0, 200);   //Change in perspective views
    else
        glFrustum(-(w/2), w/2, -(h/2), h/2, 20, 200.0);
    
    width2 = w;     //Global width and height of window 2
    height2 = h;
    
    starPos();  //Called the fucntion to get the star positions here so it wouldn't regenerate
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Keyboard input processing routine shared by both windows.

void keyInput(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'r':   //Resets moon's variables to 0
            moonX = moonY = Angle = 0.0;
            radioSelect = 3;
            glutSetWindow(id2);
            glutPostRedisplay();
            break;
        case 'p':       //Changes the perspective view
            perspective = !perspective;
            glutSetWindow(id2);
            glutPostRedisplay();
            break;
        case 't':
            ringAngle += 5;
            glutSetWindow(id2);
            glutPostRedisplay();
            break;
        case 'o':
            orbitAngle += 5;
            glutSetWindow(id2);
            glutPostRedisplay();
            break;
        case 'O':
            isOrbit = !isOrbit;
            glutSetWindow(id2);
            glutPostRedisplay();
            break;
        case 'k':           //rotates the sky by 10 degrees
            skyRotate -= 10;
            glutSetWindow(id2);
            glutPostRedisplay();
            break;
        case 'K':
            skyRotate += 10;
            glutSetWindow(id2);
            glutPostRedisplay();
            break;
        case 's':       //Scales the moon down
            if(Scale > .1){  //Will not go to 0 so the moon will not disappear
                Scale -= 0.1;
                glutSetWindow(id2);
                glutPostRedisplay();
            }
            break;
        case 'S':           //Scales the moon up
            Scale += 0.1;
            glutSetWindow(id2);
            glutPostRedisplay();
            break;
        case 'G':                       //Rotate all galaxies in animation
            galaxyRotate = !galaxyRotate;
            glutSetWindow(id2);
            glutPostRedisplay();
            break;
        case 'g':                       //Manual rotation of all galaxies
            galaxyAngle += 10.0;
            glutSetWindow(id2);
            glutPostRedisplay();
            break;
        case 'M':                       //Animation of moon rotating
            moonRotate = !moonRotate;
            glutSetWindow(id2);
            glutPostRedisplay();
            break;
        case 'm':                       //Manual rotation of the moon
            Angle += 10.0;
            if(Angle >= 360) Angle -= 360;
            glutSetWindow(id2);
            glutPostRedisplay();
            break;
        case '+':                       //Will increase the number of stars you see
            if(showStars < numStars){
                showStars += 10;
                glutSetWindow(id2);
                glutPostRedisplay();
            }
            break;
        case '-':                       //Will decrease the number of stars you see
            if(showStars > 10){
                showStars -= 10;
                glutSetWindow(id2);
                glutPostRedisplay();
            }
            break;
        case 27:
            exit(0);
            break;
        default:
            break;
    }
}

void specialKeyInput(int key, int x, int y) //Will translate the moon with the arrow keys
{
    if(key == GLUT_KEY_UP) moonY += 10;     //Didn't really mean for this to be in here
    if(key == GLUT_KEY_DOWN) moonY -= 10;   //but it was part of my separate moon.cpp code when testing, so...
    if(key == GLUT_KEY_LEFT) moonX -= 10;
    if(key == GLUT_KEY_RIGHT) moonX += 10;
    glutPostRedisplay();
}

void mouseControl1(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)   //If the left button is clicked on the control panel
    {
        x = x - (width1/2); // Correct from mouse to OpenGL co-ordinates.
        y = (height1/2) - y;
        
        if(x >= (-(width1/2)+14) &&  x <= -(width1/2)+26){  //It will see if it is in a button and select the relevant button if it is, changing something on the display
            if(y >= (height1/2)-34 && y <= (height1/2)-22){
                radioSelect = 1;
                Angle = 270;
            }
            if(y >= (height1/2)-50 && y <= (height1/2)-38){
                radioSelect = 2;
                Angle = 330;
            }
            if(y >= (height1/2)-66 && y <= (height1/2)-54){
                radioSelect = 3;
                Angle = 0;
            }
            if(y >= (height1/2)-82 && y <= (height1/2)-70){
                radioSelect = 4;
                Angle = 30;
            }
            if(y >= (height1/2)-98 && y <= (height1/2)-86){
                radioSelect = 5;
                Angle = 90;
            }
        }
        
        if(x >= (-(width1/2)+15) &&  x <= -(width1/2)+25){
            if(y >= (height1/2)-145 && y <= (height1/2)-135){
                isChecked[0] = !isChecked[0];
            }
            if(y >= (height1/2)-161 && y <= (height1/2)-151 ){
                isChecked[1] = !isChecked[1];
            }
            if(y >= (height1/2)-177 && y <= (height1/2)-167 ){
                isChecked[2] = !isChecked[2];
            }
            if(y >= (height1/2)-193 && y <= (height1/2)-183 ){
                isChecked[3] = !isChecked[3];
            }
            if(y >= (height1/2)-209 && y <= (height1/2)-199 ){
                isChecked[4] = !isChecked[4];
            }
        }
    }
    //cout << radioSelect << endl;
    glutSetWindow(id1);     //Updates both windows
    glutPostRedisplay();
    glutSetWindow(id2);
    glutPostRedisplay();
}

void mouseControl2(int button, int state, int x, int y)
{
    glutSetWindow(id2);
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)   //Origianlly just used this for testing coordinated we're in the right place as I was setting them with my x and ys for the mouse
    {
        x = x - (width2/2); // Correct from mouse to OpenGL co-ordinates.
        y = (height2/2) - y;
        //cout << x << " " << y << endl;
        
        /*if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
            if(x >= moonX - moonRadius &&  x <= moonX + moonRadius
               && y >= moonY - moonRadius && y <= moonY + moonRadius){
                moonX = x;
                moonY = y;
            }
        }*/
        
    }
    glutPostRedisplay();
}

void mouseMotion(int x, int y){ //Will set the moon to the same coordiantes as your mouse when clicked and dragging
    glutSetWindow(id2);
    x = x - (width2/2); // Correct from mouse to OpenGL co-ordinates.
    y = (height2/2) - y;
    
    if(x >= moonX - moonRadius &&  x <= moonX + moonRadius
       && y >= moonY - moonRadius && y <= moonY + moonRadius){
        moonX = x;
        moonY = y;
    }
    glutPostRedisplay();
}

void moon_size_menu(int id){    //Different sized of the moon accessable from the menu
    if(id == 5) Scale = 0.3;
    if(id == 6) Scale = 1;
    if(id == 7) Scale = 3.0;
    
    glutPostRedisplay();
}

void sky_rotate_menu(int id){       //Rotate the sky by 10 degrees in each direction
    if(id == 8) skyRotate -= 10;
    if(id == 9) skyRotate += 10;
    
    glutPostRedisplay();
}

void wire_menu(int id){     //Can set items to wired or filled mode
    if(id == 2) isWire = 1;
    if(id == 3) isWire = 0;
    if(id == 4) moonX = moonY = Angle = 0.0;
    
    glutPostRedisplay();
}

void day_menu(int id){      //Can set the sky to show eather day or night
    if(id == 0) isDay = 1;
    if(id == 1) isDay = 0;
    
    glutPostRedisplay();
}

void makeMenu(){
    int sub_menu1, sub_menu2, sub_menu3;        //Culminated the menus in some reasonably rational way
    sub_menu1 = glutCreateMenu(moon_size_menu);
    glutAddMenuEntry("Tiny",5);
    glutAddMenuEntry("Normal",6);
    glutAddMenuEntry("Super",7);
    
    
    sub_menu2 = glutCreateMenu(day_menu);
    glutAddMenuEntry("Day",0);
    glutAddMenuEntry("Night",1);
    
    sub_menu3 = glutCreateMenu(sky_rotate_menu);
    glutAddMenuEntry("Rotate Right", 8);
    glutAddMenuEntry("Rotate Left", 9);
    
    glutCreateMenu(wire_menu);
    glutAddSubMenu("Moon Size", sub_menu1);
    glutAddSubMenu("Day Or Night", sub_menu2);
    glutAddSubMenu("Rotate Sky", sub_menu3);
    glutAddMenuEntry("Wired", 2);
    glutAddMenuEntry("Filled",3);
    glutAddMenuEntry("Reset Moon", 4);
    
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void increaseMoonAngle(void){   //Used by animate fucntion to change the angle of the moon
    Angle += 10.0;              // Will reset to 0 instead of increasing past 360 to ensure
    if (Angle >= 360.0) Angle -= 360.0;  // The corresponding buttons will be shown
    glutSetWindow(id2);
    glutPostRedisplay();
}

void increaseGalaxyAngle(void){ //Used by animate fucntion to change the angle of the galaxies
    galaxyAngle += 10.0;
    if (galaxyAngle >= 360.0) galaxyAngle -= 360.0;
    glutSetWindow(id2);
    glutPostRedisplay();
}

void increaseOrbit(void){
    orbitAngle += 5;
    ringAngle +=5;
    if(orbitAngle >= 360.0) orbitAngle -= 360.0;
    glutSetWindow(id2);
    glutPostRedisplay();
}

void animate(int someValue) //Animates the moon and galaxies respectively if they are set to do so
{
    if (galaxyRotate) increaseGalaxyAngle();
    if (moonRotate) increaseMoonAngle();
    if (isOrbit) increaseOrbit();
    glutTimerFunc(animationPeriod, animate, 1);
}

void printInteraction(void){
    cout << "Interaction:" << endl;
    cout << "In order to use this space simulator properly you will have to know a few things:" << endl << "Firstly buttons on the control panel are not the same, the round buttons change the moon phase, and only one can be chosen at a time, while the square ones will allow for other objects to be visible in the sky and can allow for more than one to be active at one moment" << endl << "The moon is interactive, it can be turned by the buttons on the control panel, as well as incrimented by using the 'm' key, however if you are to press the 'M' key the moon will continue to rotate until the button is pressed again. The moon can also be picked up and draged across the sky with the mouse." << endl << "The galaxy objects can be shown via the control panel and similar to the moon can be rotated with the 'g' key, as well as given a comman to continuously spin with the 'G' key." << endl;
}

// Main routine.
int main(int argc, char **argv)
{
    printInteraction();
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    
    // First top-level window definition.
    glutInitWindowSize(250, 500);
    glutInitWindowPosition(100, 100);
    
    // Create the first window and return id.
    id1 = glutCreateWindow("Control Panel");
    
    // Initialization, display, and other routines of the first window.
    setup1();
    glutDisplayFunc(drawScene1);
    glutReshapeFunc(resize1);
    glutKeyboardFunc(keyInput); // Routine is shared by both windows.
    glutMouseFunc(mouseControl1);
    
    // Second top-level window definition.
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(400, 100);
    
    // Create the second window and return id.
    id2 = glutCreateWindow("Our Universe");
    
    // Initialization, display, and other routines of the second window.
    setup2();
    glutDisplayFunc(drawScene2);
    glutReshapeFunc(resize2);
    glutKeyboardFunc(keyInput); // Routine is shared by both windows.
    glutMouseFunc(mouseControl2);
    
    glutSpecialFunc(specialKeyInput);
    glutMotionFunc(mouseMotion);
    
    makeMenu();
    
    glutTimerFunc(5, animate, 1);
    
    glutMainLoop();
    
    return 0;
}
