#include "manager.h"
#include "object.h"

template <class T>
manager<T>& manager<T>::getSingleton()
{
	static manager instance;
	return instance;
}

template <class T>
void manager<T>::add(T *obj)
{
	mManagerContainer.push_back(obj);
}

template <class T>
void manager<T>::update()
{
	for (auto obj : mManagerContainer)
	{
		obj->update();
	}
}

template <class T>
void manager<T>::shutdown()
{
	for (auto obj : mManagerContainer)
	{
		if (obj != nullptr)
		{
			delete obj;
		}
	}
}
