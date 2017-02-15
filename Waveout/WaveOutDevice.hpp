#pragma once

#include <array>
#include <condition_variable>
#include <mutex>
#include <Windows.h>
#include "WaveInFile.hpp"


class WaveOutDevice
{
private:
	std::unique_ptr<WaveInFile> src;

	HWAVEOUT hDev;

	std::array<WAVEHDR, 2> buffers;

	std::mutex m;
	std::condition_variable cv;
	bool done;

public:
	WaveOutDevice(std::unique_ptr<WaveInFile>&& src);
	WaveOutDevice(WaveOutDevice const&) = delete;
	~WaveOutDevice();

	void operator =(WaveOutDevice const&) = delete;

	static void staticWaveProc(HWAVEOUT, UINT msg, DWORD_PTR instance, DWORD_PTR param1, DWORD_PTR param2);

	void waveProc(UINT msg, DWORD_PTR param1, DWORD_PTR param2);

	void start();
};
