#include "FinalOptimization.h"
#include "TextureManager.h"
#include "Camera.h"
#include "GraphicsHelper.h"
#include "ShaderManager.h"
#include <string>

const float FinalOptimization::CLEAR_RENDERTARGET_COLOR_BLUE[4] =  { 0.3f, 0.5f, 0.8f, 1.0f };
const float FinalOptimization::CLEAR_RENDERTARGET_COLOR_BLACK[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
const float FinalOptimization::CLEAR_RENDERTARGET_COLOR_WHITE[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

Terrain terrainFromCommandLine(com_ptr<ID3D11Device> dev) 
{
	std::string args = GetCommandLine();

	std::string fileName = args.substr(args.rfind(' ')+1, args.length());

	if(fileName.length() == 0) 
		fileName = "flatland.ter";

	return Terrain::fromDescriptor(dev, fileName);
}

FinalOptimization::FinalOptimization(GameWindow::Builder windowBuilder)
: Game(windowBuilder), mTerrain(terrainFromCommandLine(mDev))
{
	mCamera.setDistanceFromFloor(2, mTerrain.getVertexData(), mTerrain.getWidth());
	

	// Load textures
	TextureManager& textureMan = TextureManager::getInstance();
	textureMan.add("grass.jpg");
	textureMan.add("future.jpg");
	//textureMan.add("ghost.jpg");

	
	//Terrain::fromDescriptor(mDev, "albedo_512x512.ter");

	loop();
}


void FinalOptimization::move()
{
	if(mWindow.getIsFocused()){
		mCamera.update(mDeltaTime);
	}
}

void FinalOptimization::render()
{
	// Clear the screen
	mDevCon->ClearRenderTargetView(mRenderTarget, CLEAR_RENDERTARGET_COLOR_BLUE);
	mDevCon->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1, 0);
	
	mDevCon->PSSetSamplers(0, 1, &TextureManager::getDefaultSamplerState().get());
		
	mDevCon->PSSetShaderResources(0, 1, &TextureManager::getInstance().get("grass.jpg").get());
	mDevCon->PSSetShaderResources(1, 1, &TextureManager::getInstance().get("future.jpg").get());

	mTerrain.draw(mDevCon, *this);



}

