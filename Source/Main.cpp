#include <algorithm>
#include <ctime>
#include <vector>
#include <string>

#include <SFML/Graphics.hpp>

#include "Quadtree.hpp"

struct Spaceship final
{
	sf::Vector2f position, velocity;
};

int main()
{
	std::srand(std::time(nullptr));

	std::vector<Spaceship> spaceships(500'000);
	std::generate(spaceships.begin(), spaceships.end(), []()
	{
		Spaceship spaceship;
		spaceship.position = sf::Vector2f(640 + std::rand() % 11 - 5, 360 + std::rand() % 11 - 5);
		spaceship.velocity = sf::Vector2f(std::rand() % 101 - 50, std::rand() % 101 - 50);
		return spaceship;
	});

	const std::string windowTitle("Quadtree - Using C++ & SFML2.5.1");
	sf::RenderWindow window(sf::VideoMode(1280u, 720u), "Quadtree - Using C++ & SFML2.5.1");
	sf::Clock clock;

	Quadtree quadtree(sf::Vector2f(1280.0f, 720.0f), 1024);

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

		std::vector<sf::Vector2f> neighbors = quadtree.GetNeighbors(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)));

		// Render
		window.clear();

		// render the quadtree
		quadtree.Render(window);

		// render the neighbors
		sf::CircleShape shape(2.0f);
		shape.setFillColor(sf::Color::Red);
		for (const sf::Vector2f &neighbor : neighbors)
		{
			shape.setPosition(neighbor);
			window.draw(shape);
		}

		window.display();
	}

	return 0;
}