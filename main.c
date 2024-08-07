#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib/raylib.h>
#include <raylib/raymath.h>

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
    bool pressed;
    int frames_held;
    void (* callback)();
} Button;

typedef struct TextureXY{
    Texture texture;
    Texture texture_hit;
    Vector2 pos;
    float rotation;
}TextureXY;

typedef struct Frame {
    uint_16 framenum;
    char * attack;
    void * args[5];
    TextureXY textures[1000];
    int textures_len;
}Frame;


/*----STRUCTS----*/
#pragma endregion STRUCTS



#pragma region GLOBALS
/*----GLOBALS----*/


static Button buttons[99];
static TextureXY textures[1000];
static int button_index = 0;
static int current_frame = 0;
static bool holding = false;
static int updfunc_len = 0;
static int holding_texture = -1;
static int textures_len = 0;
static int ticks = 0;
Texture tex_in_hand;
Texture missing_tex;
void (*updfunc[99])();
static const TextureXY EmptyXY;
static const Texture EmptyTex;
static Frame * frames;


/*----GLOBALS----*/
#pragma endregion GLOBALS


/* Creates a button */
void CreateButton(Rectangle box,Color color,char * img_path,void (* callback)(),char * tag){
    buttons[button_index].box = box;
    buttons[button_index].color = color;
    buttons[button_index].callback = callback;
    buttons[button_index].pressed = false;
    buttons[button_index].frames_held = 0;
    buttons[button_index].tag = tag;
    buttons[button_index].texture = (img_path != NULL) ? LoadTexture(img_path) : missing_tex;

    button_index++;
}

/* Called in every function chaning the frame number */
void FrameChangedCallback(int nextframe){
    memcpy(frames[current_frame].textures,textures,sizeof(frames[current_frame].textures)); 
    frames[current_frame].textures_len = textures_len;
    memcpy(textures,frames[(int)Clamp(current_frame+nextframe,0,65534)].textures,sizeof(frames[(int)Clamp(current_frame+nextframe,0,65534)].textures));
    textures_len = frames[(int)Clamp(current_frame+nextframe,0,65534)].textures_len;
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


/* Adds function [ func ] to the [ updfunc ] function array */
void AddFunctionToItterator(void* func){
    if (holding == false){
        updfunc[updfunc_len++] = func;
        holding = true;
    }
}
/* Button function for gasterblasters */
void HoldGasterBlaster();

void GasterBlasterHold(){
    if(!IsMouseButtonDown(1)){
        float mwheel = GetMouseWheelMove();
        if (mwheel != 0){
            if(!IsKeyDown(KEY_LEFT_SHIFT)){mwheel*=5;}
            if(IsKeyDown(KEY_LEFT_ALT)){mwheel=90;}
            textures[holding_texture].rotation-=mwheel;
            if(textures[holding_texture].rotation>=360.0f){
                textures[holding_texture].rotation-=360;
            }
            if(textures[holding_texture].rotation<=-360.0f){
                textures[holding_texture].rotation+=360;
            }
        }
    }else{
        updfunc[--updfunc_len] = NULL;
        holding = false;
        textures[holding_texture] = EmptyXY;
        holding_texture = -1;
    }
    if(IsMouseButtonPressed(0)){
        float saverot = textures[holding_texture].rotation;
        holding_texture = -1;
        updfunc[--updfunc_len] = NULL;
        holding = false;
        if (IsKeyDown(KEY_LEFT_CONTROL)){
            
        HoldGasterBlaster();
        }
        textures[holding_texture].rotation = saverot;
    }
}
void HoldGasterBlaster(){
    if (!holding){
        textures[textures_len] = (TextureXY){LoadTexture("assets/gaster_blaster.png"),LoadTexture("assets/gaster_blaster_hit.png"),(Vector2){0,0},0};
        holding_texture = textures_len;
        textures_len++;
    }
    AddFunctionToItterator(&GasterBlasterHold);
}
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
bool TextureXYEquals(TextureXY t1,TextureXY t2){
    if (!TextureEquals(t1.texture,t2.texture))
        {return false;}

    if (!TextureEquals(t1.texture_hit,t2.texture_hit))
        {return false;}

    if (t1.pos.x != t2.pos.x)
        {return false;}
    if (t1.pos.y != t2.pos.y)
        {return false;}
    if (t1.rotation != t2.rotation)
        {return false;}
    return true;
}

int main(void)
{
    frames = malloc(60000*sizeof(Frame));
    memset(frames,0,sizeof(frames));
    int i,j,k,l,m; //itterators
    const int screenWidth = 800;
    const int screenHeight = 480;
    Image icon = LoadImage("assets/icon_alt.png");
    Image icon_s = LoadImage("assets/icon_small.png");
    Image *icons = malloc(sizeof(Image)*2);
    icons[0] = icon;
    icons[1] = icon_s;
    //missing_tex = LoadTexture("assets/empty.png");
    InitWindow(screenWidth, screenHeight, "Catak - Create Your Frisk attack helper [ WIP 0.2 ]");
    SetWindowIcons(icons,2);
    SetTargetFPS(60);           
    CreateButton((Rectangle){5,30,25,25},(Color){220,220,220,255},"assets/plus_tex.png",&ChangeFrame,"inc1");
    CreateButton((Rectangle){5,60,65,25},(Color){220,220,220,255},"assets/plus_20_tex.png",&ChangeFrame,"inc20");
    CreateButton((Rectangle){130,30,25,25},(Color){220,220,220,255},"assets/sub_tex.png",&ChangeFrame,"sub1");
    CreateButton((Rectangle){90,60,65,25},(Color){220,220,220,255},"assets/sub_20_tex.png",&ChangeFrame,"sub20");
    CreateButton((Rectangle){90,150,65,25},(Color){220,220,220,255},"assets/empty.png",&HoldGasterBlaster,"holdgb");


    while (!WindowShouldClose())
    {
        Vector2 mouse_pos = GetMousePosition();
        for (i=0;i<updfunc_len;i++){
            (updfunc[i])();
        }
        BeginDrawing();
            ClearBackground((Color){220,220,220,220});
            DrawRectangle(0,0,160,screenHeight,RAYWHITE);


            /*----TEXTURE-DRAWING----*/
            for(i=0;i<textures_len;i++){
                TextureXY tx = textures[i];
                Rectangle t_rect = (Rectangle){tx.pos.x,tx.pos.y,tx.texture.width,tx.texture.height};
                Rectangle t_rect_coll = (Rectangle){tx.pos.x-tx.texture.width/2,tx.pos.y-tx.texture.height/2,tx.texture.width,tx.texture.height};
                if(IsMouseButtonDown(1)){
                    if (CheckCollisionRecs((Rectangle){mouse_pos.x,mouse_pos.y,1,1},t_rect_coll)){
                        textures[i] = EmptyXY;
                        continue;
                    }
                }
                if (!TextureXYEquals(tx,EmptyXY)){
                    if (i == holding_texture){
                        tx.pos = mouse_pos;
                        textures[i] = tx;
                    }
                    DrawTexturePro(tx.texture,(Rectangle){0,0,tx.texture.width,tx.texture.height},t_rect,(Vector2){tx.texture.width/2,tx.texture.height/2},tx.rotation,WHITE);
                    if (!TextureEquals(tx.texture_hit,EmptyTex)){
                        DrawTexturePro(tx.texture_hit,(Rectangle){0,0,tx.texture.width,tx.texture.height},t_rect,(Vector2){tx.texture.width/2,tx.texture.height/2},0,WHITE);
                    }
                } //TODO: implement scaling
            }
            /*----TEXTURE-DRAWING----*/


            /*----BUTTON-LOGIC----*/
            for(i=0;i<button_index;i++){
                DrawRectangleRec(buttons[i].box,buttons[i].color);
                DrawTexture(buttons[i].texture,buttons[i].box.x,buttons[i].box.y,WHITE);
                if (IsMouseButtonDown(0)){
                    
                    if((buttons[i].box.x < mouse_pos.x && (buttons[i].box.x + buttons[i].box.width) > mouse_pos.x) && (buttons[i].box.y < mouse_pos.y && (buttons[i].box.y+ buttons[i].box.height) > mouse_pos.y)){
                        (buttons[i].callback)(i,buttons[i].tag);
                        buttons[i].pressed = true;
                    }
                }else{
                    buttons[i].pressed = false;
                    buttons[i].frames_held = 0;

                }
            }
            /*----BUTTON-LOGIC----*/

            current_frame = Clamp(current_frame,0,65534);

            DrawText(TextFormat("Current frame",current_frame), 5,5, 20, GRAY);
            DrawText(TextFormat("%i",current_frame), 35, 32, 20, GRAY);

            if((ticks % 10) == 0){
                TextureXY temp_textures[1000];
                int temp_textures_len = 0;
                for(i=0;i<textures_len;i++){
                    if (!TextureXYEquals(textures[i],EmptyXY)){
                        temp_textures[temp_textures_len++] = textures[i];
                    }
                }
                memcpy(textures,temp_textures,sizeof(textures));
                textures_len = temp_textures_len;
            }
        EndDrawing();
        ticks++;
    }
    free(frames);
    CloseWindow();
    return 0;
}