#pragma once

#include <xnamath.h>

struct SpotLight
{
	XMFLOAT4 pos;
	XMFLOAT4 dir;
	XMFLOAT4 col;
};

struct DirectionalLight
{
	XMFLOAT4 dir;
	XMFLOAT4 col;
};

struct PointLight
{
	XMFLOAT4 pos;
	XMFLOAT4 col;
};
