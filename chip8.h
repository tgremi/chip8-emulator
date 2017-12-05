#define CHIP8_H_INCLUDED
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>	

class chip8 {
public:
	chip8();
	void emula_Ciclo();//Emula o ciclo do CHIP-8
	bool carregarJogo(const char * filename);//Carrega a ROM na memoria do CHIP-8
	void resetar(); //Reseta o CHIP-8
	bool flag_Tela; //Status atual do registrador [15] (Carry Flag)
	bool inicializar(); //Testa as configuracoes antes de iniciar o emulador
	unsigned char tecla[16]; //As teclas do Chip-8 vao de 0 a F
	
	unsigned char gfx[64 * 32];
private:
	ALLEGRO_SAMPLE *sample = NULL; //Variavel que sera tocado quando o timer ativar
	ALLEGRO_DISPLAY *janela; //Janela do emulador

	unsigned short codigo_op; //Codigo da operacao
	unsigned char registrador[16]; //15 registradores (V0...VE) de 8 bits do CHIP-8 - 16� = 'carry flag'
	unsigned short i; //Index
	unsigned short PC; // Program Counter do CHIP-8

	unsigned short posicao[16]; //Em caso de jumps ou calls saberemos para onde voltar no programa
	unsigned short posicao_atual; //Guarda a posicao atual no vetor de posicoes

	unsigned char memoria[4096]; //Memoria total = 4K

	unsigned char timer_delay;
	unsigned char timer_som; //ativa o som quando o timer a diferente de 0

};



