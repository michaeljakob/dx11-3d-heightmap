#include "TextureManager.h"
#include <cassert>
#include <D3DX11.h>
#include <vector>
#include <algorithm>

using namespace std;


TextureManager *TextureManager::INSTANCE = nullptr;
const std::string TextureManager::TEXTURE_FOLDER = "./textures/";
com_ptr<ID3D11SamplerState> TextureManager::DEFAULT_SAMPLER_STATE;



void TextureManager::add(const std::string& name)
{
	std::string completeName = TEXTURE_FOLDER+name;
#ifdef _DEBUG
	auto iter = mTextures.find(completeName);
	if(iter != mTextures.cend())
		assert(iter->first != name && "Texture already added");
#endif

	D3DX11_IMAGE_LOAD_INFO ili;
	ili.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	ili.Format = DXGI_FORMAT_BC1_UNORM;

	com_ptr<ID3D11ShaderResourceView> newTexture;
	if(FAILED(D3DX11CreateShaderResourceViewFromFile(mDev, completeName.c_str(), &ili, nullptr, &newTexture, nullptr)))
	{
		throw std::runtime_error("Loading texture failed with argument name='" + name);
	}

	mTextures[name] = newTexture;
}

void TextureManager::addDirectory(const std::string& dirname)
{
	assert(GetFileAttributes((TEXTURE_FOLDER + dirname).c_str()) != INVALID_FILE_ATTRIBUTES);
	vector<string> files;

	// valid file extensions
	vector<string> allowedExt;
	allowedExt.reserve(16);
	allowedExt.push_back("jpg");
	allowedExt.push_back("jpeg");
	allowedExt.push_back("dds");
	allowedExt.push_back("png");
	allowedExt.push_back("bmp");
	allowedExt.push_back("raw");
	allowedExt.push_back("tiff");


	// TODO optimize this loopings
	WIN32_FIND_DATAA w32FD;
	HANDLE hDir = FindFirstFile((TEXTURE_FOLDER + dirname + "\\*").c_str(), &w32FD);	// get the 'directory-handle'
	while(FindNextFile(hDir, &w32FD))
	{
		string s = w32FD.cFileName;
		string ext = s.substr(s.rfind('.')+1, s.size());
		transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

		if(find(allowedExt.cbegin(), allowedExt.cend(), ext) != allowedExt.cend())
		{
			files.push_back(w32FD.cFileName);
		}
		
	}

	// create textures
	for(size_t i=0; i<files.size(); ++i)
	{
		add(files[i]);
	}
}

/**
	Removes the item with the associated texture. Will remove only one item.
	To remove all items, call this method until it returns false.
**/
bool TextureManager::remove(const com_ptr<ID3D11ShaderResourceView>& texture)
{
	for(auto it = mTextures.cbegin(); it != mTextures.cend(); ++it)
	{
		if(it->second == texture)
		{
			mTextures.erase(it);
			return true;
		}
	}

	return false;
}


bool TextureManager::remove(const std::string& name)
{
	return mTextures.erase(name) == 1;
}

const com_ptr<ID3D11ShaderResourceView>& TextureManager::get(const std::string& name) const
{
	auto iter = mTextures.find(name);
	if(iter != mTextures.cend())
	{
		return iter->second;
	}
	else
	{
		throw std::invalid_argument("No item with key " + name + " found.");
	}
}













