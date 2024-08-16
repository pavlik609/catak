#include <raylib/raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __TEXOBJ_H__
#define __TEXOBJ_H__
    typedef struct TextureOBJ TextureOBJ;
    struct TextureOBJ{
        Texture texture;
        Texture texture_hit;
        TextureOBJ * end_ptr;
        TextureOBJ * start_ptr;
        TextureOBJ * real_ptr;
        Color tint;
        Vector2 pos;
        unsigned char pickup_cooldown;
        char * id;
        int index;
        float rotation;
    };

    /* Gets the start TextureOBJ */
    inline TextureOBJ * GET_START(TextureOBJ * t){
        if ((*t).start_ptr == NULL){
            return t;
        }else{
            return (*t).start_ptr;
        }
    }

    /* Gets the end TextureOBJ */
    inline TextureOBJ * GET_END(TextureOBJ * t){
        if ((*t).end_ptr == NULL){
            return t;
        }else{
            return (*t).end_ptr;
        }
    }

    /* If two TextureOBJs equal */
    inline int TEXTURE_OBJ_EQUALS(TextureOBJ t1, TextureOBJ t2){
        if(memcmp(&t1,&t2,sizeof(TextureOBJ)) == 0){
            return true;
        }else{
            return false;
        }
    }
    
#endif