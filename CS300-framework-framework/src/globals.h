#pragma once
#include <tuple>
#include "models.h"

namespace global
{
	static const float gWidth = 1280.f;
	static const float gHeight = 720.f;
	enum  eModelList
	{
		GROUND = 0,
		BOX,
		SPHERE,
		MAX_MODEL_COUNT
	};

	enum class eObjectType : int
	{
		GAMEOBJECT = 0,
		SKYBOX,
		AMBIENT_LIGHT,
		DIRECTIONAL_LIGHT,
		POINT_LIGHT,
		SPOT_LIGHT,
	};

	enum eObjectMaterialType
	{
		COLOR = 0,
		LIGHT_COLOR,
		TEXTURE,
		TEXTURE_NORMAL,
		TEXTURE_NORMAL_SPECULAR,
		TEXTURE_SPECULAR,
		TEXTURE_SKYBOX,
		MAX_MATERIAL_COUNT,
	};

	enum eLightingType
	{
		NO_LIGHTING = 0,
		BLINN_PHONG,
		MAX_LIGHTING_COUNT,
	};

	enum eTexture
	{
		GROUND_DIFFUSE = 0,
		BOX_DIFFUSE,
		MAX_TEXTURE_COUNT,
	};
}