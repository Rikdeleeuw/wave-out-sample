#include "stdafx.h"
#include "WaveFile.h"

using namespace std;

struct WaveHdr {
	// riff chunk start
	char riff[4];
	uint32_t riffSize;
	// wave chunk start
	char wave[4];
	// fmt chunk
	char fmt_[4];
	uint32_t fmtSize;
	uint16_t audioFormat;
	uint16_t channelCount;
	uint32_t sampleRate;
	uint32_t byteRate;
	uint16_t blockAlign;
	uint16_t bitsPerSample;
	// data chunk start
	char data[4];
	uint32_t dataSize;
};

const static auto minRiffSize = sizeof(WaveHdr) - (sizeof(WaveHdr::riff) + sizeof(WaveHdr::riffSize));

WaveFile::WaveFile(unique_ptr<File>&& src) : src(move(src)), validFormat(false) {
	if (!this->src) throw exception("Invalid File");

	// read complete wave hdr
	WaveHdr hdr;
	this->src->read(hdr);
	if ((memcmp("RIFF", hdr.riff, 4) != 0)
		|| (memcmp("WAVE", hdr.wave, 4) != 0)
		|| (memcmp("fmt ", hdr.fmt_, 4) != 0)
		|| (memcmp("data", hdr.data, 4) != 0)
		|| (hdr.audioFormat != 1)
		|| (hdr.bitsPerSample == 0)
		|| (hdr.channelCount == 0)
		|| (hdr.sampleRate == 0)
		|| (hdr.fmtSize != 16)
		|| (hdr.riffSize <= minRiffSize)
		|| (hdr.dataSize == 0)) throw exception("Malformed wave header");

	this->channelCount = hdr.channelCount;
	this->sampleRate = hdr.sampleRate;
	this->bitsPerSample = hdr.bitsPerSample;
	this->validFormat = true;
}

WaveFile::~WaveFile() {
}

uint16_t WaveFile::getChannelCount() const {
	return this->channelCount;
}

uint32_t WaveFile::getSampleRate() const {
	return this->sampleRate;
}

uint16_t WaveFile::getBitsPerSample() const {
	return this->bitsPerSample;
}

size_t WaveFile::read(void* dst, size_t size) {
	return this->src->read(dst, size);
}
