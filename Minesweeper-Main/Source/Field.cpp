#include <random>
#include <SFML/Graphics.hpp>

#include "Headers/Cell.hpp"
#include "Headers/Field.hpp"
#include "Headers/GetCell.hpp"
#include "Headers/Global.hpp"



Field::Field() :
	game_over(1),
	random_engine(random_device())
{
	//Dodawanie kafelkow do wektora
	for (unsigned char a = 0; a < ROWS; a++)
	{
		for (unsigned char b = 0; b < COLUMNS; b++)
		{
			
			cells.push_back(Cell(b, a));
		}
	}

	
	restart();
}

bool Field::effect_over()
{
	//Sprawdzamy czy wszystkie skonczyly animacje
	for (Cell& cell : cells)
	{
		//Jesli nie 
		if (0 < cell.get_effect_timer())
		{
			//to zwracamy zero
			return 0;
		}
	}

	
	return 1;
}

char Field::get_game_over()
{
	//czy koniec gry
	return game_over;
}

unsigned short Field::get_flags()
{
	//Ile flag zostalo
	unsigned short total_flags = 0;

	for (Cell& cell : cells)
	{
		total_flags += cell.get_is_flagged();
	}

	
	return total_flags;
}


void Field::draw(sf::RenderWindow& i_window)
{
	//Rysowanie kafelkow
	sf::RectangleShape cell_shape(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));

	sf::Sprite icon_sprite;

	sf::Texture icons_texture;
	icons_texture.loadFromFile("Resources/Images/Icons" + std::to_string(CELL_SIZE) + ".png");

	icon_sprite.setTexture(icons_texture);

	//Lecimy przez wszystkie kafelki
	for (unsigned char a = 0; a < COLUMNS; a++)
	{
		for (unsigned char b = 0; b < ROWS; b++)
		{
			//Ustawiamy pozycje kafla
			cell_shape.setPosition(static_cast<float>(CELL_SIZE * a), static_cast<float>(CELL_SIZE * b));

			//Jesli kafelek odkryty
			if (1 == get_cell(a, b, cells)->get_is_open())
			{
				//Zgarniamy ile min dookola
				unsigned char mines_around = get_cell(a, b, cells)->get_mines_around();

				cell_shape.setFillColor(sf::Color(146, 182, 255));

				//Rysujemy szary kafelek
				i_window.draw(cell_shape);

				//Jak ma mine obok siebie
				if (0 < mines_around)
				{
					//Dodajemy numerek ile min
					icon_sprite.setPosition(static_cast<float>(CELL_SIZE * a), static_cast<float>(CELL_SIZE * b));
					icon_sprite.setTextureRect(sf::IntRect(CELL_SIZE * mines_around, 0, CELL_SIZE, CELL_SIZE));

					i_window.draw(icon_sprite);
				}
			}
			else //Jak kafelek nieodkryty
			{
				cell_shape.setFillColor(sf::Color(0, 73, 255));

				//Jesli gra nieprzegrana nieskonczona to zmieniamy kolor
				if (0 == game_over)
				{
					if (1 == get_cell(a, b, cells)->get_mouse_state())
					{//najechanie
						cell_shape.setFillColor(sf::Color(36, 109, 255));
					}
					else if (2 == get_cell(a, b, cells)->get_mouse_state())
					{//nacisniecie
						cell_shape.setFillColor(sf::Color(0, 36, 255));
					}
				}

				i_window.draw(cell_shape);

				//jesli oflagowany kafelek
				if (1 == get_cell(a, b, cells)->get_is_flagged())
				{
					//dodajemy flage
					icon_sprite.setPosition(static_cast<float>(CELL_SIZE * a), static_cast<float>(CELL_SIZE * b));
					icon_sprite.setTextureRect(sf::IntRect(0, 0, CELL_SIZE, CELL_SIZE));

					i_window.draw(icon_sprite);
				}
			}

			//Reset mychy
			get_cell(a, b, cells)->set_mouse_state(0);

			//Tutaj ta cala funkcja co robi animacje koncowa
			if (0 != game_over && EFFECT_DURATION > get_cell(a, b, cells)->get_effect_timer())
			{
				//Obliczamy rozmiar efektu
				unsigned char effect_size = static_cast<unsigned char>(2 * round(0.5f * CELL_SIZE * ((EFFECT_DURATION - get_cell(a, b, cells)->get_effect_timer()) / static_cast<float>(EFFECT_DURATION))));

				//Losujemy czas trwania animacji
				std::uniform_int_distribution<unsigned short> effect_duration_distribution(1, EFFECT_DURATION - 1);

				//wykorzystujemy kafelek do animacji
				cell_shape.setPosition(floor(CELL_SIZE * (0.5f + a) - 0.5f * effect_size), floor(CELL_SIZE * (0.5f + b) - 0.5f * effect_size));
				cell_shape.setSize(sf::Vector2f(effect_size, effect_size));

				//Kolor animacji w zaleznosci czy wygrana czy przegrana
				if (-1 == game_over)
				{
					cell_shape.setFillColor(sf::Color(255, 36, 0));
				}
				else
				{
					cell_shape.setFillColor(sf::Color(255, 255, 255));
				}

				//rysujemy efekt
				i_window.draw(cell_shape);

				//Reset rozmiaru kafelka
				cell_shape.setSize(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));

				//Jesli dkonczyl sie czas animacji
				if (1 == get_cell(a, b, cells)->update_effect_timer())
				{
					//Lecimy kolejne kafelki
					if (0 <= a - 1 && EFFECT_DURATION == get_cell(a - 1, b, cells)->get_effect_timer())
					{
						get_cell(a - 1, b, cells)->set_effect_timer(static_cast<unsigned char>(effect_duration_distribution(random_engine)));
					}

					if (0 <= b - 1 && EFFECT_DURATION == get_cell(a, b - 1, cells)->get_effect_timer())
					{
						get_cell(a, b - 1, cells)->set_effect_timer(static_cast<unsigned char>(effect_duration_distribution(random_engine)));
					}

					if (COLUMNS > 1 + a && EFFECT_DURATION == get_cell(1 + a, b, cells)->get_effect_timer())
					{
						get_cell(1 + a, b, cells)->set_effect_timer(static_cast<unsigned char>(effect_duration_distribution(random_engine)));
					}

					if (ROWS > 1 + b && EFFECT_DURATION == get_cell(a, 1 + b, cells)->get_effect_timer())
					{
						get_cell(a, 1 + b, cells)->set_effect_timer(static_cast<unsigned char>(effect_duration_distribution(random_engine)));
					}
				}
			}
		}
	}
}

void Field::flag_cell(unsigned char i_x, unsigned char i_y)
{
	//Blokada flagowania jak gra sie skonczy
	if (0 == game_over)
	{
		get_cell(i_x, i_y, cells)->flag();
	}
}

void Field::open_cell(unsigned char i_x, unsigned char i_y)
{
	//Handling pierwszego kafelka
	if (0 == first_click)
	{
		
		std::uniform_int_distribution<unsigned short> x_distribution(0, COLUMNS - 1);
		std::uniform_int_distribution<unsigned short> y_distribution(0, ROWS - 1);

		first_click = 1;

		//Generacja min
		for (unsigned short a = 0; a < MINES; a++)
		{
			unsigned char mine_x = static_cast<unsigned char>(x_distribution(random_engine));
			unsigned char mine_y = static_cast<unsigned char>(y_distribution(random_engine));

			//Jesli gracz otwiera pole a jest tam mina to zmiana pola
			if (1 == get_cell(mine_x, mine_y, cells)->get_is_mine() || (i_x == mine_x && i_y == mine_y))
			{
				//We try again
				a--;
			}
			else
			{
				//Ustawiamy mine
				get_cell(mine_x, mine_y, cells)->set_mine();
			}
		}

		//Kafelki zliczaja ile maja bomb obok siebie
		for (Cell& cell : cells)
		{
			cell.count_mines_around(cells);
		}
	}

	//Nie mozna otworzyc komorki kiedy gra sie skonczyla albo ma flage
	if (0 == game_over && 0 == get_cell(i_x, i_y, cells)->get_is_flagged())
	{
		if (1 == get_cell(i_x, i_y, cells)->open(cells))
		{
			//jesli natrafi na mine to gracz przegrywa
			game_over = -1;
		}
		else
		{
			unsigned short total_closed_cells = 0;

			//Zliczamy ilosc zamknietych kafelkow
			for (Cell& cell : cells)
			{
				total_closed_cells += 1 - cell.get_is_open();
			}

			
			if (MINES == total_closed_cells)
			{
				//Wygrana
				game_over = 1;

				//Zaczynamy animacje
				get_cell(i_x, i_y, cells)->set_effect_timer(EFFECT_DURATION - 1);
			}
		}
	}
}

void Field::restart()
{
	//Restart gry (tylko jak sie skonczy)
	if (0 != game_over)
	{
		first_click = 0;

		game_over = 0;

		for (Cell& cell : cells)
		{
			cell.reset();
		}
	}
}

//ustawienie pozycji myszki 
void Field::set_mouse_state(unsigned char i_mouse_state, unsigned char i_x, unsigned char i_y)
{
	get_cell(i_x, i_y, cells)->set_mouse_state(i_mouse_state);
}