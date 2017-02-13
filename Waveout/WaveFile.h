#pragma once
#include "File.h"

class WaveFile : public File
{
private:
	bool validFormat;
	uint16_t channelCount;
	uint32_t sampleRate;
	uint16_t bitsPerSample;

public:
	WaveFile(const std::wstring& path);
	~WaveFile();

	bool valid() const;

	uint16_t getChannelCount() const;
	uint32_t getSampleRate() const;
	uint16_t getBitsPerSample() const;
};

