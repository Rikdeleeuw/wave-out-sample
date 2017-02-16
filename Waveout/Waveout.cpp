#include <cwchar>
#include <iostream>
#include "WaveInFile.hpp"
#include "WaveOutDevice.hpp"

using namespace std;

/**
 * \brief Application entry point for Waveout.
 *
 * \param[in]	argc	Number of commandline arguments.
 * \param[in]	argv	Array of commandline arguments.
 */
int wmain(int argc, wchar_t** argv) {
	if (argc < 2) {
		wprintf(L"missing commandline parametewr: *.wav file to play\n");
		return -1;
	}
	try {
		// file->decoder->audio device
		auto f = make_unique<BinReadFile>(argv[1]);
		auto fwave = make_unique<WaveInFile>(move(f));
		auto dwave = make_unique<WaveOutDevice>(move(fwave));
		
		// start the playback
		dwave->start();
	} catch (exception e) {
		cout << e.what() << endl;
	}
    return 0;
}

