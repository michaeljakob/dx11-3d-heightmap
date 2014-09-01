#pragma once

#include "Game.h"
#include "Terrain.h"


class FinalOptimization : public Game
{
private:
	Terrain mTerrain;
	
	static const float CLEAR_RENDERTARGET_COLOR_BLUE[4]; 
	static const float CLEAR_RENDERTARGET_COLOR_BLACK[4]; 
	static const float CLEAR_RENDERTARGET_COLOR_WHITE[4]; 
public:
	FinalOptimization(GameWindow::Builder windowBuilder);

	~FinalOptimization()
	{
	}

private:

	void render();
	void move();
};

