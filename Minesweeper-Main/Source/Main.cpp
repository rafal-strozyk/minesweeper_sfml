#include <chrono>
#include <algorithm>
#include <random>
#include <SFML/Graphics.hpp>

#include "Headers/Cell.hpp"
#include "Headers/DrawText.hpp"
#include "Headers/Field.hpp"
#include "Headers/GetCell.hpp"
#include "Headers/Global.hpp"

int main()
{
	//Wymuszenie klatkowania
	unsigned lag = 0;

	//Alexander ma 4 stany:
	unsigned char Alexander_state = 0;

	
	std::chrono::time_point<std::chrono::steady_clock> previous_time;

	//SFML thing
	sf::Event event;

	//SFML okno
	sf::RenderWindow window(sf::VideoMode(CELL_SIZE * COLUMNS * SCREEN_RESIZE + 8, SCREEN_RESIZE * (FONT_HEIGHT + CELL_SIZE * COLUMNS)), "PO Saper", sf::Style::Close);
	//Resize okna
	window.setView(sf::View(sf::FloatRect(-1, -CELL_SIZE, CELL_SIZE * COLUMNS +1, FONT_HEIGHT + CELL_SIZE * ROWS)));

	//Emotka sprite
	sf::Sprite Alexander;

	//Emotka texture
	sf::Texture Alexander_texture;
	//We get the texture from an image
	Alexander_texture.loadFromFile("Resources/Images/Alexander.png");

	//Pozycjonowanie emotki
	Alexander.setPosition(static_cast<float>(CELL_SIZE * COLUMNS - Alexander_texture.getSize().y),- CELL_SIZE);
	Alexander.setTexture(Alexander_texture);

	//Plansza do gry
	Field field;

	//Obecny czas
	previous_time = std::chrono::steady_clock::now();

	while (1 == window.isOpen())
	{
		//Obliczamy opoznienie
		unsigned delta_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - previous_time).count();

		lag += delta_time;

		previous_time += std::chrono::microseconds(delta_time);

		
		while (FRAME_DURATION <= lag)
		{
			//Pozycja kursora
			unsigned char mouse_cell_x = std::clamp(static_cast<int>(floor(sf::Mouse::getPosition(window).x / static_cast<float>(CELL_SIZE * SCREEN_RESIZE))), -1, COLUMNS - 0);
			unsigned char mouse_cell_y = std::clamp(static_cast<int>(floor(sf::Mouse::getPosition(window).y / static_cast<float>(CELL_SIZE * SCREEN_RESIZE))), -1, ROWS - 0);

			lag -= FRAME_DURATION;

			while (1 == window.pollEvent(event))
			{
				switch (event.type)
				{
					//Zamkniecie okna
					case sf::Event::Closed:
					{
						//Zamkniecie okna close the window
						window.close();

						break;
					}
					case sf::Event::KeyReleased:
					{
						switch (event.key.code)
						{
							case sf::Keyboard::Enter:
							{
								//Restart gry
								field.restart();
							}
						}

						break;
					}
					case sf::Event::MouseButtonReleased:
					{
						switch (event.mouseButton.button)
						{
							
							case sf::Mouse::Left:
							{
								
								field.open_cell(mouse_cell_x, mouse_cell_y-1);

								break;
							}
							
							case sf::Mouse::Right:
							{
							
								field.flag_cell(mouse_cell_x, mouse_cell_y-1);
							}
						}
					}
				}
			}

			//Zmiana stanow emotki
			if (1 == sf::Mouse::isButtonPressed(sf::Mouse::Left) || 1 == sf::Mouse::isButtonPressed(sf::Mouse::Right))
			{
				//Otwarta buzia
				Alexander_state = 1;

				//Zmiana kolorkow kafelkow
				field.set_mouse_state(2, mouse_cell_x, mouse_cell_y-1);
			}
			//Otherwise
			else
			{
				
				Alexander_state = 0;

				field.set_mouse_state(1, mouse_cell_x, mouse_cell_y-1);
			}

			//przegrana
			if (-1 == field.get_game_over())
			{
				
				Alexander_state = 2;
			}
			//wygrana
			else if (1 == field.get_game_over())
			{
				
				Alexander_state = 3;
			}

			//Rysujemy pole
			if (FRAME_DURATION > lag)
			{
				
				window.clear();

				
				field.draw(window);

				//Ekran koncowy
				if (1 == field.effect_over())
				{
					if (1 == field.get_game_over())
					{
						draw_text(1, static_cast<unsigned short>(round(0.5f * (CELL_SIZE * COLUMNS - 8 * FONT_WIDTH))), static_cast<unsigned short>(round(0.5f * (CELL_SIZE * ROWS - FONT_HEIGHT))), "VICTORY!", window);
					}
					else
					{
						draw_text(1, static_cast<unsigned short>(round(0.5f * (CELL_SIZE * COLUMNS - 4 * FONT_WIDTH))), static_cast<unsigned short>(round(0.5f * (CELL_SIZE * ROWS - 2 * FONT_HEIGHT))), "GAME\nOVER", window);
					}
				}

				//Ilosc min
				draw_text(0, 0, 2-CELL_SIZE, "Mines:" + std::to_string(MINES - field.get_flags()), window);

				//Zmiana wyrazu twarzy emotki
				Alexander.setTextureRect(sf::IntRect(Alexander_state * Alexander_texture.getSize().y, 0, Alexander_texture.getSize().y, Alexander_texture.getSize().y));

				
				window.draw(Alexander);
				window.display();
			}
		}
	}
}