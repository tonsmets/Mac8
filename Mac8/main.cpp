//
//  main.cpp
//  Mac8
//
//  Created by Ton Smets on 06-04-16.
//  Copyright © 2016 Ton Smets. All rights reserved.
//

#include <iostream>
#include <unistd.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include "chip8.hpp"

#define PIXELSIZE 10

Chip8 mychip;

void display(void);
void reshape(int, int);
void keyboardUp(unsigned char key, int x, int y);
void keyboardDown(unsigned char key, int x, int y);

char* fileName = (char*)"/Volumes/Macintosh HD/OneDrive/Documenten/Chip8/CONNECT4";

int main(int argc, char ** argv) {
    mychip.initialize();
    mychip.loadGame(fileName);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(64*PIXELSIZE, 32*PIXELSIZE);
    glutCreateWindow("Mac8");
    
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    
    glClearColor(0,0,0,1);
    glutMainLoop();

    return 0;
}

void display()
{
    mychip.emulateCycle();
    
    if(mychip.drawFlag) {
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3f(1.0f, 1.0f, 1.0f);
        
        // Draw a rectangle for every 1 we find in the array
        for(int y = 0; y < 32; ++y) {
            for(int x = 0; x < 64; ++x)
            {
                if(mychip.gfx[(y*64) + x] != 0) {
                    glBegin(GL_QUADS);
                        glVertex3f((float)(x*PIXELSIZE), (float)(y*PIXELSIZE), 0.0f);
                        glVertex3f((float)(x*PIXELSIZE), (float)(y*PIXELSIZE) + PIXELSIZE, 0.0f);
                        glVertex3f((float)(x*PIXELSIZE) + PIXELSIZE, (float)(y*PIXELSIZE) + PIXELSIZE, 0.0f);
                        glVertex3f((float)(x*PIXELSIZE) + PIXELSIZE, (float)(y*PIXELSIZE), 0.0f);
                    glEnd();
                }
            }
        }
        glutSwapBuffers();
        mychip.drawFlag = false;
    }
    else {
        //usleep(10000);
    }
}

void reshape(int width, int height)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, height, 0);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, width, height);
}

void keyboardDown(unsigned char key, int x, int y)
{
    if(key == 27) {   // esc
        exit(0);
    }
    
    if(key == 0x7f) { // backspace
        mychip.initialize();
        mychip.loadGame(fileName);
    }
    
    if(key == '1')		mychip.key[0x1] = 1;
    else if(key == '2')	mychip.key[0x2] = 1;
    else if(key == '3')	mychip.key[0x3] = 1;
    else if(key == '4')	mychip.key[0xC] = 1;
    
    else if(key == 'q')	mychip.key[0x4] = 1;
    else if(key == 'w')	mychip.key[0x5] = 1;
    else if(key == 'e')	mychip.key[0x6] = 1;
    else if(key == 'r')	mychip.key[0xD] = 1;
    
    else if(key == 'a')	mychip.key[0x7] = 1;
    else if(key == 's')	mychip.key[0x8] = 1;
    else if(key == 'd')	mychip.key[0x9] = 1;
    else if(key == 'f')	mychip.key[0xE] = 1;
    
    else if(key == 'z')	mychip.key[0xA] = 1;
    else if(key == 'x')	mychip.key[0x0] = 1;
    else if(key == 'c')	mychip.key[0xB] = 1;
    else if(key == 'v')	mychip.key[0xF] = 1;
    
    //printf("Press key %c\n", key);
}

void keyboardUp(unsigned char key, int x, int y)
{
    if(key == '1')		mychip.key[0x1] = 0;
    else if(key == '2')	mychip.key[0x2] = 0;
    else if(key == '3')	mychip.key[0x3] = 0;
    else if(key == '4')	mychip.key[0xC] = 0;
    
    else if(key == 'q')	mychip.key[0x4] = 0;
    else if(key == 'w')	mychip.key[0x5] = 0;
    else if(key == 'e')	mychip.key[0x6] = 0;
    else if(key == 'r')	mychip.key[0xD] = 0;
    
    else if(key == 'a')	mychip.key[0x7] = 0;
    else if(key == 's')	mychip.key[0x8] = 0;
    else if(key == 'd')	mychip.key[0x9] = 0;
    else if(key == 'f')	mychip.key[0xE] = 0;
    
    else if(key == 'z')	mychip.key[0xA] = 0;
    else if(key == 'x')	mychip.key[0x0] = 0;
    else if(key == 'c')	mychip.key[0xB] = 0;
    else if(key == 'v')	mychip.key[0xF] = 0;
}

