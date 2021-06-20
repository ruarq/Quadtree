#include <algorithm>
#include <ctime>

#include <SFML/Graphics.hpp>

#include "Quadtree.hpp"

struct Spaceship final
{
	sf::Vector2f position, velocity;
};

int main()
{
	std::srand(std::time(nullptr));

	std::vector<Spaceship> spaceships(100);
	std::generate(spaceships.begin(), spaceships.end(), []()
	{
		Spaceship spaceship;
		spaceship.position = sf::Vector2f(std::rand() % 1280, std::rand() % 720);
		spaceship.velocity = sf::Vector2f(std::rand() % 201 - 100, std::rand() % 201 - 100);
		return spaceship;
	});

	sf::RenderWindow window(sf::VideoMode(1280u, 720u), "Quadtree - Using C++ & SFML2.5.1");
	sf::Clock clock;

	Quadtree quadtree(sf::Vector2f(1280.0f, 720.0f));

	while (window.isOpen())
	{
		// Handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
				case sf::Event::Closed:
					window.close();
					break;
				
				default:
					break;
			}
		}

		const float deltaTime = clock.restart().asSeconds();

		// re-generate quadtree
		quadtree.Clear();

		// update all spaceships & insert them into the quadtree
		for (Spaceship &spaceship : spaceships)
		{
			spaceship.position += spaceship.velocity * deltaTime;

			// wrap the position
			if (spaceship.position.x < 0.0f)
			{
				spaceship.position.x = 1280.0f;
			}
			if (spaceship.position.x > 1280.0f)
			{
				spaceship.position.x = 0.0f;
			}
			if (spaceship.position.y < 0.0f)
			{
				spaceship.position.y = 720.0f;
			}
			if (spaceship.position.y > 720.0f)
			{
				spaceship.position.y = 0.0f;
			}

			quadtree.Insert(spaceship.position);
		}

		// Render
		window.clear();

		quadtree.Render(window);

		window.display();
	}

	return 0;
}