#include "MidiTrack.hpp"

MidiTrack::MidiTrack(MidiInFile& src, uint64_t fileOffset, uint32_t size) : src(src), lastCmd(0), ended(size == 0), trackPos(fileOffset), endIndex(0), index(0), curTickCount(0)
{
	this->buffer.fill({ 0 });
}

MidiTrack::~MidiTrack()
{
}

bool MidiTrack::hasEnded() const {
	return this->ended && (this->trackPos < this->trackEnd);
}

midiEvent_t MidiTrack::getNextEvent(uint32_t songTickCount, const BinReadFile* src) {
	midiEvent_t res = this->peekNextEvent(songTickCount, src);
	this->curTickCount += this->buffer[this->index].deltaTime;
	++this->index;
	return res;
}

midiEvent_t MidiTrack::peekNextEvent(uint32_t songTickCount, const BinReadFile* src) {
	if (this->index >= this->endIndex) {
		if (!this->ended) {
			readBuffer(src);
			if ((this->endIndex == 0) && this->ended) return{ 0 };
		}
		else {
			return{ 0 };
		}
	}
	midiEvent_t res = this->buffer[this->index];
	res.deltaTime = (res.deltaTime + this->curTickCount) - songTickCount;
	return res;
}

void MidiTrack::readBuffer(const BinReadFile* src) {
	this->src.setPos(this->trackPos);
	uint32_t readBytes = 0;

	for (this->index = 0; (this->index < this->buffer.size()) && (!this->ended);) {
		readEvent();
	}
	this->endIndex = this->index;
	this->index = 0;
}

void MidiTrack::readEvent() {
	uint32_t delay = this->readVariableLength();
	if (this->ended) return;
	uint8_t cmd;
	if (!this->read(cmd)) return;

	if (!(cmd & 0x80)) {
		// continuation of previous message
		uint8_t param1 = cmd;
		cmd = this->lastCmd;
		this->buffer[this->index].deltaTime = delay;
		this->buffer[this->index].event = (static_cast<DWORD>(MEVT_SHORTMSG) << 24) | static_cast<DWORD>(cmd) | (static_cast<DWORD>(param1) << 8);
		// add optional second argument
		if (!(((cmd & 0xF0) == 0xC0) || ((cmd & 0xF0) == 0xD0))) {
			if (this->ended) return;
			uint8_t param2;
			if (!this->read(param2)) return;
			this->buffer[this->index].event |= static_cast<DWORD>(param2) << 16;
		}
		// event is valid increase valid buffer length
		++this->index;
	} else if (!this->ended) { // check for premature ending of track
		if (cmd == 0xFF) {
			// meta command
			uint8_t meta;
			if (!this->read(meta)) return;
			if (meta == 0x51) {
				if (this->readVariableLength() != 3) return;
				uint8_t param[3];
				if (!this->read(param)) return;
				this->buffer[this->index].deltaTime = delay;
				this->buffer[this->index].event = (static_cast<DWORD>(MEVT_TEMPO) << 24) | (static_cast<DWORD>(param[0]) << 16) | (static_cast<DWORD>(param[1]) << 8) | static_cast<DWORD>(param[2]);
				++this->index;
			}
			else {
				uint32_t metaLength = this->readVariableLength();
				if (!this->skip(metaLength)) return;
			}
		}
		else if ((cmd & 0xF0) != 0xF0) {
			// new message
			this->lastCmd = cmd;
			uint8_t param1;
			if (!this->read(param1)) return;
			this->buffer[this->index].deltaTime = delay;
			this->buffer[this->index].event = (static_cast<DWORD>(MEVT_SHORTMSG) << 24) | static_cast<DWORD>(cmd) | (static_cast<DWORD>(param1) << 8);
			if (!(((cmd & 0xF0) == 0xC0) || ((cmd & 0xF0) == 0xD0))) {
				uint8_t param2;
				if (!this->read(param2)) return;
				this->buffer[this->index].event |= static_cast<DWORD>(param2) << 16;
			}
			++this->index;
		}
	} 
	// valid end of track?
	if (this->trackPos >= this->trackEnd) this->ended = true;
}

uint32_t MidiTrack::readVariableLength() {
	uint32_t res = 0;
	for (int i = 0; (i < 4) && (this->trackPos < this->trackEnd); ++i) {
		uint8_t nextVal = 0;
		if (!src.read(nextVal)) {
			this->ended = true;
			break;
		}
		++this->trackPos;
		res = (res << 7) + (nextVal & 0x7F);
		if ((nextVal & 0x80) == 0) break;
	}
	if (this->trackPos == this->trackEnd) this->ended = true;
	return res;
}

bool MidiTrack::skip(uint64_t size) {
	bool res = false;
	if ((this->trackPos + size) <= this->trackEnd) {
		res = this->src.skip(size);
		this->trackPos += size;
		if (!res || (this->trackPos == this->trackEnd)) this->ended = true;
	} else {
		this->ended = true;
	}
	return res;
}
