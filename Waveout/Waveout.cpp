// Waveout.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cwchar>
#include "WaveFile.h"
#include "WaveOutDevice.h"

int wmain(int argc, wchar_t** argv)
{
	if (argc < 2) {
		wprintf(L"missing commandline parametewr: *.wav file to play\n");
		return -1;
	}
	WaveFile fwave(argv[1]);
	if (!fwave.valid()) return -1;

	WaveOutDevice dwave(fwave.getSampleRate(), fwave.getBitsPerSample(), fwave.getChannelCount());

    return 0;
}

