#pragma once

#include <cstdint>
#include <vector>
#include <array>

#include <SFML/Graphics.hpp>

class Quadtree final
{
	friend class Quad;

public:
	class Node
	{
	public:
		virtual ~Node() = default;

	public:
		virtual void Insert(const sf::Vector2f &point) = 0;
		virtual void Destroy() = 0;

		virtual void Render(sf::RenderWindow &window) = 0;
	};

	class Region final : public Node
	{
	public:
		void Insert(const sf::Vector2f &point) override
		{
			points.push_back(point);
		}

		void Destroy() override
		{
			points.clear();
		}

		void Render(sf::RenderWindow &window) override
		{
			static sf::CircleShape shape(2.0f);
			for (const sf::Vector2f &point : points)
			{
				shape.setPosition(point);
				window.draw(shape);
			}
		}

		std::size_t GetSize() const
		{
			return points.size();
		}

		std::vector<sf::Vector2f> GetData() const
		{
			return points;
		}

	private:
		std::vector<sf::Vector2f> points;
	};

	class Quad final : public Node
	{
	public:
		Quad(const sf::Vector2f &position, const sf::Vector2f &size, const Quadtree &quadtree, const std::uint32_t depth = 0)
			: position(position)
			, size(size)
			, quadtree(quadtree)
			, depth(depth)
		{
			nodes.fill(nullptr);
		}

	public:
		void Insert(const sf::Vector2f &point) override
		{
			sf::Vector2f quadPos;
			const std::uint32_t nodeIndex = this->GetNodeIndex(point, &quadPos);

			Node* &node = nodes[nodeIndex];

			if (!node)
			{
				node = new Region();
			}

			if (Region *region = dynamic_cast<Region*>(node))
			{
				if (region->GetSize() >= quadtree.maxObjectsPerRegion && depth + 1 < quadtree.maxDepth)
				{
					node = new Quad(quadPos, size / 2.0f, quadtree, depth + 1);
					for (const sf::Vector2f &point : region->GetData())
					{
						node->Insert(point);
					}

					delete region;
				}
			}

			node->Insert(point);
		}

		void Destroy() override
		{
			for (Node* &node : nodes)
			{
				if (node)
				{
					node->Destroy();
					delete node;
					node = nullptr;
				}
			}
		}

		std::vector<sf::Vector2f> GetNeighbors(const sf::Vector2f &point) const
		{
			const std::uint32_t nodeIndex = this->GetNodeIndex(point);

			if (Region *region = dynamic_cast<Region*>(nodes[nodeIndex]))
			{
				return region->GetData();
			}
			else if (Quad *quad = dynamic_cast<Quad*>(nodes[nodeIndex]))
			{
				return quad->GetNeighbors(point);
			}
			else
			{
				return std::vector<sf::Vector2f>();
			}
		}

		void Render(sf::RenderWindow &window) override
		{
			const sf::Vector2f center = position + size / 2.0f;
			sf::RectangleShape shape;
			shape.setSize(size / 2.0f);
			shape.setFillColor(sf::Color::Transparent);
			shape.setOutlineColor(sf::Color::White);
			shape.setOutlineThickness(1.0f);

			// top-left
			shape.setPosition(position);
			window.draw(shape);

			// top-right
			shape.setPosition(sf::Vector2f(center.x, position.y));
			window.draw(shape);

			// bottom-left
			shape.setPosition(sf::Vector2f(position.x, center.y));
			window.draw(shape);

			// bottom-right
			shape.setPosition(center);
			window.draw(shape);

			for (Node *node : nodes)
			{
				if (node)
				{
					node->Render(window);
				}
			}
		}

	private:
		std::size_t GetNodeIndex(const sf::Vector2f &point, sf::Vector2f *nodePos = nullptr) const
		{
			std::uint32_t nodeIndex;
			const sf::Vector2f center = position + size / 2.0f;

			if (point.x <= center.x && point.y <= center.y) // top-left
			{
				nodeIndex = 0;
				
				if (nodePos)
				{
					*nodePos = position;
				}
			}
			else if (point.x > center.x && point.y <= center.y) // top-right
			{
				nodeIndex = 1;

				if (nodePos)
				{
					*nodePos = sf::Vector2f(center.x, position.y);
				}
			}
			else if (point.x <= center.x && point.y > center.y) // bottom-left
			{
				nodeIndex = 2;

				if (nodePos)
				{
					*nodePos = sf::Vector2f(position.x, center.y);
				}
			}
			else if (point.x > center.x && point.y > center.y) // bottom-right
			{
				nodeIndex = 3;

				if (nodePos)
				{
					*nodePos = center;
				}
			}

			return nodeIndex;
		}

	private:
		std::array<Node*, 4> nodes;
		const sf::Vector2f position, size;
		const Quadtree &quadtree;
		std::uint32_t depth = 0;
	};

public:
	Quadtree(const sf::Vector2f &size, const std::uint32_t maxObjectsPerRegion = 4, const std::uint32_t maxDepth = 64)
		: size(size)
		, maxObjectsPerRegion(maxObjectsPerRegion)
		, maxDepth(maxDepth)
		, root(new Quad(sf::Vector2f(), size, *this))
	{
	}

public:
	void Insert(const sf::Vector2f &point)
	{
		if (sf::FloatRect(sf::Vector2f(), size).contains(point))
		{
			root->Insert(point);
		}
	}

	void Clear()
	{
		root->Destroy();
	}

	std::vector<sf::Vector2f> GetNeighbors(const sf::Vector2f &point) const
	{
		if (sf::FloatRect(sf::Vector2f(), size).contains(point))
		{
			return root->GetNeighbors(point);
		}
		else
		{
			return std::vector<sf::Vector2f>();
		}
	}

	void Render(sf::RenderWindow &window)
	{
		root->Render(window);
	}

private:
	const sf::Vector2f size;
	const std::uint32_t maxObjectsPerRegion, maxDepth;
	Quad *root;
};