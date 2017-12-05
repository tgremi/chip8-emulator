--Comando para compilar
g++ -Wall main.cpp graficos_Allegro.cpp chip8.cpp -o chip8 `pkg-config --cflags --libs allegro-5.0 allegro_acodec-5.0 allegro_audio-5.0 allegro_color-5.0 allegro_dialog-5.0 allegro_font-5.0 allegro_image-5.0 allegro_main-5.0 allegro_memfile-5.0 allegro_physfs-5.0 allegro_primitives-5.0 allegro_ttf-5.0`

--Comando para rodar um jogo
./chip8 <Nome do jogo>
