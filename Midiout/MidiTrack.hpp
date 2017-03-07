#pragma once

#include <array>
#include <memory>
#include <Windows.h>
#include "Storage/BinReadFile.hpp"

class MidiInFile;

struct midiEvent_t {
	DWORD deltaTime;
	DWORD streamID;
	DWORD event;
};

class MidiTrack
{
private:
	uint64_t trackBegin;
	uint64_t trackEnd;
	uint64_t trackIter;
	uint8_t lastCmd;
	MidiInFile& src;

	uint32_t curTickCount;
	std::array<midiEvent_t, 1024> buffer;
	uint32_t index;
	uint32_t endIndex;
	bool ended;
public:
	MidiTrack(MidiInFile& src, uint64_t fileOffset, uint32_t size);
	~MidiTrack();

	bool hasEnded() const;
	midiEvent_t getNextEvent(uint32_t songTickCount, const BinReadFile* src);
	midiEvent_t peekNextEvent(uint32_t songTickCount, const BinReadFile* src);

private:
	void readBuffer(const BinReadFile* src);
	void readEvent();
	uint32_t readVariableLength();
	bool skip(uint64_t size);

	template <typename T>
	bool read(T& dst);
};

template <typename T>
bool MidiTrack::read(T& dst) {
	bool res = false;
	if ((this->trackIter + sizeof(T)) <= this->trackEnd) {
		res = this->src.read(dst);
		this->trackIter += sizeof(T);
		if (!res || (this->trackIter == this->trackEnd)) this->ended = true;
	}
	else {
		this->ended = true;
	}
	return res;
}

#include "MidiInFile.hpp"

