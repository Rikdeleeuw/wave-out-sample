#pragma once
#include <vector>
#include <array>
#include "Storage/BinReadFile.hpp" 
#include "MidiTrack.hpp"

class MidiInFile
{
	friend MidiTrack;
private:
	std::unique_ptr<BinReadFile> src;
	
	std::vector<MidiTrack> trackPtrs;

	int16_t tickCount;
	int32_t curTickCount;

public:
	MidiInFile(std::unique_ptr<BinReadFile>&& src);
	~MidiInFile();

	int16_t getTickCount() const;
	size_t read(midiEvent_t* dst, size_t count, unsigned int trackIndex);
	size_t read(midiEvent_t* dst, size_t count);

private:
	// incase we want to protect it with a mutex in the future
	template <typename T>
	bool read(T& dst);

	bool skip(uint64_t size);
	bool setPos(uint64_t pos);
};

template <typename T>
bool MidiInFile::read(T& dst) {
	return this->src->read<T>(dst);
}
