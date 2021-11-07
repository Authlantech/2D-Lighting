// 2D Raycasting.cpp: Uygulamanın giriş noktasını tanımlar.
//

#include "2D Lighting.h"

using namespace std;

int mouseX, mouseY;
int eMouseX, eMouseY;
sf::Vector2i mousePos;

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 800), "2D Lighting");
	sf::Event eventer;

	GameEngine engine(&window, "white_glow.png");
	LightSource Source(200, 200, 1, 0);
	engine.AddSource(&Source);
	window.setFramerateLimit(60);

	while (window.isOpen())
	{
		while (window.pollEvent(eventer))
		{
			switch (eventer.type)
			{
			case sf::Event::Closed: 
				window.close();
				break;
			case sf::Event::MouseMoved: 
				mousePos = sf::Mouse::getPosition(); 
				Source.setPosition(mousePos.x - 565,mousePos.y - 170);
				break;	

			case sf::Event::MouseButtonPressed: 
				if (eventer.mouseButton.button == sf::Mouse::Left)
				{
					mouseX = eventer.mouseButton.x;
					mouseY = eventer.mouseButton.y;
				}
				else if (eventer.mouseButton.button == sf::Mouse::Right)
				{					
					Obstacles* obs = new Obstacles(eventer.mouseButton.x, eventer.mouseButton.y, 100, 100); 
					engine.AddObstacle(obs);
					obs->setFillColor(sf::Color::Blue);
				}
				break;
			case sf::Event::MouseButtonReleased: 

				if (eventer.mouseButton.button == sf::Mouse::Left)
				{
					eMouseX = eventer.mouseButton.x;
					eMouseY = eventer.mouseButton.y;

					int apsises[2];
					int ordinats[2];
					if (mouseX < eMouseX)
					{
						apsises[0] = mouseX;
						apsises[1] = eMouseX;
					}
					else
					{
						apsises[0] = eMouseX;
						apsises[1] = mouseX;
					}
					if (mouseY < eMouseY)
					{
						ordinats[0] = mouseY;
						ordinats[1] = eMouseY;
					}
					else
					{
						ordinats[0] = eMouseY;
						ordinats[1] = mouseY;
					}

					Background* background = new Background(sf::Vector2f(apsises[0], ordinats[0]),sf::Vector2f(float(apsises[1] - apsises[0]), float(ordinats[1] - ordinats[0])), sf::Color::Red);
					engine.AddBackground(background);
				}
				else if (eventer.mouseButton.button == sf::Mouse::Right)
				{

				}
				break;
			case sf::Event::KeyPressed: 
				switch (eventer.key.code)
				{
				case sf::Keyboard::Up: 					
				
					break;
				case sf::Keyboard::Down: 
				
					break;
				}
				break;
			}
		}
		window.clear(sf::Color::Color(50,50,50));	
		engine.Proccess(); 		
		window.display();

	}
}
