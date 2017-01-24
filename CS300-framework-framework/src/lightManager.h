#pragma once

#include <vector>

class light;

class lightManager
{
public:
	static lightManager* getSingleTon();
	~lightManager();

private:
	lightManager();
	lightManager* pLightMgr;
	typedef std::vector<light> lightContainer;
	lightContainer mLightContainer;
};

lightManager::lightManager()
{
}

lightManager::~lightManager()
{
}
