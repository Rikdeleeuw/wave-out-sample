#pragma once

#include <string>
#include <memory>

class File
{
protected:
	FILE* file;

public:
	File(const std::wstring& path);
	virtual ~File();

	bool valid() const;
};

