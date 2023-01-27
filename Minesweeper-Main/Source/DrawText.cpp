#include <SFML/Graphics.hpp>

#include "Headers/DrawText.hpp"

void draw_text(bool i_black, unsigned short i_x, unsigned short i_y, const std::string& i_text, sf::RenderWindow& i_window)
{
	//Ustawienie pozycji tekstu
	short character_x = i_x;
	short character_y = i_y;

	unsigned char character_width;

	sf::Sprite character_sprite;

	sf::Texture font_texture;
	font_texture.loadFromFile("Resources/Images/Font.png");

	//Obliczamy szerokosc znaku jest ich 96
	character_width = font_texture.getSize().x / 96;

	character_sprite.setTexture(font_texture);

	//Zmiana koloru tekstu
	if (1 == i_black)
	{
		character_sprite.setColor(sf::Color(0, 0, 0));
	}

	for (const char a : i_text)
	{
		if ('\n' == a)
		{
			//Obnizenie tekstu
			character_x = i_x;
			character_y += font_texture.getSize().y;

			continue;
		}

		//Ustawienie nastepnej litery
		character_sprite.setPosition(character_x, character_y);
		//Wybor litery
		character_sprite.setTextureRect(sf::IntRect(character_width * (a - 32), 0, character_width, font_texture.getSize().y));

		//Przesuniecie do next litery
		character_x += character_width;

		//Rysowanie literki
		i_window.draw(character_sprite);
	}
}