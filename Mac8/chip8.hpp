//
//  chip8.hpp
//  Mac8
//
//  Created by Ton Smets on 06-04-16.
//  Copyright © 2016 Ton Smets. All rights reserved.
//

/*
 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
 0x200-0xFFF - Program ROM and work RAM
 */

#ifndef chip8_hpp
#define chip8_hpp

#include <stdio.h>
#include <string>
#include <iostream>

class Chip8 {
private:
    unsigned short opcode;
    unsigned char memory[0xFFF];
    unsigned char V[16];
    unsigned short I;
    unsigned short pc;
    unsigned char delay_timer;
    unsigned char sound_timer;
    unsigned short stack[16];
    unsigned short sp;
public:
    void initialize();
    void loadGame(std::string);
    void emulateCycle();
    void setKeys();
    void debugRender();
    bool drawFlag = false;
    unsigned char gfx[64 * 32];
    unsigned char key[16];
};

#endif /* chip8_hpp */
