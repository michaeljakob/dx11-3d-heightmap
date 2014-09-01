#pragma once

#include <D3D11.h>
#include <string>
#include <cstddef>
#include <xnamath.h>
#include "com_ptr.h"
#include "ShaderStructs.h"
#include "Game.h"
#include <vector>
#include "VertexStruct.h"

/**
A simple bitmap based terrain class, commonly named "heightmap".

**/
class Terrain
{
private:
	static const std::string TERRAIN_FOLDER;
	static const std::string RAW_FOLDER;
	std::string mFileName;
	size_t mWidth, mHeight;
	size_t mNumPrimitives, mNumVertices, mNumIndices;

	// How large the differences between each height-step are.
	float mHeightFactor;
	size_t mRowWidth;

	std::vector<VertexStruct> mVertexData;

	com_ptr<ID3D11Buffer> mIndexBuffer;
	com_ptr<ID3D11Buffer> mVertexBuffer;

	com_ptr<ID3D11InputLayout> mInputLayout;
	com_ptr<ID3D11VertexShader> mVertexShader;
	com_ptr<ID3D11PixelShader> mPixelShader;

	com_ptr<ID3D11Device> mDev;

	com_ptr<ID3D11Buffer> mConstantBuffer;
	D3D11_MAPPED_SUBRESOURCE mLockedConstantBuffer;

	DirectionalLight mDirectionalLight;
	SpotLight mSpotLight;

private:
	struct __declspec(align(16)) ConstantBuffer
	{
		XMMATRIX worldViewProj;
		XMFLOAT4 special;
		XMFLOAT4 camPos;
		DirectionalLight dirLight;
		SpotLight spotLight;
	};

public:
	struct TerrainDescriptor 
	{
		char rawFileName[30];
		char shaderFileName[30];
		size_t width;
		size_t height;
		float heightFactor;
				
	};





private:
	void loadShaders(const std::string&);

private:
	Terrain();
	Terrain(const com_ptr<ID3D11Device>& dev, const std::string& name, size_t width, size_t height, float heightFactor, bool repeatTexture, const std::string& shaderName);	
public:
	static Terrain fromDescriptor(const com_ptr<ID3D11Device>& dev, const std::string& terrainFileName);
	static Terrain fromRawFile(const com_ptr<ID3D11Device>& dev, const std::string& rawFileName, size_t width=256, size_t height=256, float heightFactor=10.0f, bool repeatTexture=false, const std::string& shaderName="Terrain.hlsl");
	
	
	void draw(com_ptr<ID3D11DeviceContext>& devcon, const Game&);
	const std::vector<VertexStruct>& getVertexData() const { return mVertexData; }
	
	size_t getWidth() const { return mWidth; }
	size_t getHeight() const { return mHeight; }

	~Terrain() {};
};


