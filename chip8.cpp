#include <cstdio>
#include "chip8.h"

CHIP8::CHIP8() {
	pc = 0x200;
	I = 0;
	sp = 0;

	sound_timer = delay_timer = 0;
	
	int i;
	for(i=0; i<16; ++i)
		key[i] = V[i] = 0;
	
	for(i=0; i<4096; ++i)
		memory[i] = 0;
	
	for(i=0; i<16; ++i)
		stack[i] = 0;
	
	for(i=0; i<64*32; ++i)
		gfx[i] = 0;

	for(i=0; i<80; ++i)
		memory[i] = chip8_fontset[i];

	gfx_update = true;
}

void CHIP8::emulate_cycle() {

	word opcode = memory[pc] << 8 | memory[pc+1];
	decode(opcode);

	update_timers();
}

void CHIP8::update_timers() {

	if(delay_timer > 0)
		--delay_timer;

	if(sound_timer > 0) {
		--sound_timer;
	}
}

void CHIP8::decode(word opcode) {

	(this->*opcode_table[((opcode & 0xF000) >> 12)])(opcode);
	//void (CHIP8::*ptr)(word) = opcode_table[(opcode & 0xF000)>>12];
	//(this->*ptr)(opcode);
}

void CHIP8::zero_wrapper(word opcode) {

	switch(opcode & 0x000F) {
		case 0x0:
			disp_clear_00E0(opcode);
			break;

		case 0xE:
			return_00EE(opcode);
			break;

		default:
			unknown(opcode);
	}
}

void CHIP8::eight_wrapper(word opcode) {

	(this->*arithmetic_table[(opcode & 0x000F)])(opcode);
}

void CHIP8::fourteen_wrapper(word opcode) {

	switch(opcode & 0x000F) {
		case 0x1:
			skip_key_not_press_EXA1(opcode);
			break;
		case 0xE:
			skip_keypress_EX9E(opcode);
			break;
		default:
			unknown(opcode);
	}
}

void CHIP8::fifteen_wrapper(word opcode) {

	switch(opcode & 0x00FF) {
		case 0x15:
			set_delay_VX_FX15(opcode);
			break;

		case 0x55:
			reg_dump_FX55(opcode);
			break;

		case 0x65:
			reg_load_FX65(opcode);
			break;

		default:
			(this->*misc_table[opcode & 0x000F])(opcode);
	}
}

int CHIP8::load_game(const char *str) {

	FILE *fp = fopen(str, "r");
	if(!fp) {
		printf("Error opening file \'%s\'\n", str);
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	
	/* Get the number of bytes to be read */
	int size = ftell(fp);
	rewind(fp);

	if(size > MAX_FILE_SIZE) {
		printf("Error! File too large\n");
		fclose(fp);
		return -1;
	}
	
	/* Allocate buffer for reading file */
	byte *buf = new byte[size];
	
	fread(buf, sizeof(byte), size, fp);
	if(ferror(fp)) {
		printf("Error reading from \'%s\'\n", str);
		delete buf;
		fclose(fp);
		return -1;
	}

	/* Copy program data from buf to memory */
	for(int i=0; i<size; ++i)
		memory[0x200 + i] = buf[i];

	delete buf;
	fclose(fp);
	
	return 0;
}
