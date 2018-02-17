#ifndef __CHIP8_H__
#define __CHIP8_H__

#include <cstdint>

#define MAX_FILE_SIZE (0xFFF - 0x200 + 1)

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

		/* Stack */
		word stack[16];
		word sp;

		void update_timers();
		void decode(word opcode);
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
