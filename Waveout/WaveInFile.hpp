#pragma once

#include "BinReadFile.hpp"

class WaveInFile
{
private:
	bool validFormat;
	uint16_t channelCount;
	uint32_t sampleRate;
	uint16_t bitsPerSample;

	std::unique_ptr<BinReadFile> src;

public:
	WaveInFile(std::unique_ptr<BinReadFile>&& src);
	~WaveInFile();

	uint16_t getChannelCount() const;
	uint32_t getSampleRate() const;
	uint16_t getBitsPerSample() const;

	size_t read(void* dst, size_t size);
};

