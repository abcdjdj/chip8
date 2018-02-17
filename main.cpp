#include <cstdio>
#include "chip8.h"

int main(int argc, char **argv) {

	if(argc < 2) {
		printf("Usage : chip8 <rom_file>\n");
		return 0;
	}

	CHIP8 mychip;

	if(mychip.load_game(argv[1]) == -1)
		return 0;

	for(;;) {
		mychip.emulate_cycle();
		mychip.debug();
	}
}
