#include "ShaderManager.h"
#include <cassert>
#include <fstream>
#include <vector>
#include <D3DX11.h>

using namespace std;

ShaderManager *ShaderManager::INSTANCE = nullptr;
const std::string ShaderManager::SHADER_FOLDER = "./shaders/";

/**
 Compiles and adds a new Shader from file.
 The included vertex shader should have the name VxShader,
 the included pixel shader should have the name PxShader.
**/
ShaderManager::Item ShaderManager::add(const std::string& name, const D3D11_INPUT_ELEMENT_DESC inputElementDesc[], size_t numElements)
{
	std::string completeName = SHADER_FOLDER + name;

#ifdef _DEBUG
	auto iter = mShaders.find(completeName);
	if(iter != mShaders.cend())
		assert(iter->first != name && "Shader already added");
#endif

	ShaderManager::Item newShaderItem;

// If in debug mode, we compile the shaders all the time
#ifdef _DEBUG
	com_ptr<ID3D10Blob> vx, px, error;

	// Compile shader
	if(FAILED(D3DX11CompileFromFile(completeName.c_str(), NULL, 0, "VxShader", "vs_5_0", 0, 0, 0, &vx, &error, NULL)))
	{
		TCHAR* errMsg = static_cast<TCHAR*>(error->GetBufferPointer());
		throw std::runtime_error("Failed to compile vertex shader in file " + completeName + errMsg);
	}
	

	if(FAILED(D3DX11CompileFromFile(completeName.c_str(), NULL, 0, "PxShader", "ps_5_0", 0, 0, 0, &px, &error, NULL)))
	{
		TCHAR* errMsg = static_cast<TCHAR*>(error->GetBufferPointer());
		throw std::runtime_error("Failed to compile pixel shader in file " + completeName);
	}
	
	if(FAILED(mDev->CreateVertexShader(vx->GetBufferPointer(), vx->GetBufferSize(), NULL, &(newShaderItem.vertexShader))))
	{
		TCHAR* errMsg = static_cast<TCHAR*>(error->GetBufferPointer());
		throw std::runtime_error("Failed to create vertex shader in file " + completeName);
	}

	if(FAILED(mDev->CreatePixelShader(px->GetBufferPointer(), px->GetBufferSize(), NULL, &(newShaderItem.pixelShader))))
	{
		TCHAR* errMsg = static_cast<TCHAR*>(error->GetBufferPointer());
		throw std::runtime_error("Failed to create pixel shader in file " + completeName);
	}

	// Create input layout
	if(FAILED(mDev->CreateInputLayout(inputElementDesc, numElements, vx->GetBufferPointer(), vx->GetBufferSize(), &(newShaderItem.inputLayout))))
		throw std::runtime_error("Failed to create inputlayout at file " + completeName);
#else
	// Load already compiled shader. Their name is of the form "<shader_folder>/<name>(_ps|_vs).hlslx"
	const string baseName = completeName.substr(0, completeName.length() - 5); // cut off 5 signs ".hlsl"
	const string compiledVS = loadCompiledShader(baseName + ".vsh");
	const string compiledPS = loadCompiledShader(baseName + ".psh");


	// Create shader
	if(FAILED(mDev->CreateVertexShader(compiledVS.c_str(), compiledVS.length(), NULL, &(newShaderItem.vertexShader))))
		throw std::runtime_error("Failed to create vertex shader in file " + completeName);
	

	if(FAILED(mDev->CreatePixelShader(compiledPS.c_str(), compiledPS.length(), NULL, &(newShaderItem.pixelShader))))
		throw std::runtime_error("Failed to create pixel shader in file " + completeName);

	// Create input layout
	if(FAILED(mDev->CreateInputLayout(inputElementDesc, numElements, compiledVS.c_str(), compiledVS.length(), &(newShaderItem.inputLayout))))
		throw std::runtime_error("Failed to create inputlayout at file " + completeName);

#endif
	
	// TODO return the reference
	mShaders[name] = newShaderItem;
	return newShaderItem;
}

void ShaderManager::addDirectory(const std::string& dirname, const D3D11_INPUT_ELEMENT_DESC inputElementDesc[], size_t numElements)
{
	assert(false && "Not implemented");
}

/**
Removes the item with the associated texture. Will remove only one item.
To remove all items, call this method until it returns false.
**/
bool ShaderManager::remove(const com_ptr<ID3D11PixelShader>& pixelShader)
{
	for(auto it = mShaders.cbegin(); it != mShaders.cend(); ++it)
	{
		if(it->second.pixelShader == pixelShader)
		{
			mShaders.erase(it);
			return true;
		}
	}

	return false;
}

/**
Removes the item with the associated texture. Will remove only one item.
To remove all items, call this method until it returns false.
**/
bool ShaderManager::remove(const com_ptr<ID3D11VertexShader>& vertexShader)
{
	for(auto it = mShaders.cbegin(); it != mShaders.cend(); ++it)
	{
		if(it->second.vertexShader == vertexShader)
		{
			mShaders.erase(it);
			return true;
		}
	}

	return false;
}


bool ShaderManager::remove(const std::string& name)
{
	return mShaders.erase(name) == 1;
}

const com_ptr<ID3D11VertexShader>& ShaderManager::getVertexShader(const std::string& name) const
{
	return getShader(name).vertexShader;
}

const com_ptr<ID3D11PixelShader>& ShaderManager::getPixelShader(const std::string& name) const
{
	return getShader(name).pixelShader;
}


const ShaderManager::Item& ShaderManager::getShader(const std::string& name) const
{	
	auto iter = mShaders.find(name);
	if(iter != mShaders.cend())
	{
		return iter->second;
	}
	else
	{
		throw std::invalid_argument("No item with key " + name + " found.");
	}
}


const std::string ShaderManager::loadCompiledShader(const std::string& name)
{
	ifstream in(name, ios::in | ios::binary);
	
	in.seekg(0, ios::end);
	size_t length = static_cast<size_t>(in.tellg());
	in.seekg(0, ios::beg);

	std::string strBuffer;
	strBuffer.resize(length);

	in.read(&strBuffer[0], length);


	return strBuffer;
}