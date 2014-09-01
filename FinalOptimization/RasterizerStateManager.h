#pragma once

#include <D3D11.h>
#include "com_ptr.h"


class RasterizerStateManager
{
private:
	RasterizerStateManager(com_ptr<ID3D11Device>& dev);

	static RasterizerStateManager* inst;

	com_ptr<ID3D11Device> mDev;
	
	com_ptr<ID3D11RasterizerState> mDefaultSolid;
	com_ptr<ID3D11RasterizerState> mDefaultWireframe;

public:
	
	enum DefaultStates { SOLID, WIREFRAME };
	
	static RasterizerStateManager* getInstance() { return inst; }

	static void init(com_ptr<ID3D11Device>& dev)
	{
		if(inst == nullptr)
		{
			inst = new RasterizerStateManager(dev);
		}
	}

	static void release()
	{
		if(inst != nullptr)
			delete inst;
	}

	/**
	Set one of the default rasterizer states.
	**/
	void setDefault(com_ptr<ID3D11DeviceContext>& devcon, DefaultStates state);

};
