#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include <time.h>
#include <math.h>


using namespace std;

const int screen_width = 800;
const int screen_height = 448;

// use essa função pra converter int pra string
template <typename T>

// use essa função pra converter um valor pra string
string ToString(T val)
{
    stringstream stream;
    stream << val;
    return stream.str();
}

/*
essa classe representa um vetor 2d de posição
essa classe não precisa de dependência
*/

class Vector2
{
	private:
	int x, y;

	public:

	Vector2(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	~Vector2(void)
	{

	}

	int getX()
	{
		return x;
	}
	int getY()
	{
		return y;
	}

	void setX(int x)
	{
		this->x = x;
	}

	void setY(int y)
	{
		this->y = y;
	}
};

SDL_Surface *BackgroundImage = NULL;

// carrega arquivos
void LoadFiles()
{
    BackgroundImage = SDL_LoadBMP("fundo.bmp");
}

// fecha arquivos
void CloseFiles()
{
    SDL_FreeSurface(BackgroundImage);
}


/*
No SDL2, a tela tem que ser criada com
a função SDL_GetWindowSurface
depois de criar a janela
*/

SDL_Surface* tela = NULL;

// use essa função pra desenhar uma imagem na tela
void DrawImage(int x, int y, SDL_Surface *image)
{
    SDL_Rect mover;
    mover.x = x;
    mover.y = y;

    SDL_BlitSurface(image, NULL, tela, &mover);
}


int main(int argc, char *argv[])
{
SDL_Init(SDL_INIT_EVERYTHING);

// objeto ponteiro de janela
SDL_Window *janela = NULL;

janela = SDL_CreateWindow("janela", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_SHOWN);


tela = SDL_GetWindowSurface(janela);

SDL_Event evento;
bool executando = true;
LoadFiles();


while(executando)
{
    while(SDL_PollEvent(&evento))
    {
        if(evento.type == SDL_QUIT) // se clicar no x da janela
        {
            executando = false; // fecha o programa
        }
    }


    DrawImage(0,0,BackgroundImage); // essa função vai ser removida depois
    SDL_UpdateWindowSurface(janela); // atualiza a janela
}


CloseFiles();
SDL_DestroyWindow(janela); // destroi a janela
SDL_Quit();
return 0;
}
