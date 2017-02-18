#pragma once

#include <array>
#include <Windows.h>
#include "MidiInFile.hpp"

class MidiOutDevice {
private:
	std::unique_ptr<MidiInFile> src;

	HMIDISTRM dev;
	std::array<MIDIHDR, 2> buffers;

public:
	MidiOutDevice(std::unique_ptr<MidiInFile>&& src);
	~MidiOutDevice();

	void start();

private:
	static void CALLBACK staticMidiProc(HMIDIOUT, UINT msg, DWORD_PTR instance, DWORD_PTR param1, DWORD_PTR param2);
	void midiProc(UINT msg, DWORD_PTR param1, DWORD_PTR param2);

};

