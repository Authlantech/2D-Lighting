
/*
* Created by Emirhan Kotan 
*/

#pragma once 
#include <SFML/Graphics.hpp>
#include <iostream> 
using namespace std; 
#include <string> 
#include <vector>
#include <thread>
#include <math.h>
#include <fstream> 

class LightSource;
class Obstacles;
class Background; 
class Map; 
class Camera;


struct verticalEdge
{
	int x;
	int y[2];
};

struct horizontalEdge
{
	int y;
	int x[2];
};

struct column
{
	float x; 
	vector<sf::Vector2f>Spots;
};

struct line
{
	float y; 
	vector<sf::Vector2f>Spots;
};

class Background
{
protected:
	sf::Texture backgroundTexture;
	vector<sf::RectangleShape> TexturePieces;
	bool movable = true;

	bool singleColor = false;
	sf::RectangleShape single_color_rect;
public:

	enum BackgroundTextureStyle
	{
		FullSize,
		RepeatingTexture,
	};

	Background(sf::Vector2f position, sf::Vector2f size, string texture_path, BackgroundTextureStyle textureStyle = FullSize, sf::Vector2f RepeatingTextureSize = sf::Vector2f(50, 50));
	Background(sf::Vector2f position, sf::Vector2f size, sf::Color backgroundColor);
	void MoveBacground(sf::Vector2f movement);
	friend class GameEngine;
};


class GameEngine
{
protected : 

	//GAME LIGHTS : 

	sf::RenderWindow* window;
	//Elements : 
	vector<Obstacles*>obstacles;
	vector<LightSource*>sources;	
	vector<Background*>backgrounds;

	//Light texture : 
	sf::Texture light;
	//Middle point of the texture : 
	sf::Vector2f middle;

	//Vector that holds the positions that all rays hitted 
	vector<sf::Vector2f>hittedPos;

	//Positions that vertical and horizontal edges cross : 
	vector<sf::Vector2f>IntersectionPositions;
	bool IntersectionpointConfigured = false;
	//Window edges : 

	verticalEdge leftWEdge; 
	verticalEdge rightWEdge; 
	horizontalEdge upperWEdge;
	horizontalEdge lowerWEdge;

	//Developer Tools : 
	
	bool developerMode = false;
	bool virtualRays_on = false; 
	bool cornerRays_on = false; 

public : 

	//Engine settings : 

	void SaveElementsCoordinates(string storage_location);
	void OpenDeveloperMode();
	void SetDeveloperMode(int mode);

	//Element processes :

	GameEngine(sf::RenderWindow* w,string lightTexturepath);
	void AddObstacle(Obstacles* obstacle);
	void AddSource(LightSource* source);
	void AddBackground(Background* background);
		
	//Source functions : 

	void SetSourceLightColor(int source_index, sf::Color color);
	void SetSourceRange(int source_index,float change);

	//Background functions : 

	void CreateNewBackground(sf::Vector2f position, sf::Vector2f size, string texture_path, Background::BackgroundTextureStyle textureStyle = Background::BackgroundTextureStyle::FullSize, sf::Vector2f RepeatingTextureSize = sf::Vector2f(50, 50));

	//Map properties : 

	void MoveCamera(sf::Vector2f movement);

	//Ray processing functions : 
private : 
	void DrawObstacleRay(LightSource* source, Obstacles* obstacle,int corner_to_exclude);
	void DrawObstacleRay(LightSource* source, Obstacles* obstacle, int corner_to_exclude,int corner_to_exclude2);
	sf::Vector2f CreateRay(LightSource* sourceptr, sf::Vector2f target,bool strech);
	sf::Vector2f CreateRay(LightSource* sourceptr, float angle,int sector);
	void SetIntersectionPositions();	
	void CreateVisibleArea(LightSource*source);
	void Render();

public : 
	void Proccess();
};


class LightSource : public sf::CircleShape
{
protected : 
	//Lightining properties : 

	const static int RIGHT = 1;
	const static int LEFT = 9;
	const static int UP = 19;
	const static int DOWN = 29;
	const static int X_IN_MIDDLE = 49;
	const static int Y_IN_MIDDLE = 79;

	const static int RIGHT_UP = 20;
	const static int RIGHT_DOWN = 30;
	const static int LEFT_UP = 28;
	const static int LEFT_DOWN = 38;
	const static int X_IN_MIDDLE_UP = X_IN_MIDDLE + UP;
	const static int X_IN_MIDDLE_DOWN = X_IN_MIDDLE + DOWN; 
	const static int Y_IN_MIDDLE_RIGHT = Y_IN_MIDDLE + RIGHT; 
	const static int Y_IN_MIDDLE_LEFT = Y_IN_MIDDLE + LEFT;
	int pos = 0;

	sf::Color lightcolor;
	float range = 1;	
	bool movable = true;

public : 
	LightSource(float x, float y,int rad,float mass,sf::Color lightcolor = sf::Color::White);
	void SetLightColor(sf::Color color);
	void SetRange(float change);
	void MoveSource(sf::Vector2f movement);	
	sf::Vector2f giveMiddle();
	void SetMovable();
	void SetUnmovable();
	friend class GameEngine;
};

class Obstacles : public sf::RectangleShape
{
protected :
	sf::Vector2f corners[4];
	verticalEdge vEdges[2];
	horizontalEdge hEdges[2];
	void MoveObstacle(sf::Vector2f movement);
	bool movable = true;
public : 
	Obstacles(float x, float y,float widht,float height);
	friend class GameEngine;
};
 
class Map
{
protected : 
	vector<Obstacles*>obstacles; 
	vector<LightSource*>sources;
	vector<Background*>backgrounds;
	vector<Camera*>camera;

	Camera* ActiveCamera; 

	int stored_identifier;
	sf::Vector2f mapsize;

public : 
	friend class GameEngine;
};

class Camera
{
protected: 
	sf::Vector2f position;	
public : 
	friend class GameEngine;
	friend class Map;
};