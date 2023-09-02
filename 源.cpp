#define  _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include<SDL_ttf.h>

#define WIDTH 1000
#define HEIGHT 600
#define FRAMERATE 60


SDL_Renderer* renderer;
SDL_Window* window;
SDL_FRect r1;
int clickCount = 0;

class Heita
{
public:
    Heita(SDL_Renderer* renderer);
    ~Heita();
    void DrawHeita(SDL_Renderer* renderer, int i);
    void begingzhuan();
    int Index;
private:
    uint32_t countfarm;
    SDL_Rect frame[5];
    SDL_FRect dest;
    bool _playing;
    float speed;
    SDL_Texture* texture[5];
};
Heita::Heita(SDL_Renderer* renderer)
{
    texture[0] = IMG_LoadTexture(renderer, "zhuanquanquan1.png");
    texture[1] = IMG_LoadTexture(renderer, "zhuanquanquan2.png");
    texture[2] = IMG_LoadTexture(renderer, "zhuanquanquan3.png");
    texture[3] = IMG_LoadTexture(renderer, "zhuanquanquan4.png");
    texture[4] = IMG_LoadTexture(renderer, "zhuanquanquan5.png");
    texture[5] = IMG_LoadTexture(renderer, "zhuanquanquan6.png");
    if (texture == NULL)
    {
        SDL_Log("Can not load, %s", IMG_GetError());
        return;
    }
    for (int i = 0; i < 6; i++)
    {
        frame[i].x = 0;
        frame[i].y = 0;
        frame[i].w = 500;
        frame[i].h = 500;
    }
    dest = { -500,0,500,500 };
    Index = 0;
    countfarm = 0;
    _playing = false;
    speed = 30;
}
Heita::~Heita()
{
    for (int i = 0; i < 5; i++)
    {
        SDL_DestroyTexture(texture[i]);
    }
}
void Heita::DrawHeita(SDL_Renderer* renderer,int i)
{
    SDL_RenderCopyF(renderer, texture[i], &(frame[i]), &dest);
    countfarm++;
    if (_playing)
    {
        if (countfarm % 5 == 0)
        {
            dest.x = dest.x + speed;
            if (dest.x == 1000)
            {
                dest.x = -500;
                _playing = false;
            }
            Index++;
        }
    }
}
void Heita::begingzhuan()
{
    _playing = true;
}

class Text
{
public:
    Text(uint32_t setcolor, const char* fontname, int setptsize);
    ~Text();
    void SetText(SDL_Renderer* renderer, char* name);
private:
    char* value;
    int ptsize;
    uint32_t color;
    TTF_Font* font;
    SDL_Surface* textSurface;
    SDL_FRect dest;
};
Text::Text(uint32_t setcolor, const char* fontname, int setptsize)
{
    ptsize = setptsize;
    font = TTF_OpenFont(fontname, ptsize);
    if (font == NULL)
    {
        SDL_Log("Can not open ttf, %s", TTF_GetError());
        return;
    }
    color = setcolor;
    textSurface = NULL;
    dest = { 400,550,0,0 };
}
Text::~Text()
{
    SDL_FreeSurface(textSurface);
}
void Text::SetText(SDL_Renderer* renderer, char* name)
{
    value = name;
    SDL_Color textcolor = { (color & 0x00ff0000) >> 16,
        (color & 0x0000ff00) >> 8,
        (color & 0x000000ff),
        (color & 0xff000000) >> 24 };
    if (textSurface != NULL)
    {
        SDL_FreeSurface(textSurface);
    }
    textSurface = TTF_RenderUTF8_Blended(font, value, textcolor);
    dest.w = textSurface->w;
    dest.h = textSurface->h;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_RenderCopyF(renderer, texture, NULL, &dest);
    SDL_RenderDrawRectF(renderer, &dest);
}

void draw(Heita* heita, Text* text)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
    SDL_RenderFillRectF(renderer, &r1);

    heita->DrawHeita(renderer, heita->Index);

    char buf[30];
    sprintf(buf, "Click to kuru~: %d", clickCount);
    text->SetText(renderer, buf);

    SDL_RenderPresent(renderer);
}



void event_loop(Heita* heita, Text* text)
{
    while (1)
    {
        uint32_t begin = SDL_GetTicks();

        if (heita->Index == 5)
        {
            heita->Index = 0;
        }
        draw(heita, text);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_MOUSEMOTION:
            case SDL_MOUSEBUTTONDOWN:
                break;
            case SDL_MOUSEBUTTONUP:
                clickCount++;
                SDL_FPoint mouse;
                mouse.x = event.button.x;
                mouse.y = event.button.y;
                if (SDL_PointInFRect(&mouse, &r1))
                {
                    heita->begingzhuan();
                }
                break;
            case SDL_QUIT:
                return;
            }
        }
        long current = SDL_GetTicks();
        long cost = current - begin;
        long frame = 1000 / FRAMERATE;
        long delay = frame - cost;

        if (delay > 0)
        {
            SDL_Delay(delay);
        }
    }
}

int main(int argc, char** argv)
{
    if (TTF_Init())
    {
        SDL_Log("Can not init ttf, %s", TTF_GetError());
        return 1;
    }
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Can not init video, %s", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow(
        "Hello World",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (window == NULL)
    {
        SDL_Log("Can not create window, %s", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        SDL_Log("Can not create renderer, %s", SDL_GetError());
        return 1;
    }

    r1 = { 400,550,300,40 };
    Heita* heita = new Heita(renderer);
    Text* text = new Text(0xff000000, "simsun.ttf", 32);

    event_loop(heita, text);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}