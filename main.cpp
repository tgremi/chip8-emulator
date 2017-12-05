#include "graficos_Allegro.h"
#include <allegro5/allegro.h>
#include <sstream>
#include <iostream>

using namespace std;

//g++ -Wall Main.cpp graficos_Allegro.cpp chip8.cpp -o chip8 `pkg-config --cflags --libs allegro-5.0 allegro_acodec-5.0 allegro_audio-5.0 allegro_color-5.0 allegro_dialog-5.0 allegro_font-5.0 allegro_image-5.0 allegro_main-5.0 allegro_memfile-5.0 allegro_physfs-5.0 allegro_primitives-5.0 allegro_ttf-5.0`


int main(int argc, char **argv)
{
	stringstream ss;
	chip8 chip_8;
	graficos_Allegro graficos;

	if (argc <= 2)
		graficos = graficos_Allegro(5);
		
	else if (argc == 3)
	{
		int width = 0;
		ss << argv[2];
		ss >> width;
		width /= 64;

		graficos = graficos_Allegro(width);
	}
	graficos.iniciar();
	chip_8.resetar();

	bool closeGame = false;

	if (argv[1] != NULL)
		chip_8.carregarJogo(argv[1]);

	while (!closeGame)
	{

		chip_8.emula_Ciclo();
		closeGame = graficos.input(chip_8);
	}

	return 0;
}


