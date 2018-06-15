/* 
 * This file is part of the CHIP8 interpreter.
 * Copyright (c) 2018 Madhav Kanbur.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <cstdio>
#include <cstdlib>
#include "chip8.h"

void CHIP8::disp_clear_00E0(word opcode) {

	int i;
	for(i=0; i<64*32; ++i)
		gfx[i] = 0;

	gfx_update = true;
	pc += 2;
}

void CHIP8::return_00EE(word opcodes) {

	pc = stack[--sp];
	pc += 2;
}

void CHIP8::goto_1NNN(word opcode) {

	pc = opcode & 0x0FFF;
}

void CHIP8::call_2NNN(word opcode) {

	stack[sp++] = pc;
	pc = opcode & 0x0FFF;
}

void CHIP8::skip_equal_3XNN(word opcode) {

	byte X = (opcode & 0x0F00) >> 8;
	if(V[X] == (opcode & 0x00FF))
		pc += 4;
	else
		pc += 2;
}

void CHIP8::skip_notequal_4XNN(word opcode) {


	byte X = (opcode & 0x0F00) >> 8;
	if(V[X] != (opcode & 0x00FF))
		pc += 4;
	else
		pc += 2;
}

void CHIP8::skip_equal_5XY0(word opcode) {

	byte X = (opcode & 0x0F00) >> 8;
	byte Y = (opcode & 0x00F0) >> 4;

	if(V[X] == V[Y])
		pc += 4;
	else
		pc += 2;
}

void CHIP8::set_VX_6XNN(word opcode) {

	byte X = (opcode & 0x0F00) >> 8;
	V[X] = (opcode & 0x00FF);
	pc += 2;
}

void CHIP8::add_VX_to_NN_7XNN(word opcode) {

	byte X = (opcode & 0x0F00) >> 8;
	V[X] += opcode & 0x00FF;
	pc += 2;
}

void CHIP8::set_VX_to_VY_8XY0(word opcode) {

	byte X, Y;
	X = (opcode & 0x0F00) >> 8;
	Y = (opcode & 0x00F0) >> 4;

	V[X] = V[Y];

	pc += 2;
}

void CHIP8::or_VX_VY_8XY1(word opcode) {

	byte X, Y;
	X = (opcode & 0x0F00) >> 8;
	Y = (opcode & 0x00F0) >> 4;

	V[X] = V[X] | V[Y];

	pc += 2;
}

void CHIP8::and_VX_VY_8XY2(word opcode) {

	byte X, Y;
	X = (opcode & 0x0F00) >> 8;
	Y = (opcode & 0x00F0) >> 4;

	V[X] = V[X] & V[Y];

	pc += 2;
}

void CHIP8::xor_VX_VY_8XY3(word opcode) {

	byte X, Y;
	X = (opcode & 0x0F00) >> 8;
	Y = (opcode & 0x00F0) >> 4;

	V[X] = V[X] ^ V[Y];

	pc += 2;
}

void CHIP8::add_VX_VY_8XY4(word opcode) {

	byte X, Y;
	X = (opcode & 0x0F00) >> 8;
	Y = (opcode & 0x00F0) >> 4;

	/* Check if VX + VY > 255 */
	if(V[X] > 0xFF - V[Y])
		V[0xF] = 1;
	else
		V[0xF] = 0;
	V[X] += V[Y];
	
	pc += 2;
}

void CHIP8::sub_VX_VY_8XY5(word opcode) {

	byte X, Y;
	X = (opcode & 0x0F00) >> 8;
	Y = (opcode & 0x00F0) >> 4;

	/* Check if VX < VY */
	if(V[X] < V[Y])
		V[0xF] = 0;
	else
		V[0xF] = 1;
	V[X] -= V[Y];
	
	pc += 2;
}

void CHIP8::rshift_VX_VY_8XY6(word opcode) {

	byte X;
	X = (opcode & 0x0F00) >> 8;

#ifdef SHIFT_HACK
	V[0xF] = V[X] & 0x01;
	V[X] >>= 1;
#else
	byte Y = (opcode & 0x00F0) >> 4;

	/* VX = VY = VY >> 1 */
	V[0xF] = V[Y] & 0x01;
	V[X] = V[Y] = V[Y] >> 1;
#endif

	pc += 2;
}

void CHIP8::sub_VY_VX_8XY7(word opcode) {

	byte X, Y;
	X = (opcode & 0x0F00) >> 8;
	Y = (opcode & 0x00F0) >> 4;

	/* VX = VY - VX */
	if(V[Y] < V[X])
		V[0xF] = 0;
	else
		V[0xF] = 1;
	V[X] = V[Y] - V[X];

	pc += 2;
}

void CHIP8::lshift_VX_VY_8XYE(word opcode) {

	byte X;
	X = (opcode & 0x0F00) >> 8;

#ifdef SHIFT_HACK
	/* VX <<= 1 */
	V[0xF] = V[X] >> 7;
	V[X] <<= 1;
#else
	byte Y = (opcode & 0x00F0) >> 4;

	/* VX = VY = VY << 1 */
	V[0xF] = (V[Y] & 0x80) >> 7;
	V[X] = V[Y] = V[Y] << 1;
#endif

	pc += 2;
}

void CHIP8::skip_notequal_9XY0(word opcode) {
	
	byte X = (opcode & 0x0F00) >> 8;
	byte Y = (opcode & 0x00F0) >> 4;
	
	if(V[X]!=V[Y])
		pc += 4;
	else
		pc += 2;
}

void CHIP8::set_I_NNN_ANNN(word opcode) {

	I = opcode & 0x0FFF;
	pc += 2;
}

void CHIP8::set_PC_BNNN(word opcode) {
	
	pc = V[0] + (opcode & 0x0FFF);
}

void CHIP8::set_VX_random_CXNN(word opcode) {

	byte X = (opcode & 0x0F00) >> 8;
	V[X] = (rand()%256) & (opcode & 0x00FF);
	pc += 2;
}

void CHIP8::draw_DXYN(word opcode) {

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

	gfx_update = true;
	pc += 2;
}

void CHIP8::skip_keypress_EX9E(word opcode) {

	byte X = (opcode & 0x0F00) >> 8;
	if(key[V[X]]!=0)
		pc += 4;
	else
		pc += 2;
}

void CHIP8::skip_key_not_press_EXA1(word opcode) {

	byte X = (opcode & 0x0F00) >> 8;
	if(key[V[X]]!=0)
		pc += 2;
	else
		pc += 4;
}

void CHIP8::set_VX_delay_FX07(word opcode) {

	byte X = (opcode & 0x0F00) >> 8;
	V[X] = get_delay_timer();
	pc += 2;
}

void CHIP8::key_block_FX0A(word opcode) {

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
}

void CHIP8::set_delay_VX_FX15(word opcode) {

	byte X = (opcode & 0x0F00) >> 8;
	set_delay_timer(V[X]);
	pc += 2;
}

void CHIP8::set_sound_VX_FX18(word opcode) {

	byte X = (opcode & 0x0F00) >> 8;
	set_sound_timer(V[X]);
	pc += 2;
}

void CHIP8::add_I_VX_FX1E(word opcode) {

	if(I > 0xFFF - V[(opcode & 0x0F00)>>8])
		V[0xF] = 1;
	else
		V[0xF] = 0;

	I += V[(opcode & 0x0F00)>>8];
	pc += 2;
}

void CHIP8::sprite_addr_FX29(word opcode) {

	I = V[(opcode & 0x0F00)>>8] * 5;
	pc += 2;
}

void CHIP8::bcd_FX33(word opcode) {

	int val;
	val = V[(opcode & 0x0F00)>>8];
	memory[I] = val/100;
	memory[I+1] = (val%100)/10;
	memory[I+2] = val%10;
	pc += 2;
}

void CHIP8::reg_dump_FX55(word opcode) {

	byte X;
	X = (opcode & 0x0F00) >> 8;
	for(int i=0; i<=X; ++i)
		memory[I+i] = V[i];
#ifdef FX55_HACK
	I += X + 1;
#endif
	pc += 2;
}

void CHIP8::reg_load_FX65(word opcode) {

	byte Y;
	Y = (opcode & 0x0F00) >> 8;
	for(int i=0; i<=Y; ++i)
		V[i] = memory[I+i];
#ifdef FX55_HACK
	I += Y + 1;
#endif
	pc += 2;
}

void CHIP8::unknown(word opcode) {

	printf("Unknown opcode : %x\n", opcode);
}
	
