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


int main(int argc, char *argv[])
{
SDL_Init(SDL_INIT_EVERYTHING);



SDL_Quit();
return 0;
}
