#include "stdafx.h"
#include "WaveFile.h"

using namespace std;

struct RiffHdr {
	char riff[4];
	uint32_t size;
	char wave[4];
};

struct FmtHdr {
	char fmt_[4];
	uint32_t size;
	uint16_t audioFormat;
	uint16_t channelCount;
	uint32_t sampleRate;
	uint32_t byteRate;
	uint16_t blockAlign;
	uint16_t bitsPerSample;
};

struct DataHdr {
	char data[4];
	uint32_t size;
};


WaveFile::WaveFile(const wstring& path) : File(path), validFormat(false)
{
	if (!this->File::valid()) return;

	// read RIFF header
	RiffHdr riffHdr;
	auto size = fread_s(&riffHdr, sizeof(riffHdr), sizeof(riffHdr), 1, this->file);
	if ((size != 1) || (memcmp("RIFF", riffHdr.riff, 4) != 0) || (memcmp("WAVE", riffHdr.wave, 4) != 0) || (riffHdr.size <= (sizeof(riffHdr.wave) + sizeof(FmtHdr) + sizeof(DataHdr)))) return;
		
	// read Format header
	FmtHdr fmtHdr;
	size = fread_s(&fmtHdr, sizeof(fmtHdr), sizeof(fmtHdr), 1, this->file);
	if ((size != 1) || (memcmp("fmt ", fmtHdr.fmt_, 4) != 0) || (fmtHdr.size != 16) || (fmtHdr.audioFormat != 1) || (fmtHdr.bitsPerSample == 0) || (fmtHdr.channelCount == 0) || (fmtHdr.sampleRate == 0)) return;

	// read Data header
	DataHdr dataHdr;
	size = fread_s(&dataHdr, sizeof(dataHdr), sizeof(dataHdr), 1, this->file);
	if ((size != 1) || (memcmp("data", dataHdr.data, 4) != 0)) return;

	this->channelCount = fmtHdr.channelCount;
	this->sampleRate = fmtHdr.sampleRate;
	this->bitsPerSample = fmtHdr.bitsPerSample;

	wprintf(L"Wave format: %u bits %s @ %u Hz %s\n", this->bitsPerSample, (this->channelCount == 1) ? L"Mono" : ((this->channelCount == 2) ? L"Stereo" : L"Surround sound"), this->sampleRate, path.c_str());

	this->validFormat = true;
}

WaveFile::~WaveFile()
{
}

bool WaveFile::valid() const {
	return this->File::valid() && this->validFormat;
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
