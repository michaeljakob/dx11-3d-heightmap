#include <Windows.h>
#include <xnamath.h>
#include "Game.h"
#include "GraphicsHelper.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "RasterizerStateManager.h"
#include <fstream>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>

const float Game::DEFAULT_FIELD_OF_VIEW = static_cast<float>(GH_DEGTORAD(45));



void initializeDebugConsole();

Game::~Game()
{
	TextureManager::release();
	ShaderManager::release();
	RasterizerStateManager::release();
}

Game::Game(GameWindow::Builder windowBuilder, float fieldOfView /*= DEFAULT_FIELD_OF_VIEW*/)
: mWindow(windowBuilder.build()), mFieldOfView(fieldOfView), mDeltaTime(0), mGlobalTime(0), mWorldMatrix(XMMatrixIdentity()), mViewMatrix(XMMatrixIdentity()), mProjectionMatrix(XMMatrixIdentity()), mNearClippingPlane(2), mFarClippingPlane(500)
{
#ifdef GH_LOGGING_ENABLED
	mFrameCounter = 0;
	mLatestFrameMeasuredAt = 0;
#endif


	mWindow.show();

	// Create swapchain
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.Height = mWindow.getHeight();
	swapChainDesc.BufferDesc.Width = mWindow.getWidth();
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 1000;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER;
	swapChainDesc.Flags = 0 ;
	swapChainDesc.OutputWindow = mWindow.getHandle();
	swapChainDesc.SampleDesc.Count = 8;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Windowed = mWindow.getIsWindowed();

#ifdef _DEBUG
	DWORD CREATE_DEVICE_FLAGS = D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS | D3D11_CREATE_DEVICE_SINGLETHREADED;
#else
	DWORD CREATE_DEVICE_FLAGS = 0;
#endif

	
	if(FAILED(D3D11CreateDeviceAndSwapChain(0,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		CREATE_DEVICE_FLAGS,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&mSwapChain,
		&mDev,
		NULL,
		&mDevCon)))
	{
		throw std::runtime_error("D3D11CreateDeviceAndSwapChain failed.");
	}

	if(!mWindow.getIsWindowed())
		mSwapChain->SetFullscreenState(true, nullptr);


	// set rendertarget
	com_ptr<ID3D11Texture2D> ptrBackbuffer;
	mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&ptrBackbuffer));
	if(FAILED(mDev->CreateRenderTargetView(ptrBackbuffer, nullptr, &mRenderTarget)))
		throw std::runtime_error("mDev->CreateRenderTargetView failed.");

	// stencil and depth
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	com_ptr<ID3D11DepthStencilState> depthStencilState;
	if(FAILED(mDev->CreateDepthStencilState(&depthStencilDesc, &depthStencilState)))
		throw std::runtime_error("mDev->CreateDepthStencilState");

	mDevCon->OMSetDepthStencilState(depthStencilState, 1);
	com_ptr<ID3D11Texture2D> depthStencilTexture;
	D3D11_TEXTURE2D_DESC depthStencilTextureDesc;
	depthStencilTextureDesc.ArraySize = 1;
	depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilTextureDesc.CPUAccessFlags = 0;
	depthStencilTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilTextureDesc.Height = mWindow.getHeight();
	depthStencilTextureDesc.Width = mWindow.getWidth();
	depthStencilTextureDesc.MiscFlags = 0;
	depthStencilTextureDesc.MipLevels = 1;
	depthStencilTextureDesc.SampleDesc.Count = 8;
	depthStencilTextureDesc.SampleDesc.Quality = 0;
	depthStencilTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	mDev->CreateTexture2D(&depthStencilTextureDesc, nullptr, &depthStencilTexture);
	if(FAILED(mDev->CreateDepthStencilView(depthStencilTexture, 0, &mDepthStencilView)))
		throw std::runtime_error("mDev->CreateDepthStencilView");

	mDevCon->OMSetRenderTargets(1, &mRenderTarget, mDepthStencilView);

	// set viewport
	D3D11_VIEWPORT viewport;
	viewport.MaxDepth = 1;
	viewport.MinDepth = 0;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<FLOAT>(mWindow.getWidth());
	viewport.Height = static_cast<FLOAT>(mWindow.getHeight());

	// params: number of viewports to bind, array of viewports
	mDevCon->RSSetViewports(1, &viewport);

	// Statically initialize separate engines after basic 3d stuff is set up.	
	TextureManager::init(mDev);
	ShaderManager::init(mDev);
	RasterizerStateManager::init(mDev);

#ifdef GH_SHOW_CONSOLE
	initializeDebugConsole();
	printf("Loading data...\n");
#endif
}


void Game::loop()
{
	MSG msg;
	DWORD timeStart;
	while(true)
	{
		timeStart = GetTickCount();

		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(msg.message == WM_QUIT)
			break;

		// update projection matrix
		mProjectionMatrix = XMMatrixPerspectiveFovLH(mFieldOfView, mWindow.getResolutionRatio(), mNearClippingPlane, mFarClippingPlane);


		this->move();
		this->render();
		mSwapChain->Present(0, 0);

		mDeltaTime = static_cast<float>(GetTickCount() - timeStart) / 1000.0f;

#ifdef GH_LOGGING_ENABLED
		if((int) mGlobalTime != (int)mLatestFrameMeasuredAt)
		{
			printf("fps: %f\n", mFrameCounter/(mGlobalTime-mLatestFrameMeasuredAt));
			mLatestFrameMeasuredAt = mGlobalTime;

			mFrameCounter = 0;
		} else {
			++mFrameCounter;
		}
#endif

		mGlobalTime += mDeltaTime;
	}
}


void initializeDebugConsole()
{	
    //Create a console for this application
    AllocConsole();
    //Redirect unbuffered STDOUT to the console
	HANDLE ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    int SystemOutput = _open_osfhandle(intptr_t(ConsoleOutput), _O_TEXT);
    FILE *COutputHandle = _fdopen(SystemOutput, "w" );
    *stdout = *COutputHandle;
    setvbuf(stdout, NULL, _IONBF, 0);

    //Redirect unbuffered STDERR to the console
	HANDLE ConsoleError = GetStdHandle(STD_ERROR_HANDLE);
    int SystemError = _open_osfhandle(intptr_t(ConsoleError), _O_TEXT);
    FILE *CErrorHandle = _fdopen(SystemError, "w" );
    *stderr = *CErrorHandle;
    setvbuf(stderr, NULL, _IONBF, 0);

    //Redirect unbuffered STDIN to the console
	HANDLE ConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
    int SystemInput = _open_osfhandle(intptr_t(ConsoleInput), _O_TEXT);
    FILE *CInputHandle = _fdopen(SystemInput, "r" );
    *stdin = *CInputHandle;
    setvbuf(stdin, NULL, _IONBF, 0);
    
    //make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog point to console as well
    std::ios::sync_with_stdio(true);
}
