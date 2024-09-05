#include "Shared.h"
#include "TextureOBJ.h"

#ifndef __GASTER_BLASTER_H__
#define __GASTER_BLASTER_H__

    /* Adds a gaster blaster to [ updfunc ] and initializes it */
    inline void HoldGasterBlaster();

    /* Gaster blaster holding logic */
    inline void GasterBlasterHold(int i){

        if(!IsMouseButtonDown(1)){
            RotateSpriteMouse(holding_texture);
        }else{
            updfunc[i] = NULL;
            holding = false;
            if(textures[holding_texture].end_ptr != NULL){
                textures[*(int*)GET_ADD_DATA(textures[holding_texture].end_ptr,T_OBJ_INDEX)] = EmptyTexOBJ;
                //*(textures[holding_texture].end_ptr) = EmptyTexOBJ;
            }else if(textures[holding_texture].start_ptr != NULL){
                textures[*(int*)GET_ADD_DATA(textures[holding_texture].start_ptr,T_OBJ_INDEX)] = EmptyTexOBJ;
                //*(textures[holding_texture].start_ptr) = EmptyTexOBJ;
            }
            textures[holding_texture] = EmptyTexOBJ;
            holding_texture = -1;
            selected_texture = -1;
            glob_tex_len-=1;
        }
        if((IsMouseButtonPressed(0) || IsKeyPressed(KEY_SPACE))){
            float saverot = textures[holding_texture].rotation;
            textures[holding_texture].pickup_cooldown = 1;
            updfunc[i] = NULL;
            holding = false;
            selected_texture = holding_texture;
            glob_pickup_cd = 1;
            if (IsKeyDown(KEY_LEFT_SHIFT) && canShift){
                HoldGasterBlaster();
                textures[holding_texture].rotation = saverot;
                canShift = false;
            }else if (canShift){
                holding_texture = -1;
            }
            FrameChangedCallback(0);
        }else{
            canShift = true;
        }
    }

    /* Adds a gaster blaster to [ updfunc ] and initializes it */
    inline void HoldGasterBlaster(){
        if (!holding && glob_pickup_cd == 0){
            Texture blaster = gaster_blaster_tex;
            Texture frame = gaster_blaster_box_tex;
            textures[textures_len+1] = (TextureOBJ){.texture = blaster,
                                                    .texture_hit = frame,
                                                    .end_ptr = NULL,
                                                    .start_ptr = &textures[textures_len],
                                                    .pos = (Vector2){480,280},
                                                    .id = "gb",
                                                    .rotation = 90,
                                                    .tint = GREEN,
                                                    .pickup_cooldown = 0};
            textures[textures_len] = (TextureOBJ){.texture = blaster,
                                                  .texture_hit = frame,
                                                  .end_ptr = &textures[textures_len+1],
                                                  .start_ptr = NULL,
                                                  .pos = (Vector2){480,240},
                                                  .id = "gb",
                                                  .rotation = 0,
                                                  .tint = WHITE,
                                                  .pickup_cooldown = 0};
            int test = 0;
            int val_to_write = 12;
            int temp_val = textures_len+1;
            SET_ADD_DATA(&textures[textures_len+1],&temp_val,T_OBJ_INDEX);
            temp_val = textures_len;
            SET_ADD_DATA(&textures[textures_len],&temp_val,T_OBJ_INDEX);
            memcpy((char*)textures[textures_len].additional_data,&val_to_write,4);
            //memcpy((char*)textures[textures_len].additional_data+8,&test,4);
            //memcpy((char*)textures[textures_len].additional_data+16,&test,4);
            holding_texture = textures_len;
            selected_texture = textures_len;
            textures_len+=2;
            glob_tex_len+=1;
            holding = true;
            AddFunctionToItterator(&GasterBlasterHold);
        }
    }
#endif
