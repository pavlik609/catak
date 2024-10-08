#pragma region INCLUDES
/*----INCLUDES----*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib/raylib.h>
#include <raylib/raymath.h>
#include <raylib/reasings.h>
#include "includes/TextureOBJ.h"
#include "includes/Easings.h"
#include "includes/GasterBlaster.h"
#include "includes/Shared.h"
#include "includes/UIDefines.h"
/*----INCLUDES----*/
#pragma endregion INCLUDES

/*--EXTENRS--*/

extern void HoldGasterBlaster();
extern void GasterBlasterHold(int i);
extern void FrameChangedCallback(int nextframe);
extern void AddFunctionToItterator(void* func);
extern void RotateSpriteMouse(int i);
extern int TEXTURE_OBJ_EQUALS();
extern TextureOBJ * GET_START();
extern TextureOBJ * GET_END();
extern void * GET_ADD_DATA(TextureOBJ * t,int index);
extern void SET_ADD_DATA(TextureOBJ * t,void* data,int index);

/*--EXTENRS--*/

#pragma region STRUCTS

/*----STRUCTS----*/
typedef union uint_16{
    unsigned char half;
    unsigned char half2;
} uint_16;

typedef struct Button {
    Rectangle box;
    Color color;
    Texture texture;
    char * tag;
    char * texture_tag;
    bool pressed;
    bool updfunc_firstfram;
    bool has_added_updfunc;
    bool * override_visibility;
    int frames_held;
    float * callback_values[10];
    void (* callback)();
} Button;
/*----STRUCTS----*/

#pragma endregion STRUCTS

#pragma region GLOBALS
/*----GLOBALS----*/


/*-INTS-*/

/* Button array length */
static int button_index = 0;
/* Ticks since the app started */
static int ticks = 0;
/* Framecounter for the { Exported! } element */
static int framesExport = 0;
/* Index of the last button */
static int last_button = -1;

int num_f_i_len;
/*-INTS-*/

/*-BOOLS-*/
static bool has_changes = false;
static bool is_selected = false;
static bool is_selected_gb = false;
/*-BOOLS-*/

/*-TEXTURES-*/
Texture tex_in_hand;
Texture missing_tex;
Texture empty_tex;
/*-TEXTURES-*/

/*-OTHER-*/
char num_field_inp[10];
static Button buttons[99];

/*-OTHER-*/


/*-EMPTY-*/
static const Texture EmptyTex;
/*-EMPTY-*/

/*----GLOBALS----*/
#pragma endregion GLOBALS

/* If two textures equal */
bool TextureEquals(Texture t1, Texture t2){
    if(memcmp(&t1,&t2,sizeof(Texture)) == 0){
        return true;
    }else{
        return false;
    }
}

/* from : https://stackoverflow.com/a/786007 Tries to parse from a character to an intiger */ 
bool TryParseChar(int * i, char c){
    if ('0' <= c &&  c <= '9') {
        *i = (int)c - 48;
        return true;
    } else {
        if (c != '\0'){
            //printf("ERR: INVALID CHAR BEING PARSED\n");
        }
        return false;
    }
}

/* Creates a button */
void CreateButton(Rectangle box,Color color,Texture tex_override,void (* callback)(),char * tag,char * texture_tag,char * img_path, bool * override_vis){
    buttons[button_index].box = box;
    buttons[button_index].color = color;
    buttons[button_index].callback = callback;
    buttons[button_index].pressed = false;
    buttons[button_index].frames_held = 0;
    buttons[button_index].tag = tag;
    buttons[button_index].texture_tag = texture_tag;
    buttons[button_index].texture = (img_path != NULL) ? LoadTexture(img_path) : (!TextureEquals(tex_override, EmptyTex)) ? tex_override : missing_tex;
    buttons[button_index].updfunc_firstfram = false;
    buttons[button_index].has_added_updfunc = false;
    buttons[button_index].override_visibility = override_vis;

    button_index++;
}

/* Frame change ui */
void ChangeFrame(int i,char * context_tag){
    if (buttons[i].pressed == false || (buttons[i].frames_held >= 20 && buttons[i].frames_held % 2)){
        int addVal; 
        if(context_tag == "inc1")
            addVal = (int)Clamp(buttons[i].frames_held % 20,1,2);
        else if(context_tag == "inc20")
            addVal = 20;
        else if(context_tag == "sub1")
            addVal = -(int)Clamp(buttons[i].frames_held % 20,1,2);
        else if(context_tag == "sub20")
            addVal = -20;
        FrameChangedCallback(addVal);
        current_frame+= addVal;
    }
    buttons[i].frames_held++;
}

/* Change a property of the last selected bullet */
void ChangeProperty(int i,char * context_tag){
    if ((buttons[i].pressed == false || (buttons[i].frames_held >= 20 && buttons[i].frames_held % 2 )) && selected_texture != -1){
        int multiplier;
        int cap = INT_MAX;
        float * edit_val;
        char substr[10];
        memcpy(substr,&context_tag[strlen(context_tag)-3],3);
        TextureOBJ * start = GET_START(&textures[selected_texture]);
        TextureOBJ * end   = GET_END(&textures[selected_texture]);
        TextureOBJ * real;
        if (memcmp(substr,"add",3*sizeof(char)) == 0){
            multiplier = 1;
        }
        if (memcmp(substr,"sub",3*sizeof(char)) == 0){
            multiplier = -1;
        }
        memcpy(substr,&context_tag[2],1);
        if (memcmp(substr,"o",1) == 0){
            real = start;
        }
        if (memcmp(substr,"e",1) == 0){
            real = end;
        }
        memcpy(substr,&context_tag[0],1);
        if (memcmp(substr,"x",1) == 0){
            edit_val = &real->pos.x;
        }
        if (memcmp(substr,"y",1) == 0){
            edit_val = &real->pos.y;
        }
        if (memcmp(substr,"r",1) == 0){
            edit_val = &real->rotation;
        }
        int temp_idx;
        if (TryParseChar(&temp_idx,substr[0])){
            if (temp_idx == 1){
                cap = 29;
            }
            int val = *(int*)GET_ADD_DATA(real,temp_idx);
            val+=((int)Clamp(buttons[i].frames_held % 20,1,2)*multiplier);
            val = (int)Clamp(val,0,cap);
            SET_ADD_DATA(real,&val,temp_idx);
            //memcpy(data,&val,4);
        }else{
            *edit_val+=((int)Clamp(buttons[i].frames_held % 20,1,2)*multiplier);
        }
        memcpy(frames[current_frame].textures,textures,sizeof(frames[current_frame].textures)); 
        frames[current_frame].textures_len = textures_len;
    }
    buttons[i].frames_held++;
}

/* [ updfunc ] function that is used to write into input fields */
void TypeInputField(int i){
    if (!buttons[last_button].updfunc_firstfram){
        
    }
    if (buttons[last_button].tag == "numinp"){
        int val;
        char c = GetCharPressed();
        if (TryParseChar(&val,c) || (c == '-' && num_f_i_len == 0)){
            strcpy(&num_field_inp[num_f_i_len++],&c);
            printf("%s\n",num_field_inp);
            *buttons[last_button].callback_values[0] = strtol(num_field_inp,NULL,10)+160;
        }
        if(IsKeyPressed(KEY_BACKSPACE)){
            strcpy(&num_field_inp[--num_f_i_len],"\0");
            printf("%s\n",num_field_inp);
            *buttons[last_button].callback_values[0] = strtol(num_field_inp,NULL,10)+160;
        }
        DrawText(num_field_inp,50,50,15,DARKGRAY);
    }else{

    }

    if(IsMouseButtonPressed(0) || IsMouseButtonPressed(1)){
        strcpy(num_field_inp,"\0");
        num_f_i_len = 0;
        updfunc[i] = NULL;
        buttons[last_button].has_added_updfunc = false;
    }
}

/* Adds [ TypeInputField ] to [ updfunc ]*/
void AddTypeInpField(int i){
    has_changes = true;
    if (!buttons[i].pressed && !buttons[i].has_added_updfunc){
        AddFunctionToItterator(&TypeInputField);
        buttons[i].has_added_updfunc = true;
    }
}

/* Exports the current workspace into a catak.lua parsable file */
void Export(int i,int override_pressed){
    if (buttons[i].pressed == false || override_pressed == true){
        FILE *fptr;
        has_changes = false;
        fptr = fopen("export.txt","w+");

        fprintf(fptr,"%i",glob_tex_len);
        for(int i=0;i<largest_frame+1;i++){
            Frame locframe = frames[i];
            for(int j=0;j<locframe.textures_len;j++){
                TextureOBJ txj = locframe.textures[j];
                if(txj.end_ptr == NULL) {continue;}
                /*-           fr id x- y- ex ey r- er-*/
                fprintf(fptr,"\n%i %s %i %i %i %i %i %i %i %i",i,txj.id,(int)txj.pos.x,(int)txj.pos.y,(int)txj.end_ptr->pos.x,(int)txj.end_ptr->pos.y,(int)txj.rotation,(int)txj.end_ptr->rotation,*(int*)(txj.additional_data),*(int*)(txj.additional_data+1*8));
                fprintf(fptr," %s",EasingTypes[*(int*)(txj.additional_data+1*8)]);
            }
        }

        fclose(fptr);

        framesExport = 80;
    }
}

/* Prints a warning */
void UnimplementedButtonFunctionality(int i){
    printf("UNIMPLEMENTED BUTTON FUNCTIONALITY '%s'\n", buttons[i].tag);
}
/* Adds up (and clamps) two color values */
Color AddColors(Color c1, Color c2){
    Color returnCol;
    return (Color){.r = Clamp(c1.r+c2.r,0,255),.g = Clamp(c1.g+c2.g,0,255),.b = Clamp(c1.b+c2.b,0,255),.a = Clamp(c1.a+c2.a,0,255)};
}
/* Comares two color values */
bool ColorsEqual(Color c1, Color c2){
    if (c1.r != c2.r ){ return false; }
    if (c1.g != c2.g ){ return false; }
    if (c1.b != c2.b ){ return false; }
    if (c1.a != c2.a ){ return false; }
    return true;
}

char * expand_tag(char*tag){
    if(strcmp(tag,"gb") == 0){
        return "GASTER BLASTER";
    }
    return strcat(strcat("INVALID TAG '",tag),"'");
}

/* Entrypoint */
int main(void)
{
    frames = malloc(sizeof(Frame)*65536);
    memset(frames,0,sizeof(frames));
    int i,j,k,l,m; //itterators
    const int screenWidth = 960;
    const int screenHeight = 480;
    Image icon = LoadImage("assets/icon_task.png");
    Image icon_s = LoadImage("assets/icon_small.png");
    Image *icons = malloc(sizeof(Image)*2);
    icons[0] = icon;
    icons[1] = icon_s;
    num_f_i_len = 0;
    InitWindow(screenWidth, screenHeight, "Catak - Create Your Frisk attack helper [ ALPHA 0.3.2 ]");
    /*TEXTURES*/
    
    gaster_blaster_tex = LoadTexture("assets/gaster_blaster.png");
    gaster_blaster_box_tex = LoadTexture("assets/gaster_blaster_hit.png");
    empty_tex = LoadTexture("assets/empty.png");

    Texture arrow_tex = LoadTexture("assets/arrow_ui.png");
    
    /*TEXTURES*/
    
    //missing_tex = LoadTexture("assets/empty.png");
    SetWindowIcons(icons,2);
    SetTargetFPS(60);           
    /*--WALL-OF-BUTTONS--*/
    CreateButton((Rectangle){5,30,25,25},(Color){220,220,220,255},EmptyTex,&ChangeFrame,"inc1","","assets/plus_tex.png",NULL);
    CreateButton((Rectangle){5,60,65,25},(Color){220,220,220,255},EmptyTex,&ChangeFrame,"inc20","","assets/plus_20_tex.png",NULL);
    CreateButton((Rectangle){130,30,25,25},(Color){220,220,220,255},EmptyTex,&ChangeFrame,"sub1","","assets/sub_tex.png",NULL);
    CreateButton((Rectangle){90,60,65,25},(Color){220,220,220,255},EmptyTex,&ChangeFrame,"sub20","","assets/sub_20_tex.png",NULL);

    CreateButton((Rectangle){15,100,72,72},GRAY,EmptyTex,&HoldGasterBlaster,"holdgb","","assets/gb_ui.png",NULL);

    //CreateButton((Rectangle){10,360,140,50},(Color){220,220,220,255},EmptyTex,&UnimplementedButtonFunctionality,"load","","assets/load.png",NULL);
    CreateButton((Rectangle){10,420,140,50},(Color){220,220,220,255},EmptyTex,&Export,"export","","assets/save.png",NULL);

    /*FIELDS*/
    CreateButton((Rectangle){807,37,46,16},(Color){50,50,50,25},empty_tex,&AddTypeInpField,"numinp","curr_selec_x",NULL,&is_selected);
    /*FIELDS*/

    CreateButton((Rectangle){855,Y_POSES[0][0],20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"x_orig_add","property_arrowup",NULL,NULL);
    CreateButton((Rectangle){855,Y_POSES[1][0],20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"x_orig_sub","property_arrowdown",NULL,NULL);
    CreateButton((Rectangle){930,Y_POSES[0][0],20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"y_orig_sub","property_arrowup",NULL,NULL);
    CreateButton((Rectangle){930,Y_POSES[1][0],20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"y_orig_add","property_arrowdown",NULL,NULL);
    CreateButton((Rectangle){855,Y_POSES[0][2],20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"r_orig_add","property_arrowup",NULL,NULL);
    CreateButton((Rectangle){855,Y_POSES[1][2],20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"r_orig_sub","property_arrowdown",NULL,NULL);

    CreateButton((Rectangle){855,Y_POSES[0][1],20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"x_end_add","property_arrowup",NULL,NULL);
    CreateButton((Rectangle){855,Y_POSES[1][1],20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"x_end_sub","property_arrowdown",NULL,NULL);
    CreateButton((Rectangle){930,Y_POSES[0][1],20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"y_end_sub","property_arrowup",NULL,NULL);
    CreateButton((Rectangle){930,Y_POSES[1][1],20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"y_end_add","property_arrowdown",NULL,NULL);
    CreateButton((Rectangle){930,Y_POSES[0][2],20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"r_end_add","property_arrowup",NULL,NULL);
    CreateButton((Rectangle){930,Y_POSES[1][2],20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"r_end_sub","property_arrowdown",NULL,NULL);

    /* Slight docs */
    /*
     *  0 - INDEX
     *  1 - EASING TYPE
     *  2 - X SCALE
     *  3 - Y SCALE
     *  4 - ALPHA
     *  5 - [ Dependant on type ]
     *  6 - { UNIMPLEMENTED }
     *  7 - { UNIMPLEMENTED }
     *  8 - { UNIMPLEMENTED }
     *  9 - { UNIMPLEMENTED }
    */


    /*GASTERBLASTER SPECIFIC*/
    //    CreateButton((Rectangle){855,Y_POSES[0][4]+100,20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"0_orig_add","property_arrowup",NULL,&is_selected_gb);
    //    CreateButton((Rectangle){855,Y_POSES[1][4]+100,20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"0_orig_sub","property_arrowdown",NULL,&is_selected_gb);
    /*GASTERBLASTER SPECIFIC*/
    //CreateButton((Rectangle){930,Y_POSES[0][4]+100,20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"1_orig_add","property_arrowup",NULL,&is_selected_gb); //enum
    //CreateButton((Rectangle){930,Y_POSES[1][4]+100,20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"1_orig_sub","property_arrowdown",NULL,&is_selected_gb);
//===₌₌₌==₌₌=₌=₌=₌=₌=₌=₌=₌=₌=₌=₌=₌=₌=₌
    CreateButton((Rectangle){855,Y_POSES[0][3],20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"3_orig_add","property_arrowup",NULL,NULL);
    CreateButton((Rectangle){855,Y_POSES[1][3],20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"3_orig_sub","property_arrowdown",NULL,NULL);
    CreateButton((Rectangle){930,Y_POSES[0][3],20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"2_end_add","property_arrowup",NULL,NULL);
    CreateButton((Rectangle){930,Y_POSES[1][3],20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"2_end_sub","property_arrowdown",NULL,NULL);

    //CreateButton((Rectangle){855,Y_POSES[0][4],20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"4_orig_add","property_arrowup",NULL,NULL);
    //CreateButton((Rectangle){855,Y_POSES[1][4],20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"4_orig_sub","property_arrowdown",NULL,NULL);
    //CreateButton((Rectangle){930,Y_POSES[0][4],20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"4_end_add","property_arrowup",NULL,NULL);
    //CreateButton((Rectangle){930,Y_POSES[1][4],20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"4_end_sub","property_arrowdown",NULL,NULL);
    
    /*--WALL-OF-BUTTONS--*/
    while (!WindowShouldClose())
    {
        if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S)){
            Export(0,true);
        }
        if(IsKeyPressed(KEY_PERIOD)){
            printf("\x1b[38;2;255;128;0mWARNING\x1b[0m: WEIRD SHIT MIGHT HAPPEN BECAUSE YOU PRESSED THE EMERGENCY HOTKEY \x1b[38;2;0;255;128m'.>' \x1b[38;2;255;0;0mCONSIDER SAVING\x1b[0m\n");
            holding = false;
            holding_texture = -1;
        }
        mwheel = GetMouseWheelMove();
        Vector2 mouse_pos = GetMousePosition();
        for (i=0;i<updfunc_len;i++){
            if (updfunc[i] != NULL){
                (updfunc[i])(i);
            } 
        }
        is_selected = false;
        #pragma region FILE LOADING
        /*--FILE-LOADING--*/
        if (IsFileDropped())
        {
            if (has_changes){
                printf("IMPLEMENT: UNSAVED OVERRIDE WARNING\n");
            }
            FilePathList droppedFiles = LoadDroppedFiles();
            const char* filePath = droppedFiles.paths[0];
            //memset(frames,0,sizeof(frames));
            memset(&textures,0,sizeof(textures));
            textures_len = 0;
            //textures = 
            FILE * fptr = fopen(filePath,"r");
            char spriteTag[20];
            char garbage[50];
            for (int i=0;i<largest_frame+1;i++){
                memset(&frames[i],0,sizeof(Frame));
            }
            fscanf(fptr,"%i",&glob_tex_len);
            for(int i=0;i<glob_tex_len;i++){
                TextureOBJ currtxtr;
                TextureOBJ endtxtr;
                int frameIdx;
                fscanf(fptr,"%d %s %f %f %f %f %f %f %i %i %s",&frameIdx,&spriteTag,&currtxtr.pos.x,&currtxtr.pos.y,&endtxtr.pos.x,&endtxtr.pos.y,&currtxtr.rotation,&endtxtr.rotation,&currtxtr.additional_data,&currtxtr.additional_data+1*8,&garbage);
                printf("whdashdhasdhsad %s\n",garbage);
                currtxtr.id = spriteTag;
                endtxtr.id = spriteTag;
                currtxtr.tint = WHITE;
                endtxtr.tint = GREEN;
                if (strcmp(spriteTag,"gb") == 0){
                    currtxtr.texture = gaster_blaster_tex;
                    currtxtr.texture_hit = gaster_blaster_box_tex;
                    endtxtr.texture = gaster_blaster_tex;
                    endtxtr.texture_hit = gaster_blaster_box_tex;
                }
                //locframe.textures[0] = currtxtr;
                currtxtr.start_ptr = NULL;
                endtxtr.end_ptr = NULL;
                
                SET_ADD_DATA(&currtxtr,T_OBJ_INDEX,frames[frameIdx].textures_len);
                SET_ADD_DATA(&endtxtr,T_OBJ_INDEX,frames[frameIdx].textures_len+1);
                
                currtxtr.end_ptr = &(frames[frameIdx].textures[frames[frameIdx].textures_len+1]);
                endtxtr.start_ptr = &frames[frameIdx].textures[frames[frameIdx].textures_len];
                frames[frameIdx].textures[frames[frameIdx].textures_len++] = currtxtr;
                frames[frameIdx].textures[frames[frameIdx].textures_len++] = endtxtr;
                //currtxtr.end_ptr = &frames[frameIdx].textures[frames[frameIdx].textures_len++];
                //endtxtr.start_ptr = &frames[frameIdx].textures[frames[frameIdx].textures_len++];
                Frame locframe;
                if (frameIdx == current_frame){
                    printf("erm waht te sigam %f\n",currtxtr.pos.x);
                    SET_ADD_DATA(&currtxtr,T_OBJ_INDEX,textures_len);
                    SET_ADD_DATA(&endtxtr,T_OBJ_INDEX,textures_len+1);
                    currtxtr.end_ptr = &textures[textures_len+1];
                    endtxtr.start_ptr = &textures[textures_len];
                    textures[textures_len++] = currtxtr;
                    textures[textures_len++] = endtxtr;
                    //currtxtr.end_ptr = &textures[0];
                    //endtxtr.start_ptr = &textures[1];
                    //memcpy(textures,frames[frameIdx].textures,sizeof(textures));
                }
                //locframe.textures[0] = endtxtr;
                //frames[0] = locframe;
                printf("BE SURE THAT EVERY TEXRTURE INTERACTION INCREASES glob_tex_len!\n");
                //memcpy(&frames,&locframe,sizeof(Frame));
                //printf("frmidx %i\n",frames[0].textures_len);
            }
            fclose(fptr);
            UnloadDroppedFiles(droppedFiles);    // Unload filepaths from memory
        }
        /*--FILE-LOADING--*/
        #pragma endregion FILE LOADING
        
        BeginDrawing();
            ClearBackground((Color){220,220,220,220});


            #pragma region TEXTURE LOGIC
            /*----TEXTURE-LOGIC----*/
            for(i=0;i<textures_len;i++){
                TextureOBJ tx = textures[i];
                Rectangle t_rect = (Rectangle){tx.pos.x,tx.pos.y,tx.texture.width,tx.texture.height};
                Rectangle t_rect_coll = (Rectangle){tx.pos.x-tx.texture.width/2,tx.pos.y-tx.texture.height/2,tx.texture.width,tx.texture.height};
                Vector2 line_p1;
                Vector2 line_p2;
                TextureOBJ * other_ptr;
                Color line_col;
                if(IsMouseButtonPressed(1) && selected_texture != -1){
                    selected_texture = -1;
                }
                if (i == selected_texture && holding_texture == -1){
                    RotateSpriteMouse(i);
                    tx = textures[i];
                }
                if (CheckCollisionRecs((Rectangle){mouse_pos.x,mouse_pos.y,1,1},t_rect_coll)){
                    if(IsMouseButtonPressed(0) && holding_texture == -1 && tx.pickup_cooldown <= 0 && glob_pickup_cd == 0){
                        holding_texture = i;
                        selected_texture = i;
                        AddFunctionToItterator(&GasterBlasterHold);
                    }
                    if(IsMouseButtonDown(1)){
                        selected_texture = -1;
                        glob_tex_len-=1;
                        if (tx.end_ptr != NULL){
                            textures[*(int*)GET_ADD_DATA(tx.end_ptr,T_OBJ_INDEX)] = EmptyTexOBJ;
                            //*(textures[i].end_ptr) = EmptyTexOBJ; old way
                        }else {
                            textures[*(int*)GET_ADD_DATA(tx.start_ptr,T_OBJ_INDEX)] = EmptyTexOBJ;
                            //*(textures[i].start_ptr) = EmptyTexOBJ;
                        }
                        textures[i] = EmptyTexOBJ;
                        tx = textures[i];
                        continue;
                    }
                    if(IsMouseButtonPressed(2)){
                        selected_texture = i;
                    }
                    if (tx.pickup_cooldown > 0) { tx.pickup_cooldown--;}
                }
                if (!TEXTURE_OBJ_EQUALS(tx,EmptyTexOBJ)){
                    if (tx.end_ptr != NULL){
                        other_ptr = tx.end_ptr;
                        line_p1 = tx.pos;
                        line_p2 = tx.end_ptr->pos;
                        tx.real_ptr = tx.end_ptr;
                    }else{
                        tx.real_ptr = tx.start_ptr;
                        line_p2 = tx.pos;
                        line_p1 = tx.start_ptr->pos;
                        other_ptr = tx.start_ptr;
                    }
                    if ((i == selected_texture || *(int*)GET_ADD_DATA(other_ptr,T_OBJ_INDEX) == selected_texture)){
                        line_col = BLUE;
                    }else{
                        line_col = RED;
                    }
                    if (i == holding_texture){
                        tx.pos = mouse_pos;
                        textures[i] = tx;
                    }
                    if(textures[i].end_ptr != NULL)
                        DrawLineEx(line_p1,line_p2,3,line_col);
                    Color temptint = tx.tint;
                    if (i == selected_texture){
                        temptint = AddColors(tx.tint, (Color){170,170,238,255});
                        if (ColorsEqual(temptint,WHITE)){
                            temptint = (Color){170,170,238,255};
                        }
                    }
                    DrawTexturePro(tx.texture,(Rectangle){0,0,tx.texture.width,tx.texture.height},t_rect,(Vector2){tx.texture.width/2,tx.texture.height/2},tx.rotation,temptint);
                    if (!TextureEquals(tx.texture_hit,EmptyTex)){
                        DrawTexturePro(tx.texture_hit,(Rectangle){0,0,tx.texture.width,tx.texture.height},t_rect,(Vector2){tx.texture.width/2,tx.texture.height/2},0,WHITE);
                    }
                } //TODO: implement scaling
                textures[i] = tx;
            }
            /*----TEXTURE-LOGIC----*/
            #pragma endregion TEXTURE LOGIC

            if (selected_texture != -1){
                if (strcmp(textures[selected_texture].id,"gb") == 0){
                    is_selected_gb = true;
                }else{
                    is_selected_gb = false;
                }
            }
            #pragma region UI
            /*----UI----*/
            DrawRectangle(0,0,160,screenHeight,RAYWHITE);
            DrawRectangle(800,0,160,screenHeight,RAYWHITE);
            //DrawRectangle(5,90,150,200,LIGHTGRAY);
            
            if(selected_texture != -1){
                is_selected = true;
                DrawText(expand_tag(textures[selected_texture].id),810,2,15,GRAY);
                DrawLineEx((Vector2){880,15},(Vector2){880,470},1,GRAY);
                TextureOBJ orig = *GET_START(&textures[selected_texture]);
                TextureOBJ end = *GET_END(&textures[selected_texture]);

                /*LINES*/
                DrawLineEx((Vector2){805,15},(Vector2){956,15},1,GRAY);
                DrawLineEx((Vector2){805,55},(Vector2){956,55},1,GRAY);
                DrawLineEx((Vector2){805,95},(Vector2){956,95},1,GRAY);
                DrawLineEx((Vector2){805,135},(Vector2){956,135},1,GRAY);
                DrawLineEx((Vector2){805,175},(Vector2){956,175},1,GRAY);
                DrawLineEx((Vector2){805,215},(Vector2){956,215},2,GRAY);
                DrawLineEx((Vector2){805,255},(Vector2){956,255},1,GRAY);
                DrawLineEx((Vector2){805,255},(Vector2){956,255},1,GRAY);
                /*LINES*/
                DrawText("START",UI_TXT_LEFT_X ,Y_POSES_TEXT[1][0],12,GRAY);
                DrawText("START",UI_TXT_RIGHT_X,Y_POSES_TEXT[1][0],12,GRAY);
                DrawText("START",UI_TXT_LEFT_X ,Y_POSES_TEXT[1][2],12,GRAY);
                DrawText("START",UI_TXT_LEFT_X ,Y_POSES_TEXT[1][3],12,GRAY);
                DrawText("START",UI_TXT_LEFT_X ,Y_POSES_TEXT[1][4],12,GRAY);
                DrawText("START",UI_TXT_LEFT_X ,Y_POSES_TEXT[1][5],12,GRAY);

                DrawText("X"     ,UI_TXT_LEFT_X ,Y_POSES_TEXT[0][0],12,GRAY);
                DrawText("Y"     ,UI_TXT_RIGHT_X,Y_POSES_TEXT[0][0],12,GRAY);
                DrawText("ROT"   ,UI_TXT_LEFT_X ,Y_POSES_TEXT[0][2],12,GRAY);
                DrawText("XSCALE",UI_TXT_LEFT_X ,Y_POSES_TEXT[0][3],12,GRAY);
                DrawText("YSCALE",UI_TXT_LEFT_X ,Y_POSES_TEXT[0][4],12,GRAY);
                DrawText("ALPHA",UI_TXT_LEFT_X ,Y_POSES_TEXT[0][5],12,GRAY);

                DrawRectangle(806,36,48,18,DARKGRAY); //inp field
                DrawRectangle(807,37,46,16,WHITE);    //inp field
                DrawText(TextFormat("\n%i",(int)orig.pos.x-160),808,21,20,GRAY);
                DrawText(TextFormat("\n%i",(int)orig.pos.y*-1+480),UI_TXT_RIGHT_X,20,20,GRAY);
                DrawText(TextFormat("\n%i",(int)orig.rotation),UI_TXT_LEFT_X,100,20,GRAY);
                DrawText(TextFormat("\n%i",((textures[*(int*)GET_ADD_DATA(&orig,T_OBJ_INDEX)].additional_data+3*8))),UI_TXT_LEFT_X,140,20,GRAY);
                
                DrawText("END",UI_TXT_LEFT_X ,Y_POSES_TEXT[1][1],12,GRAY);
                DrawText("END",UI_TXT_RIGHT_X,Y_POSES_TEXT[1][1],12,GRAY);
                DrawText("END",UI_TXT_RIGHT_X,Y_POSES_TEXT[1][2],12,GRAY);
                DrawText("END",UI_TXT_RIGHT_X,Y_POSES_TEXT[1][3],12,GRAY);
                DrawText("END",UI_TXT_RIGHT_X,Y_POSES_TEXT[1][4],12,GRAY);
                DrawText("END",UI_TXT_RIGHT_X,Y_POSES_TEXT[1][5],12,GRAY);
                
                DrawText("X"     ,UI_TXT_LEFT_X ,Y_POSES_TEXT[0][1],12,GRAY);
                DrawText("Y"     ,UI_TXT_RIGHT_X,Y_POSES_TEXT[0][1],12,GRAY);
                DrawText("ROT"   ,UI_TXT_RIGHT_X,Y_POSES_TEXT[0][2],12,GRAY);
                DrawText("XSCALE",UI_TXT_RIGHT_X,Y_POSES_TEXT[0][3],12,GRAY);
                DrawText("YSCALE",UI_TXT_RIGHT_X,Y_POSES_TEXT[0][4],12,GRAY);
                DrawText("ALPHA",UI_TXT_RIGHT_X,Y_POSES_TEXT[0][5],12,GRAY);

                DrawText(TextFormat("\n%i",(int)end.pos.x-160),UI_TXT_LEFT_X,60,20,GRAY);
                DrawText(TextFormat("\n%i",(int)end.pos.y*-1+480),UI_TXT_RIGHT_X,60,20,GRAY);
                DrawText(TextFormat("\n%i",(int)end.rotation),UI_TXT_RIGHT_X,100,20,GRAY);
                DrawText(TextFormat("\n%i",((textures[*(int*)GET_ADD_DATA(&end,T_OBJ_INDEX)].additional_data+2*8))),UI_TXT_RIGHT_X,140,20,GRAY);
                //DrawText(TextFormat("\n%i",(*(int*)(end.additional_data+4*8))),UI_TXT_RIGHT_X-500,180,20,GRAY);

                //char * current_ease_str = (char*)EasingTypes[(*(int*)(GET_START(&textures[selected_texture])->additional_data+8))];
                //int num_strs;
                //const char ** strs_res = TextSplit(current_ease_str,(char)'_',&num_strs);

                //char * strs;// = strs_res[0];
                //strcpy(strs,strs_res[0]);
                //char * strs = ((char**)TextSplit(current_ease_str,(char)'\n',ptrtmp))[0];
                //DrawText("EASE",UI_TXT_RIGHT_X,136,14,GRAY);
                //for(int i=0;i<num_strs;i++){
                    //DrawText(TextFormat("%s",strs_res[i]),UI_TXT_RIGHT_X,136+10*(i+1),12,GRAY);
                //}
                //DrawText(TextFormat("\n%s",strs_res[1]),rightX,133,18,GRAY);
                //DrawText(TextFormat("\n%s",strs_res[2]),rightX,145,18,GRAY);


                if(strcmp(textures[selected_texture].id,"gb") == 0){
                    //DrawText("CHARGE",UI_TXT_LEFT_X,136,12,GRAY);
                    //DrawText("TIME",UI_TXT_LEFT_X,146,12,GRAY);
                    //DrawText(TextFormat("\n%i",(*(int*)(GET_START(&textures[selected_texture])->additional_data))),UI_TXT_LEFT_X,140,20,GRAY);
                }
                
            }
            /*----UI----*/
            #pragma endregion UI


            #pragma region BUTTON LOGIC
            /*----BUTTON-LOGIC----*/
            for(i=0;i<button_index;i++){
                if(buttons[i].override_visibility == NULL || *buttons[i].override_visibility == true){
                    Color color = buttons[i].color;
                    Color tint = WHITE;
                    bool active = true;
                    if((buttons[i].texture_tag == "property_arrowdown" || buttons[i].texture_tag == "property_arrowup") && selected_texture == -1){
                        color.a = 0;
                        tint.a = 0;
                        active = false;
                        buttons[i].frames_held = 0;
                    }
                    if((buttons[i].texture_tag == "numinp" || buttons[i].texture_tag == "curr_selec_x") && selected_texture != -1){
                        buttons[i].callback_values[0] = &textures[selected_texture].pos.x;
                    }else if((buttons[i].texture_tag == "numinp" || buttons[i].texture_tag == "curr_selec_y") && selected_texture != -1){
                        buttons[i].callback_values[0] = &textures[selected_texture].pos.y;
                    }else if((buttons[i].texture_tag == "numinp" || buttons[i].texture_tag == "curr_selec_rot") && selected_texture != -1){
                        buttons[i].callback_values[0] = &textures[selected_texture].rotation;
                    }else if((buttons[i].texture_tag == "numinp" || buttons[i].texture_tag == "end_selec_x") && selected_texture != -1){
                        buttons[i].callback_values[0] = &textures[selected_texture].real_ptr->pos.x;
                    }else if((buttons[i].texture_tag == "numinp" || buttons[i].texture_tag == "end_selec_y") && selected_texture != -1){
                        buttons[i].callback_values[0] = &textures[selected_texture].real_ptr->pos.y;
                    }else if((buttons[i].texture_tag == "numinp" || buttons[i].texture_tag == "end_selec_rot") && selected_texture != -1){
                        buttons[i].callback_values[0] = &textures[selected_texture].real_ptr->rotation;
                    }
                    if(((buttons[i].box.x < mouse_pos.x && (buttons[i].box.x + buttons[i].box.width) > mouse_pos.x) && (buttons[i].box.y < mouse_pos.y && (buttons[i].box.y+ buttons[i].box.height) > mouse_pos.y)) && active == true){
                        color.r -= 25;
                        color.g -= 25;
                        color.b -= 25;
                        tint.r -= 10;
                        tint.g -= 10;
                        tint.b -= 10;
                        if (IsMouseButtonDown(0)){
                            color.r += 15;
                            color.g += 15;
                            color.b += 15;
                            tint.r += 10;
                            tint.g += 10;
                            tint.b += 10;
                            last_button = i;
                            has_changes = true;
                            (buttons[i].callback)(i,buttons[i].tag);
                            buttons[i].pressed = true;
                        }else{
                            buttons[i].pressed = false;
                            buttons[i].frames_held = 0;
                        }
                    }else{
                        buttons[i].pressed = false;
                        buttons[i].frames_held = 0;

                    }
                    float rot = 0;
                    Vector2 origin = {0,0};
                    if(buttons[i].texture_tag == "property_arrowdown") {rot = 180; origin = (Vector2){buttons[i].box.width,buttons[i].box.height};}
                    Rectangle src = buttons[i].box;
                    src.x = 0;
                    src.y = 0;
                    DrawRectangleRec(buttons[i].box,color);
                    DrawTexturePro(buttons[i].texture,src,buttons[i].box,origin,rot,tint);
                }
            }
            /*----BUTTON-LOGIC----*/
            #pragma endregion BUTTON LOGIC

            current_frame = Clamp(current_frame,0,65534);

            DrawText(TextFormat("Current frame",current_frame), 5,5, 20, GRAY);
            DrawText(TextFormat("%i",current_frame), 35, 32, 20, GRAY);
            DrawRectangle(0,447,13+MeasureText("Exported!",20),33,(Color){130,130,130,EaseExpoOut(framesExport,0,255,80)});
            DrawRectangle(0,450,10+MeasureText("Exported!",20),30,(Color){245,245,245,EaseExpoOut(framesExport,0,255,80)});
            DrawText("Exported!",5,455,20,(Color){80,80,80,EaseExpoOut(framesExport,0,255,80)});
            if((ticks % 10) == 0){
                TextureOBJ temp_textures[1000];
                int temp_textures_len = 0;
                int total_shifted = 0;
                for(i=0;i<textures_len;i++){
                    if (!TEXTURE_OBJ_EQUALS(textures[i],EmptyTexOBJ)){
                        temp_textures[temp_textures_len] = textures[i];
                        SET_ADD_DATA(&temp_textures[temp_textures_len],&temp_textures_len,temp_textures_len); 
                        temp_textures_len++;
                    }else{
                        total_shifted++;
                    }
                }
                textures_len = temp_textures_len;
                for(i=0;i<textures_len;i++){
                    memcpy(textures->additional_data+i*8,temp_textures->additional_data+i*8,sizeof(textures->additional_data)); 
                    if(textures[i].end_ptr != NULL){
                        if(*(int*)GET_ADD_DATA(textures[i].end_ptr,T_OBJ_INDEX) >= textures_len){
                            textures[i].end_ptr = &textures[*(int*)GET_ADD_DATA(textures[i].end_ptr,T_OBJ_INDEX)-total_shifted];
                        }
                    }
                    if(textures[i].start_ptr != NULL){
                        if(*(int*)GET_ADD_DATA(textures[i].start_ptr,T_OBJ_INDEX) >= textures_len){
                            textures[i].start_ptr = &textures[*(int*)GET_ADD_DATA(textures[i].start_ptr,T_OBJ_INDEX)-total_shifted];
                        }
                    }
                }
                void (*temp_updfunc[99])();
                int temp_updfunc_len = 0;
                for(i=0;i<updfunc_len;i++){
                    if (updfunc[i] != NULL){
                        temp_updfunc[temp_updfunc_len++] = updfunc[i];
                    }
                }
                updfunc_len = temp_updfunc_len;
            }
        EndDrawing();
        framesExport--;
        framesExport = (int)Clamp(framesExport,0,80);
        ticks++;
        if (glob_pickup_cd > 0){
            glob_pickup_cd--;
        }
    }
    free(frames);
    CloseWindow();
    return 0;
}
