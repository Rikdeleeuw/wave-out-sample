#include "WaveInFile.hpp"
#include "Windows.h"

using namespace std;

/**
 * \brief Common WAVE audio file header structure;
 */
struct WaveHdr {
	char riff[4];						//< \brief Magic "RIFF".
	uint32_t riffSize;				//< \brief Size of the file after this field in bytes.
	char wave[4];					//< \brief Magic "WAVE".

	// fmt chunk
	char fmt_[4];					//< \brief Magic "fmt ".
	uint32_t fmtSize;				//< \brief Size of the fmt chunk after this field in bytes (16).
	uint16_t audioFormat;		//< \brief Audio codec used we only support pcm audio.
	uint16_t channelCount;	//< \brief Number of channels in the audio signal (1 mono, 2 stereo...).
	uint32_t sampleRate;		//< \brief Sample rate of the audio signal in samples/second.
	uint32_t byteRate;			//< \brief Byte rate of the audio sample in bytes/second.
	uint16_t blockAlign;		//< \brief Size of one sample played on all channels in bytes.
	uint16_t bitsPerSample;	//< \brief Size of a sample for a single channel in bits.
	
	// data chunk
	char data[4];						//< \brief Magic "data".
	uint32_t dataSize;			//< \brief size of the rest of the file (the raw audio data).
};

const static auto minRiffSize = sizeof(WaveHdr) - (sizeof(WaveHdr::riff) + sizeof(WaveHdr::riffSize));

WaveInFile::WaveInFile(unique_ptr<BinReadFile>&& src) : src(move(src)) {
	if (!this->src) throw exception("Invalid File");

	// read complete wave hdr
	WaveHdr hdr;
	this->src->read(hdr);
	// check for header corruption
	if ((memcmp("RIFF", hdr.riff, 4) != 0)
		|| (memcmp("WAVE", hdr.wave, 4) != 0)
		|| (memcmp("fmt ", hdr.fmt_, 4) != 0)
		|| (memcmp("data", hdr.data, 4) != 0)
		|| (hdr.audioFormat != WAVE_FORMAT_PCM)
		|| (hdr.bitsPerSample == 0)
		|| (hdr.channelCount == 0)
		|| (hdr.sampleRate == 0)
		|| (hdr.fmtSize != 16)
		|| (hdr.riffSize <= minRiffSize)
		|| (hdr.dataSize == 0)) throw exception("Malformed wave header");

	// stroe important information
	this->channelCount = hdr.channelCount;
	this->sampleRate = hdr.sampleRate;
	this->bitsPerSample = hdr.bitsPerSample;
}

WaveInFile::~WaveInFile() {
}

uint16_t WaveInFile::getChannelCount() const {
	return this->channelCount;
}

uint32_t WaveInFile::getSampleRate() const {
	return this->sampleRate;
}

uint16_t WaveInFile::getBitsPerSample() const {
	return this->bitsPerSample;
}

size_t WaveInFile::read(void* dst, size_t size) {
	return this->src->read(dst, size);
}
