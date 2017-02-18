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

MidiInFile::MidiInFile(unique_ptr<BinReadFile>&& src) : src(move(src)) {
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
		this->trackPtrs.push_back({this->src->getPos(), curTrack.size});
		this->src->setPos(this->trackPtrs[trackIndex].fileOffset + this->trackPtrs[trackIndex].size);
	}
}

MidiInFile::~MidiInFile() {
}

uint32_t MidiInFile::readVariableLength(uint32_t &dst) {
	uint32_t readBytes = 0;
	for (int i = 0; i < 4; ++i) {
		uint8_t nextVal = 0;
		if (!this->src->read(nextVal)) break;
		++readBytes;
		dst = (dst << 7) + (nextVal & 0x7F);
		if ((nextVal & 0x80) == 0) break;
	}
	return readBytes;
}

int16_t MidiInFile::getTickCount() const {
	return this->tickCount;
}

size_t MidiInFile::read(void* dst, size_t size, unsigned int trackIndex) {
	auto& track = this->trackPtrs[trackIndex];
	this->src->setPos(track.fileOffset);
	uint32_t readBytes = 0;
	
	uint32_t delay;
	readBytes += readVariableLength(delay);

	track.fileOffset += readBytes;
	track.size -= readBytes;
	return 0;
}
