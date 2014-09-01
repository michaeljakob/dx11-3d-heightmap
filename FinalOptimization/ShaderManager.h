#pragma once

#include <D3D11.h>
#include <map>
#include <string>
#include "com_ptr.h"

class ShaderManager
{
public:	
	struct Item
	{
		com_ptr<ID3D11VertexShader> vertexShader;
		com_ptr<ID3D11PixelShader> pixelShader;
		com_ptr<ID3D11InputLayout> inputLayout;
	};
private:
	static const std::string SHADER_FOLDER;
	static ShaderManager *INSTANCE;
	com_ptr<ID3D11Device> mDev;
	std::map<std::string, ShaderManager::Item> mShaders;

	


private:	
	ShaderManager(com_ptr<ID3D11Device>& dev)
		: mDev(dev)
	{
	};

	ShaderManager(const ShaderManager&) {};
	ShaderManager& operator = (const ShaderManager&) {};
	~ShaderManager() {};

private:
	Item add(const std::string&, const D3D11_INPUT_ELEMENT_DESC inputElementDesc[], size_t numElements);
	void addDirectory(const std::string&, const D3D11_INPUT_ELEMENT_DESC inputElementDesc[], size_t numElements);
	const std::string loadCompiledShader(const std::string& name);

public:	
	static void init(com_ptr<ID3D11Device>& dev)
	{
		if(INSTANCE == nullptr)
		{
			INSTANCE = new ShaderManager(dev);
		}
	}

	static void release()
	{
		if(INSTANCE != nullptr)
			delete INSTANCE;
	}

	static ShaderManager& getInstance() { return *INSTANCE; }

	
	template<std::size_t N>
	Item add(const std::string& name, const D3D11_INPUT_ELEMENT_DESC (&inputElementDesc)[N]) { return add(name, inputElementDesc, N); }
	template<std::size_t N>
	void addDirectory(const std::string& dirname, const D3D11_INPUT_ELEMENT_DESC (&inputElementDesc)[N]) { addDirectory(dirname, inputElementDesc, N); }

	bool remove(const com_ptr<ID3D11PixelShader>&);
	bool remove(const com_ptr<ID3D11VertexShader>&);
	bool remove(const std::string&);
	const com_ptr<ID3D11PixelShader>& getPixelShader(const std::string&) const;
	const com_ptr<ID3D11VertexShader>& getVertexShader(const std::string&) const;
	const Item& getShader(const std::string&) const;

};