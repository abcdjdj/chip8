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

#include <SFML/Graphics.hpp>
#include <iostream>
#include <array>
#include <chrono>
#include <thread>
#include "chip8.h"

#define RECTANGLE_WIDTH 12
#define RECTANGLE_HEIGHT 12

void key_handler(const sf::Event &event, CHIP8 &mychip, const byte &val);
void draw_screen(sf::RenderWindow &window, CHIP8 &mychip);
void rectangles_init();

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
        rectangles_init();
	sf::Clock clock;

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
				key_handler(event, mychip, 1);
			} else if(event.type == sf::Event::KeyReleased) {
				key_handler(event, mychip, 0);
			}

		}

		mychip.emulate_cycle();

		if(mychip.gfx_update) {

			/* clear the window with black color */
			window.clear(sf::Color::Black);

			/* draw stuff here */
			draw_screen(window, mychip);

			/* Draw frames at 60 FPS */
			auto diff = clock.getElapsedTime().asSeconds();
			long long sleep_time = 1000.0/60 - diff*1000;
			if(sleep_time > 0) {
				std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
			}

			window.display();

			clock.restart();
			mychip.gfx_update = false;
		}
	}

	return 0;
}

std::array<sf::RectangleShape, 64*32> rectangle;

void rectangles_init()
{
        for(int i = 0; i < 64 * 32; ++i) {
                rectangle[i].setSize(sf::Vector2f(RECTANGLE_WIDTH, RECTANGLE_HEIGHT));
                rectangle[i].setOutlineColor(sf::Color::White);
                rectangle[i].setOutlineThickness(1);
                rectangle[i].setPosition((i % 64) * RECTANGLE_WIDTH, (i / 64) * RECTANGLE_HEIGHT);
        }
}

void draw_screen(sf::RenderWindow &window, CHIP8 &mychip) {

        for(int i = 0; i < 64 * 32; ++i)
                if(mychip.gfx[i])
                        window.draw(rectangle[i]);
}

void key_handler(const sf::Event &event, CHIP8 &mychip, const byte &val) {

	sf::Keyboard::Key key_press = event.key.code;
	switch(key_press) {
		case sf::Keyboard::Key::Num1:
			mychip.key[0x1] = val;
			break;
		case sf::Keyboard::Key::Num2:
			mychip.key[0x2] = val;
			break;
                case sf::Keyboard::Key::Num3:
			mychip.key[0x3] = val;
			break;
                case sf::Keyboard::Key::Num4:
			mychip.key[0xC] = val;
			break;

                case sf::Keyboard::Key::Q:
                        mychip.key[0x4] = val;
                        break;
                case sf::Keyboard::Key::W:
                        mychip.key[0x5] = val;
                        break;
                case sf::Keyboard::Key::E:
                        mychip.key[0x6] = val;
                        break;
                case sf::Keyboard::Key::R:
                        mychip.key[0xD] = val;
                        break;

                case sf::Keyboard::Key::A:
                        mychip.key[0x7] = val;
                        break;
                case sf::Keyboard::Key::S:
                        mychip.key[0x8] = val;
                        break;
                case sf::Keyboard::Key::D:
                        mychip.key[0x9] = val;
                        break;
                case sf::Keyboard::Key::F:
                        mychip.key[0xE] = val;
                        break;

                case sf::Keyboard::Key::Z:
                        mychip.key[0xA] = val;
                        break;
                case sf::Keyboard::Key::X:
                        mychip.key[0x0] = val;
                        break;
                case sf::Keyboard::Key::C:
                        mychip.key[0xB] = val;
                        break;
                case sf::Keyboard::Key::V:
                        mychip.key[0xF] = val;
                        break;

                case sf::Keyboard::Key::Escape:
                        exit(EXIT_SUCCESS);

                default:
                        break;
        }
}
