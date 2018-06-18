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

#ifndef __CHIP8_H__
#define __CHIP8_H__

#include <cstdint>
#include <chrono>

#define MAX_FILE_SIZE (0xFFF - 0x200 + 1)
#define CLK_FREQ 500

typedef uint8_t byte;
typedef uint16_t word;

class CHIP8 {
	private :
		/* Main Memory */
		byte memory[4096];

		/* V0-VF Registers */
		byte V[16];

		/* Index Register and Program Counter */
		word I;
		word pc;

		/* Hardware timers */
		byte delay_timer;
		byte sound_timer;
		long long delay_previous;
		long long sound_previous;
		void set_delay_timer(byte val);
		void set_sound_timer(byte val);
		byte get_delay_timer();

		/* Stack */
		word stack[16];
		word sp;

		void decode(word opcode);
		long long opcode_previous_time;

		/* Opcodes */
		void disp_clear_00E0(word opcode);
		void return_00EE(word opcode);
		void goto_1NNN(word opcode);
		void call_2NNN(word opcode);
		void skip_equal_3XNN(word opcode);
		void skip_notequal_4XNN(word opcode);
		void skip_equal_5XY0(word opcode);
		void set_VX_6XNN(word opcode);
		void add_VX_to_NN_7XNN(word opcode);
		void set_VX_to_VY_8XY0(word opcode);
		void or_VX_VY_8XY1(word opcode);
		void and_VX_VY_8XY2(word opcode);
		void xor_VX_VY_8XY3(word opcode);
		void add_VX_VY_8XY4(word opcode);
		void sub_VX_VY_8XY5(word opcode);
		void rshift_VX_VY_8XY6(word opcode);
		void sub_VY_VX_8XY7(word opcode);
		void lshift_VX_VY_8XYE(word opcode);
		void skip_notequal_9XY0(word opcode);
		void set_I_NNN_ANNN(word opcode);
		void set_PC_BNNN(word opcode);
		void set_VX_random_CXNN(word opcode);
		void draw_DXYN(word opcode);
		void skip_keypress_EX9E(word opcode);
		void skip_key_not_press_EXA1(word opcode);
		void set_VX_delay_FX07(word opcode);
		void key_block_FX0A(word opcode);
		void set_delay_VX_FX15(word opcode);
		void set_sound_VX_FX18(word opcode);
		void add_I_VX_FX1E(word opcode);
		void sprite_addr_FX29(word opcode);
		void bcd_FX33(word opcode);
		void reg_dump_FX55(word opcode);
		void reg_load_FX65(word opcode);
		void unknown(word opcode);

		/* Wrapper functions */
		void zero_wrapper(word opcode);
		void eight_wrapper(word opcode);
		void fourteen_wrapper(word opcode);
		void fifteen_wrapper(word opcode);

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

		void (CHIP8::*opcode_table[16])(word) =
		{
			&CHIP8::zero_wrapper,
			&CHIP8::goto_1NNN,
			&CHIP8::call_2NNN,
			&CHIP8::skip_equal_3XNN,
			&CHIP8::skip_notequal_4XNN,
			&CHIP8::skip_equal_5XY0,
			&CHIP8::set_VX_6XNN,
			&CHIP8::add_VX_to_NN_7XNN,
			&CHIP8::eight_wrapper,
			&CHIP8::skip_notequal_9XY0,
			&CHIP8::set_I_NNN_ANNN,
			&CHIP8::set_PC_BNNN,
			&CHIP8::set_VX_random_CXNN,
			&CHIP8::draw_DXYN,
			&CHIP8::fourteen_wrapper,
			&CHIP8::fifteen_wrapper
		};

		void (CHIP8::*arithmetic_table[16])(word) =
		{
			&CHIP8::set_VX_to_VY_8XY0,
			&CHIP8::or_VX_VY_8XY1,
			&CHIP8::and_VX_VY_8XY2,
			&CHIP8::xor_VX_VY_8XY3,
			&CHIP8::add_VX_VY_8XY4,
			&CHIP8::sub_VX_VY_8XY5,
			&CHIP8::rshift_VX_VY_8XY6,
			&CHIP8::sub_VY_VX_8XY7,
			&CHIP8::unknown, &CHIP8::unknown, &CHIP8::unknown,
			&CHIP8::unknown, &CHIP8::unknown, &CHIP8::unknown,
			&CHIP8::lshift_VX_VY_8XYE,
			&CHIP8::unknown
		};

		/* Function lookup table for opcodes starting
		 * with 'F' */
		void (CHIP8::*misc_table[16])(word) =
		{
			&CHIP8::unknown, &CHIP8::unknown, &CHIP8::unknown,
			&CHIP8::bcd_FX33, // 3
			&CHIP8::unknown,
			&CHIP8::unknown, // 5 is handled in fifteen_handler
			&CHIP8::unknown,
			&CHIP8::set_VX_delay_FX07,
			&CHIP8::set_sound_VX_FX18,
			&CHIP8::sprite_addr_FX29,
			&CHIP8::key_block_FX0A,
			&CHIP8::unknown, &CHIP8::unknown, &CHIP8::unknown,
			&CHIP8::add_I_VX_FX1E,
			&CHIP8::unknown
		};

		public :
			/* Pixel array */
			byte gfx[64*32];

			/* State of the keypad */
			byte key[16];
		
			/* GFX updation flag */
			bool gfx_update;
		
			int load_game(const char *str);
			void emulate_cycle();
			CHIP8();
};

#endif
