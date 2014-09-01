#pragma once
#include <D3D11.h>
#include "com_ptr.h"
#include <cfloat>
#include <string>
#include <map>
#include <utility>


class TextureManager
{
private:
	static const std::string TEXTURE_FOLDER;
	static TextureManager *INSTANCE;
	com_ptr<ID3D11Device> mDev;	
	std::map<std::string, com_ptr<ID3D11ShaderResourceView>> mTextures;
	static com_ptr<ID3D11SamplerState> DEFAULT_SAMPLER_STATE;

private:
	TextureManager(com_ptr<ID3D11Device>& dev)
		: mDev(dev)
	{
		// Create default samplerState
		D3D11_SAMPLER_DESC defaultSamplerStateDesc;
		defaultSamplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
		defaultSamplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
		defaultSamplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
		//defaultSamplerStateDesc.BorderColor = modifiableLValue;
		defaultSamplerStateDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		defaultSamplerStateDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		defaultSamplerStateDesc.MaxAnisotropy = 16;
		defaultSamplerStateDesc.MinLOD = -FLT_MAX;
		defaultSamplerStateDesc.MipLODBias = 0;
		defaultSamplerStateDesc.MaxLOD = FLT_MAX;
		dev->CreateSamplerState(&defaultSamplerStateDesc, &TextureManager::DEFAULT_SAMPLER_STATE);
	};

	TextureManager(const TextureManager&) {};
	TextureManager& operator = (const TextureManager&) {};
	~TextureManager() {};



public:
	static void init(com_ptr<ID3D11Device>& dev)
	{
		if(INSTANCE == nullptr)
		{
			INSTANCE = new TextureManager(dev);
		}
	}

	static void release()
	{
		if(INSTANCE != nullptr)
			delete INSTANCE;
	}

	static TextureManager& getInstance() { return *INSTANCE; }
	static const com_ptr<ID3D11SamplerState>& getDefaultSamplerState() { return DEFAULT_SAMPLER_STATE; }

	void add(const std::string&);
	void addDirectory(const std::string&);
	bool remove(const com_ptr<ID3D11ShaderResourceView>&);
	bool remove(const std::string&);
	const com_ptr<ID3D11ShaderResourceView>& get(const std::string&) const;



};