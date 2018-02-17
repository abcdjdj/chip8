#include <cstdio>
#include <clocale>
#include <unistd.h>
#include <ncurses.h>
#include "chip8.h"

#define BLOCK_CHAR (0x2588)

void draw(const CHIP8 &mychip) {

	const wchar_t block[] = { BLOCK_CHAR, 0 };
	int i, j;
	for(i=0; i<32; ++i) {
		for(j=0; j<64; ++j) {
			if(mychip.gfx[j+i*64]==1)
				mvaddwstr(i, j, block);
			else
				mvaddch(i, j,' ');
		}
	}
}

int main(int argc, char **argv) {

	if(argc < 2) {
		printf("Usage : chip8 <rom_file>\n");
		return 0;
	}

	CHIP8 mychip;
	
	if(mychip.load_game(argv[1]) == -1)
		return 0;
	
	setlocale(LC_ALL, "");
	
	/* Initialize ncurses mode on stdscr */
	initscr();

	/* Disable the cursor */
	curs_set(0); 
	
	/* Black and White color scheme */
	if(has_colors()) {
		start_color();
		init_color(COLOR_WHITE, 1000, 1000, 1000);
		init_color(COLOR_BLACK, 0, 0, 0);
		init_pair(1, COLOR_WHITE, COLOR_BLACK);
		attron(COLOR_PAIR(1));
	}
	
	cbreak();
	noecho();

	/* Non blocking getch() */
	nodelay(stdscr, TRUE);

	char key_press;
	int count_cycles = 3;

	for(;;) {
		
		mychip.emulate_cycle();
		draw(mychip);
		if(mychip.gfx_update) {
			refresh();
			usleep(15*1000);
			mychip.gfx_update = false;
		}
		
		if(count_cycles >= 3) {
			key_press = getch();
			
			for(byte &i : mychip.key)
				i = 0;
			
			if(key_press == '1')
				mychip.key[0x1] = 1;
			else if(key_press == '2')
				mychip.key[0x2] = 1;
			else if(key_press == '3')
				mychip.key[0x3] = 1;
			else if(key_press == '4')
				mychip.key[0xC] = 1;
			
			else if(key_press == 'q')
				mychip.key[0x4] = 1;
			else if(key_press == 'w')
				mychip.key[0x5] = 1;
			else if(key_press == 'e')
				mychip.key[0x6] = 1;
			else if(key_press == 'r')
				mychip.key[0xD] = 1;
			
			
			else if(key_press == 'a')
				mychip.key[0x7] = 1;
			else if(key_press == 's')
				mychip.key[0x8] = 1;
			else if(key_press == 'd')
				mychip.key[0x9] = 1;
			else if(key_press == 'f')
				mychip.key[0xE] = 1;

			else if(key_press == 'z')
				mychip.key[0xA] = 1;
			else if(key_press == 'x')
				mychip.key[0x0] = 1;
			else if(key_press == 'c')
				mychip.key[0xB] = 1;
			else if(key_press == 'v')
				mychip.key[0xF] = 1;
			
			count_cycles = 0;
		}
		++count_cycles;
	} 
	
	endwin();
}
