#include <vector>

#include "Headers/Cell.hpp"
#include "Headers/GetCell.hpp"
#include "Headers/Global.hpp"

Cell::Cell(unsigned char i_x, unsigned char i_y) :
	mines_around(0),
	x(i_x),
	y(i_y)
{
	//Reset komórki
	reset();
}

bool Cell::get_is_flagged()
{
	//Czy oflagowana
	return is_flagged;
}

bool Cell::get_is_mine()
{
	//Czy ma mine
	return is_mine;
}

bool Cell::get_is_open()
{
	//Czy odkryta
	return is_open;
}

bool Cell::open(std::vector<Cell>& i_cells)
{
	//jesli nieotwarta
	if (0 == is_open)
	{
		is_open = 1;

		//jak nie ma miny ona i dookola to wszystkie otwieramy
		if (0 == is_mine && 0 == mines_around)
		{
			for (char a = -1; a < 2; a++)
			{
				for (char b = -1; b < 2; b++)
				{
					//Sprawdzenie czy nie wychodzimy poza obszar gry
					if ((0 == a && 0 == b) || (0 > a + x || 0 > b + y || COLUMNS == a + x || ROWS == b + y))
					{
						continue;
					}

					//otwieramy sasiednie komorki
					get_cell(a + x, b + y, i_cells)->open(i_cells);
				}
			}
		}

		//Rozpoczecie animacji jesli ma mine
		effect_timer -= is_mine;

		//Zwracamy czy ma mine
		return is_mine;
	}

	return 0;
}

bool Cell::update_effect_timer()
{
	//Rozpoczynamy animacje
	if (0 < effect_timer)
	{
		//Dekrement
		effect_timer--;

		
		if (0 == effect_timer)
		{
			//koniec animacji
			return 1;
		}
	}

	
	return 0;
}

unsigned char Cell::get_effect_timer()
{
	
	return effect_timer;
}

unsigned char Cell::get_mines_around()
{
	
	return mines_around;
}

unsigned char Cell::get_mouse_state()
{
	
	return mouse_state;
}

void Cell::count_mines_around(std::vector<Cell>& i_cells)
{
	
	mines_around = 0;

	
	if (0 == is_mine)
	{
		for (char a = -1; a < 2; a++)
		{
			for (char b = -1; b < 2; b++)
			{
				if ((0 == a && 0 == b) || (0 > a + x || 0 > b + y || COLUMNS == a + x || ROWS == b + y))
				{
					continue;
				}

				
				if (1 == get_cell(a + x, b + y, i_cells)->get_is_mine())
				{
					
					 mines_around++; 
					
				}
			}
		}
	}
}

void Cell::flag()
{
	//jesli nieotwarte
	if (0 == is_open)
	{
		//oflaguj
		is_flagged = 1 - is_flagged;
	}
}

void Cell::reset()
{
	
	is_flagged = 0;
	is_mine = 0;
	is_open = 0;

	
	effect_timer = EFFECT_DURATION;
	mouse_state = 0;
}


void Cell::set_effect_timer(unsigned char i_effect_timer)
{
	effect_timer = i_effect_timer;
}

void Cell::set_mine()
{
	is_mine = 1;
}

void Cell::set_mouse_state(unsigned char i_mouse_state)
{
	mouse_state = i_mouse_state;
}