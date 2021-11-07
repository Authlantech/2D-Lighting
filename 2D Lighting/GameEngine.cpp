#include "GameEngine.h"

#define DrawRay(fx,fy,lx,ly) ray = new sf::Vertex[2];ray[0].position = sf::Vector2f(fx,fy);ray[0].color = sf::Color::Red;ray[1].color = sf::Color::Red;ray[1].position = sf::Vector2f(lx,ly);window->draw(ray, 2, sf::Lines);delete[2] ray;ray = nullptr;

void drawRay(sf::RenderWindow* window, sf::Vector2f source, sf::Vector2f target,sf::Color color)
{
	sf::Vertex* ray;
	ray = new sf::Vertex[2]; 
	ray[0].position = source; 
	ray[0].color = color; 
	ray[1].color = color;
	ray[1].position = target; 
	window->draw(ray, 2, sf::Lines);
	delete[2] ray;
	ray = nullptr;
}

#define left 1
#define right 2
#define up 3 
#define down 4 
#define absolute(a) fabsf(a)
#define FixBugValue 20

#define SLOPE 0.01

LightSource::LightSource
(
	float x, float y,
	int rad,float mass_,sf::Color lightcolor_
)
{
	setPosition(x, y);
	setRadius(rad);
	setFillColor(sf::Color::Red);
	lightcolor = lightcolor_;	
}

void LightSource::SetLightColor(sf::Color color)
{
	lightcolor = color;
}

void LightSource::SetRange(float change)
{
	range += change;
}

void LightSource::MoveSource(sf::Vector2f movement)
{
	if(movable == false)
	{
		return; 
	}
	this->move(movement);
}

void LightSource::SetMovable()
{
	movable = true;
}

void LightSource::SetUnmovable()
{
	movable = false;
}

sf::Vector2f LightSource::giveMiddle()
{
	return sf::Vector2f(getPosition().x + getRadius(),getPosition().y + getRadius());
}

Obstacles::Obstacles(float x, float y, float widht, float height)
{
	setPosition(x, y); 
	setSize(sf::Vector2f(widht, height));
	corners[0].x = x; 
	corners[0].y = y;
	corners[1].x = x + widht;
	corners[1].y = y;
	corners[2].x = x;
	corners[2].y = y + height;
	corners[3].x = x + widht;
	corners[3].y = y + height;

	vEdges[0].x = getPosition().x;
	vEdges[0].y[0] = getPosition().y;
	vEdges[0].y[1] = getPosition().y + height;

	vEdges[1].x = getPosition().x + widht;
	vEdges[1].y[0] = getPosition().y;
	vEdges[1].y[1] = getPosition().y + height;

	hEdges[0].y = getPosition().y;
	hEdges[0].x[0] = getPosition().x;
	hEdges[0].x[1] = getPosition().x + widht;

	hEdges[1].y = getPosition().y + height;
	hEdges[1].x[0] = getPosition().x;
	hEdges[1].x[1] = getPosition().x + widht;
}

void Obstacles::MoveObstacle(sf::Vector2f movement)
{

	if (movable == false)
	{
		return;
	}

	setPosition(getPosition().x + movement.x, getPosition().y + movement.y); 
	vEdges[0].x += movement.x;
	vEdges[1].x += movement.x; 
	vEdges[0].y[0] += movement.y; 
	vEdges[0].y[1] += movement.y; 
	vEdges[1].y[0] += movement.y; 
	vEdges[1].y[1] += movement.y; 
	
	hEdges[0].y += movement.y; 
	hEdges[1].y += movement.y;
	hEdges[0].x[0] += movement.x;
	hEdges[0].x[1] += movement.x; 
	hEdges[1].x[0] += movement.x;
	hEdges[1].x[1] += movement.x;

	for (int a = 0; a < 4; a++)
	{
		corners[a].x += movement.x; 
		corners[a].y += movement.y;
	}
}

void GameEngine::SaveElementsCoordinates(string storage_location)
{
	ofstream file(storage_location);

	for (Obstacles* obstacle : obstacles)
	{
		file << "Obstacle : " << endl; 
		file << "x : " << obstacle->getPosition().x << endl; 
		file << "y : " << obstacle->getPosition().y << endl;
		file << "cx: " << obstacle->getSize().x << endl; 
		file << "cy: " << obstacle->getSize().y << endl << endl;
	}	
}

void GameEngine::OpenDeveloperMode()
{
	developerMode = !developerMode;			
}

void GameEngine::SetDeveloperMode(int mode)
{
	if (developerMode)
	{
		switch (mode)
		{
		case 1: 
			virtualRays_on = true; 
			cornerRays_on = true; 
			break; 
		case 2: 
			virtualRays_on = !virtualRays_on; 
			break; 
		case 3 : 
			cornerRays_on = !cornerRays_on; 
			break; 
		}
	}
}

GameEngine::GameEngine(sf::RenderWindow* w,string lightTexturePath)
{
	window = w;		
	upperWEdge.y = 0;
	upperWEdge.x[0] = 0; 
	upperWEdge.x[1] = window->getSize().x;
	lowerWEdge.y = window->getSize().y;
	lowerWEdge.x[0] = 0;
	lowerWEdge.x[1] = window->getSize().x;

	leftWEdge.x = 0; 
	leftWEdge.y[0] = 0;
	leftWEdge.y[1] = window->getSize().y; 
	
	rightWEdge.x = window->getSize().x; 
	rightWEdge.y[0] = 0;
	rightWEdge.y[1] = window->getSize().y;	

	sf::Image image; 
	image.loadFromFile(lightTexturePath);
	middle = sf::Vector2f(image.getSize().x / 2, image.getSize().y / 2);	
	light.loadFromFile(lightTexturePath);
	light.setSmooth(true);
	
}

void GameEngine::AddObstacle(Obstacles* obstacle)
{	
	obstacles.push_back(obstacle);	
	IntersectionpointConfigured = false;
}

void GameEngine::AddSource(LightSource* source)
{
	sources.push_back(source);
}

void GameEngine::AddBackground(Background* background)
{
	backgrounds.push_back(background);
}

void GameEngine::SetSourceLightColor(int source_index, sf::Color color)
{
	sources[source_index]->SetLightColor(color);
}

void GameEngine::SetSourceRange(int source_index,float change)
{
	sources[source_index]->SetRange(change);
}

void GameEngine::CreateNewBackground(sf::Vector2f position, sf::Vector2f size, string texture_path, Background::BackgroundTextureStyle textureStyle, sf::Vector2f RepeatingTextureSize)
{
	if (size.x <= 0 || size.y <= 0)
	{
		cout << "Failed to create background!" << endl;
		return;
	}
	Background* newBackground = new Background(position, size, texture_path, textureStyle, RepeatingTextureSize);
	backgrounds.push_back(newBackground);
}

void GameEngine::MoveCamera(sf::Vector2f movement)
{
	movement = -movement;
	for (Obstacles* currobstacle : obstacles)
	{
		currobstacle->MoveObstacle(movement); 
	}

	for (LightSource* currsource : sources)
	{
		currsource->MoveSource(movement); 
	}

	for (Background* currbackground : backgrounds)
	{
		currbackground->MoveBacground(movement);
	}

	IntersectionpointConfigured = false;

}


void GameEngine::DrawObstacleRay(LightSource* source, Obstacles* obstacle, int corner_to_exclude)
{
	for (int a = 0; a < 4; a++)
	{
		if (a == corner_to_exclude)
			continue;
		else if (obstacle->corners[a].x > window->getSize().x || obstacle->corners[a].x < 0 || obstacle->corners[a].y > window->getSize().y || obstacle->corners[a].y < 0)
			continue;
		else
		{			
			float appendx = 0;
			float appendy = 0;
			switch (a)
			{
			case 0:
				appendx = -0.1;
				appendy = -0.1;
				break;
			case 1:
				appendx = 0.1;
				appendy = -0.1;
				break;
			case 2:
				appendx = -0.1;
				appendy = 0.1;
				break;
			case 3:
				appendx = 0.1;
				appendy = 0.1;
			}

			sf::Vector2f target;			
			sf::Vector2f target1;
			switch (corner_to_exclude)
			{
			case 0: 
				if (a == 3)
				{
					target = CreateRay(source, sf::Vector2f(obstacle->corners[a]), false);					
				}
				else
				{
					target = CreateRay(source, sf::Vector2f(obstacle->corners[a].x + appendx, obstacle->corners[a].y + appendy), true);
					target1 = CreateRay(source, sf::Vector2f(obstacle->corners[a]), false);
				}
				break;
			case 1: 
				if (a == 2)
				{
					target = CreateRay(source, sf::Vector2f(obstacle->corners[a]), false);					
				}
				else
				{
					target = CreateRay(source, sf::Vector2f(obstacle->corners[a].x + appendx, obstacle->corners[a].y + appendy), true);
					target1 = CreateRay(source, sf::Vector2f(obstacle->corners[a]), false);
				}
				break;
			case 2: 
				if (a == 1)
				{
					target = CreateRay(source, sf::Vector2f(obstacle->corners[a]), false);
				}
				else
				{
					target = CreateRay(source, sf::Vector2f(obstacle->corners[a].x + appendx, obstacle->corners[a].y + appendy), true);
					target1 = CreateRay(source, sf::Vector2f(obstacle->corners[a]), false);
				}
				break;
			case 3: 
				if (a == 0)
				{
					target = CreateRay(source, sf::Vector2f(obstacle->corners[a]), false);
				}
				else
				{
					target = CreateRay(source, sf::Vector2f(obstacle->corners[a].x + appendx, obstacle->corners[a].y + appendy), true);
					target1 = CreateRay(source, sf::Vector2f(obstacle->corners[a]), false);
				}
				break;
			}						

			hittedPos.push_back(target);			

			if (target1.x != 0 && target1.y != 0 && target1 != target)
			{							
				hittedPos.push_back(target1);
			}

		}		
	}
}

void  GameEngine::DrawObstacleRay(LightSource* source, Obstacles* obstacle, int corner_to_exclude, int corner_to_exclude2)
{
	for (int a = 0; a < 4; a++)
	{
		if (a == corner_to_exclude || a == corner_to_exclude2)
			continue;
		else if (obstacle->corners[a].x > window->getSize().x || obstacle->corners[a].x < 0 || obstacle->corners[a].y > window->getSize().y || obstacle->corners[a].y < 0)
			continue;
		else
		{		
			float appendx = 0; 
			float appendy = 0;
			switch (a)
			{
			case 0: 
				appendx = -0.1;
				appendy = -0.1;
				break;
			case 1: 
				appendx = 0.1;
				appendy = -0.1;
				break;
			case 2: 
				appendx = -0.1;
				appendy = 0.1;
				break;
			case 3: 
				appendx = 0.1;
				appendy = 0.1;
			}
			sf::Vector2f target = CreateRay(source, sf::Vector2f(obstacle->corners[a].x + appendx, obstacle->corners[a].y + appendy),true);
			sf::Vector2f target1 = CreateRay(source, sf::Vector2f(obstacle->corners[a]), false);			

			hittedPos.push_back(target);
		

			if (target1 != target)
			{
				hittedPos.push_back(target1);
			}
		}			
	}
}

sf::Vector2f GameEngine::CreateRay(LightSource* sourceptr, sf::Vector2f target,bool strech)
{
	// line equation : y = mx

	sf::Vector2f source; 
	source.x = sourceptr->getPosition().x + sourceptr->getRadius(); 
	source.y = sourceptr->getPosition().y + sourceptr->getRadius();

	int vertical_direction = 0;
	int horizontal_direction = 0;

	if (source.x <= target.x)
		horizontal_direction = right;
	else if (source.x >= target.x)
		horizontal_direction = left;
	if (source.y <= target.y)
		vertical_direction = down;
	else if (source.y >= target.y)
		vertical_direction = up;


	const float xEdge = fabsf(target.x - source.x);
	const float yEdge = fabsf(target.y - source.y);
	const float m = yEdge / xEdge;	
	const float n = xEdge / yEdge;
								
	float targetXpos = -1;
	float targetYpos = -1;

	float vObsDistance = -1; 
	float hObsDistance = -1;

	if (horizontal_direction == left && vertical_direction == up)
	{

		//Calculating where will ray hit if it doesn't hit any obstacle : 

		if (strech)
		{						
			float xDis = source.x;
			float xDis1 = fabsf(source.x - target.x);
			float yDis1 = fabsf(source.y - target.y);
			float yDis = (yDis1 * xDis) / xDis1;			
			float yPos = source.y - yDis; 
			if (yPos < 0)
			{
				//Ray hitted horizontal edge : 
				yDis = source.y; 
				xDis1 = fabsf(source.x - target.x);
				yDis1 = fabsf(source.y - target.y); 
				xDis = (xDis1 * yDis) / yDis1;
				float xPos = source.x - xDis; 
				targetXpos = xPos; 
				targetYpos = 0; 
			}
			else if(yPos >= 0)
			{
				//Ray hitted vertical edge : 
				targetXpos = 0; 
				targetYpos = yPos;			
			}

		}

		else
		{
			targetXpos = target.x; 
			targetYpos = target.y;
		}
		
		//Deciding which vertical obstacles will be checked : 


		vector<verticalEdge*>verticals;

		for (int a = 0; a < obstacles.size(); a++)
		{
			for (int b = 0; b < 2; b++)
			{
				if (
					obstacles[a]->vEdges[b].x < source.x &&
					obstacles[a]->vEdges[b].x > targetXpos  
					)
				{
						verticals.push_back(&obstacles[a]->vEdges[b]);
				}
			}
		}

		

		verticalEdge* toLeft = new verticalEdge[verticals.size()];		
		for (int a = 0; a < verticals.size(); a++)
		{
			toLeft[a].x = -1; 			
		}

		for (int a = 0; a < verticals.size(); a++)
		{

			int priorityIndex = verticals.size() - 1;

			for (int b = 0; b < verticals.size(); b++)
			{
				if (a == b)
				{
					continue;
				}

				if (verticals[a]->x > verticals[b]->x)
				{
					priorityIndex--;
				}				
			}

			while (toLeft[priorityIndex].x != -1)
			{
				priorityIndex--;
			}

			toLeft[priorityIndex] = *verticals[a];

		}		

		vector<verticalEdge>fixedtoLeft;
		for (int a = 0; a < verticals.size(); a++)
		{
			if (toLeft[a].x != -1)
			{
				fixedtoLeft.push_back(toLeft[a]);
			}
		}

		//Deciding which horizontal obstacles will be checked : 
		
		vector<horizontalEdge*>horizontals;	

		for (int a = 0; a < obstacles.size(); a++)
		{
			for (int b = 0; b < 2; b++)
			{
				if (
					obstacles[a]->hEdges[b].y < source.y &&
					obstacles[a]->hEdges[b].y > targetYpos &&
					obstacles[a]->hEdges[b].y <= window->getSize().y && 
					obstacles[a]->hEdges[b].y >= 0
					)
				{
					horizontals.push_back(&obstacles[a]->hEdges[b]);
				}
			}
		}

		horizontalEdge* toUp = new horizontalEdge[horizontals.size()];

		for (int a = 0; a < horizontals.size(); a++)
		{
			toUp[a].y = -1;
		}

		for (int a = 0; a < horizontals.size(); a++)
		{

			int priorityIndex = horizontals.size() - 1;

			for (int b = 0; b < horizontals.size(); b++)
			{
				if (a == b)
				{
					continue;
				}

				if (horizontals[a]->y > horizontals[b]->y)
				{
					priorityIndex--;
				}			
				
			}

			while (toUp[priorityIndex].y != -1)
			{
				priorityIndex--;
			}

			toUp[priorityIndex] = *horizontals[a];

		}

		vector<horizontalEdge>fixedtoUp; 

		for (int a = 0; a < horizontals.size(); a++)
		{
			if (toUp[a].y != -1)
			{
				fixedtoUp.push_back(toUp[a]);
			}
		}

		//Checking if ray hits any vertical obstacle : 

		for (int b = 0; b < fixedtoLeft.size(); b++)
		{
			float hDis = fabsf(source.x - fixedtoLeft[b].x);
			float vDis = fabsf(m * hDis);
			float yPos = source.y - vDis;
			if (yPos > fixedtoLeft[b].y[0] && yPos < fixedtoLeft[b].y[1])
			{
				targetXpos = fixedtoLeft[b].x;
				targetYpos = yPos;
				vObsDistance = sqrtf(powf(hDis, 2) + powf(vDis, 2));
				break;
			}
		}

		delete[verticals.size()] toLeft;
		toLeft = nullptr;				

		//Checking if ray hits any horizontal obstacle : 

		for (int b = 0; b < fixedtoUp.size(); b++)
		{
			float vDis = fabsf(source.y - fixedtoUp[b].y); 
			float hDis = fabsf(vDis * n);
			float xPos = source.x - hDis;
			if (xPos > fixedtoUp[b].x[0] && xPos < fixedtoUp[b].x[1])
			{					
				if (vObsDistance != -1)
				{
					//Deciding which edge did ray hit first : 

					hObsDistance = sqrtf(powf(vDis, 2) + powf(hDis, 2));					

					//Ray hitted horizontal edge first situation : 

					if (hObsDistance < vObsDistance)
					{
						targetXpos = xPos;
						targetYpos = fixedtoUp[b].y;	
						delete[horizontals.size()] toUp;
						toUp = nullptr;
						return sf::Vector2f(targetXpos, targetYpos);
					}

					//Ray hitted vertical edge first situation

					else
					{						
						delete[horizontals.size()] toUp;
						toUp = nullptr;
						return sf::Vector2f(targetXpos, targetYpos);
					}
				}
				else
				{
					targetXpos = xPos;
					targetYpos = fixedtoUp[b].y;	
					delete[horizontals.size()] toUp;
					toUp = nullptr;
					return sf::Vector2f(targetXpos, targetYpos);					 
				}
			}
		}

		delete[horizontals.size()] toUp;
		toUp = nullptr;

	}

	else if (horizontal_direction == left && vertical_direction == down)
	{

		//Calculating where will ray hit if it doesn't hit any obstacle : 

		if (strech)
		{
			float xDis1 = fabsf(source.x - target.x); 
			float yDis1 = fabsf(source.y - target.y); 
			float xDis = source.x; 
			float yDis = (xDis * yDis1) / xDis1;
			float yPos = source.y + yDis;
			if (yPos > window->getSize().y)
			{
				//Hitted horizontal edge first : 
				yDis1 = fabsf(source.y - target.y); 
				yDis = fabsf(source.y - window->getSize().y); 
				xDis1 = fabsf(source.x - target.x); 
				xDis = (xDis1 * yDis) / yDis1;
				float xPos = source.x - xDis;
				targetXpos = xPos;
				targetYpos = window->getSize().y;

			}

			else if (yPos <= window->getSize().y)
			{
				//Hitted vertical edge first
				targetXpos = 0;
				targetYpos = yPos;
			}
		}
		
		else
		{
			targetXpos = target.x; 
			targetYpos = target.y;
		}

		//Deciding which vertical obstacles will be checked : 

		vector<verticalEdge*>verticals;

		for (int a = 0; a < obstacles.size(); a++)
		{
			for (int b = 0; b < 2; b++)
			{
				if (
					obstacles[a]->vEdges[b].x < source.x &&
					obstacles[a]->vEdges[b].x > targetXpos &&
					obstacles[a]->vEdges[b].x <= window->getSize().x &&
					obstacles[a]->vEdges[b].x >= 0
					)
				{
					verticals.push_back(&obstacles[a]->vEdges[b]);
				}
			}
		}

		verticalEdge* toLeft = new verticalEdge[verticals.size()];

		for (int a = 0; a < verticals.size(); a++)
		{
			toLeft[a].x = -1;
		}

		for (int a = 0; a < verticals.size(); a++)
		{

			int priorityIndex = verticals.size() - 1;

			for (int b = 0; b < verticals.size(); b++)
			{
				if (a == b)
				{
					continue;
				}

				if (verticals[a]->x > verticals[b]->x)
				{
					priorityIndex--;
				}
			}

			while (toLeft[priorityIndex].x != -1)
			{
				priorityIndex--;
			}

			toLeft[priorityIndex] = *verticals[a];

		}

		vector<verticalEdge*>fixedtoLeft;

		for (int a = 0; a < verticals.size(); a++)
		{
			if (toLeft[a].x != -1)
			{
				fixedtoLeft.push_back(&toLeft[a]);
			}
		}

		//Deciding which horizontal obstacles will be checked :

		vector<horizontalEdge*>horizontals; 

		for (int a = 0; a < obstacles.size(); a++)
		{
			for (int b = 0; b < 2; b++)
			{
				if (
					obstacles[a]->hEdges[b].y < targetYpos &&
					obstacles[a]->hEdges[b].y > source.y &&
					obstacles[a]->hEdges[b].y <= window->getSize().y &&
					obstacles[a]->hEdges[b].y >= 0
					)
				{
					horizontals.push_back(&obstacles[a]->hEdges[b]);
				}
			}
		}

		horizontalEdge* toDown = new horizontalEdge[horizontals.size()];

		for (int a = 0; a < horizontals.size(); a++)
		{
			toDown[a].y = -1;
		}

		for (int a = 0; a < horizontals.size(); a++)
		{

			int priorityIndex = horizontals.size() - 1;

			for (int b = 0; b < horizontals.size(); b++)
			{
				if (a == b)
				{
					continue;
				}

				if (horizontals[a]->y < horizontals[b]->y)
				{
					priorityIndex--;
				}
			}

			while (toDown[priorityIndex].y != -1)
			{
				priorityIndex--;
			}

			toDown[priorityIndex] = *horizontals[a];

		}

		vector<horizontalEdge*>fixedtoDown;

		for (int a = 0; a < horizontals.size(); a++)
		{
			if (toDown[a].y != -1)
			{
				fixedtoDown.push_back(&toDown[a]);
			}
		}

		//Checking if ray hits any vertical obstacle : 

		for (int b = 0; b < fixedtoLeft.size(); b++)
		{
			float hDis = fabsf(source.x - fixedtoLeft[b]->x);
			float vDis = fabsf(m * hDis);
			float yPos = source.y + vDis;
			if (yPos > fixedtoLeft[b]->y[0] && yPos < fixedtoLeft[b]->y[1])
			{
				targetXpos = fixedtoLeft[b]->x;
				targetYpos = yPos;				
				vObsDistance = sqrtf(powf(hDis, 2) + powf(vDis, 2)); 
				break;
			}
		}

		delete[verticals.size()] toLeft;		
		toLeft = nullptr;

		//Checking if ray hits any horizontal obstacle : 

		for (int b = 0; b < fixedtoDown.size(); b++)
		{
			float vDis = fabsf(source.y - fixedtoDown[b]->y);
			float hDis = fabsf(n * vDis);
			float xPos = source.x - hDis;
			if (xPos > fixedtoDown[b]->x[0] && xPos < fixedtoDown[b]->x[1])
			{
				if (vObsDistance != -1)
				{
					//Deciding which edge did ray hit first : 

					hObsDistance = sqrtf(powf(vDis, 2) + powf(hDis, 2));

					//Ray hitted horizontal edge first situation : 

					if (vObsDistance > hObsDistance)
					{
						targetXpos = xPos;
						targetYpos = fixedtoDown[b]->y;
						delete[horizontals.size()] toDown;
						toDown = nullptr;
						return sf::Vector2f(targetXpos, targetYpos);
					}

					//Ray hitted vertical edge first situation : 

					else
					{
						delete[horizontals.size()] toDown;
						toDown = nullptr;
						return sf::Vector2f(targetXpos, targetYpos);
					}

				}
				else
				{
					targetXpos = xPos;
					targetYpos = fixedtoDown[b]->y;
					delete[horizontals.size()] toDown;
					toDown = nullptr;
					return sf::Vector2f(targetXpos, targetYpos);
				}
			}
		}

		delete[horizontals.size()] toDown;
		toDown = nullptr;

	}

	else if (horizontal_direction == right && vertical_direction == up)
	{


		//Calculating where will ray hit if it doesn't hit any obstacle : 

		if (strech)
		{
			double vDis = fabs((double)target.y - 0);
			double hDis = fabs(n * vDis); 
			float xPos = (double)target.x + hDis;
			if (xPos > window->getSize().x)
			{
				hDis = fabsf(target.x - window->getSize().x);
				vDis = fabsf(m * hDis);
				float yPos = (double)target.y - vDis;
				targetXpos = window->getSize().x; 
				targetYpos = yPos;
			}
			else
			{
				targetXpos = xPos;
				targetYpos = 0;
			}
		}
		else
		{
			targetXpos = target.x; 
			targetYpos = target.y;
		}

		//Deciding which vertical obstacles will be checked : 	

		vector<verticalEdge*>verticals;

		for (int a = 0; a < obstacles.size(); a++)
		{
			for (int b = 0; b < 2; b++)
			{
				if (
					obstacles[a]->vEdges[b].x > source.x &&
					obstacles[a]->vEdges[b].x < targetXpos &&
					obstacles[a]->vEdges[b].x <= window->getSize().x &&
					obstacles[a]->vEdges[b].x >= 0
					)
				{
					verticals.push_back(&obstacles[a]->vEdges[b]);
				}
			}
		}

		verticalEdge* toRight = new verticalEdge[verticals.size()];

		for (int a = 0; a < verticals.size(); a++)
		{
			toRight[a].x = -1;
		}

		for (int a = 0; a < verticals.size(); a++)
		{

			int priorityIndex = verticals.size() - 1;

			for (int b = 0; b < verticals.size(); b++)
			{
				if (a == b)
				{
					continue;
				}

				if (verticals[a]->x < verticals[b]->x)
				{
					priorityIndex--;
				}
			}

			while (toRight[priorityIndex].x != -1)
			{
				priorityIndex--;
			}

			toRight[priorityIndex] = *verticals[a];

		}

		vector<verticalEdge*>fixedtoRight;

		for (int a = 0; a < verticals.size(); a++)
		{
			if (toRight[a].x != -1)
			{
				fixedtoRight.push_back(&toRight[a]);
			}
		}

		//Deciding which horizontal obstacles will be checked : 

		vector<horizontalEdge*>horizontals;

		for (int a = 0; a < obstacles.size(); a++)
		{
			for (int b = 0; b < 2; b++)
			{
				if (
					obstacles[a]->hEdges[b].y < source.y &&
					obstacles[a]->hEdges[b].y > targetYpos &&
					obstacles[a]->hEdges[b].y <= window->getSize().y &&
					obstacles[a]->hEdges[b].y >= 0
					)
				{
					horizontals.push_back(&obstacles[a]->hEdges[b]);
				}
			}
		}

		horizontalEdge* toUp = new horizontalEdge[horizontals.size()];

		for (int a = 0; a < horizontals.size(); a++)
		{
			toUp[a].y = -1;
		}

		for (int a = 0; a < horizontals.size(); a++)
		{

			int priorityIndex = horizontals.size() - 1;

			for (int b = 0; b < horizontals.size(); b++)
			{
				if (a == b)
				{
					continue;
				}

				if (horizontals[a]->y > horizontals[b]->y)
				{
					priorityIndex--;
				}
			}

			while (toUp[priorityIndex].y != -1)
			{
				priorityIndex--;
			}

			toUp[priorityIndex] = *horizontals[a];

		}

		vector<horizontalEdge*>fixedtoUp;

		for (int a = 0; a < horizontals.size(); a++)
		{
			if (toUp[a].y != -1)
			{
				fixedtoUp.push_back(&toUp[a]);
			}
		}

		//Checking if ray hits any vertical obstacle : 

		for (int b = 0; b < fixedtoRight.size(); b++)
		{
			float vDis = fabsf(source.x - fixedtoRight[b]->x);
			float hDis = fabsf(m * vDis);
			float yPos = source.y - hDis;
			if (yPos > fixedtoRight[b]->y[0] && yPos < fixedtoRight[b]->y[1])
			{
				targetXpos = fixedtoRight[b]->x;
				targetYpos = yPos;
				vObsDistance = sqrtf(powf(hDis, 2) + powf(vDis, 2));
				break;
			}
		}

		delete[verticals.size()] toRight;
		toRight = nullptr;		

		//Checking if ray hits any horizontal obstacle : 

		for (int b = 0; b < fixedtoUp.size(); b++)
		{
			float vDis = fabsf(source.y - fixedtoUp[b]->y);
			float hDis = fabsf(n * vDis); 
			float xPos = source.x + hDis;
			if (xPos > fixedtoUp[b]->x[0] && xPos < fixedtoUp[b]->x[1])
			{				
				if (vObsDistance != -1)
				{

					//Deciding which edge did ray hitted first : 

					hObsDistance = sqrtf(powf(hDis, 2) + powf(vDis, 2));

					//Hitted horizontal edge first situation : 

					if (vObsDistance > hObsDistance)
					{
						targetXpos = xPos;
						targetYpos = fixedtoUp[b]->y;
						delete[horizontals.size()] toUp;
						toUp = nullptr;
						return sf::Vector2f(targetXpos, targetYpos);
					}

					//Hitted vertical edge first situation : 

					else
					{
						delete[horizontals.size()] toUp;
						toUp = nullptr;
						return sf::Vector2f(targetXpos, targetYpos);
					}
				}
				else
				{
					targetXpos = xPos;
					targetYpos = fixedtoUp[b]->y;
					delete[horizontals.size()] toUp;
					toUp = nullptr;
					return sf::Vector2f(targetXpos, targetYpos);
				}
			}
		}

		delete[horizontals.size()] toUp;
		toUp = nullptr;

	}

	else if (horizontal_direction == right && vertical_direction == down)
	{
	
		//Calculating where will ray hit if it doesn't hit any obstacle : 

		if (strech)
		{
			double vDis = fabsf((double)target.y - (double)window->getSize().y);
			double hDis = fabsf(n * vDis); 
			float xPos = (double)target.x + hDis;
			if (xPos > window->getSize().x)
			{
				hDis = fabsf(target.x - window->getSize().x);
				vDis = fabsf(m * hDis); 
				float yPos = (double)target.y + vDis;
				targetXpos = window->getSize().x; 
				targetYpos = yPos;
			}
			else
			{
				targetXpos = xPos;
				targetYpos = window->getSize().y;
			}
		}
		else
		{
			targetXpos = target.x; 
			targetYpos = target.y;
		}


		//Deciding which vertical obstacles will be checked : 	
		vector<verticalEdge*>verticals;
		for (int a = 0; a < obstacles.size(); a++)
		{
			for (int b = 0; b < 2; b++)
			{
				if (
					obstacles[a]->vEdges[b].x > source.x &&
					obstacles[a]->vEdges[b].x < targetXpos &&
					obstacles[a]->vEdges[b].x <= window->getSize().x &&
					obstacles[a]->vEdges[b].x >= 0
					)
				{
					verticals.push_back(&obstacles[a]->vEdges[b]);
				}
			}
		}

		verticalEdge* toRight = new verticalEdge[verticals.size()];

		for (int a = 0; a < verticals.size(); a++)
		{
			toRight[a].x = -1;
		}

		for (int a = 0; a < verticals.size(); a++)
		{

			int priorityIndex = verticals.size() - 1;

			for (int b = 0; b < verticals.size(); b++)
			{
				if (a == b)
				{
					continue;
				}

				if (verticals[a]->x < verticals[b]->x)
				{
					priorityIndex--;
				}			
			}

			while (toRight[priorityIndex].x != -1)
			{
				priorityIndex--;
			}
			
			toRight[priorityIndex] = *verticals[a];

		}

		vector<verticalEdge*>fixedtoRight;

		for (int a = 0; a < verticals.size(); a++)
		{
			if (toRight[a].x != -1)
			{
				fixedtoRight.push_back(&toRight[a]);
			}
		}

		//Deciding which horizontal obstacles will be checked :

		vector<horizontalEdge*>horizontals;

		for (int a = 0; a < obstacles.size(); a++)
		{
			for (int b = 0; b < 2; b++)
			{
				if (
					obstacles[a]->hEdges[b].y < targetYpos &&
					obstacles[a]->hEdges[b].y > source.y &&
					obstacles[a]->hEdges[b].y <= window->getSize().y &&
					obstacles[a]->hEdges[b].y >= 0
					)
				{
					horizontals.push_back(&obstacles[a]->hEdges[b]);
				}
			}
		}

		horizontalEdge* toDown = new horizontalEdge[horizontals.size()];

		for (int a = 0; a < horizontals.size(); a++)
		{
			toDown[a].y = -1;
		}

		for (int a = 0; a < horizontals.size(); a++)
		{

			int priorityIndex = horizontals.size() - 1;

			for (int b = 0; b < horizontals.size(); b++)
			{
				if (a == b)
				{
					continue;
				}

				if (horizontals[a]->y < horizontals[b]->y)
				{
					priorityIndex--;
				}
			}

			while (toDown[priorityIndex].y != -1)
			{
				priorityIndex--;
			}

			toDown[priorityIndex] = *horizontals[a];			
			
		}

		vector<horizontalEdge*>fixedtoDown;

		for (int a = 0; a < horizontals.size(); a++)
		{
			if (toDown[a].y != -1)
			{
				fixedtoDown.push_back(&toDown[a]);
			}
		}

		//Checking if ray hits any vertical obstacle : 

		for (int b = 0; b < fixedtoRight.size(); b++)
		{		
			float hDis = fabsf(source.x - fixedtoRight[b]->x);
			float vDis = fabsf(m * hDis);
			float yPos = source.y + vDis;
			if (yPos > fixedtoRight[b]->y[0] && yPos < fixedtoRight[b]->y[1])
			{
				targetXpos = fixedtoRight[b]->x;
				targetYpos = yPos;
				vObsDistance = sqrtf(powf(hDis, 2) + powf(vDis, 2));
				break;
			}
		}

		delete[verticals.size()] toRight;
		toRight = nullptr;

		//Checking if ray hits any horizontal obstacle : 

		for (int b = 0; b < fixedtoDown.size(); b++)
		{
			float vDis = absolute(source.y - fixedtoDown[b]->y);
			float hDis = absolute(n * vDis); 
			float xPos = source.x + hDis; 
			if (xPos > fixedtoDown[b]->x[0] && xPos < fixedtoDown[b]->x[1])
			{	

				//Deciding which edge did ray hitted first : 

				hObsDistance = sqrtf(powf(hDis, 2) + powf(vDis, 2));
				if (vObsDistance != -1)
				{					
					//Ray hitted horizontal edge first situation : 

					if (vObsDistance > hObsDistance)
					{
						targetXpos = xPos;
						targetYpos = fixedtoDown[b]->y;
						delete[horizontals.size()] toDown;
						toDown = nullptr;
						return sf::Vector2f(targetXpos, targetYpos);
					}

					//Ray hitted vertical edge first situation : 

					else
					{
						delete[horizontals.size()] toDown;
						toDown = nullptr;
						return sf::Vector2f(targetXpos, targetYpos);
					}
				}
				else
				{
					targetXpos = xPos; 
					targetYpos = fixedtoDown[b]->y;
					delete[horizontals.size()] toDown;
					toDown = nullptr;
					return sf::Vector2f(targetXpos, targetYpos);
				}
			}
		}

		delete[horizontals.size()] toDown;
		toDown = nullptr;
	
	}

	return sf::Vector2f(targetXpos, targetYpos);
}

sf::Vector2f GameEngine::CreateRay(LightSource* sourceptr, float slope,int sector)
{

	sf::Vector2f source; 
	source.x = sourceptr->getPosition().x + sourceptr->getRadius(); 
	source.y = sourceptr->getPosition().y + sourceptr->getRadius();

	float vDis[2], hDis[2], xPos[2], yPos[2],RayDis[2], targetXpos, targetYpos;
	switch (sector)
	{
	case 1:
		//Hitted vertical edge : 
		hDis[0] = source.x;
		vDis[0] = hDis[0] * slope;
		xPos[0] = 0;
		yPos[0] = source.y - vDis[0];
		RayDis[0] = sqrtf(powf(vDis[0], 2) + powf(hDis[0], 2));
		//Hitted horizontal edge
		vDis[1] = source.y;
		hDis[1] = vDis[1] / slope;
		xPos[1] = source.x - hDis[1];
		yPos[1] = 0;
		RayDis[1] = sqrtf(powf(vDis[1], 2) + powf(hDis[1], 2));

		if (RayDis[0] < RayDis[1])
		{
			targetXpos = xPos[0];
			targetYpos = yPos[0];
		}

		else if (RayDis[0] > RayDis[1])
		{
			targetXpos = xPos[1];
			targetYpos = yPos[1];
		}

		else
		{
			targetXpos = 0;
			targetYpos = 0;
		}
		break;
	case 2:
		//Hitted vertical edge : 
		hDis[0] = window->getSize().x - source.x;
		vDis[0] = slope * hDis[0];
		xPos[0] = window->getSize().x;
		yPos[0] = source.y - vDis[0];
		RayDis[0] = sqrtf(powf(vDis[0], 2) + powf(hDis[0], 2));
		//Hitted horizontal edge : 
		vDis[1] = source.y;
		hDis[1] = vDis[1] / slope;
		xPos[1] = source.x + hDis[1];
		yPos[1] = 0;
		RayDis[1] = sqrtf(powf(vDis[1], 2) + powf(hDis[1], 2));

		if (RayDis[0] < RayDis[1])
		{
			targetXpos = xPos[0];
			targetYpos = yPos[0];
		}

		else if (RayDis[0] > RayDis[1])
		{
			targetXpos = xPos[1];
			targetYpos = yPos[1];
		}

		else
		{
			targetXpos = window->getSize().x;
			targetYpos = 0;
		}
		break;
	case 3:
		//Hitted vertical edge : 
		hDis[0] = window->getSize().x - source.x;
		vDis[0] = slope * hDis[0];
		xPos[0] = window->getSize().x;
		yPos[0] = source.y + vDis[0];
		RayDis[0] = sqrtf(powf(vDis[0], 2) + powf(hDis[0], 2));
		//Hitted horizontal edge : 
		vDis[1] = window->getSize().y - source.y;
		hDis[1] = vDis[1] / slope;
		xPos[1] = source.x + hDis[1];
		yPos[1] = window->getSize().y;
		RayDis[1] = sqrtf(powf(vDis[1], 2) + powf(hDis[1], 2));

		if (RayDis[0] < RayDis[1])
		{
			targetXpos = xPos[0];
			targetYpos = yPos[0];
		}

		else if (RayDis[0] > RayDis[1])
		{
			targetXpos = xPos[1];
			targetYpos = yPos[1];
		}

		else
		{
			targetXpos = window->getSize().x ;
			targetYpos = window->getSize().y;
		}
		break;
	case 4 : 
		//Hitted vertical edge : 
		hDis[0] = source.x;
		vDis[0] = slope * hDis[0];
		xPos[0] = 0;
		yPos[0] = source.y + vDis[0];
		RayDis[0] = sqrtf(powf(vDis[0], 2) + powf(hDis[0], 2));
		//Hitted horizontal edge : 
		vDis[1] = window->getSize().y - source.y;
		hDis[1] = vDis[1] / slope;
		xPos[1] = source.x - hDis[1];
		yPos[1] = window->getSize().y;
		RayDis[1] = sqrtf(powf(vDis[1], 2) + powf(hDis[1], 2));

		if (RayDis[0] < RayDis[1])
		{
			targetXpos = xPos[0];
			targetYpos = yPos[0];
		}

		else if (RayDis[0] > RayDis[1])
		{
			targetXpos = xPos[1];
			targetYpos = yPos[1];
		}

		else
		{
			targetXpos = 0;
			targetYpos = window->getSize().y;
		}
		break;
	}

	sf::Vector2f Result = CreateRay(sourceptr, sf::Vector2f(targetXpos, targetYpos), true);

	return Result;

}

void GameEngine::SetIntersectionPositions()
{
	IntersectionPositions.clear();
	vector<verticalEdge*>verticalEdges;
	vector<horizontalEdge*>horizontalEdges; 

	verticalEdges.push_back(&leftWEdge);
	verticalEdges.push_back(&rightWEdge);

	horizontalEdges.push_back(&upperWEdge);
	horizontalEdges.push_back(&lowerWEdge);


	for (Obstacles* obstacle : obstacles)
	{
		verticalEdges.push_back(&obstacle->vEdges[0]);
		verticalEdges.push_back(&obstacle->vEdges[1]);

		horizontalEdges.push_back(&obstacle->hEdges[0]);
		horizontalEdges.push_back(&obstacle->hEdges[1]);
	}

	for (verticalEdge* vertical : verticalEdges)
	{
		for (horizontalEdge* horizontal : horizontalEdges)
		{
			
			if (
				vertical->x > horizontal->x[0] && vertical->x < horizontal->x[1] &&
				horizontal->y > vertical->y[0] && horizontal->y < vertical->y[1]
				)
			{
				IntersectionPositions.push_back(sf::Vector2f(vertical->x, horizontal->y));
			}
		}
	}

	IntersectionpointConfigured = true;	

}


void GameEngine::CreateVisibleArea(LightSource*source)
{

	//Finding hittesPos coordinates that are on same column :

	vector<int>checkedIndexes; 
	vector<vector<sf::Vector2f>>columns;
	for (int a = 0; a < hittedPos.size(); a++)
	{

		if (std::find(checkedIndexes.begin(), checkedIndexes.end(), a) != checkedIndexes.end())
		{
			continue;
		}

		vector<sf::Vector2f>column; 
		sf::Vector2f* columninorder;
		for (int b = 0; b < hittedPos.size(); b++)
		{
			if (a == b)
			{
				continue;
			}

			else if (std::find(checkedIndexes.begin(), checkedIndexes.end(), b) != checkedIndexes.end())
			{
				continue;
			}

			if (hittedPos[a].x == hittedPos[b].x)
			{
				column.push_back(hittedPos[b]);
				checkedIndexes.push_back(b);
			}
		}

		if (column.size() > 0)
		{
			column.push_back(hittedPos[a]);
			checkedIndexes.push_back(a);
			columns.push_back(column);
		}

		else
		{
			checkedIndexes.push_back(a);
			continue;
		}			

	}

	//Putting columns elements in order and drawing the visible areas : 

	for (int a = 0; a < columns.size(); a++)
	{

		//Putting current column elements in order : 

		sf::Vector2f* currcolumn = new sf::Vector2f[columns[a].size()];

		for (int b = 0; b < columns[a].size(); b++)
		{
			currcolumn[b].x = -1;
			currcolumn[b].y = -1;
		}

		float currcolmApsis = columns[a][0].x;
		
		for (int b = 0; b < columns[a].size(); b++)
		{
			int priorityIndex = columns[a].size() - 1;
			for (int c = 0; c < columns[a].size(); c++)
			{
				if (b == c)
				{
					continue;
				}

				if (columns[a][b].y > columns[a][c].y)
				{
					priorityIndex--;
				}				
			
			}

			currcolumn[priorityIndex] = columns[a][b];			
		
		}

		vector<sf::Vector2f> currcolumninOrder;

		for (int b = 0; b < columns[a].size(); b++)
		{
			if (currcolumn[b].x == -1 && currcolumn[b].y == -1)
			{
				continue;
			}
			else
			{
				currcolumninOrder.push_back(currcolumn[b]);
			}
		}

		//Drawing the visible area : 

		for (int c = 0; c < currcolumninOrder.size() - 1; c++)
		{
			
			float targetY = currcolumninOrder[c].y - (absolute(currcolumninOrder[c].y - currcolumninOrder[c + 1].y) / 2);
			float hittedX = CreateRay(source, sf::Vector2f(currcolmApsis, targetY), true).x;
			if (hittedX == currcolmApsis && !developerMode)
			{				
				sf::Vertex triangle[3];
				triangle[0].position = sf::Vector2f(source->getPosition().x + source->getRadius(), source->getPosition().y + source->getRadius());
				triangle[1].position = currcolumninOrder[c];
				triangle[2].position = currcolumninOrder[c + 1]; 

				triangle[0].color = source->lightcolor;
				triangle[1].color = source->lightcolor;
				triangle[2].color = source->lightcolor;

				triangle[0].texCoords = middle;
				triangle[1].texCoords = sf::Vector2f(middle.x + (currcolumninOrder[c].x - (source->getPosition().x + source->getRadius())) * source->range, middle.y + (currcolumninOrder[c].y - (source->getPosition().y + source->getRadius())) * source->range);
				triangle[2].texCoords = sf::Vector2f(middle.x + (currcolumninOrder[c + 1].x - (source->getPosition().x + source->getRadius())) * source->range, middle.y + (currcolumninOrder[c + 1].y - (source->getPosition().y + source->getRadius())) * source->range);
				
				sf::RenderStates states;
				states.texture = &light;
				window->draw(triangle, 3, sf::Triangles, states);
				
			}
			if (developerMode && virtualRays_on)
			{
				drawRay(window, sf::Vector2f(source->getPosition().x + source->getRadius(),source->getPosition().y + source->getRadius()), sf::Vector2f(currcolmApsis, targetY), sf::Color::Green);
			}			
		}

		delete[columns[a].size()] currcolumn;

	}

	//finding hittespos coordinates that are on same line : 

	checkedIndexes.clear();
	vector<vector<sf::Vector2f>>lines; 


	for (int a = 0; a < hittedPos.size(); a++)
	{

		if (std::find(checkedIndexes.begin(), checkedIndexes.end(), a) != checkedIndexes.end())
		{
			continue;
		}

		vector<sf::Vector2f>line;
		for (int b = 0; b < hittedPos.size(); b++)
		{
			if (a == b)
			{
				continue;
			}

			if (std::find(checkedIndexes.begin(), checkedIndexes.end(), b) != checkedIndexes.end())
			{
				continue;
			}

			if (hittedPos[a].y == hittedPos[b].y)
			{
				line.push_back(hittedPos[b]);
				checkedIndexes.push_back(b);
			}
		}

		if (line.size() > 0)
		{
			line.push_back(hittedPos[a]); 
			checkedIndexes.push_back(a);
			lines.push_back(line);
		}

		else
		{
			checkedIndexes.push_back(a); 
			continue;
		}

	}

	for (int a = 0; a < lines.size(); a++)
	{

		//Putting lines[a] elements in order : 

		sf::Vector2f* currline = new sf::Vector2f[lines[a].size()];

		for (int b = 0; b < lines[a].size(); b++)
		{
			currline[b].x = -1; 
			currline[b].y = -1;
		}

		for (int b = 0; b < lines[a].size(); b++)
		{

			int priorityIndex = lines[a].size() - 1;
			for (int c = 0; c < lines[a].size(); c++)
			{
				if (b == c)
				{
					continue;
				}

				if (lines[a][b].x > lines[a][c].x)
				{
					priorityIndex--;
				}

			}

			currline[priorityIndex] = lines[a][b];
		
			
		}

		vector<sf::Vector2f>currlineinOrder; 

		for (int b = 0; b < lines[a].size(); b++)
		{
			if (currline[b].x == -1 && currline[b].y == -1)
			{
				continue;
			}
			else
			{
				currlineinOrder.push_back(currline[b]);
			}
		}

		//Drawing visible area : 

		float currLineOrdinat = lines[a][0].y;

		for (int b = 0; b < currlineinOrder.size() - 1; b++)
		{			
			float targetX = currlineinOrder[b].x - (absolute(currlineinOrder[b].x - currlineinOrder[b + 1].x) / 2);
			float hittedY = CreateRay(source, sf::Vector2f(targetX, currLineOrdinat),true).y;
			if (hittedY == currLineOrdinat && !developerMode)
			{				
				sf::Vertex triangle[3]; 
				triangle[0].position = sf::Vector2f(source->getPosition().x + source->getRadius(), source->getPosition().y + source->getRadius());
				triangle[1].position = currlineinOrder[b];
				triangle[2].position = currlineinOrder[b + 1];	

				triangle[0].color = source->lightcolor;
				triangle[1].color = source->lightcolor;
				triangle[2].color = source->lightcolor;

				triangle[0].texCoords = middle;
				triangle[1].texCoords = sf::Vector2f(middle.x + (currlineinOrder[b].x - (source->getPosition().x + source->getRadius())) * source->range, middle.y + (currlineinOrder[b].y - (source->getPosition().y + source->getRadius())) * source->range);
				triangle[2].texCoords = sf::Vector2f(middle.x + (currlineinOrder[b + 1].x - (source->getPosition().x + source->getRadius())) * source->range, middle.y + (currlineinOrder[b + 1].y - (source->getPosition().y + source->getRadius())) * source->range);				
				sf::RenderStates states; 
				states.texture = &light; 
				window->draw(triangle, 3, sf::Triangles,states);
			}

			if (developerMode && virtualRays_on)
			{
				drawRay(window, sf::Vector2f(source->getPosition().x + source->getRadius(), source->getPosition().y + source->getRadius()), sf::Vector2f(targetX, currLineOrdinat), sf::Color::Green);
			}

		}
		delete[lines[a].size()] currline; 		

	}

}


void GameEngine::Proccess()
{
	Render();
}

void GameEngine::Render()
{
	//Calculating lights : 	

	for (int a = 0; a < sources.size(); a++)
	{

		hittedPos.clear();

		window->draw(*sources[a]);

		const float sourceX = sources[a]->getPosition().x + sources[a]->getRadius();
		const float sourceY = sources[a]->getPosition().y + sources[a]->getRadius();

		for (int b = 0; b < obstacles.size(); b++)
		{		
			sources[a]->pos = 0;			

			//Calculating the source's x position compared to obstacle : 

			if (sourceX > obstacles[b]->corners[1].x)
			{
				sources[a]->pos += LightSource::RIGHT;				
			}
			else if (sourceX < obstacles[b]->corners[2].x)
			{
				sources[a]->pos += LightSource::LEFT;
			}
			else
			{
				sources[a]->pos += LightSource::X_IN_MIDDLE;
			}

			//Calculating the source's y position compared to obstacle : 

			if (sourceY > obstacles[b]->corners[2].y)
			{
				sources[a]->pos += LightSource::DOWN;
			}
			else if (sourceY < obstacles[b]->corners[1].y)
			{
				sources[a]->pos += LightSource::UP;
			}
			else
			{
				sources[a]->pos += LightSource::Y_IN_MIDDLE;
			}

			//Drawing lines acording to sources position :
			 
			switch (sources[a]->pos)
			{
			case LightSource::RIGHT_UP: 
				DrawObstacleRay(sources[a], obstacles[b], 2);
				break; 
			case LightSource::RIGHT_DOWN : 
				DrawObstacleRay(sources[a], obstacles[b], 0);
				break;
			case LightSource::LEFT_UP : 
				DrawObstacleRay(sources[a], obstacles[b], 3);
				break;
			case LightSource::LEFT_DOWN : 
				DrawObstacleRay(sources[a], obstacles[b], 1);
				break;		
			case LightSource::X_IN_MIDDLE_UP : 
				DrawObstacleRay(sources[a], obstacles[b],2,3);
				break; 
			case LightSource::X_IN_MIDDLE_DOWN: 
				DrawObstacleRay(sources[a], obstacles[b], 0,1);
				break; 
			case LightSource::Y_IN_MIDDLE_RIGHT: 
				DrawObstacleRay(sources[a], obstacles[b],0,2);
				break;				
			case LightSource::Y_IN_MIDDLE_LEFT: 
				DrawObstacleRay(sources[a], obstacles[b], 1, 3);
				break; 
			}			

		}			

		if (!IntersectionpointConfigured)
		{
			SetIntersectionPositions();
		}

		for (sf::Vector2f intersectionposition : IntersectionPositions)
		{
			hittedPos.push_back(intersectionposition);
		}

		if (developerMode)
		{
			for (auto temp : hittedPos)
			{
				drawRay(window, sf::Vector2f(sourceX,sourceY), temp, sf::Color::Red);
			}
		}

		//Sending rays to corners of the window : 

		sf::Vector2f CornerLines[4] =
		{
			CreateRay(sources[a],sf::Vector2f(0,0),false),
			CreateRay(sources[a],sf::Vector2f(window->getSize().x,0),false),
			CreateRay(sources[a],sf::Vector2f(0,window->getSize().y),false),
			CreateRay(sources[a],sf::Vector2f(window->getSize().x,window->getSize().y),false),
		};

		for (sf::Vector2f temp : CornerLines)
		{
			hittedPos.push_back(temp);
		}
		
		if (cornerRays_on && developerMode)
		{
			for (auto temp : CornerLines)
			{
				drawRay(window, sf::Vector2f(sourceX, sourceY), temp, sf::Color::Blue);
			}
		}


		CreateVisibleArea(sources[a]);

	}

	for (Background* currbackground : backgrounds)
	{
		if (currbackground->singleColor == true)
		{
			window->draw(currbackground->single_color_rect, sf::BlendMultiply);
		}
		else
		{
			for (sf::RectangleShape currshape : currbackground->TexturePieces)
			{
				if (currshape.getPosition().x >= window->getSize().x || currshape.getPosition().y > window->getSize().y)
					continue;
				else if (currshape.getPosition().x + currshape.getSize().x <= 0 || currshape.getPosition().y + currshape.getSize().x <= 0)
					continue;
				window->draw(currshape, sf::RenderStates(sf::BlendMultiply));
			}
		}
	}


	for (int b = 0; b < obstacles.size(); b++)
	{
		window->draw(*obstacles[b]);
	}

}

Background::Background(sf::Vector2f position, sf::Vector2f size,string texture_path, BackgroundTextureStyle textureStyle, sf::Vector2f RepeatingTextureSize)
{
	sf::RectangleShape fullsizeBackground;

	switch (textureStyle)
	{
	case BackgroundTextureStyle::FullSize:
		backgroundTexture.loadFromFile(texture_path);
		fullsizeBackground.setTexture(&backgroundTexture);
		fullsizeBackground.setPosition(position);
		fullsizeBackground.setSize(size);
		TexturePieces.push_back(fullsizeBackground);
		break;
	case BackgroundTextureStyle::RepeatingTexture:
		backgroundTexture.loadFromFile(texture_path);
		for (int a = 0; a <= size.x; a += RepeatingTextureSize.x)
		{
			for (int b = 0; b <= size.y; b += RepeatingTextureSize.y)
			{
				sf::RectangleShape backgroundPiece; 
				backgroundPiece.setPosition(position.x + a, position.y + b);
				sf::Vector2f BackgroundPieceSize(RepeatingTextureSize);
				if (size.x - a < RepeatingTextureSize.x)
				{
					BackgroundPieceSize.x = size.x - a;
				}
				if (size.y - b < RepeatingTextureSize.y)
				{
					BackgroundPieceSize.y = size.y - b;
				}
				backgroundPiece.setSize(BackgroundPieceSize);
				backgroundPiece.setTexture(&backgroundTexture); 
				TexturePieces.push_back(backgroundPiece);
			}
		}
		break;
	}

}

Background::Background(sf::Vector2f position, sf::Vector2f size, sf::Color backgroundColor)
{
	singleColor = true; 	
	single_color_rect.setFillColor(backgroundColor);
	single_color_rect.setPosition(position); 
	single_color_rect.setSize(size);
}

void Background::MoveBacground(sf::Vector2f movement)
{

	if (movable == false)
	{
		return;
	}

	for (sf::RectangleShape& currpiece : TexturePieces)
	{
		currpiece.move(movement);
	}
}
