// VisualNovelQuizGame.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>

int main()
{   
	sf::RenderWindow window(sf::VideoMode(800, 600), "Visual Novel Quiz Game");
	sf::CircleShape shape(50.f);
	shape.setFillColor(sf::Color::Green);
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		window.clear();
		window.draw(shape);
		window.display();
	}
    std::cout << "Hello World!\n";
}
