#pragma once

#include "GameWindow.h"
#include <D3D11.h>
#include  <xnamath.h>
#include "Camera.h"
#include "com_ptr.h"
#include "GraphicsHelper.h"


/**
 Derive from this class to define your custom game class.
 Note to implement void render() and void move().
 Additionally note to invoke "loop()" in your constructur to start the message loop.

**/
class Game
{
protected:
	// Primitive members
	float mGlobalTime;
	float mDeltaTime;
	float mFieldOfView;
	float mNearClippingPlane, mFarClippingPlane;

	// Static const
	static const float DEFAULT_FIELD_OF_VIEW;

#ifdef GH_LOGGING_ENABLED
	float mFrameCounter;
	float mLatestFrameMeasuredAt;
#endif

	// Class members
	GameWindow mWindow;
	Camera mCamera;

	// Matrices
	// TODO Store as XMFLOAT4X4
	XMMATRIX mWorldMatrix, mViewMatrix, mProjectionMatrix;

	// Direct3D
	com_ptr<ID3D11Device> mDev;
	com_ptr<ID3D11DeviceContext> mDevCon;
	com_ptr<IDXGISwapChain> mSwapChain;
	com_ptr<ID3D11RenderTargetView> mRenderTarget; // "Backbuffer"
	com_ptr<ID3D11DepthStencilView> mDepthStencilView;

protected:
	virtual void render() = 0;
	virtual void move() = 0;
	void loop();

protected:
	Game(GameWindow::Builder windowBuilder, float fieldOfView = DEFAULT_FIELD_OF_VIEW);
	virtual ~Game();

public:
	float getGlobalTime() const { return mGlobalTime; }
	float getDeltaTime() const { return mDeltaTime; }
	const XMMATRIX& getWorldMatrix() const { return mWorldMatrix; }
	const XMMATRIX& getViewMatrix() const { return mViewMatrix; }
	const XMMATRIX& getProjectionMatrix() const { return mProjectionMatrix; }
	const Camera& getCamera() const { return mCamera; }

	float getFieldOfView() const { return mFieldOfView; }
	void setFieldOfView(float fov) { mFieldOfView = fov; }
};