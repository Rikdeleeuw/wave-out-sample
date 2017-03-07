#include "BinReadFile.hpp"

using namespace std;

BinReadFile::BinReadFile(const wstring& path) : file(nullptr) {
	if (_wfopen_s(&this->file, path.c_str(), L"rb") != 0) throw exception("Can't open file");
}

BinReadFile::~BinReadFile() {
	if (this->file)fclose(this->file);
}

size_t BinReadFile::read(void* dst, size_t size) {
	return fread(dst, 1, size, this->file);
}

uint64_t BinReadFile::getPos() const {
	return _ftelli64(this->file);
}

bool BinReadFile::setPos(uint64_t pos) {
	return _fseeki64(this->file, pos, SEEK_SET) == 0;
}

bool BinReadFile::skip(uint64_t pos) {
	return _fseeki64(this->file, pos, SEEK_CUR) == 0;
}