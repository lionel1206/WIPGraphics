#pragma once

class object;

class singleton
{
public:
	static singleton& getSingleton()
	{
		static singleton instance;
		return instance;
	}

	singleton(singleton const& rhs) = delete; // do not want the copy constructor
	singleton(singleton&& rhs) = delete; // do not want move constructor
	singleton& operator=(singleton const& rhs) = delete; // do not want assignment operator
	singleton& operator=(singleton&& rhs) = delete; // do not want move operator

private:
	singleton();
	~singleton() = default; // want the default destructor always
};

