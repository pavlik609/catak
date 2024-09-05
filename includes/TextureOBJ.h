#include <raylib/raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __TEXOBJ_H__
#define __TEXOBJ_H__

    /* Additional data index, points to the index */
    #define T_OBJ_INDEX 0

    /* Additional data index, points to the easing type */
    #define T_OBJ_EASE_TYPE 1

    /* Additional data index, points to the indedx */
    #define T_OBJ_XSCALE 2
    
    /* Additional data index, points to the indedx */
    #define T_OBJ_YSCALE 3
    
    /* Maximum [ additional_data ] length */
    #define T_M_ADD_DATA 15

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
        float rotation;
        void (*additional_data[10]);
    };

    /* Empty TextureOBJ */
    const TextureOBJ EmptyTexOBJ;

    /* Gets the Nth element of [ additional_data ] */
    inline void* GET_ADD_DATA(TextureOBJ * t,int index){
        return t->additional_data+8*index;
    }

    /* Sets the Nth element if [ additional_data ] */
    inline void SET_ADD_DATA(TextureOBJ * t,void* data,int index){
        memcpy(((char*)(t->additional_data))+8*index,data,sizeof(data));
    }

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

    /*  */
    inline void SET_ADITIONAL_DATA(TextureOBJ* t,void* data,size_t size){
        memcpy((char*)((t))->additional_data,data,size);
    }
    
#endif
