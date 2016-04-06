//
//  main.cpp
//  Mac8
//
//  Created by Ton Smets on 06-04-16.
//  Copyright © 2016 Ton Smets. All rights reserved.
//

#include <iostream>
#include "chip8.hpp"

Chip8 mychip;

int main(int argc, const char * argv[]) {
    // Set up render system and register input callbacks
    //setupGraphics();
    //setupInput();
    
    // Initialize the Chip8 system and load the game into the memory
    mychip.initialize();
    mychip.loadGame("/Volumes/Macintosh HD/OneDrive/Documenten/Chip8/PONG");
    
    // Emulation loop
    for(;;)
    {
        // Emulate one cycle
        mychip.emulateCycle();
        
        // If the draw flag is set, update the screen
        if(mychip.drawFlag)
            //drawGraphics();
        
        // Store key press state (Press and Release)
        mychip.setKeys();
    }
    
    return 0;
}
