#pragma region INCLUDES
/*----INCLUDES----*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib/raylib.h>
#include <raylib/raymath.h>
#include <raylib/reasings.h>
/*----INCLUDES----*/
#pragma endregion INCLUDES

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
    int frames_held;
    int * callback_values[10];
    void (* callback)();
} Button;
typedef struct TextureOBJ TextureOBJ;
 struct TextureOBJ{
    Texture texture;
    Texture texture_hit;
    TextureOBJ * end_ptr;
    TextureOBJ * start_ptr;
    Color tint;
    Vector2 pos;
    unsigned char pickup_cooldown;
    char * id;
    int index;
    float rotation;
};

typedef struct Frame {
    char * attack;
    void * args[5];
    TextureOBJ textures[1000];
    int textures_len;
}Frame;


/*----STRUCTS----*/
#pragma endregion STRUCTS

/* If two textures equal */
bool TextureEquals(Texture t1, Texture t2){
    if (t1.id != t2.id)
        {return false;}
    if (t1.width != t2.width)
        {return false;}
    if (t1.height != t2.height)
        {return false;}
    if (t1.mipmaps != t2.mipmaps)
        {return false;}
    if (t1.format != t2.format)
        {return false;}

    return true;
}

/* If two textureobjs equal*/
bool TextureOBJEquals(TextureOBJ t1,TextureOBJ t2){
    if (!TextureEquals(t1.texture,t2.texture))
        {return false;}

    if (!TextureEquals(t1.texture_hit,t2.texture_hit))
        {return false;}

    if (t1.pos.x != t2.pos.x)
        {return false;}
    if (t1.pos.y != t2.pos.y)
        {return false;}
    if (t1.end_ptr != t2.end_ptr)
        {return false;}
    if (t1.rotation != t2.rotation)
        {return false;}
    if (t1.id != t2.id)
        {return false;}
    if (t1.index != t2.index)
        {return false;}
    return true;
}

#pragma region GLOBALS
/*----GLOBALS----*/


/*-INTS-*/
static int button_index = 0;
static int current_frame = 0;
static int updfunc_len = 0;
static int largest_frame = 0;
static int ticks = 0;
static int holding_texture = -1;
static int selected_texture = -1;
static int textures_len = 0;
static int framesExport = 0;
/*-INTS-*/

/*-TEXTURES-*/
Texture tex_in_hand;
Texture missing_tex;
Texture gaster_blaster_tex;
Texture empty_tex;
/*-TEXTURES-*/

/*-OTHER-*/
static float mwheel;
static bool holding = false;
static Button buttons[99];
void (*updfunc[99])();
static TextureOBJ textures[1000];
static Frame * frames;
/*-OTHER-*/


/*-EMPTY-*/
static const TextureOBJ EmptyTexOBJ;
static const Texture EmptyTex;
/*-EMPTY-*/

/*----GLOBALS----*/
#pragma endregion GLOBALS


/* Creates a button */
void CreateButton(Rectangle box,Color color,Texture tex_override,void (* callback)(),char * tag,char * texture_tag,char * img_path){
    buttons[button_index].box = box;
    buttons[button_index].color = color;
    buttons[button_index].callback = callback;
    buttons[button_index].pressed = false;
    buttons[button_index].frames_held = 0;
    buttons[button_index].tag = tag;
    buttons[button_index].texture_tag = texture_tag;
    buttons[button_index].texture = (img_path != NULL) ? LoadTexture(img_path) : (!TextureEquals(tex_override, EmptyTex)) ? tex_override : missing_tex;

    button_index++;
}

/* Called in every function chaning the frame number */
void FrameChangedCallback(int nextframe){
    int real_nextframe = (int)Clamp(current_frame+nextframe,0,65534);
    memcpy(frames[current_frame].textures,textures,sizeof(frames[current_frame].textures)); 
    frames[current_frame].textures_len = textures_len;
    memcpy(textures,frames[real_nextframe].textures,sizeof(frames[real_nextframe].textures));
    textures_len = frames[real_nextframe].textures_len;
    if (real_nextframe > largest_frame) {largest_frame = real_nextframe;}
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
        if (textures[selected_texture].end_ptr == NULL){
            if(context_tag == "x_orig_add"){
                textures[selected_texture].start_ptr->pos.x+=(int)Clamp(buttons[i].frames_held % 20,1,2);
            }else if(context_tag == "x_orig_sub"){
                textures[selected_texture].start_ptr->pos.x-=(int)Clamp(buttons[i].frames_held % 20,1,2);
            }else if(context_tag == "x_end_add"){
                textures[selected_texture].pos.x+=(int)Clamp(buttons[i].frames_held % 20,1,2);
            }else if(context_tag == "x_end_sub"){
                textures[selected_texture].pos.x-=(int)Clamp(buttons[i].frames_held % 20,1,2);
            }else if(context_tag == "y_orig_add"){
                textures[selected_texture].start_ptr->pos.y-=(int)Clamp(buttons[i].frames_held % 20,1,2);
            }else if(context_tag == "y_orig_sub"){
                textures[selected_texture].start_ptr->pos.y+=(int)Clamp(buttons[i].frames_held % 20,1,2);
            }else if(context_tag == "y_end_add"){
                textures[selected_texture].pos.y-=(int)Clamp(buttons[i].frames_held % 20,1,2);
            }else if(context_tag == "y_end_sub"){
                textures[selected_texture].pos.y+=(int)Clamp(buttons[i].frames_held % 20,1,2);
            }else if(context_tag == "rot_orig_add"){
                textures[selected_texture].start_ptr->rotation+=(int)Clamp(buttons[i].frames_held % 20,1,2);
            }else if(context_tag == "rot_orig_sub"){
                textures[selected_texture].start_ptr->rotation+=(int)Clamp(buttons[i].frames_held % 20,1,2);
            }else if(context_tag == "rot_end_add"){
                textures[selected_texture].rotation+=(int)Clamp(buttons[i].frames_held % 20,1,2);
            }else if(context_tag == "rot_end_sub"){
                textures[selected_texture].rotation+=(int)Clamp(buttons[i].frames_held % 20,1,2);
            }
        }else{
            if(context_tag == "x_orig_add"){
                textures[selected_texture].pos.x+=(int)Clamp(buttons[i].frames_held % 20,1,2);
            }else if(context_tag == "x_orig_sub"){
                textures[selected_texture].pos.x-=(int)Clamp(buttons[i].frames_held % 20,1,2);
            }else if(context_tag == "x_end_add"){
                textures[selected_texture].end_ptr->pos.x+=(int)Clamp(buttons[i].frames_held % 20,1,2);
            }else if(context_tag == "x_end_sub"){
                textures[selected_texture].end_ptr->pos.x-=(int)Clamp(buttons[i].frames_held % 20,1,2);
            }else if(context_tag == "y_orig_add"){
                textures[selected_texture].pos.y-=(int)Clamp(buttons[i].frames_held % 20,1,2);
            }else if(context_tag == "y_orig_sub"){
                textures[selected_texture].pos.y+=(int)Clamp(buttons[i].frames_held % 20,1,2);
            }else if(context_tag == "y_end_add"){
                textures[selected_texture].end_ptr->pos.y-=(int)Clamp(buttons[i].frames_held % 20,1,2);
            }else if(context_tag == "y_end_sub"){
                textures[selected_texture].end_ptr->pos.y+=(int)Clamp(buttons[i].frames_held % 20,1,2);
            }else if(context_tag == "rot_orig_add"){
                textures[selected_texture].rotation+=(int)Clamp(buttons[i].frames_held % 20,1,2);
            }else if(context_tag == "rot_orig_sub"){
                textures[selected_texture].rotation+=(int)Clamp(buttons[i].frames_held % 20,1,2);
            }else if(context_tag == "rot_end_add"){
                textures[selected_texture].end_ptr->rotation+=(int)Clamp(buttons[i].frames_held % 20,1,2);
            }else if(context_tag == "rot_end_sub"){
                textures[selected_texture].end_ptr->rotation+=(int)Clamp(buttons[i].frames_held % 20,1,2);
            }
        }
    }
    buttons[i].frames_held++;
}


/* Adds function [ func ] to the [ updfunc ] function array */
void AddFunctionToItterator(void* func){
    if (holding == false){
        updfunc[updfunc_len++] = func;
        holding = true;
    }
}
/* Button function for gasterblasters */
void HoldGasterBlaster();

/* Sprite rotation using the mousewheel */
void RotateSpriteMouse(int i){
    if (mwheel != 0){
        if(!IsKeyDown(KEY_LEFT_CONTROL)){mwheel*=10;}
        if(IsKeyDown(KEY_LEFT_ALT)){mwheel=90;}
        textures[i].rotation-=mwheel;
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

/* Gaster blaster holding logic */
void GasterBlasterHold(){
    if(!IsMouseButtonDown(1)){
        RotateSpriteMouse(holding_texture);
    }else{
        updfunc[updfunc_len] = NULL;
        holding = false;
        if(textures[holding_texture].end_ptr != NULL){
            *(textures[holding_texture].end_ptr) = EmptyTexOBJ;
        }else if(textures[holding_texture].start_ptr != NULL){
            *(textures[holding_texture].start_ptr) = EmptyTexOBJ;
        }
        textures[holding_texture] = EmptyTexOBJ;
        holding_texture = -1;
        selected_texture = -1;
    }
    if(IsMouseButtonPressed(0) || IsKeyPressed(KEY_SPACE)){
        float saverot = textures[holding_texture].rotation;
        textures[holding_texture].pickup_cooldown = 1;
        updfunc[updfunc_len] = NULL;
        holding = false;
        selected_texture = holding_texture;
        if (IsKeyDown(KEY_LEFT_SHIFT) && textures[holding_texture].end_ptr != NULL){
            HoldGasterBlaster();
            textures[holding_texture].rotation = saverot;
        }else{
            holding_texture = -1;
        }
        FrameChangedCallback(0);
    }
}

/* Adds a gaster blaster to [ updfunc ] and initializes it */
void HoldGasterBlaster(){
    if (!holding){
        Texture blaster = gaster_blaster_tex;
        Texture frame = LoadTexture("assets/gaster_blaster_hit.png");
        textures[textures_len+1] = (TextureOBJ){.texture = blaster,
                                                .texture_hit = frame,
                                                .end_ptr = NULL,
                                                .start_ptr = &textures[textures_len],
                                                .pos = (Vector2){480,280},
                                                .id = "gb",
                                                .rotation = 90,
                                                .tint = GREEN,
                                                .pickup_cooldown = 0,
                                                .index = textures_len+1};
        textures[textures_len] = (TextureOBJ){.texture = blaster,
                                              .texture_hit = frame,
                                              .end_ptr = &textures[textures_len+1],
                                              .start_ptr = NULL,
                                              .pos = (Vector2){480,240},
                                              .id = "gb",
                                              .rotation = 0,
                                              .tint = WHITE,
                                              .pickup_cooldown = 0,
                                              .index = textures_len};
        holding_texture = textures_len;
        selected_texture = textures_len;
        textures_len+=2;
    }
    AddFunctionToItterator(&GasterBlasterHold);
}

/* [ updfunc ] function that is used to write into input fields */
void TypeInputField(int i){
    if (buttons[i].tag == "numinp"){
        //printf("%i",GetCharPressed());
    }else{

    }
}

/* adds [ TypeInputField ] to [ updfunc ]*/
void AddTypeInpField(){
    AddFunctionToItterator(&TypeInputField);
}

/* Exports the current workspace into a catak.lua parsable file */
void Export(void){
    FILE *fptr;

    fptr = fopen("export.txt","w+");

    for(int i=0;i<largest_frame+1;i++){
        Frame locframe = frames[i];
        for(int j=0;j<locframe.textures_len;j++){
            TextureOBJ txj = locframe.textures[j];
            if(txj.end_ptr == NULL) {continue;}
            /*-           fr id x- y- ex ey r- er-*/
            fprintf(fptr,"%i %s %i %i %i %i %i %i\n",i,txj.id,(int)txj.pos.x-160,(int)txj.pos.y,(int)txj.end_ptr->pos.x-160,(int)txj.end_ptr->pos.y,(int)txj.rotation,(int)txj.end_ptr->rotation);
        }
    }

    fclose(fptr);

    framesExport = 80;
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

/* Entrypoint */
int main(void)
{
    frames = malloc(60000*sizeof(Frame));
    memset(frames,0,sizeof(frames));
    int i,j,k,l,m; //itterators
    const int screenWidth = 960;
    const int screenHeight = 480;
    Image icon = LoadImage("assets/icon_alt.png");
    Image icon_s = LoadImage("assets/icon_small.png");
    Image *icons = malloc(sizeof(Image)*2);
    icons[0] = icon;
    icons[1] = icon_s;
    
    InitWindow(screenWidth, screenHeight, "Catak - Create Your Frisk attack helper [ ALPHA 0.2 ]");
    /*TEXTURES*/
    
    gaster_blaster_tex = LoadTexture("assets/gaster_blaster.png");
    empty_tex = LoadTexture("assets/empty.png");

    Texture arrow_tex = LoadTexture("assets/arrow_ui.png");
    
    /*TEXTURES*/
    
    //missing_tex = LoadTexture("assets/empty.png");
    SetWindowIcons(icons,2);
    SetTargetFPS(60);           
    /*--WALL-OF-BUTTONS--*/
    CreateButton((Rectangle){5,30,25,25},(Color){220,220,220,255},EmptyTex,&ChangeFrame,"inc1","","assets/plus_tex.png");
    CreateButton((Rectangle){5,60,65,25},(Color){220,220,220,255},EmptyTex,&ChangeFrame,"inc20","","assets/plus_20_tex.png");
    CreateButton((Rectangle){130,30,25,25},(Color){220,220,220,255},EmptyTex,&ChangeFrame,"sub1","","assets/sub_tex.png");
    CreateButton((Rectangle){90,60,65,25},(Color){220,220,220,255},EmptyTex,&ChangeFrame,"sub20","","assets/sub_20_tex.png");

    CreateButton((Rectangle){15,100,72,72},GRAY,EmptyTex,&HoldGasterBlaster,"holdgb","","assets/gb_ui.png");

    CreateButton((Rectangle){10,360,140,50},(Color){220,220,220,255},EmptyTex,&UnimplementedButtonFunctionality,"load","","assets/load.png");
    CreateButton((Rectangle){10,420,140,50},(Color){220,220,220,255},EmptyTex,&Export,"export","","assets/save.png");

    /*FIELDS*/
    CreateButton((Rectangle){100,360,140,50},(Color){220,220,220,255},empty_tex,&AddTypeInpField,"numinp","",NULL);
    /*FIELDS*/

    CreateButton((Rectangle){855,20,20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"x_orig_add","property_arrowup",NULL);
    CreateButton((Rectangle){855,38,20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"x_orig_sub","property_arrowdown",NULL);
    CreateButton((Rectangle){930,20,20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"y_orig_add","property_arrowup",NULL);
    CreateButton((Rectangle){930,38,20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"y_orig_sub","property_arrowdown",NULL);
    CreateButton((Rectangle){855,100,20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"rot_orig_add","property_arrowup",NULL);
    CreateButton((Rectangle){855,118,20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"rot_orig_sub","property_arrowdown",NULL);

    CreateButton((Rectangle){855,60,20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"x_end_add","property_arrowup",NULL);
    CreateButton((Rectangle){855,78,20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"x_end_sub","property_arrowdown",NULL);
    CreateButton((Rectangle){930,60,20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"y_end_add","property_arrowup",NULL);
    CreateButton((Rectangle){930,78,20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"y_end_sub","property_arrowdown",NULL);
    CreateButton((Rectangle){930,100,20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"rot_end_add","property_arrowup",NULL);
    CreateButton((Rectangle){930,118,20,15},(Color){220,220,220,255},arrow_tex,&ChangeProperty,"rot_end_sub","property_arrowdown",NULL);
    /*--WALL-OF-BUTTONS--*/
    while (!WindowShouldClose())
    {  
        mwheel = GetMouseWheelMove();
        Vector2 mouse_pos = GetMousePosition();
        for (i=0;i<updfunc_len;i++){
            (updfunc[i])();
        }
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
                    if(IsMouseButtonPressed(0) && holding_texture == -1 && tx.pickup_cooldown <= 0){
                        holding_texture = i;
                        selected_texture = i;
                        AddFunctionToItterator(&GasterBlasterHold);
                    }
                    if(IsMouseButtonDown(1)){
                        selected_texture = -1;
                        if (tx.end_ptr != NULL){
                            *(textures[i].end_ptr) = EmptyTexOBJ;
                        }else {
                            printf("%f %i\n",textures[i].start_ptr->pos.y,textures[i].start_ptr->index);
                            *(textures[i].start_ptr) = EmptyTexOBJ;
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
                if (!TextureOBJEquals(tx,EmptyTexOBJ)){
                    if (tx.end_ptr != NULL){
                        other_ptr = tx.end_ptr;
                        line_p1 = tx.pos;
                        line_p2 = tx.end_ptr->pos;
                    }else{
                        line_p2 = tx.pos;
                        line_p1 = tx.start_ptr->pos;
                        other_ptr = tx.start_ptr;
                    }
                    if ((i == selected_texture || other_ptr->index == selected_texture)){
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

            
            #pragma region UI
            /*----UI----*/
            DrawRectangle(0,0,160,screenHeight,RAYWHITE);
            DrawRectangle(800,0,160,screenHeight,RAYWHITE);
            DrawRectangle(5,90,150,200,LIGHTGRAY);
            
            if(selected_texture != -1){
                DrawText("GASTER BLASTER",810,2,15,GRAY);
                DrawLineEx((Vector2){880,15},(Vector2){880,470},1,GRAY);
                TextureOBJ orig;
                TextureOBJ end;
                if (textures[selected_texture].end_ptr != NULL ){
                    orig = textures[selected_texture];
                    end = *textures[selected_texture].end_ptr;
                }else{
                    orig = *textures[selected_texture].start_ptr;
                    end = textures[selected_texture];
                }
                /*LINES*/
                DrawLineEx((Vector2){805,15},(Vector2){956,15},1,GRAY);
                DrawLineEx((Vector2){805,55},(Vector2){956,55},1,GRAY);
                DrawLineEx((Vector2){805,95},(Vector2){956,95},1,GRAY);
                DrawLineEx((Vector2){805,135},(Vector2){956,135},1,GRAY);
                /*LINES*/
                DrawText("START",810,26,12,GRAY);
                DrawText("START",884,26,12,GRAY);
                DrawText("START",810,106,12,GRAY);
                DrawText("X",810,16,12,GRAY);
                DrawText("Y",884,16,12,GRAY);
                DrawText("ROT",810,96,12,GRAY);

                DrawRectangle(808,35,42,18,DARKGRAY);
                DrawRectangle(809,36,40,16,WHITE);
                DrawText(TextFormat("\n%i",(int)orig.pos.x-160),810,20,20,GRAY);
                DrawText(TextFormat("\n%i",(int)orig.pos.y),884,20,20,GRAY);
                DrawText(TextFormat("\n%i",(int)orig.rotation),810,100,20,GRAY);
                
                DrawText("END",810,66,12,GRAY);
                DrawText("END",884,66,12,GRAY);
                DrawText("END",884,106,12,GRAY);
                DrawText("X",810,56,12,GRAY);
                DrawText("Y",884,56,12,GRAY);
                DrawText("ROT",884,96,12,GRAY);
                DrawText(TextFormat("\n%i",(int)end.pos.x-160),810,60,20,GRAY);
                DrawText(TextFormat("\n%i",(int)end.pos.y),884,60,20,GRAY);
                DrawText(TextFormat("\n%i",(int)end.rotation),884,100,20,GRAY);
                
            }
            /*----UI----*/
            #pragma endregion UI


            #pragma region BUTTON LOGIC
            /*----BUTTON-LOGIC----*/
            for(i=0;i<button_index;i++){
                Color color = buttons[i].color;
                Color tint = WHITE;
                bool active = true;
                if((buttons[i].texture_tag == "property_arrowdown" || buttons[i].texture_tag == "property_arrowup") && selected_texture == -1){
                    color.a = 0;
                    tint.a = 0;
                    active = false;
                    buttons[i].frames_held = 0;
                }
                if(((buttons[i].box.x < mouse_pos.x && (buttons[i].box.x + buttons[i].box.width) > mouse_pos.x) && (buttons[i].box.y < mouse_pos.y && (buttons[i].box.y+ buttons[i].box.height) > mouse_pos.y)) && active == true){
                    color.r -= 25;
                    color.g -= 25;
                    color.b -= 25;
                    tint.r -= 10;
                    tint.g -= 10;
                    tint.b -= 10;
                    if (IsMouseButtonDown(0)){
                    
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
                    if (!TextureOBJEquals(textures[i],EmptyTexOBJ)){
                        temp_textures[temp_textures_len] = textures[i];
                        temp_textures[temp_textures_len].index = temp_textures_len;
                        temp_textures_len++;
                    }else{
                        total_shifted++;
                    }
                }
                printf("%i \n",temp_textures_len);
                memcpy(&textures,&temp_textures,sizeof(textures)); 
                textures_len = temp_textures_len;
                for(i=0;i<textures_len;i++){
                    if(textures[i].end_ptr != NULL){
                        if(textures[i].end_ptr->index >= textures_len){
                            textures[i].end_ptr = &textures[textures[i].end_ptr->index-total_shifted];
                        }
                    }
                    if(textures[i].start_ptr != NULL){
                        if(textures[i].start_ptr->index >= textures_len){
                            textures[i].start_ptr = &textures[textures[i].start_ptr->index-total_shifted];
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
    }
    free(frames);
    CloseWindow();
    return 0;
}