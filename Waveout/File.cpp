#include "stdafx.h"
#include "File.h"

using namespace std;

File::File(const wstring& path) : file(nullptr) {
	if (_wfopen_s(&this->file, path.c_str(), L"rb") != 0) throw exception("Can't open file");
}

File::~File() {
	if (this->file)fclose(this->file);
}

size_t File::read(void* dst, size_t size) {
	return fread(dst, 1, size, this->file);
}
