#include "Map.h"
#include "Engine\TextureManager.h"

int startingTiles[19][19] = {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
							 {1, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 1},
							 {1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1},
							 {1, 2, 1, 1, 2, 1, 2, 1, 2, 2, 2, 1, 2, 1, 2, 1, 1, 2, 1},
							 {1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1},
							 {1, 2, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 2, 1},
							 {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
							 {1, 2, 1, 2, 1, 1, 2, 1, 1, 0, 1, 1, 2, 1, 1, 2, 1, 2, 1},
							 {1, 2, 1, 2, 1, 1, 2, 1, 0, 0, 0, 1, 2, 1, 1, 2, 1, 2, 1},
							 {1, 2, 2, 2, 2, 2, 2, 1, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 1},
							 {1, 2, 1, 2, 1, 1, 2, 1, 0, 0, 0, 1, 2, 1, 1, 2, 1, 2, 1},
							 {1, 2, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 2, 1},
							 {1, 2, 1, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 1, 2, 1},
							 {1, 2, 1, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 2, 1, 2, 1},
							 {1, 2, 2, 2, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 2, 2, 2, 1},
							 {1, 2, 1, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 1, 2, 1},
							 {1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 1},
							 {1, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 1},
							 {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1} };

Map::Map()
{
	free = TextureManager::LoadTexture("assets/free.png");
	block = TextureManager::LoadTexture("assets/block.png");
	dot = TextureManager::LoadTexture("assets/dot.png");
	energizer = TextureManager::LoadTexture("assets/energizer.png");

	LoadMap(startingTiles);

	src.x = src.y = 0;
	src.w = dest.w = Game::TILE_SIZE;
	src.h = dest.h = Game::TILE_SIZE;

	dest.x = dest.y = 0;
}

Map::~Map()
{
	delete free;
	delete block;
	delete dot;
	delete energizer;
}

void Map::LoadMap(int tiles[tilesCountX][tilesCountY])
{
	for (int row = 0; row < tilesCountY; ++row)
	{
		for (int col = 0; col < tilesCountX; ++col)
		{
			this->tiles[row][col] = tiles[row][col];
		}
	}
}

void Map::DrawMap()
{
	for (int row = 0; row < tilesCountY; ++row)
	{
		for (int col = 0; col < tilesCountX; ++col)
		{
			DrawTile(tiles[row][col], row, col);
		}
	}
}

void Map::DrawTile(const int& type, const int& row, const int& col)
{
	dest.x = col * Game::TILE_SIZE;
	dest.y = row * Game::TILE_SIZE;

	switch (type)
	{
	case 0:
		TextureManager::Draw(free, src, dest);
		break;
	case 1:
		TextureManager::Draw(block, src, dest);
		break;
	case 2:
		TextureManager::Draw(dot, src, dest);
		break;
	case 3:
		TextureManager::Draw(energizer, src, dest);
	default:
		break;
	}
}
