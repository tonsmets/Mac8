//
//  chip8.cpp
//  Mac8
//
//  Created by Ton Smets on 06-04-16.
//  Copyright © 2016 Ton Smets. All rights reserved.
//

#include "chip8.hpp"

#define BUFFERSIZE 512

unsigned char chip8_fontset[80] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void Chip8::initialize() {
    pc     = 0x200;  // Program counter starts at 0x200
    opcode = 0;      // Reset current opcode
    I      = 0;      // Reset index register
    sp     = 0;      // Reset stack pointer
    
    // Clear display
    memset(gfx, 0, sizeof(gfx));
    
    // Clear stack
    memset(stack, 0, sizeof(stack));
    
    // Clear registers V0-VF
    memset(V, 0, sizeof(V));
    
    // Clear memory
    memset(memory, 0, sizeof(memory));
    
    // Load fontset
    for(int i = 0; i < 80; ++i) {
        memory[i] = chip8_fontset[i];
    }
    
    // Reset timers
    delay_timer = 0;
    sound_timer = 0;
    
    // Clear screen once
    drawFlag = true;
    
    srand((int)time(NULL));
}

void Chip8::loadGame(std::string filename) {
    unsigned char buffer[BUFFERSIZE];
    
    FILE * filp = fopen(filename.c_str(), "rb");
    if(filp != NULL) {
        int bufferSize = (int)fread(buffer, sizeof(unsigned char), BUFFERSIZE, filp);
        
        for(int i = 0; i < bufferSize; i++) {
            memory[i + 0x200] = buffer[i];
        }
        std::cout << "Loaded game of " << bufferSize << " bytes" << std::endl;
    }
    else {
        std::cout << "Unable to load game" << std::endl;
        exit(1);
    }
    
}

/*
 Opcodes fetched from: https://en.wikipedia.org/wiki/CHIP-8#Opcode_table
 */
void Chip8::emulateCycle() {
    // Fetch opcode
    opcode = memory[pc] << 8 | memory[pc + 1];
    
    // Decode opcode
    switch(opcode & 0xF000)
    {
        case 0x0000:
            switch(opcode & 0x000F)
            {
                case 0x0000: // 00E0: Clears the screen.
                    // Clear display
                    memset(gfx, 0, sizeof(gfx));
                    pc += 2;
                    break;
                case 0x000E: // 00EE: Returns from subroutine. (reverse of 0xA000)
                    --sp;
                    pc = stack[sp];
                    pc += 2;
                    break;
                default:
                    printf ("Unknown opcode [0x0000]: 0x%X\n", opcode);
            }
            break;
        case 0x1000: // 1NNN: Jumps to address NNN.
            pc = opcode & 0x0FFF;
            break;
        case 0x2000: // 2NNN: Calls subroutine at NNN.
            stack[sp] = pc;
            ++sp;
            pc = opcode & 0x0FFF;
            break;
        case 0x3000: // 3XNN: Skips the next instruction if VX equals NN.
            if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
                pc += 4;
            }
            else {
                pc += 2;
            }
            break;
        case 0x4000: // 4XNN: Skips the next instruction if VX doesn't equal NN.
            if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
                pc += 4;
            }
            else {
                pc += 2;
            }
            break;
        case 0x5000: // 5XY0: Skips the next instruction if VX equals VY.
            if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
                pc += 4;
            }
            else {
                pc += 2;
            }
            break;
        case 0x6000: // 6XNN: Sets VX to NN.
            V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
            pc += 2;
            break;
        case 0x7000: // 7XNN: Adds NN to VX.
            V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            pc += 2;
            break;
        case 0x8000:
            switch(opcode & 0x000F)
            {
                case 0x0000: // 8XY0: Sets VX to the value of VY.
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0001: // 8XY1: Sets VX to VX or VY.
                    V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0002: // 8XY2: Sets VX to VX and VY.
                    V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0003: // 8XY3: Sets VX to VX xor VY.
                    V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0004: // 8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
                    if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])) {
                        V[0xF] = 1; //carry
                    }
                    else {
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                    pc += 2;          
                    break;
                case 0x0005: // 8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
                    if(V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8]) {
                        V[0xF] = 0;
                    }
                    else {
                        V[0xF] = 1;
                    }
                    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0006: // 8XY6: Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.
                    V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
                    V[(opcode & 0x0F00) >> 8] >>= 1;
                    pc += 2;
                    break;
                case 0x0007: // 8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
                    if(V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]) {
                        V[0xF] = 0;
                    }
                    else {
                        V[0xF] = 1;
                    }
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                case 0x000E: // 8XYE: Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.
                    V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
                    V[(opcode & 0x0F00) >> 8] <<= 1;
                    pc += 2;
                    break;
                default:
                    printf ("Unknown opcode [0x0000]: 0x%X\n", opcode);
            }
            break;
        case 0x9000:
            // Execute opcode
            break;
        case 0xA000: // ANNN: Sets I to the address NNN
            // Execute opcode
            I = opcode & 0x0FFF;
            pc += 2;
            break;
        case 0xB000:
            // Execute opcode
            break;
        case 0xC000:
            // Execute opcode
            break;
        case 0xD000:
            {
                unsigned short x = V[(opcode & 0x0F00) >> 8];
                unsigned short y = V[(opcode & 0x00F0) >> 4];
                unsigned short height = opcode & 0x000F;
                unsigned short pixel;
                
                V[0xF] = 0;
                for (int yline = 0; yline < height; yline++)
                {
                    pixel = memory[I + yline];
                    for(int xline = 0; xline < 8; xline++)
                    {
                        if((pixel & (0x80 >> xline)) != 0)
                        {
                            if(gfx[(x + xline + ((y + yline) * 64))] == 1)
                                V[0xF] = 1;                                 
                            gfx[x + xline + ((y + yline) * 64)] ^= 1;
                        }
                    }
                }
                
                drawFlag = true;
                pc += 2;
            }
            break;
        case 0xE000:
            switch(opcode & 0x00FF)
            {
                case 0x009E:
                    // Execute opcode
                    break;
                case 0x00A1:
                    // Execute opcode
                    break;
                default:
                    printf ("Unknown opcode [0x0000]: 0x%X\n", opcode);
            }
            break;
        case 0xF000:
            switch(opcode & 0x00FF)
            {
                case 0x0007:
                    // Execute opcode
                    break;
                case 0x000A:
                    // Execute opcode
                    break;
                case 0x0015:
                    // Execute opcode
                    break;
                case 0x0018:
                    // Execute opcode
                    break;
                case 0x001E:
                    // Execute opcode
                    break;
                case 0x0029:
                    // Execute opcode
                    break;
                case 0x0033:
                    memory[I]     = V[(opcode & 0x0F00) >> 8] / 100;
                    memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                    memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
                    pc += 2;
                    break;
                case 0x0055:
                    // Execute opcode
                    break;
                case 0x0065:
                    // Execute opcode
                    break;
                default:
                    printf ("Unknown opcode [0x0000]: 0x%X\n", opcode);
            }
            break;
        default:
            printf ("Unknown opcode: 0x%X\n", opcode);
    }
    
    // Update timers
    if(delay_timer > 0)
        --delay_timer;
    
    if(sound_timer > 0)
    {
        if(sound_timer == 1)
            printf("BEEP!\n");
        --sound_timer;
    }
}

void Chip8::setKeys() {
    
}