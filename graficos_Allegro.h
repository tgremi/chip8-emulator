#include <allegro5/allegro5.h>
#include <allegro5/keyboard.h>
#include <allegro5/allegro_primitives.h>
#include "chip8.h"
#include <stdio.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

class graficos_Allegro
{
private:
	int largura, altura, FPS, modificador;
	bool redesenhar;
	ALLEGRO_DISPLAY *janela;
	ALLEGRO_EVENT_QUEUE *fila_eventos;
	ALLEGRO_TIMER *timer;
	ALLEGRO_BITMAP *pixel;
	ALLEGRO_SAMPLE *sample = NULL;

public:
	graficos_Allegro();
	graficos_Allegro(int altura);
	bool iniciar();
	bool input(chip8 &chip_8);
	bool encerrar();
};