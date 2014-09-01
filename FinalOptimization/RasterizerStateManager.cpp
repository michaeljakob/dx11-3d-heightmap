#include "RasterizerStateManager.h"


using namespace std;


RasterizerStateManager *RasterizerStateManager::inst = nullptr;


RasterizerStateManager::RasterizerStateManager(com_ptr<ID3D11Device>& dev) : mDev(dev)
{		
	// Rasterizer descs
	D3D11_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthClipEnable = false;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = true;
	rasterizerDesc.AntialiasedLineEnable = false; 
	mDev->CreateRasterizerState(&rasterizerDesc, &mDefaultSolid);

	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	mDev->CreateRasterizerState(&rasterizerDesc, &mDefaultWireframe);
}

/**
Set one of the default rasterizer states.
**/
void RasterizerStateManager::setDefault(com_ptr<ID3D11DeviceContext>& devcon, DefaultStates state)
{
	switch(state)
	{
	case SOLID:
		devcon->RSSetState(mDefaultSolid);
		break;
	case WIREFRAME:
		devcon->RSSetState(mDefaultWireframe);
		break;
	}
}














