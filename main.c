#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib/raylib.h>
#include <raylib/raymath.h>

typedef union uint_16{
    unsigned char half;
    unsigned char half2;
} uint_16;

typedef struct Button {
    Rectangle box;
    Color color;
    Texture texture;
    bool pressed;
    int frames_held;
    void (* callback)();
} Button;

typedef struct TextureXY{
    Texture texture;
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

static Button buttons[99];
static TextureXY textures[1000];
static int button_index = 0;
static int current_frame = 0;
static bool holding = false;
static int updfunc_len = 0;
static int holding_texture = -1;
static int textures_len = 0;
Texture tex_in_hand;
Texture missing_tex;
void (*updfunc[99])();
static const TextureXY EmptyXY;
static Frame * frames;


void CreateButton(Rectangle box,Color color,char * img_path,void (* callback)()){
    buttons[button_index].box = box;
    buttons[button_index].color = color;
    buttons[button_index].callback = callback;
    buttons[button_index].pressed = false;
    buttons[button_index].frames_held = 0;
    buttons[button_index].texture = (img_path != NULL) ? LoadTexture(img_path) : missing_tex;

    button_index++;
}
void Increment(int i){
    if (buttons[i].pressed == false || (buttons[i].frames_held >= 20 && buttons[i].frames_held % 2)){
        current_frame+= (int)Clamp(buttons[i].frames_held % 20,1,2);
    }
    buttons[i].frames_held++;
}
void Increment20(int i){
    if (buttons[i].pressed == false || (buttons[i].frames_held >= 20 && buttons[i].frames_held % 2)){
        current_frame+= 20;
    }
    buttons[i].frames_held++;
}
void Subtract(int i){
    if (buttons[i].pressed == false || (buttons[i].frames_held >= 20 && buttons[i].frames_held % 2)){
        current_frame-= (int)Clamp(buttons[i].frames_held % 20,1,2);
    }
    buttons[i].frames_held++;
}
void Subtract20(int i){
    if (buttons[i].pressed == false || (buttons[i].frames_held >= 20 && buttons[i].frames_held % 2)){
        current_frame-= 20;
    }
    buttons[i].frames_held++;
}

void AddFunctionToItterator(void* func){
    if (!holding){
        updfunc[updfunc_len++] = func;
        holding = true;
    }
}

void GasterBlasterHold(){
    if(!IsMouseButtonDown(1)){
        if (GetMouseWheelMove() > 0){
            textures[holding_texture].rotation ++;
        }
    }else{
        updfunc[--updfunc_len] = NULL;
        holding = false;
        textures[holding_texture] = EmptyXY;
    }
}
void HoldGasterBlaster(){
    AddFunctionToItterator(&GasterBlasterHold);
    textures[textures_len] = (TextureXY){LoadTexture("assets/gaster_blaster.png"),(Vector2){0,0},0};
    holding_texture = textures_len;
    textures_len++;
}
bool TextureXYEquals(TextureXY t1,TextureXY t2){
    if (t1.texture.id != t2.texture.id)
        {return false;}
    if (t1.texture.width != t2.texture.width)
        {return false;}
    if (t1.texture.height != t2.texture.height)
        {return false;}
    if (t1.texture.mipmaps != t2.texture.mipmaps)
        {return false;}
    if (t1.texture.format != t2.texture.format)
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
    //missing_tex = LoadTexture("assets/empty.png");
    InitWindow(screenWidth, screenHeight, "Catak - Create Your Frisk attack helper");
    SetTargetFPS(60);           
    CreateButton((Rectangle){5,30,25,25},(Color){220,220,220,255},"assets/plus_tex.png",&Increment);
    CreateButton((Rectangle){5,60,65,25},(Color){220,220,220,255},"assets/plus_20_tex.png",&Increment20);
    CreateButton((Rectangle){130,30,25,25},(Color){220,220,220,255},"assets/sub_tex.png",&Subtract);
    CreateButton((Rectangle){90,60,65,25},(Color){220,220,220,255},"assets/sub_20_tex.png",&Subtract20);
    CreateButton((Rectangle){90,150,65,25},(Color){220,220,220,255},"assets/empty.png",&HoldGasterBlaster);
    while (!WindowShouldClose())
    {
        Vector2 mouse_pos = GetMousePosition();
        for (i=0;i<updfunc_len;i++){
            (updfunc[i])();
        }
        BeginDrawing();
            ClearBackground((Color){220,220,220,220});
            DrawRectangle(0,0,160,screenHeight,RAYWHITE);
            for(i=0;i<textures_len;i++){
                TextureXY tx = textures[i];
                if (!TextureXYEquals(tx,EmptyXY))
                    {DrawTextureEx(tx.texture,tx.pos,tx.rotation,1,WHITE);} //TODO: implement scaling
            }
            for(i=0;i<button_index;i++){
                //printf("%f %f ,rec : %f %f\n",mouse_pos.x,mouse_pos.y,buttons[i].box.x,buttons[i].box.width);
                DrawRectangleRec(buttons[i].box,buttons[i].color);
                DrawTexture(buttons[i].texture,buttons[i].box.x,buttons[i].box.y,WHITE);
                if (IsMouseButtonDown(0)){
                    if((buttons[i].box.x < mouse_pos.x && (buttons[i].box.x + buttons[i].box.width) > mouse_pos.x) && (buttons[i].box.y < mouse_pos.y && (buttons[i].box.y+ buttons[i].box.height) > mouse_pos.y)){
                        (buttons[i].callback)(i);
                        buttons[i].pressed = true;
                    }
                }else{
                    buttons[i].pressed = false;
                    buttons[i].frames_held = 0;

                }
            }
            current_frame = Clamp(current_frame,0,65534);

            DrawText(TextFormat("Current frame",current_frame), 5,5, 20, GRAY);
            DrawText(TextFormat("%i",current_frame), 35, 32, 20, GRAY);

        EndDrawing();
    }
    free(frames);
    CloseWindow();
    return 0;
}