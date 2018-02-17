#include <cstdlib>
#include <cstdio>
#include "chip8.h"

const byte chip8_fontset[80] =
{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

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

	switch(opcode & 0xF000) {
	
		case 0x0000: {
			switch(opcode & 0x000F) {
				case 0x0:
					int i;
					for(i=0; i<64*32; ++i)
						gfx[i] = 0;
					gfx_update = true;
					pc += 2;
					break;
				
				case 0xE:
					pc = stack[--sp];
					pc += 2;
					break;

				default:
					printf("Error! Invalid opcode : %x\n", opcode);
			}
			break;
		}
	
		case 0x1000: {
			pc = opcode & 0x0FFF;
			break;
		}

		case 0x2000: {
			stack[sp++] = pc;
			pc = opcode & 0x0FFF;
			break;
		}

		case 0x3000: {
			byte X = (opcode & 0x0F00) >> 8;
			if(V[X] == (opcode & 0x00FF))
				pc += 4;
			else
				pc += 2;
			break;
	 	}

		case 0x4000: {
			byte X = (opcode & 0x0F00) >> 8;
			if(V[X] != (opcode & 0x00FF))
				pc += 4;
			else
				pc += 2;
			break;
		}

		case 0x5000: {
			byte X = (opcode & 0x0F00) >> 8;
			byte Y = (opcode & 0x00F0) >> 4;
			if(V[X] == V[Y])
				pc += 4;
			else
				pc += 2;
			break;
	 	}

		case 0x6000: {
			byte X = (opcode & 0x0F00) >> 8;
			V[X] = (opcode & 0x00FF);
			pc += 2;
			break;
	 	}

		case 0x7000: {
			byte X = (opcode & 0x0F00) >> 8;
			V[X] += opcode & 0x00FF;
			pc += 2;
			break;
	  	}
	
		case 0x8000: {
			byte X, Y;
			X = (opcode & 0x0F00) >> 8;
			Y = (opcode & 0x00F0) >> 4;
			
			switch(opcode & 0x000F) {	
				case 0x0:
				V[X] = V[Y];
				break;

				case 0x1:
				V[X] |= V[Y];
				break;

				case 0x2:
				V[X] &= V[Y];
				break;

				case 0x3:
				V[X] ^= V[Y];
				break;

				case 0x4:
				/* Check if VX + VY > 255 */
				if(V[X] > 0xFF - V[Y])
					V[0xF] = 1;
				else
					V[0xF] = 0;
				V[X] += V[Y];
				break;

				case 0x5:
				/* Check if VX < VY */
				if(V[X] < V[Y])
					V[0xF] = 0;
				else
					V[0xF] = 1;
				V[X] -= V[Y];
				break;

				case 0x6:
#if (1)
				V[0xF] = V[X] & 0x01;
				V[X] >>= 1;
#elif
				/* VX = VY = VY >> 1 */
				V[0xF] = V[Y] & 0x01;
				V[X] = V[Y] = V[Y] >> 1;
#endif
				break;

				case 0x7:
				/* VX = VY - VX */
				if(V[Y] < V[X])
					V[0xF] = 0;
				else
					V[0xF] = 1;
				V[X] = V[Y] - V[X];
				break;

				case 0xE:
#if (1)
				/* VX <<= 1 */
				V[0xF] = V[X] >> 7;
				V[X] <<= 1;
#elif
				/* VX = VY = VY << 1 */
				V[0xF] = (V[Y] & 0x80) >> 7;
				V[X] = V[Y] = V[Y] << 1;
#endif
				break;
			}
			pc += 2;
			break;
		}

		case 0x9000: {
			byte X = (opcode & 0x0F00) >> 8;
			byte Y = (opcode & 0x00F0) >> 4;
			if(V[X]!=V[Y])
				pc += 4;
			else
				pc += 2;
			break;
		}


		case 0xA000: {
			I = opcode & 0x0FFF;
			pc += 2;
			break;
		}
			
	       	case 0xB000: {
			pc = V[0] + (opcode & 0x0FFF);
			break;
		}
		
		case 0xC000: {
			V[(opcode & 0x0F00)>>8] = (rand()%256) & (opcode & 0x00FF);
			pc += 2;
			break;
		}

		case 0xD000: {
			byte x = V[(opcode & 0x0F00)>>8];
			byte y = V[(opcode & 0x00F0)>>4];
			byte n = opcode & 0x000F;
			byte pixel;

			/* Initially set VF=0, and later check for 1->0 transitions */
			V[0xF] = 0;
			for(int yline = 0; yline < n; ++yline) {
				pixel = memory[I + yline]; 
				for(int xline = 0; xline < 8; ++xline) {
					int index = x + xline + (y + yline)*64;
					int check_bit = (pixel & (0x80 >> xline));
					/* If check_bit is non zero ==> XOR with 1 
					 * Else do nothing (since x XOR 0 = x) */
					if(check_bit != 0) {
						if(gfx[index] == 1)
						      V[0xF] = 1;
						gfx[index] ^= 1;
					}
				}
			}

			pc += 2;
			gfx_update = true;
			break;
		}	
		
		case 0xE000: {
			switch(opcode & 0x00FF) {
				case 0x009E:
					if(key[V[(opcode & 0x0F00)>>8]]!=0)
						pc += 4;
					else
						pc += 2;
					break;
				case 0x00A1:
					if(key[V[(opcode & 0x0F00)>>8]]!=0)
						pc += 2;
					else
						pc += 4;
					break;
				default:
					printf("Error! Invalid opcode : %x\n", opcode);
			}
			break;
		}
		case 0xF000: {
			switch(opcode & 0x00FF) {
				case 0x07:
					V[(opcode&0x0F00)>>8] = delay_timer;
					pc += 2;
					break;
				
				case 0x0A:
					bool keyPress;
				        keyPress = false;

					for(int i=0; i<16; ++i) {
						if(key[i] != 0) {
							V[(opcode & 0x0F00)>>8] = i;
							keyPress = true;
						}
					}

					if(!keyPress)
						return;
					pc += 2;
					break;
				
				case 0x15:
					delay_timer = V[(opcode & 0x0F00)>>8];
					pc += 2;
					break;

				case 0x18:
					sound_timer = V[(opcode & 0x0F00)>>8];
					pc += 2;
					break;

				case 0x1E:
					if(I > 0xFFF - V[(opcode & 0x0F00)>>8])
						V[0xF] = 1;
					else
						V[0xF] = 0;
					I += V[(opcode & 0x0F00)>>8];
					pc += 2;
					break;

				case 0x29:
					I = V[(opcode & 0x0F00)>>8] * 5;
					pc += 2;
					break;

				case 0x33:
					int val;
				        val = V[(opcode & 0x0F00)>>8];
					memory[I] = val/100;
					memory[I+1] = (val%100)/10;
					memory[I+2] = val%10;
					pc += 2;
					break;

				case 0x55:
					byte X;
				        X = (opcode & 0x0F00) >> 8;
					for(int i=0; i<=X; ++i)
						memory[I+i] = V[i];
					I += X + 1;
					pc += 2;
					break;

				case 0x65:
					byte Y;
					Y = (opcode & 0x0F00) >> 8;
					for(int i=0; i<=Y; ++i)
						V[i] = memory[I+i];
					I += Y + 1;
					pc += 2;
					break;
			}
			break;
		}
		default:
			printf("Error! Invalid opcode : %x\n", opcode);
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
