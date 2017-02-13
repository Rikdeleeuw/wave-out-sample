#include "stdafx.h"
#include "File.h"

using namespace std;

File::File(const wstring& path) : file(nullptr)
{
	auto error = _wfopen_s(&this->file, path.c_str(), L"rb");
	if (error != 0) {
		wprintf(L"Could not find %s\n", path.c_str());
		this->file = nullptr;
	}
}

File::~File()
{
	if (this->file)fclose(this->file);
}

bool File::valid() const {
	return this->file != nullptr;
}
