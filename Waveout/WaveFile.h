#pragma once
#include "File.h"

class WaveFile
{
private:
	bool validFormat;
	uint16_t channelCount;
	uint32_t sampleRate;
	uint16_t bitsPerSample;

	std::unique_ptr<File> src;

public:
	WaveFile(std::unique_ptr<File>&& src);
	~WaveFile();

	uint16_t getChannelCount() const;
	uint32_t getSampleRate() const;
	uint16_t getBitsPerSample() const;

	size_t read(void* dst, size_t size);
};

