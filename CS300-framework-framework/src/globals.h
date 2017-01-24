#pragma once
#include "camera.h"

namespace global
{
	static int gWidth = 1280;
	static int gHeight = 720;
	static enum class gModelList : int
	{
		GROUND = 0,
		BOX,
		SPHERE,
		MAX_MODEL_COUNT
	};

	static camera gEditorCamera;
}