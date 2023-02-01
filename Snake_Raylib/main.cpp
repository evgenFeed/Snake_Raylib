#include "raylib.h"
#include <vector>

int ScreenWidth = 800;
int ScreenHeigth = 600;

enum Direction
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

Vector2 GetRandomV2();

class Node
{
public:
	Node(Vector2 pos, Direction direction, Color color) :
		m_pos{ pos },
		m_direction{ direction },
		m_rect(),
		m_color(color)
	{
		m_rect.x = m_pos.x;
		m_rect.y = m_pos.y;
		m_rect.width = node_width;
		m_rect.height = node_height;
	}
	void draw() const
	{
		DrawRectangleRec(m_rect, m_color);
	}
	Direction getDirection() const
	{
		return m_direction;
	}
	void setDirection(Direction direction)
	{
		m_direction = direction;
	}
	void move()
	{
		if (m_direction == Direction::UP)
		{
			m_pos.y -= step;
		}
		else if (m_direction == Direction::DOWN)
		{
			m_pos.y += step;
		}
		else if (m_direction == Direction::LEFT)
		{
			m_pos.x -= step;
		}
		else if (m_direction == Direction::RIGHT)
		{
			m_pos.x += step;
		}
		updatePos();
	}

	Rectangle getRect() const
	{
		return m_rect;
	}

	static const int node_width = 15;
	static const int node_height = 15;
	static const int step = 15;
private:
	Direction m_direction;
	Vector2 m_pos;
	Rectangle m_rect;
	Color m_color;
	void updatePos()
	{
		m_rect.x = m_pos.x;
		m_rect.y = m_pos.y;
	}
};

class Snake
{
public:
	Snake() : nodes{}
	{
		nodes.push_back(Node{ GetRandomV2(), Direction::UP, YELLOW});
	}
	void draw() const
	{
		for(const auto& node : nodes)
		{
			node.draw();
		}
	}
	void eat() 
	{
		Node lastNode = nodes[nodes.size()-1];
		Vector2 nodePos = Vector2();
		Direction lastNodeDirection = nodes.back().getDirection();
		if (lastNodeDirection == UP)
		{
			nodePos.x = lastNode.getRect().x;
			nodePos.y = lastNode.getRect().y + Node::node_height;
		}
		else if (lastNodeDirection == DOWN)
		{
			nodePos.x = lastNode.getRect().x;
			nodePos.y = lastNode.getRect().y - Node::node_height;
		}
		else if (lastNodeDirection == LEFT)
		{
			nodePos.x = lastNode.getRect().x + Node::node_width;
			nodePos.y = lastNode.getRect().y;
		}
		else if (lastNodeDirection == RIGHT)
		{
			nodePos.x = lastNode.getRect().x - Node::node_width;
			nodePos.y = lastNode.getRect().y;
		}
		Node newNode = Node(nodePos, lastNodeDirection, WHITE);
		nodes.push_back(newNode);
	}
	void setDirection(Direction direction)
	{
		nodes[0].setDirection(direction);
	}
	void move() 
	{
		if (nodes.size() > 1)
		{
			Direction fPrev;
			Direction sPrev;
			for (size_t i = 0; i < nodes.size(); i++)
			{
				if (i == 0)
				{
					nodes[i].move();
					fPrev = nodes[i].getDirection();
				}
				else
				{
					sPrev = nodes[i].getDirection();
					nodes[i].move();
					nodes[i].setDirection(fPrev);
					fPrev = sPrev;
				}
			}
		}
		else
		{
			nodes[0].move();
		}
	}
	Node getHead() const
	{
		return nodes[0];
	}

private:
	std::vector<Node> nodes;
};

class Player
{
public:
	Player(Snake* snake) : m_direction{Direction::UP}
	{
		m_controlled_snake = snake;
	};

	void controlInput()
	{
		if (IsKeyPressed(87) && m_direction != DOWN)
		{
			m_direction = UP;
		}
		if (IsKeyPressed(83) && m_direction != UP)
		{
			m_direction = DOWN;
		}
		if (IsKeyPressed(65) && m_direction != RIGHT)
		{
			m_direction = LEFT;
		}
		if (IsKeyPressed(68) && m_direction != LEFT)
		{
			m_direction = RIGHT;
		}
		m_controlled_snake->setDirection(m_direction);
	}
private:
	Snake* m_controlled_snake;
	Direction m_direction;
};

Vector2 GetRandomV2()
{
	int rndX = GetRandomValue(0 + Node::node_width, ScreenWidth - Node::node_width);
	int rndY = GetRandomValue(0 + Node::node_height, ScreenHeigth - Node::node_height);
	Vector2 res = Vector2();
	res.x = (rndX / Node::node_width) * Node::node_width;
	res.y = (rndY / Node::node_height) * Node::node_height;
	return res;
}

int main()
{
	InitWindow(ScreenWidth, ScreenHeigth, "Snake");
	Vector2 rndApplePos = GetRandomV2();
	int score = 0;
	SetTargetFPS(16);//FLAG_VSYNC_HINT);
	Snake snake = Snake();
	Player player(&snake);
	std::vector<Rectangle> borders(4);
	
	
	//LEFT
	borders[0].x = -15;
	borders[0].y = 0;
	borders[0].width = 30;
	borders[0].height = ScreenHeigth;
	//RIGHT
	borders[1].x = ScreenWidth - 15;
	borders[1].y = 0;
	borders[1].width = 30;
	borders[1].height = ScreenHeigth;

	//TOP
	borders[2].x = 0;
	borders[2].y = -15;
	borders[2].width = ScreenWidth;
	borders[2].height = 30;
	//BOTTOM
	borders[3].x = 0;
	borders[3].y = ScreenHeigth - 15;
	borders[3].width = ScreenWidth;
	borders[3].height = 30;

	Node *apple = new Node(rndApplePos, UP, RED);
	while (!WindowShouldClose())
	{
		for (auto& b : borders)
		{
			DrawRectangleRec(b, WHITE);
		}
		BeginDrawing();
		ClearBackground(DARKGREEN);
		player.controlInput();
		snake.draw();
		apple->draw();
		
		snake.move();

		for (auto b : borders)
		{
			if (CheckCollisionRecs(snake.getHead().getRect(), b))
			{
				CloseWindow();
			}
		}
		
		if (CheckCollisionRecs(snake.getHead().getRect(), apple->getRect()))
		{
			score++;
			snake.eat();
			delete apple;
			rndApplePos = GetRandomV2();
			apple = new Node(rndApplePos, UP, RED);
		}

		DrawFPS(10,10);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
