#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <array>
#include <condition_variable>
#include <mutex>
#include "WaveFile.h"

class WaveOutDevice
{
private:
	std::unique_ptr<WaveFile> src;

	HWAVEOUT hDev;

	std::array<WAVEHDR, 2> buffers;

	std::mutex m;
	std::condition_variable cv;
	bool done;

public:
	WaveOutDevice(std::unique_ptr<WaveFile>&& src);
	WaveOutDevice(WaveOutDevice const&) = delete;
	~WaveOutDevice();

	void operator =(WaveOutDevice const&) = delete;

	static void staticWaveProc(HWAVEOUT, UINT msg, DWORD_PTR instance, DWORD_PTR param1, DWORD_PTR param2);

	void waveProc(UINT msg, DWORD_PTR param1, DWORD_PTR param2);

	void start();
};

