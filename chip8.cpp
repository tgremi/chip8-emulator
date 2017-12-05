#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
//#include "graficos_Allegro.h"

/*gcc -Wall main.c `pkg-config --cflags --libs allegro-5.0 allegro_acodec-5.0 allegro_audio-5.0 allegro_color-5.0 allegro_dialog-5.0 allegro_font-5.0 allegro_image-5.0 allegro_main-5.0 allegro_memfile-5.0 allegro_physfs-5.0 allegro_primitives-5.0 allegro_ttf-5.0`*/

bool chip8::inicializar()
{
	if (!al_init()) //Testa a inicializacao do Allegro
	{
		fprintf(stderr, "ERRO - Falha ao inicializar Allegro.\n");
		return false;
	}

	if (!al_install_audio()) //Testa a inicializacao do audio
	{
		fprintf(stderr, "ERRO - Falha ao inicializar audio.\n");
		return false;
	}

	if (!al_init_acodec_addon()) //Testa os codecs de audio
	{
		fprintf(stderr, "ERRO - Falha ao inicializar codecs de audio.\n");
		return false;
	}

	if (!al_reserve_samples(1)) //Testa os canais
	{
		fprintf(stderr, "ERRO - Falha ao alocar canais de audio.\n");
		return false;
	}

	sample = al_load_sample("Som.wav"); //Seta o som do Timer;
	if (!sample)						//Testa o som
	{
		fprintf(stderr, "ERRO - Falha ao carregar sample!\n");
		al_destroy_display(janela);
		return false;
	}
}
chip8::chip8()
{
}
unsigned char chip8_fontset[80] =
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, //0
		0x20, 0x60, 0x20, 0x20, 0x70, //1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
		0x90, 0x90, 0xF0, 0x10, 0x10, //4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
		0xF0, 0x10, 0x20, 0x40, 0x40, //7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
		0xF0, 0x90, 0xF0, 0x90, 0x90, //A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
		0xF0, 0x80, 0x80, 0x80, 0xF0, //C
		0xE0, 0x90, 0x90, 0x90, 0xE0, //D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
		0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

void chip8::resetar()
{
	codigo_op = 0;
	i = 0;
	flag_Tela = 0;
	PC = 0x200; //O CHIP-8 ocupava os primeiros 512 bytes de memoria, por isso comecamos na posicao 0x200
	posicao_atual = 0;

	//Limpa o display
	for (int i = 0; i < 2048; ++i)
		gfx[i] = 0;

	//Limpa os registradores, posicao e teclas
	for (int i = 0; i < 16; i++)
	{
		registrador[i] = 0;
		posicao[i] = 0;
		tecla[i] = 0;
	}

	//Limpa toda a memoria
	for (int i = 0; i < 4096; i++)
		memoria[i] = 0;

	timer_delay = 0;
	timer_som = 0;
	flag_Tela = 1; //Limpa a tela

	// Carrega o fontset
	for (int i = 0; i < 80; ++i)
		memoria[i] = chip8_fontset[i];
	inicializar();
}

void chip8::emula_Ciclo()
{
	//graficos_Allegro graficos;
	//Copia o codigo op da memoria
	codigo_op = memoria[PC] << 8 | memoria[PC + 1]; //adiciona 8 zeros e usa a operacao logica OR para juntar a instrucao que esta dividida em memoria[PC] e memoria [PC+1]
	printf("Executando OP 0x%04X da memoria em [%04X], I= %02X e PC =:%02X\n", codigo_op, memoria[PC], i, PC);
	//Switch que verifica as operacoes
	switch (codigo_op & 0xF000)
	{   //Utiliza a operacao logica AND para fazer uma mascara de bits que ira verificar apenas os 4 bits da esquerda para direita (os bits que contem a operacao)
		//Casos os 4 primeiros bits da esquerda para a direita derem 0 sabemos que existem duas instrucoes possiveis
	case 0x0000:
		switch (codigo_op & 0x000F) //mascara de bits para verificar os 4 ultimos e determinar a instrucao
		{
		case 0x0000: //00E0: Limpa a tela
			for (int i = 0; i < 2048; ++i)
				gfx[i] = 0x0;
			flag_Tela = 1;
			PC = PC + 2;
			break;

		case 0x000E: //0x00EE: Retorna da subrotina
			posicao_atual = posicao_atual - 1; // 16 Niveis da pilha, o ponteiro da pilha e decrementado pra previnir erro
			PC = posicao[posicao_atual];	   // Coloca o endereco de retorno da pilha de volta no PC
			PC = PC + 2;
			break;

		default:
			printf("Codigo OP desconhecido[0x0000]: 0x%X\n", codigo_op);
			printf("Entrou no Switch das OPs 00E0 e 0x00EE\n");
		}
		break;

	case 0x1000: // 0x1NNN: Pula para o endereco NNN
		PC = codigo_op & 0x0FFF; //mascara de bits para pegar o endereco
		break;

	case 0x2000: // 0x2NNN: Chama a subrotina no endereco NNN
		posicao[posicao_atual] = PC; //Armazena a posicao atual do codigo para podermos retornar depois
		PC = codigo_op & 0x0FFF;	 //mascara de bits para pegar o endereco
		posicao_atual++;
		break;

	case 0x3000: // 0x3XNN: Pula a instrucao se o Registrador [X] for igual a NN
		if (registrador[(codigo_op & 0x0F00) >> 8] == (codigo_op & 0x00FF)) //Pega o numero do registrador e valor a ser comparado com o conteudo do registrador
			PC = PC + 4; // Caso o valores sejam iguais, incrementa o PC contando a instrucao 0x3XNN e a instrucao pulada
		else
			PC = PC + 2; // Do contrario conta apenas a instrucao 0x3XNN

		break;

	case 0x4000: // 0x4XNN: Pula a instrucao se o Registrador [X] for diferente a NN
		if (registrador[(codigo_op & 0x0F00) >> 8] != (codigo_op & 0x00FF))	//Pega o numero do registrador e valor a ser comparado com o conteudo do registrador
			PC = PC + 4; // Caso o valores sejam diferentes, incrementa o PC contando a instrucao 0x3XNN e a instrucao pulada
		else
			PC = PC + 2; // Do contrario conta apenas a instrucao 0x4XNN
		break;

	case 0x5000: // 0x5XY0: Pula a instrucao se o Registrador [X] for diferente do Registrador [Y]
		if (registrador[(codigo_op & 0x0F00) >> 8] == registrador[(codigo_op & 0x00F0) >> 4]) //Pega os numero dos registradores X e Y e compara
			PC = PC + 4; // Caso o valores sejam iguais, incrementa o PC contando a instrucao 0x5XY0 e a instrucao pulada
		else
			PC = PC + 2; // Do contrario conta apenas a instrucao 0x5XY0

		break;

	case 0x6000:													   // 0x6XNN: Seta o Registrador [X] for igual a NN
		registrador[(codigo_op & 0x0F00) >> 8] = (codigo_op & 0x00FF); //Pega o valor em NN e seta no registrador [X]
		PC = PC + 2;												   //Conta a instrucao 0x6XNN
		break;

	case 0x7000:																								// 0x7XNN: Soma NN No Registrador [X]
		registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00) >> 8] + (codigo_op & 0x00FF); //Pega o valor em NN e soma no registrador [X]
		PC = PC + 2;																							//Conta a instrucao 0x7XNN
		break;

	case 0x8000:
		switch (codigo_op & 0x000F)
		{
		case 0x0000: // 0x8XY0 Seta o Registrador [X] igual ao valor do Registrador [Y]
			registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x00F0) >> 4];
			PC = PC + 2; //Conta a instrucao 0x8XY0
			break;
			
		case 0x0001: // 0x8XY1 Realiza a operacao logica OR entre os registradores [X] e [Y]
			//registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00)] | registrador[(codigo_op & 0x00F0) >> 4];
			registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00) >> 8] | registrador[(codigo_op & 0x00F0) >> 4];
			PC = PC + 2; //Conta a instrucao 0x8XY1
			break;

		case 0x0002: // 0x8XY2 Realiza a operacao logica AND entre os registradores [X] e [Y]
			//registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00)] & registrador[(codigo_op & 0x00F0) >> 4];
			registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00) >> 8] & registrador[(codigo_op & 0x00F0) >> 4];
			PC = PC + 2; //Conta a instrucao 0x8XY2
			break;

		case 0x0003: // 0x8XY3 Realiza a operacao logica XOR entre os registradores [X] e [Y]
			//registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00)] ^ registrador[(codigo_op & 0x00F0) >> 4];
			registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00) >> 8] ^ registrador[(codigo_op & 0x00F0) >> 4];
			PC = PC + 2; //Conta a instrucao 0x8XY3
			break;

		case 0x0004: // 0x8XY4 Soma os registradores [X] e [Y] e guarda em [X]
			registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00) >> 8] + registrador[(codigo_op & 0x00F0) >> 4];
			if (registrador[(codigo_op & 0x00F0) >> 4] > (0xFF - registrador[(codigo_op & 0x0F00) >> 8])) //Seta o registrador [F] para 1 se tiver carry
				registrador[15] = 1;

			else //Do contrario seta para 0
				registrador[15] = 0;

			PC = PC + 2; //Conta a instrucao 0x8XY4
			break;
			
		case 0x0005: // 0x8XY5 Subtrai os registradores [Y] de [X] e guarda em [X]. Registrador [15] = 1 se tiver borrow.
			if (registrador[(codigo_op & 0x00F0) >> 4] > registrador[(codigo_op & 0x0F00) >> 8]) //Verifica o conteudo dos registradores [X] e [Y]
				registrador[15] = 0; //Tem borrow
			else
				registrador[15] = 1; //Sem borrow

			registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00) >> 8] - registrador[(codigo_op & 0x00F0) >> 4];

			PC = PC + 2; //Conta a instrucao 0x8XY5
			break;
			
		case 0x0006: // 0x8XY6 Seta o Registrador [15] com o ultimo bit do Registrador [X] e joga o Registrador [X]  1 bit e direita
			registrador[15] = registrador[(codigo_op & 0x0F00) >> 8] & 0x01; //mascara de bits para pegar o bit LSB
			registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00) >> 8] >> 1;
			PC = PC + 2; //Conta a instrucao 0x8XY6
			break;
		
		case 0x0007: // 0x8XY7 Subtrai os Registradores [X] de [Y] e guarda em [X]. Registrador [15] = 1 se tiver borrow.
			if (registrador[(codigo_op & 0x0F00) >> 8] > registrador[(codigo_op & 0x00F0) >> 4]) //Verifica o conteudo dos registradores [X] e [Y]
				registrador[15] = 0; //Tem borrow
			else
				registrador[15] = 1; //Sem borrow

			registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00) >> 4] - registrador[(codigo_op & 0x00F0) >> 8];
			PC = PC + 2; //Conta a instrucao 0x8XY7
			break;
		
		case 0x000E: // 0x8XYE Seta o Registrador [15] com o ultimo bit do Registrador [X] e joga o Registrador [X]  1 bit e direita
			registrador[15] = registrador[(codigo_op & 0x0F00) >> 8] >> 7; //Mascara de bits para pegar o bit MSB
			registrador[(codigo_op & 0x0F00) >> 8] = registrador[(codigo_op & 0x0F00) >> 8] << 1;
			PC = PC + 2; //Conta a instrucao 0x8XYE
			break;

		default:
			printf("Codigo OP desconhecido [0x8000]: 0x%X\n", codigo_op);
			printf("Entrou no Switch das OPs 0x8000\n");
		}
		break;
		
	case 0x9000: // 0x9XY0: Pula a proxima instrucao se o Registrador [X] for diferente do Registrador [Y]
		if (registrador[(codigo_op & 0x0F00) >> 8] != registrador[(codigo_op & 0x00F0) >> 4])
			PC = PC + 4;
		else
			PC = PC + 2;
		break;
		
	case 0xA000: // 0xANNN: Seta I igual ao endereco NNN
		i = (codigo_op & 0x0FFF);
		PC = PC + 2;
		break;

	case 0xB000: // 0xB000: Pula para o endereco de NNN + registrador [0]
		PC = registrador[0] + (codigo_op & 0x0FFF);
		break;
		
	case 0xC000: // 0xCXNN: Seta o registrador [X] com o resultado da operacao entre NN e um numero aleatorio (0 e 255)
		registrador[(codigo_op & 0x0F00) >> 8] = (codigo_op & 0x00FF) & (rand() % 0xFF);
		PC = PC + 2;
		break;

	case 0xD000: // 0xDXYN: Desenha um sprite 8XN nas coordenadas (Registrado[X], Registrador[Y])
				 //Desenhar na tela atraves da operacao XOR
				 //Checa colisao e marca o registrador [15]
				 //Le a imagem da memoria
	{
		unsigned short x = registrador[(codigo_op & 0x0F00) >> 8];
		unsigned short y = registrador[(codigo_op & 0x00F0) >> 4];
		unsigned short altura = codigo_op & 0x000F;
		unsigned short pixel;

		registrador[15] = 0;
		for (int linha_y = 0; linha_y < altura; linha_y++){
			pixel = memoria[i + linha_y];
			for (int linha_x = 0; linha_x < 8; linha_x++){

				if ((pixel & (0x80 >> linha_x)) != 0){

					if (gfx[(x + linha_x + ((y + linha_y) * 64))] == 1)
						registrador[15] = 1;
					gfx[x + linha_x + ((y + linha_y) * 64)] ^= 1;
				}
			}
		}

		flag_Tela = 1;
		PC = PC + 2;
	}

	break;

	case 0xE000:
		switch (codigo_op & 0x00FF)
		{
		case 0x009E: //0xEX9E: Pula a proxima instrucao se o registrador [X] for igual a tecla (pressionada)
			if (tecla[registrador[(codigo_op & 0x0F00) >> 8]] != 0) //Verifica
				PC = PC + 4;
			else
				PC = PC + 2;
			break;

		case 0x00A1: //0xEXA1: Pula a proxima instrucao se o registrador [X] for diferente da tecla (pressionada)
			if (tecla[registrador[(codigo_op & 0x0F00) >> 8]] == 0) //Verifica
				PC += 4;
			else
				PC += 2;
			break;

		default:
			printf("Codigo OP desconhecido [0x8000]: 0x%X\n", codigo_op);
			printf("Entrou no Switch das OPs 0xE000\n");
		}
		break;

	case 0xF000:
		switch (codigo_op & 0x00FF)
		{
		case 0x0007: // 0xFX07: Pega o valor do timer de delay e joga no Registrador[X]
			registrador[(codigo_op & 0x0F00) >> 8] = timer_delay;
			PC = PC + 2;
			break;
			
		case 0x000A: // 0xANNN: Quando uma tecla e pressionada a mesma e guardada no registrador [X]
		{
			bool tecla_pressionada = 0;
			for (int i = 0; i < 16; i++){
				if (tecla[i] != 0){
					registrador[(codigo_op & 0x0F00) >> 8] = i;
					tecla_pressionada = 1;
				}
			}
			if (tecla[i] != 1)
				return;

			PC = PC + 2;
		}
		break;
		
		case 0x0015: // 0xFX15: Pega o valor do Registrador[X] e joga no timer de delay
			timer_delay = registrador[(codigo_op & 0x0F00) >> 8];
			PC = PC + 2;
			break;
		
		case 0x0018: // 0xFX18: Pega o valor do Registrador[X] e joga no timer de som
			timer_som = registrador[(codigo_op & 0x0F00) >> 8];
			PC = PC + 2;
			break;
		
		case 0x001E: // 0xFX1E: Soma o Registrador[X] + I e joga o resultado em I
			if (i + registrador[(codigo_op & 0x0F00) >> 8] > 0xFFF)
				registrador[15] = 1;
			else
				registrador[15] = 0;

			i = i + registrador[(codigo_op & 0x0F00) >> 8];
			PC = PC + 2;
			break;

		case 0x0029: // 0xFX29: Pega o valor do timer de delay e joga no Registrador[X]
			i = registrador[(codigo_op & 0x0F00) >> 8] * 0x5;
			PC = PC + 2;
			break;

		case 0x0033: // 0xFX33: Guarda a representacao binaria do valor no Registrador[X]
			memoria[i] = registrador[(codigo_op & 0x0F00) >> 8] / 100;			  //3 digitos mais significativos
			memoria[i + 1] = (registrador[(codigo_op & 0x0F00) >> 8] / 10) % 10;  //digito do meio
			memoria[i + 2] = (registrador[(codigo_op & 0x0F00) >> 8] % 100) % 10; //digito menos significativo
			PC = PC + 2;
			break;

		case 0x0055: // 0xFX55: Guarda os dados do Registrador [0] ao Registrador[X] na memoria
			for (int contador = 0; contador <= registrador[(codigo_op & 0x0F00) >> 8]; contador++)
				memoria[i + contador] = registrador[contador];
			PC = PC + 2;
			break;
		
		case 0x0065: // 0xFX65: Pega os dados da memoria e guarda do Registrador [0] ao Registrador[X]
			for (int contador = 0; contador <= ((codigo_op & 0x0F00) >> 8); contador++)
				registrador[contador] = memoria[i + contador];
			PC = PC + 2;
			break;

		default:
			printf("Codigo OP desconhecido [0x8000]: 0x%X\n", codigo_op);
			printf("Entrou no Switch das OPs 0xF000\n");
		}
		break;

	default:
		printf("Codigo OP desconhecido [0x8000]: 0x%X\n", codigo_op);
		printf("Entrou no primeiro Switch\n");
	}

	// Atualiza os timers
	if (timer_delay > 0)
		timer_delay = timer_delay - 1;

	if (timer_som > 0){
		if (timer_som == 1)
			al_play_sample(sample, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL); // toca o som caso o timer seja igual e 1
		timer_som = timer_som - 1;
	}
}

bool chip8::carregarJogo(const char *filename)
{
	resetar();
	printf("Carregando: %s\n", filename);

	// Abre o arquivo
	FILE *ROM = fopen(filename, "rb");
	if (ROM == NULL)
	{
		fputs("Erro no arquivo", stderr);
		return false;
	}

	// Checa tamanho do arquivo
	fseek(ROM, 0, SEEK_END);
	long lSize = ftell(ROM);
	rewind(ROM);
	printf("Tamanho do arquivo: %d\n", (int)lSize);

	// Reserva espaco para a ROM na memoria
	char *buffer = (char *)malloc(sizeof(char) * lSize);
	if (buffer == NULL)
	{
		fputs("Erro na memoria.", stderr);
		return false;
	}

	// Copia a ROM para o buffer
	size_t result = fread(buffer, 1, lSize, ROM);
	if (result != lSize)
	{
		fputs("Erro na leitura.", stderr);
		return false;
	}

	// Copia o buffer para a memoria do CHIP-8
	if ((4096 - 512) > lSize)
	{
		for (int i = 0; i < lSize; ++i)
			memoria[i + 512] = buffer[i];
	}
	else
		printf("Error: A ROM e maior que a memoria.");

	// Fecha o arquivo e libera o buffer
	fclose(ROM);
	free(buffer);

	return true;
}