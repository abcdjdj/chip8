#include <SFML/Graphics.hpp>
#include <iostream>
#include "chip8.h"

#define RECTANGLE_WIDTH 12
#define RECTANGLE_HEIGHT 12

void keypress_handler(sf::Event &event, CHIP8 &mychip);
void keyrelease_handler(sf::Event &event, CHIP8 &mychip);
void draw_screen(sf::RenderWindow &window, CHIP8 &mychip);

int main(int argc, char **argv)
{

	if(argc < 2) {
		printf("Usage : chip8 <rom_file>\n");
		return 0;
	}

	CHIP8 mychip;
	
	if(mychip.load_game(argv[1]) == -1)
		return 0;

	sf::RenderWindow window(sf::VideoMode(RECTANGLE_WIDTH*64, RECTANGLE_HEIGHT*32), "Chip8 by abcdjdj");

	// run the program as long as the window is open
	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed) {
				window.close();
			} else if(event.type == sf::Event::KeyPressed) {
				keypress_handler(event, mychip);
			} else if(event.type == sf::Event::KeyReleased) {
				keyrelease_handler(event, mychip);
			}

		}

		mychip.emulate_cycle();

		if(mychip.gfx_update) {
			// clear the window with black color
			window.clear(sf::Color::Black);

			// draw stuff here
			draw_screen(window, mychip);

			window.display();

			mychip.gfx_update = false;
		}
	}

	return 0;
}

void draw_screen(sf::RenderWindow &window, CHIP8 &mychip) {

	int i;

	for(i=0; i<64*32; ++i) {
		if(mychip.gfx[i] == 1) {
			int x = i % 64;
			int y = i / 64;

			sf::RectangleShape rect;
			rect.setSize(sf::Vector2f(RECTANGLE_WIDTH, RECTANGLE_HEIGHT));
			rect.setOutlineColor(sf::Color::White);
			rect.setOutlineThickness(1);
			rect.setPosition(x * RECTANGLE_WIDTH, y * RECTANGLE_HEIGHT);

			window.draw(rect);
		}
	}
}

void keypress_handler(sf::Event &event, CHIP8 &mychip) {

	sf::Keyboard::Key key_press = event.key.code;
			
	if(key_press == sf::Keyboard::Key::Num1)
		mychip.key[0x1] = 1;
	else if(key_press == sf::Keyboard::Key::Num2)
		mychip.key[0x2] = 1;
	else if(key_press == sf::Keyboard::Key::Num3)
		mychip.key[0x3] = 1;
	else if(key_press == sf::Keyboard::Key::Num4)
		mychip.key[0xC] = 1;
	
	else if(key_press == sf::Keyboard::Key::Q)
		mychip.key[0x4] = 1;
	else if(key_press == sf::Keyboard::Key::W)
		mychip.key[0x5] = 1;
	else if(key_press == sf::Keyboard::Key::E)
		mychip.key[0x6] = 1;
	else if(key_press == sf::Keyboard::Key::R)
		mychip.key[0xD] = 1;
	
	
	else if(key_press == sf::Keyboard::Key::A)
		mychip.key[0x7] = 1;
	else if(key_press == sf::Keyboard::Key::S)
		mychip.key[0x8] = 1;
	else if(key_press == sf::Keyboard::Key::D)
		mychip.key[0x9] = 1;
	else if(key_press == sf::Keyboard::Key::F)
		mychip.key[0xE] = 1;

	else if(key_press == sf::Keyboard::Key::Z)
		mychip.key[0xA] = 1;
	else if(key_press == sf::Keyboard::Key::X)
		mychip.key[0x0] = 1;
	else if(key_press == sf::Keyboard::Key::C)
		mychip.key[0xB] = 1;
	else if(key_press == sf::Keyboard::Key::V)
		mychip.key[0xF] = 1;

	else if(key_press == sf::Keyboard::Key::Escape)
		exit(EXIT_SUCCESS);
}

void keyrelease_handler(sf::Event &event, CHIP8 &mychip) {

	sf::Keyboard::Key key_press = event.key.code;
			
	if(key_press == sf::Keyboard::Key::Num1)
		mychip.key[0x1] = 0;
	else if(key_press == sf::Keyboard::Key::Num2)
		mychip.key[0x2] = 0;
	else if(key_press == sf::Keyboard::Key::Num3)
		mychip.key[0x3] = 0;
	else if(key_press == sf::Keyboard::Key::Num4)
		mychip.key[0xC] = 0;
	
	else if(key_press == sf::Keyboard::Key::Q)
		mychip.key[0x4] = 0;
	else if(key_press == sf::Keyboard::Key::W)
		mychip.key[0x5] = 0;
	else if(key_press == sf::Keyboard::Key::E)
		mychip.key[0x6] = 0;
	else if(key_press == sf::Keyboard::Key::R)
		mychip.key[0xD] = 0;
	
	
	else if(key_press == sf::Keyboard::Key::A)
		mychip.key[0x7] = 0;
	else if(key_press == sf::Keyboard::Key::S)
		mychip.key[0x8] = 0;
	else if(key_press == sf::Keyboard::Key::D)
		mychip.key[0x9] = 0;
	else if(key_press == sf::Keyboard::Key::F)
		mychip.key[0xE] = 0;

	else if(key_press == sf::Keyboard::Key::Z)
		mychip.key[0xA] = 0;
	else if(key_press == sf::Keyboard::Key::X)
		mychip.key[0x0] = 0;
	else if(key_press == sf::Keyboard::Key::C)
		mychip.key[0xB] = 0;
	else if(key_press == sf::Keyboard::Key::V)
		mychip.key[0xF] = 0;
}
