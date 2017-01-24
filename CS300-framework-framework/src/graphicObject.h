#pragma once

#include "object.h"
#include "models.h"
#include "globals.h"

class graphicObject : public object
{
public:
	graphicObject();
	~graphicObject();
	void initialize();
	void update();

private:
	global::gModelList modelType;
};