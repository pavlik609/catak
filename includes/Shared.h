#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "TextureOBJ.h"

#ifndef __SHARED_H__
#define __SHARED_H__

    typedef struct Frame {
        char * attack;
        void * args[5];
        TextureOBJ textures[1000];
        int textures_len;
    }Frame;
    
    /* Empty frame */
    const Frame EmptyFrame;

    /* Largest used frame */
    int largest_frame = 0;

    /* Index of the current frame */
    int current_frame = 0;

    /* Index of the selected texture, -1 means none */
    int selected_texture = -1;
    
    /* Global texture length */
    int glob_tex_len = 0;

    /* Global element pickup cooldown */
    int glob_pickup_cd = 0;

    /* Index of the texture that is being held, -1 means none */
    int holding_texture = -1;

    /* Length of the current texture array */
    int textures_len = 0;

    /* Length of the [ updfunc ] array */
    int updfunc_len = 0;

    /* State of the mouse wheel */
    float mwheel;

    /* Weird workaround variable */
    bool canShift = true;

    /* If you are holding something */
    bool holding = false;

    /* Gaster blaster texture */
    Texture gaster_blaster_tex;

    /* Gaster blaster hitbox texture */
    Texture gaster_blaster_box_tex;

    /* Array of current textures */
    TextureOBJ textures[1000];

    /* Array of frames */
    Frame* frames;

    /* Array of functions that are updated at the start of each frame */
    void (*updfunc[99])();
    
    /* Called in every function chaning the frame number */
    inline void FrameChangedCallback(int nextframe){
        selected_texture = -1;
        holding_texture = -1;
        int real_nextframe = (int)Clamp(current_frame+nextframe,0,65534);
        memcpy(frames[current_frame].textures,textures,sizeof(frames[current_frame].textures)); 
        frames[current_frame].textures_len = textures_len;
        memcpy(textures,frames[real_nextframe].textures,sizeof(frames[real_nextframe].textures));
        textures_len = frames[real_nextframe].textures_len;
        if (real_nextframe > largest_frame) {largest_frame = real_nextframe;}
    }

    /* Adds function [ func ] to the [ updfunc ] function array */
    inline void AddFunctionToItterator(void* func){
        updfunc[updfunc_len++] = func;
    }

    /* Sprite rotation using the mousewheel */
    inline void RotateSpriteMouse(int i){
        if (mwheel != 0){
            //mwheel = mwheel/abs(mwheel);
            int multiplier = 1; /* Market piler */
            int additioner = 0;
            if(!IsKeyDown(KEY_LEFT_CONTROL)){multiplier=10;}
            if(IsKeyDown(KEY_LEFT_ALT)){multiplier=0;additioner=90;}
            textures[i].rotation-=mwheel*multiplier+additioner;
            if(textures[i].rotation>=360.0f){
                textures[i].rotation-=360;
            }
            if(textures[i].rotation<=-360.0f){
                textures[i].rotation+=360;
            }
            if(textures[i].rotation<0.0f){
                textures[i].rotation+=360;
            }
        }
    }
#endif