#include "Terrain.h"
#include <cassert>
#include <array>
#include <fstream>
#include <vector>
#include "Array2D.h"
#include "ShaderManager.h"
#include "RasterizerStateManager.h"


const std::string Terrain::TERRAIN_FOLDER = "./terrains/";
const std::string Terrain::RAW_FOLDER = Terrain::TERRAIN_FOLDER + "raw/";

using namespace std;


Terrain Terrain::fromDescriptor(const com_ptr<ID3D11Device>& dev, const std::string& terrainFileName) 
{	
	
	//TerrainDescriptor d = {0};
	//d.height = d.width = 225;
	//d.heightFactor = 35;
	//strcpy_s(d.rawFileName, 30, "desert.raw");
	//strcpy_s(d.shaderFileName, 30, "desert.hlsl");

	//ofstream stream(TERRAIN_FOLDER + "desert.ter", ios::binary);
	//if(stream)
	//{
	//	stream.write(reinterpret_cast<const char*>(&d), sizeof(TerrainDescriptor));
	//}

	//return Terrain::fromRawFile(dev, d.rawFileName, d.width, d.height, d.heightFactor, true);
	
	
	ifstream stream(TERRAIN_FOLDER + terrainFileName, ios::binary);
	if(stream)
	{
		TerrainDescriptor desc = {0};
		stream.read(reinterpret_cast<char*>(&desc), sizeof(TerrainDescriptor));
		stream.close();

		printf("%s\n%s\n%d\n%d\n%f\n", 
			 desc.rawFileName,
			 desc.shaderFileName,
			 desc.width,
			 desc.height,
			 desc.heightFactor);

		return Terrain::fromRawFile(dev, desc.rawFileName, desc.width, desc.height, desc.heightFactor, true, desc.shaderFileName);
	}
	

	throw std::exception("file could not be read. Does it exist?");
}



Terrain Terrain::fromRawFile(const com_ptr<ID3D11Device>& dev, const std::string& name, size_t width, size_t height, float heightFactor, bool repeatTexture, const std::string& shaderName)
{
	return Terrain(dev, name, width, height, heightFactor, repeatTexture, shaderName);
}


Terrain::Terrain(const com_ptr<ID3D11Device>& dev, const std::string& name, size_t width, size_t height, float heightFactor, bool repeatTexture, const std::string& shaderName)
: mDev(dev), mFileName(name), mWidth(width), mHeight(height), mHeightFactor(heightFactor), mNumPrimitives((width-1) * (height-1) * 2), mNumVertices(width * height), mNumIndices(mNumPrimitives*3)
{
	string completeName = RAW_FOLDER + name;
	assert(dev != nullptr);
	assert(GetFileAttributes(completeName.c_str()) != INVALID_FILE_ATTRIBUTES);

	// init lights
	XMStoreFloat4(&(mDirectionalLight.col), XMVectorSet(1.0f, 0.8f, 0.6f, 1));
	XMStoreFloat4(&(mDirectionalLight.dir), XMVector3Normalize(XMVectorSet(-50, -1, 0, 0)));

	XMStoreFloat4(&(mSpotLight.dir), XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));
	XMStoreFloat4(&(mSpotLight.pos), XMVectorSet(50, 50, 50, 1));
	XMStoreFloat4(&(mSpotLight.col), XMVectorSet(0, 1, 0, 1));

	// Start processing the bitmap
	Array2D<size_t> coordinates(width, height);
	// read in bitmap
	ifstream fileStream(completeName, ios::binary);
	fileStream.seekg(0);
	for(size_t y=0; y<height; ++y)
	{
		for(size_t x=0; x<width; ++x)
		{
			coordinates(x, y) = fileStream.get();
		}
	}
	fileStream.close();

	// create memory for index and vertex data
	vector<VertexStruct>	vertexData;
	vector<UINT>					indexData;
	vertexData.reserve(mNumVertices);
	indexData.reserve(mNumIndices);

	// fill vertex data
	VertexStruct vertexStruct;
	// repeat texture? :)
	if(repeatTexture)
	{
		for(size_t y=0; y<mHeight; ++y)
		{
			for(size_t x=0; x<mWidth; ++x)
			{
				XMStoreFloat3(&vertexStruct.pos, XMVectorSet((float) x,
					(float) coordinates(x, y) / heightFactor,
					(float) y,
					0));
				vertexStruct.u = (float) x ;
				vertexStruct.v = (float) y ;
				XMStoreFloat3(&vertexStruct.normal, XMVectorSet(0, 0, 0, 0));
				vertexData.push_back(vertexStruct);
			}
		}
	}
	else
	{
		// only one texture over the whole terrain
		for(size_t y=0; y<mHeight; ++y)
		{
			for(size_t x=0; x<mWidth; ++x)
			{
				XMStoreFloat3(&vertexStruct.pos, XMVectorSet(	(float) x,
					(float) coordinates(x, y) / heightFactor,
					(float) y ,
					0 ));
				vertexStruct.u = (float) x  / (float) mWidth;
				vertexStruct.v = (float) y  / (float) mHeight;
				XMStoreFloat3(&vertexStruct.normal, XMVectorSet(0, 0, 0, 0));
				vertexData.push_back(vertexStruct);
			}
		}
	}

	// Initialize indicies
	for(size_t y=0; y < mHeight-1; ++y)
	{
		for(size_t x=0; x < mWidth-1; ++x)
		{
			indexData.push_back(x+y * (mWidth)); 
			indexData.push_back(x+1+y * (mWidth)); 
			indexData.push_back(x+1+(y+1) * (mWidth)); 

			indexData.push_back(x+y * (mWidth)); 
			indexData.push_back(x+1+(y+1) * (mWidth)); 
			indexData.push_back(x+(y+1) * (mWidth)); 
		}
	}


	// Initialize primitives
	for(size_t i=0; i<mNumPrimitives; ++i)
	{
		// compute crossproduct
		XMVECTOR vec = XMVector3Cross(XMVectorSubtract(XMLoadFloat3(&vertexData[indexData[i*3]].pos), XMLoadFloat3(&vertexData[indexData[i*3+1]].pos)),
			XMVectorSubtract(XMLoadFloat3(&vertexData[indexData[i*3]].pos), XMLoadFloat3(&vertexData[indexData[i*3+2]].pos)));

		XMStoreFloat3(&vertexData[indexData[i*3]].normal, XMVector3Normalize(XMVectorAdd(XMLoadFloat3(&vertexData[indexData[i*3]].normal), vec)));
		XMStoreFloat3(&vertexData[indexData[i*3+1]].normal, XMVector3Normalize(XMVectorAdd(XMLoadFloat3(&vertexData[indexData[i*3+1]].normal), vec)));
		XMStoreFloat3(&vertexData[indexData[i*3+2]].normal, XMVector3Normalize(XMVectorAdd(XMLoadFloat3(&vertexData[indexData[i*3+2]].normal), vec)));
	}

	// vertex buffer initialization
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = static_cast<UINT>(mNumVertices * sizeof(VertexStruct));
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	D3D11_SUBRESOURCE_DATA vertexBufferSubresourceData;
	vertexBufferSubresourceData.pSysMem = &(vertexData[0]);
	vertexBufferSubresourceData.SysMemPitch = 0;
	vertexBufferSubresourceData.SysMemSlicePitch = 0;
	if(FAILED(mDev->CreateBuffer(&(vertexBufferDesc), &vertexBufferSubresourceData, &(mVertexBuffer))))
	{
		throw std::runtime_error("CreateBuffer failed in vertex buffer initialization.");
	}

	mVertexData = vertexData;


	// index buffer initialization
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth = static_cast<UINT>(mNumIndices * sizeof(indexData.front()));
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	D3D11_SUBRESOURCE_DATA indexBufferSubresourceData;
	indexBufferSubresourceData.pSysMem = &(indexData[0]);
	indexBufferSubresourceData.SysMemPitch = 0;
	indexBufferSubresourceData.SysMemSlicePitch = 0;
	if(FAILED(mDev->CreateBuffer(&(indexBufferDesc), &indexBufferSubresourceData, &(mIndexBuffer))))
	{
		throw std::runtime_error("CreateBuffer failed in index buffer initialization.");
	}


	// create constant buffer
	D3D11_BUFFER_DESC constantBufferDesc;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.ByteWidth = sizeof(Terrain::ConstantBuffer);
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	if(FAILED(mDev->CreateBuffer(&constantBufferDesc, nullptr, &mConstantBuffer)))
		throw std::runtime_error("CreateBuffer failed in constant buffer initialization.");

	loadShaders(shaderName);

}

void Terrain::draw(com_ptr<ID3D11DeviceContext>& devcon, const Game& game)
{
	RasterizerStateManager::getInstance()->setDefault(devcon, RasterizerStateManager::SOLID);

	devcon->VSSetConstantBuffers(0, 1, &mConstantBuffer);
	devcon->PSSetConstantBuffers(0, 1, &mConstantBuffer);

	// set input layout and shaders
	devcon->IASetInputLayout(mInputLayout);
	devcon->VSSetShader(mVertexShader, nullptr, 0);
	devcon->PSSetShader(mPixelShader, nullptr, 0);

	// set buffers and draw
	UINT strides = sizeof(VertexStruct);
	UINT offsets = 0;
	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devcon->IASetVertexBuffers(0, 1, &mVertexBuffer, &strides, &offsets);
	devcon->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Constant buffer
	devcon->Map(mConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mLockedConstantBuffer);
	Terrain::ConstantBuffer *cbuffer = reinterpret_cast<Terrain::ConstantBuffer*>(mLockedConstantBuffer.pData);
	cbuffer->worldViewProj = XMMatrixTranspose(XMMatrixTranslation(0, 0, 0) * game.getCamera().getViewMatrix() * game.getProjectionMatrix());
	XMStoreFloat4(&(cbuffer->camPos), game.getCamera().getPosition());
	cbuffer->dirLight = mDirectionalLight;
	cbuffer->spotLight = mSpotLight;
	XMStoreFloat4(&(cbuffer->special), XMVectorSet(game.getGlobalTime(), game.getDeltaTime(), 0, 0));
	devcon->Unmap(mConstantBuffer, 0);

	devcon->DrawIndexed(static_cast<UINT>(mNumIndices), 0, 0);

	/*
	// draw wireframe
	RasterizerStateManager::getInstance()->setDefault(devcon, RasterizerStateManager::WIREFRAME);
	
	devcon->Map(mConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mLockedConstantBuffer);
	cbuffer = reinterpret_cast<Terrain::ConstantBuffer*>(mLockedConstantBuffer.pData);
	cbuffer->worldViewProj = XMMatrixTranspose(XMMatrixTranslation(0, 0, 0) * game.getCamera().getViewMatrix() * game.getProjectionMatrix());
	XMStoreFloat4(&(cbuffer->camPos), game.getCamera().getPosition());
	cbuffer->dirLight = mDirectionalLight;
	cbuffer->spotLight = mSpotLight;
	XMStoreFloat4(&(cbuffer->special), XMVectorSet(game.getGlobalTime(), game.getDeltaTime(), 1, 0));	
	devcon->Unmap(mConstantBuffer, 0);	

	//XMStoreFloat4(&(cbuffer->special), XMVectorSet(game.getGlobalTime(), game.getDeltaTime(), 1, 0));	
	//devcon->UpdateSubresource(mConstantBuffer, 0, nullptr, mLockedConstantBuffer.pData, 0, 0);

	devcon->DrawIndexed(static_cast<UINT>(mNumIndices), 0, 0);
	*/
}


void Terrain::loadShaders(const string& shaderName)
{
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"TEXCOORDS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"TEXCOORDS", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	ShaderManager& shaderManager = ShaderManager::getInstance();
	ShaderManager::Item item = shaderManager.add(shaderName, ied);
	mPixelShader = item.pixelShader;
	mVertexShader = item.vertexShader;
	mInputLayout = item.inputLayout;
}










