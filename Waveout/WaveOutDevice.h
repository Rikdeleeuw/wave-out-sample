#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>

class WaveOutDevice
{
private:
	DWORD sampleRate;
	WORD bitsPerSample;
	WORD channelCount;

	HWAVEOUT hDev;
public:
	WaveOutDevice(DWORD sampleRate, WORD bitsPerSample, WORD channelCount);
	~WaveOutDevice();

	bool valid() const;
};

