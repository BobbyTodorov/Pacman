#include "Game.h"
#include "Engine/Components.h"
#include "Engine/RenderSystem.h"
#include "Engine/CollisionSystem.h"
#include "Engine/KeyboardController.h"
#include "Engine/TextureManager.h"
#include "Engine/UILabel.h"
#include "GhostController.h"
#include "PacmanController.h"
#include "Math.h"

#include <string>


int scoreForWinning = -1; // negative => the number of dots on map. Change this value to a positive int to 
						  // explicitly set the amount of score to be required for winning.

const unsigned COLLIDER_BOX_SIZE = 20; // these collider boxes are used for Pacman/Ghosts colliding logic

const unsigned Game::TILE_SIZE = 32; // Textures are 32x32p. Having higher tile size will decrease quality and might affect speeds.
const unsigned Game::HALF_TILE_SIZE = Game::TILE_SIZE / 2;
const unsigned Game::WINDOW_WIDTH = tilesCountX * Game::TILE_SIZE; // tilesCount is stored in the Map file
const unsigned Game::WINDOW_HEIGHT = tilesCountY * Game::TILE_SIZE;
const unsigned Game::GHOST_HOUSE_DOOR_TILE_X = 9;
const unsigned Game::GHOST_HOUSE_DOOR_TILE_Y = 6;
const unsigned Game::GHOST_HOUSE_DOOR_XPOS = Game::GHOST_HOUSE_DOOR_TILE_X * Game::TILE_SIZE;
const unsigned Game::GHOST_HOUSE_DOOR_YPOS = Game::GHOST_HOUSE_DOOR_TILE_Y * Game::TILE_SIZE;

const unsigned INKY_START_XPOS = 10 * Game::TILE_SIZE;
const unsigned INKY_START_YPOS = 9 * Game::TILE_SIZE;
const unsigned BLINKY_START_XPOS = 8 * Game::TILE_SIZE;
const unsigned BLINKY_START_YPOS = 9 * Game::TILE_SIZE;
const unsigned PINKY_START_XPOS = 9 * Game::TILE_SIZE;
const unsigned PINKY_START_YPOS = 8 * Game::TILE_SIZE;
const unsigned CLYDE_START_XPOS = 8 * Game::TILE_SIZE;
const unsigned CLYDE_START_YPOS = 8 * Game::TILE_SIZE;
const unsigned PACMAN_START_XPOS = 9 * Game::TILE_SIZE;
const unsigned PACMAN_START_YPOS = 12 * Game::TILE_SIZE;

const int ghostsSpeed = 2;
const int pacmanSpeed = 2;

SDL_Renderer* Game::renderer = nullptr;
Map* Game::map = nullptr;
SDL_Event Game::event;

TextureRenderer* scoreLabel;
TTF_Font *scoreLabelFont, *winLoseLabelFont;
SDL_Color white = { 255, 255, 255, 255 }; // for the labels' font color
unsigned Game::currentScore = 0;

TextureRenderer* winLoseLabel;
const std::string WIN_MESSAGE = "WIN";
const std::string LOSE_MESSAGE = "OPS";

PacmanController* pacman;
GhostController* inky;
GhostController* pinky;
GhostController* blinky;
GhostController* clyde;

bool Game::isRunning = false;
bool Game::isWinOrLoseConditionReached = false;

std::shared_ptr<RenderSystem> renderSystem = nullptr;
std::shared_ptr<CollisionSystem> collisionSystem = nullptr;


Game::Game()
{
	srand((unsigned)time(NULL));
}

Game::~Game()
{
	delete Game::renderer;
	delete Game::map;
}

// The functions down below are not members of the Game class
// in order not to overburden the header code and references. 
// However, if desired, there is no problem for them to be methods.
void RegisterComponents()
{
	engine.RegisterComponent<Transform>();
	engine.RegisterComponent<TextureRenderer>();
	engine.RegisterComponent<Collider>();
	engine.RegisterComponent<KeyboardController>();
	engine.RegisterComponent<GhostController>();
	engine.RegisterComponent<PacmanController>();
	engine.RegisterComponent<UILabel>();
}

void RegisterSystems()
{
	renderSystem = engine.RegisterSystem<RenderSystem>();
	collisionSystem = engine.RegisterSystem<CollisionSystem>();
}

void SetSystemsSignatures()
{
	Signature renderSignature;
	renderSignature.set(engine.GetComponentType<TextureRenderer>());
	engine.SetSystemSignature<RenderSystem>(renderSignature);

	Signature collideSignature;
	collideSignature.set(engine.GetComponentType<Collider>());
	engine.SetSystemSignature<Collider>(collideSignature);
}


void InitPacman()
{
	Entity pacmanEntity = engine.CreateEntity();

	engine.AddComponent(pacmanEntity, Transform{ PACMAN_START_XPOS, PACMAN_START_YPOS, Game::TILE_SIZE, Game::TILE_SIZE });
	engine.AddComponent(pacmanEntity, TextureRenderer{ TextureManager::LoadTexture("assets/Pacman/lookingRight.png") });
	engine.AddComponent(pacmanEntity, Collider{ { 0, 0, COLLIDER_BOX_SIZE, COLLIDER_BOX_SIZE} });
	engine.AddComponent(pacmanEntity, KeyboardController{ false, false, false, false });
	engine.AddComponent(pacmanEntity, PacmanController(pacmanSpeed, engine, pacmanEntity));

	pacman = &engine.GetComponent<PacmanController>(pacmanEntity);
}

void InitInky()
{
	Entity inkyEntity = engine.CreateEntity();

	engine.AddComponent<Transform>(inkyEntity, Transform{ INKY_START_XPOS, INKY_START_YPOS, Game::TILE_SIZE, Game::TILE_SIZE});
	engine.AddComponent<TextureRenderer>(inkyEntity, TextureRenderer{ TextureManager::LoadTexture("assets/Ghosts/inky.png") });
	engine.AddComponent<Collider>(inkyEntity, Collider{ { 0, 0, COLLIDER_BOX_SIZE, COLLIDER_BOX_SIZE} });
	engine.AddComponent<GhostController>(inkyEntity, GhostController(ghostsSpeed, engine, inkyEntity));

	inky = &engine.GetComponent<GhostController>(inkyEntity);
}

void InitBlinky()
{
	Entity blinkyEntity = engine.CreateEntity();

	engine.AddComponent<Transform>(blinkyEntity, Transform{ BLINKY_START_XPOS, BLINKY_START_YPOS, Game::TILE_SIZE, Game::TILE_SIZE});
	engine.AddComponent<TextureRenderer>(blinkyEntity, TextureRenderer{ TextureManager::LoadTexture("assets/Ghosts/blinky.png") });
	engine.AddComponent<Collider>(blinkyEntity, Collider{ { 0, 0, COLLIDER_BOX_SIZE, COLLIDER_BOX_SIZE} });
	engine.AddComponent<GhostController>(blinkyEntity, GhostController(ghostsSpeed, engine, blinkyEntity));

	blinky = &engine.GetComponent<GhostController>(blinkyEntity);
}

void InitPinky()
{
	Entity pinkyEntity = engine.CreateEntity();

	engine.AddComponent<Transform>(pinkyEntity, Transform{ PINKY_START_XPOS, PINKY_START_YPOS, Game::TILE_SIZE, Game::TILE_SIZE});
	engine.AddComponent<TextureRenderer>(pinkyEntity, TextureRenderer{ TextureManager::LoadTexture("assets/Ghosts/pinky.png") });
	engine.AddComponent<Collider>(pinkyEntity, Collider{ { 0, 0, COLLIDER_BOX_SIZE, COLLIDER_BOX_SIZE} });
	engine.AddComponent<GhostController>(pinkyEntity, GhostController(ghostsSpeed, engine, pinkyEntity));

	pinky = &engine.GetComponent<GhostController>(pinkyEntity);
}

void InitClyde()
{
	Entity clydeEntity = engine.CreateEntity();

	engine.AddComponent<Transform>(clydeEntity, Transform{ CLYDE_START_XPOS, CLYDE_START_YPOS, Game::TILE_SIZE, Game::TILE_SIZE});
	engine.AddComponent<TextureRenderer>(clydeEntity, TextureRenderer{ TextureManager::LoadTexture("assets/Ghosts/clyde.png") });
	engine.AddComponent<Collider>(clydeEntity, Collider{ { 0, 0, COLLIDER_BOX_SIZE, COLLIDER_BOX_SIZE} });
	engine.AddComponent<GhostController>(clydeEntity, GhostController(ghostsSpeed, engine, clydeEntity));

	clyde = &engine.GetComponent<GhostController>(clydeEntity);
}

void InitGhosts()
{
	InitInky();
	InitBlinky();
	InitPinky();
	InitClyde();
}

void InitUI()
{
	// labels properties are set locally here: (let's say extraction is not needed for two property-fixed labels)
	scoreLabelFont = TTF_OpenFont("assets/arial.ttf", 16);

	Entity scoreLabelEntity = engine.CreateEntity();
	engine.AddComponent<Transform>(scoreLabelEntity, Transform{ 2,8,100,20 });
	engine.AddComponent<TextureRenderer>(scoreLabelEntity, TextureRenderer{ UILabel::CreateTextLabelTexture("SCORE: 0", scoreLabelFont, white) });

	scoreLabel = &engine.GetComponent<TextureRenderer>(scoreLabelEntity);

	winLoseLabelFont = TTF_OpenFont("assets/arial.ttf", 100);

	Entity winLoseLabelEntity = engine.CreateEntity();
	engine.AddComponent<Transform>(winLoseLabelEntity, Transform{ Game::WINDOW_WIDTH / 3, Game::WINDOW_HEIGHT / 2, 220, 100 });
	engine.AddComponent<TextureRenderer>(winLoseLabelEntity, TextureRenderer{ UILabel::CreateTextLabelTexture(" ", winLoseLabelFont, white) });

	winLoseLabel = &engine.GetComponent<TextureRenderer>(winLoseLabelEntity);
}


void CountScore()
{
	scoreForWinning = 0;

	for (int row = 0; row < tilesCountY; ++row)
	{
		for (int col = 0; col < tilesCountX; ++col)
		{
			if (Game::map->tiles[row][col] == 2)
			{
				scoreForWinning++;
			}
		}
	}
}


void Game::Init()
{
	if (SDL_Init(SDL_INIT_VIDEO) == 0)
	{
		window = SDL_CreateWindow("Pacman", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, tilesCountX * TILE_SIZE, tilesCountY * TILE_SIZE, 0);
		renderer = SDL_CreateRenderer(window, -1, 0);

		isRunning = true;
	}
	else
	{
		isRunning = false;
	}

	if (TTF_Init() == -1)
	{
		std::cerr << "TTF Initialization failed. There is a problem with the UI system." << std::endl;
	}

	engine.Init();

	RegisterComponents();
	RegisterSystems();
	SetSystemsSignatures();

	map = new Map();

	if (scoreForWinning < 0)
	{
		CountScore();
	}

	engine.CreateEntity();

	InitPacman();
	InitGhosts();
	InitUI();
}

void Game::HandleEvents()
{
	SDL_PollEvent(&event);

	switch (event.type)
	{
	case SDL_QUIT:
		isRunning = false;
		break;
	default:
		break;
	}
}

void Game::IncrementScore()
{
	scoreLabel->defaultTexture = UILabel::CreateTextLabelTexture("SCORE: " + std::to_string(currentScore++), scoreLabelFont, white);
	if (currentScore >= scoreForWinning)
	{
		winLoseLabel->defaultTexture = UILabel::CreateTextLabelTexture(WIN_MESSAGE, winLoseLabelFont, white);
		isWinOrLoseConditionReached ^= 1;
	}
}

bool Game::IsPacmanInGhostHouse()
{
	int pacmanTileX = 0;
	int pacmanTileY = 0;

	CoordinatesToTiles(pacmanTileX, pacmanTileY, pacman->GetPosX(), pacman->GetPosY());

	return (pacmanTileX == 8 || pacmanTileX == 9 || pacmanTileX == 10) // Currently fixed. Changes in map might require changes here!
		&& (pacmanTileY == 8 || pacmanTileY == 9 || pacmanTileY == 10);
}

void Game::PacmanCollidingWith(const Entity& ghost)
{
	if (!engine.HasComponent<GhostController>(ghost))
	{
		return;
	}

	GhostController* ghostController = &engine.GetComponent<GhostController>(ghost);

	if (ghostController->GetMode() < 2) // if ghost is scattering/chasing
	{
		winLoseLabel->defaultTexture = UILabel::CreateTextLabelTexture(LOSE_MESSAGE, winLoseLabelFont, white);
		isWinOrLoseConditionReached ^= 1;
	}
	else if (ghostController->GetMode() == 2) // if ghost is frightened
	{
		ghostController->SetMode(3); // it becomes eaten
	}
}

void Game::FrightenGhosts()
{
	inky->SetMode(2);
	blinky->SetMode(2);
	pinky->SetMode(2);
	clyde->SetMode(2);
}

void Game::UpdateDifficulty()
{
	if (currentScore >= 30 && inky->GetMode() == 0)
	{
		inky->SetMode(1);
	} 

	if (currentScore >= 40 && pinky->GetMode() == 0)
	{
		pinky->SetMode(1);
	}

	if (currentScore >= 50 && blinky->GetMode() == 0)
	{
		blinky->SetMode(1);
	}

	if (currentScore >= 60 && clyde->GetMode() == 0)
	{
		clyde->SetMode(1);
	}
}

void Game::UpdateGhosts()
{
	inky->Update(pacman->GetPosX(), pacman->GetPosY());
	blinky->Update(pacman->GetPosX(), pacman->GetPosY());
	pinky->Update(pacman->GetPosX(), pacman->GetPosY());
	clyde->Update(pacman->GetPosX(), pacman->GetPosY());
}

void Game::Update()
{
	pacman->Update();

	UpdateGhosts();

	collisionSystem->CheckForCollisions();

	UpdateDifficulty();
}

void Game::Render()
{
	SDL_RenderClear(renderer);
	map->DrawMap();
	renderSystem->Render();
	SDL_RenderPresent(renderer);

	if (isWinOrLoseConditionReached)
	{
		SDL_Delay(2000);
		Game::isRunning ^= 1;
	}
}

void Game::Clean()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();

	TTF_CloseFont(scoreLabelFont);
	TTF_CloseFont(winLoseLabelFont);
	TTF_Quit();
}

