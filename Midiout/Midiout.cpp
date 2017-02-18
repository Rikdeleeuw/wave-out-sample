#include <cwchar>
#include <iostream>
#include <Windows.h>
#include "MidiOutDevice.hpp"

using namespace std;

void CALLBACK midiOutProc(HMIDIOUT, UINT msg, DWORD_PTR instance, DWORD_PTR param1, DWORD_PTR param2) {

}

/**
* \brief Application entry point for Midiout.
*
* \param[in]	argc	Number of commandline arguments.
* \param[in]	argv	Array of commandline arguments.
*/
int wmain(int argc, wchar_t** argv)
{
	if (argc < 2) {
		wprintf(L"missing commandline parametewr: *.mid file to play\n");
		return -1;
	}
	try {
		// file->decoder->audio device
		auto f = make_unique<BinReadFile>(argv[1]);
		auto fmidi = make_unique<MidiInFile>(move(f));
		auto dmidi = make_unique<MidiOutDevice>(move(fmidi));

		dmidi->start();
	}
	catch (exception e) {
		cout << e.what() << endl;
	}
	return 0;
}

