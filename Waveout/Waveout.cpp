// Waveout.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cwchar>
#include <iostream>
#include "WaveFile.h"
#include "WaveOutDevice.h"

using namespace std;

int wmain(int argc, wchar_t** argv) {
	if (argc < 2) {
		wprintf(L"missing commandline parametewr: *.wav file to play\n");
		return -1;
	}
	try {
		auto f = make_unique<File>(argv[1]);
		auto fwave = make_unique<WaveFile>(move(f));
		auto dwave = make_unique<WaveOutDevice>(move(fwave));
		dwave->start();
	} catch (exception e) {
		cout << e.what() << endl;
	}
    return 0;
}

