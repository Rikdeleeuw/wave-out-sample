#include <iostream>
#include <Windows.h>
#include "MidiInFile.hpp"

using namespace std;

#pragma pack(push,2)
struct header_t {
	char mthd[4];
	uint32_t size;
	uint16_t format;
	uint16_t trackCount;
	int16_t deltaTime;
};
#pragma pack(pop)
static_assert(sizeof(header_t) == 14, "pack failed");

struct track_t {
	char mtrk[4];
	uint32_t size;
};

MidiInFile::MidiInFile(unique_ptr<BinReadFile>&& src) : src(move(src)), curTickCount(0) {
	if (!this->src) throw exception("Invalid File");

	header_t hdr;
	if ((!this->src->read(hdr))
		|| (memcmp(hdr.mthd, "MThd", 4) != 0)) throw exception("Unsupported midi format");
	hdr.size = _byteswap_ulong(hdr.size);
	hdr.format = _byteswap_ushort(hdr.format);
	hdr.trackCount = _byteswap_ushort(hdr.trackCount);
	uint16_t temp = _byteswap_ushort(*reinterpret_cast<uint16_t*>(&hdr.deltaTime));
	hdr.deltaTime = *reinterpret_cast<int16_t*>(&temp);
	this->tickCount = hdr.deltaTime;
	if ((hdr.size != 6)
		|| (hdr.format > 1) // we don't support multi song files
		|| (hdr.trackCount == 0)) throw exception("Unsupported midi format");
	track_t curTrack;
	this->trackPtrs.reserve(hdr.trackCount);
	for (uint16_t trackIndex = 0; trackIndex < hdr.trackCount; ++trackIndex) {
		if (!this->src->read(curTrack)
			|| (memcmp(curTrack.mtrk, "MTrk", 4) != 0)
			|| (curTrack.size == 0)) throw exception("Unsupported midi format");
		curTrack.size = _byteswap_ulong(curTrack.size);
		this->trackPtrs.push_back(MidiTrack(*this, this->src->getPos(), curTrack.size));
		this->src->skip(curTrack.size);
	}
}

MidiInFile::~MidiInFile() {
}

int16_t MidiInFile::getTickCount() const {
	return this->tickCount;
}

size_t MidiInFile::read(midiEvent_t* dst, size_t count, unsigned int trackIndex) {
	if (trackIndex >= this->trackPtrs.size()) return 0;
	uint32_t res = 0;
	for (uint32_t i = 0; i < count; ++i) {
		dst[i] = this->trackPtrs[trackIndex].getNextEvent(this->curTickCount, this->src.get());
		if ((dst[i].event == 0) && this->trackPtrs[trackIndex].hasEnded()) {
			break;
		}
		else {
			++res;
			this->curTickCount += dst[i].deltaTime;
		}
	}
	return res;
}

size_t MidiInFile::read(midiEvent_t* dst, size_t count) {
	uint32_t res = 0;
	for (uint32_t i = 0; i < count; ++i) {
		midiEvent_t first = { 0xFFFFFFFF, 0, 0 };
		uint32_t firstTrackIndex = 0;
		for (unsigned int trackIndex = 0; trackIndex < this->trackPtrs.size(); ++trackIndex) {
			midiEvent_t temp = this->trackPtrs[trackIndex].peekNextEvent(this->curTickCount, this->src.get());
			if ((temp.event == 0) && this->trackPtrs[trackIndex].hasEnded()) {
				continue;
			}
			else {
				if (first.deltaTime > temp.deltaTime) {
					first = temp;
					firstTrackIndex = trackIndex;
				}
			}
		}
		if (first.event == 0) {
			break;
		}
		else {
			this->trackPtrs[firstTrackIndex].getNextEvent(this->curTickCount, this->src.get());
			dst[i] = first;
			++res;
			this->curTickCount += dst[i].deltaTime;
		}
	}
	return res;
}

bool MidiInFile::skip(uint64_t size) {
	return this->src->skip(size);
}

bool MidiInFile::setPos(uint64_t pos) {
	return this->src->setPos(pos);
}