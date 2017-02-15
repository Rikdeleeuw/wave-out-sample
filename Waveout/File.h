#pragma once

#include <string>
#include <memory>

class File
{
private:
	FILE* file;

public:
	File(const std::wstring& path);
	File(File const&) = delete;
	virtual ~File();

	void operator =(File const &) = delete;

	template <typename T>
	bool read(T& dst);

	size_t read(void* dst, size_t size);
};

template <typename T>
bool File::read(T& dst) {
	return fread_s(&dst, sizeof(T), sizeof(T), 1, this->file) == 1;
}

