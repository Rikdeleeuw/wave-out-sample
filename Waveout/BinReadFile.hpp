#pragma once

#include <string>
#include <memory>

class BinReadFile
{
private:
	FILE* file;

public:
	BinReadFile(const std::wstring& path);
	BinReadFile(BinReadFile const&) = delete;
	virtual ~BinReadFile();

	void operator =(BinReadFile const &) = delete;

	template <typename T>
	bool read(T& dst);

	size_t read(void* dst, size_t size);
};

template <typename T>
bool BinReadFile::read(T& dst) {
	return fread_s(&dst, sizeof(T), sizeof(T), 1, this->file) == 1;
}

