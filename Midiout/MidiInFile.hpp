#pragma once
#include <vector>
#include "../Waveout/BinReadFile.hpp" 

class MidiInFile
{
private:
	std::unique_ptr<BinReadFile> src;
	
	struct trackEntry_t {
		uint64_t fileOffset;
		uint32_t size;
	};
	std::vector<trackEntry_t> trackPtrs;

	int16_t tickCount;

public:
	MidiInFile(std::unique_ptr<BinReadFile>&& src);
	~MidiInFile();

	int16_t getTickCount() const;
	size_t read(void* dst, size_t size, unsigned int trackIndex);

private:
	uint32_t readVariableLength(uint32_t& dst);
};

