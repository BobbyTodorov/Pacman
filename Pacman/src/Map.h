#ifndef _MAP_HEADER_INCLUDED_
#define _MAP_HEADER_INCLUDED_

#include "SDL.h"

static const int tilesCountX = 19, tilesCountY = 19;

class Map
{
private:

	SDL_Rect src, dest;

	SDL_Texture* free;
	SDL_Texture* block;
	SDL_Texture* dot;
	SDL_Texture* energizer;

	void DrawTile(const int& type, const int& row, const int& col);

public:
	int tiles[tilesCountX][tilesCountY];

	Map();
	~Map();

	void LoadMap(int tiles[tilesCountX][tilesCountY]);
	void DrawMap();
};

#endif // !_MAP_HEADER_INCLUDED_

