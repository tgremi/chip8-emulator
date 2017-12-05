#include "graficos_Allegro.h"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
graficos_Allegro::graficos_Allegro()
{
	FPS = 120.0;
	janela = NULL;
	fila_eventos = NULL;
	timer = NULL;
	redesenhar = true;
	largura = 64;
	altura = 32;
	modificador = 1;
	pixel = NULL;
}

graficos_Allegro::graficos_Allegro(int mod)
{
	modificador = mod;
	FPS = 120.0;
	janela = NULL;
	fila_eventos = NULL;
	timer = NULL;
	redesenhar = true;
	largura = 64 * modificador;
	altura = 32 * modificador;
	pixel = NULL;
}

bool graficos_Allegro::iniciar()
{
	if (!al_init()) //Verifica a inicializacaoo do Allegro
	{
		fprintf(stderr, "ERRO - Falha ao inicializar a biblioteca Allegro!\n");
		return false;
	}

	if (!al_install_audio()) //Verifica a inicializacaoo do audio
	{
		fprintf(stderr, "ERRO - Falha ao inicializar audio!\n");
		return false;
	}

	if (!al_init_acodec_addon()) //Testa os codecs de audio
	{
		fprintf(stderr, "ERRO - Falha ao inicializar codecs de audio!\n");
		return false;
	}

	if (!al_reserve_samples(1)) //Testa os canais
	{
		fprintf(stderr, "ERRO - Falha ao alocar canais de audio!\n");
		return false;
	}

	if (!al_install_keyboard()) //Verifica a inicializacaoo do teclado
	{
		fprintf(stderr, "ERRO - Falha ao inicializar o teclado!\n");
		return false;
	}

	timer = al_create_timer(1.0 / FPS);	//Inicializa o Timer																																																															)																																																																																																																																																																																																																																																																																																									; //Inicializa o Timer
	if (!timer)
	{
		fprintf(stderr, "ERRO - Falha ao criar o timer!\n");
		return false;
	}

	al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_DIRECT3D_INTERNAL);

	janela = al_create_display(largura, altura); //Cria janela da aplicacao
	if (!janela)
	{
		fprintf(stderr, "ERRO - Falha ao criar a janela da aplicacao!\n");
		al_destroy_timer(timer);
		return false;
	}
	al_set_window_title(janela, "Emulador Chip-8"); //Coloca o titulo da janela

	fila_eventos = al_create_event_queue(); //Cria uma fila de eventos
	if (!fila_eventos)
	{
		fprintf(stderr, "ERRO - Falha ao criar a fila de eventos!\n");
		al_destroy_display(janela);
		al_destroy_timer(timer);
		return false;
	}

	al_init_primitives_addon();

	al_register_event_source(fila_eventos, al_get_display_event_source(janela)); //Prepara a fila de evenos para pegar qualquer evento relacionado � janela
	al_register_event_source(fila_eventos, al_get_timer_event_source(timer));	//Prepara a fila de evenos para pegar qualquer evento relacionado aos Timers
	al_register_event_source(fila_eventos, al_get_keyboard_event_source());		 //Prepara a fila de evenos para pegar qualquer evento relacionado ao teclado

	pixel = al_create_bitmap(modificador, modificador);
	al_set_target_bitmap(pixel);
	al_draw_filled_rectangle(0, 0, modificador, modificador, al_map_rgb(255, 255, 255));

	al_set_target_bitmap(al_get_backbuffer(janela));
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_flip_display();

	al_start_timer(timer);

	return true;
}

bool graficos_Allegro::input(chip8 &chip8)
{
	ALLEGRO_EVENT evento;

	al_wait_for_event(fila_eventos, &evento);

	if (evento.type == ALLEGRO_EVENT_TIMER)
		chip8.flag_Tela = true;

	else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		return encerrar();

	else if (evento.type == ALLEGRO_EVENT_KEY_DOWN)
	{

		if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) // esc
			return encerrar();

		if (evento.keyboard.keycode == ALLEGRO_KEY_1)
			chip8.tecla[0x1] = 1;
		else if (evento.keyboard.keycode == ALLEGRO_KEY_2)
			chip8.tecla[0x2] = 1;
		else if (evento.keyboard.keycode == ALLEGRO_KEY_3)
			chip8.tecla[0x3] = 1;
		else if (evento.keyboard.keycode == ALLEGRO_KEY_4)
			chip8.tecla[0xC] = 1;

		else if (evento.keyboard.keycode == ALLEGRO_KEY_Q)
			chip8.tecla[0x4] = 1;
		else if (evento.keyboard.keycode == ALLEGRO_KEY_W)
			chip8.tecla[0x5] = 1;
		else if (evento.keyboard.keycode == ALLEGRO_KEY_E)
			chip8.tecla[0x6] = 1;
		else if (evento.keyboard.keycode == ALLEGRO_KEY_R)
			chip8.tecla[0xD] = 1;

		else if (evento.keyboard.keycode == ALLEGRO_KEY_A)
			chip8.tecla[0x7] = 1;
		else if (evento.keyboard.keycode == ALLEGRO_KEY_S)
			chip8.tecla[0x8] = 1;
		else if (evento.keyboard.keycode == ALLEGRO_KEY_D)
			chip8.tecla[0x9] = 1;
		else if (evento.keyboard.keycode == ALLEGRO_KEY_F)
			chip8.tecla[0xE] = 1;

		else if (evento.keyboard.keycode == ALLEGRO_KEY_Y)
			chip8.tecla[0xA] = 1;
		else if (evento.keyboard.keycode == ALLEGRO_KEY_X)
			chip8.tecla[0x0] = 1;
		else if (evento.keyboard.keycode == ALLEGRO_KEY_C)
			chip8.tecla[0xB] = 1;
		else if (evento.keyboard.keycode == ALLEGRO_KEY_V)
			chip8.tecla[0xF] = 1;
		// keycode stuff set to true
	}
	else if (evento.type == ALLEGRO_EVENT_KEY_UP)
	{
		if (evento.keyboard.keycode == ALLEGRO_KEY_1)
			chip8.tecla[0x1] = 0;
		else if (evento.keyboard.keycode == ALLEGRO_KEY_2)
			chip8.tecla[0x2] = 0;
		else if (evento.keyboard.keycode == ALLEGRO_KEY_3)
			chip8.tecla[0x3] = 0;
		else if (evento.keyboard.keycode == ALLEGRO_KEY_4)
			chip8.tecla[0xC] = 0;

		else if (evento.keyboard.keycode == ALLEGRO_KEY_Q)
			chip8.tecla[0x4] = 0;
		else if (evento.keyboard.keycode == ALLEGRO_KEY_W)
			chip8.tecla[0x5] = 0;
		else if (evento.keyboard.keycode == ALLEGRO_KEY_E)
			chip8.tecla[0x6] = 0;
		else if (evento.keyboard.keycode == ALLEGRO_KEY_R)
			chip8.tecla[0xD] = 0;

		else if (evento.keyboard.keycode == ALLEGRO_KEY_A)
			chip8.tecla[0x7] = 0;
		else if (evento.keyboard.keycode == ALLEGRO_KEY_S)
			chip8.tecla[0x8] = 0;
		else if (evento.keyboard.keycode == ALLEGRO_KEY_D)
			chip8.tecla[0x9] = 0;
		else if (evento.keyboard.keycode == ALLEGRO_KEY_F)
			chip8.tecla[0xE] = 0;

		else if (evento.keyboard.keycode == ALLEGRO_KEY_Y)
			chip8.tecla[0xA] = 0;
		else if (evento.keyboard.keycode == ALLEGRO_KEY_X)
			chip8.tecla[0x0] = 0;
		else if (evento.keyboard.keycode == ALLEGRO_KEY_C)
			chip8.tecla[0xB] = 0;
		else if (evento.keyboard.keycode == ALLEGRO_KEY_V)
			chip8.tecla[0xF] = 0;
		// keycode stuff set to false
	}

	if ((chip8.flag_Tela == true) && (al_is_event_queue_empty(fila_eventos))){
		chip8.flag_Tela = false;

		al_clear_to_color(al_map_rgb(0, 0, 0));

		for (int y = 0; y < 32; y++){
			for (int x = 0; x < 64; x++){
				if (chip8.gfx[(y * 64) + x] == 1)
					al_draw_bitmap(pixel, x * modificador, y * modificador, ALLEGRO_VIDEO_BITMAP);
			}
		}

		al_flip_display();
	}

	return false;
}

bool graficos_Allegro::encerrar()
{
	al_destroy_bitmap(pixel);
	al_destroy_timer(timer);
	al_destroy_display(janela);
	al_destroy_event_queue(fila_eventos);

	return true;
}