#include "stdafx.h"
#include "WaveOutDevice.h"


WaveOutDevice::WaveOutDevice(DWORD sampleRate, WORD bitsPerSample, WORD channelCount) : sampleRate(sampleRate), bitsPerSample(bitsPerSample), channelCount(channelCount), hDev(nullptr)
{
	WAVEFORMATEX devFmt;
	devFmt.nSamplesPerSec = this->sampleRate;
	devFmt.wBitsPerSample = this->bitsPerSample;
	devFmt.nChannels = this->channelCount;

	devFmt.cbSize = 0;
	devFmt.wFormatTag = WAVE_FORMAT_PCM;
	devFmt.nBlockAlign = (devFmt.wBitsPerSample / 8) * devFmt.nChannels;
	devFmt.nAvgBytesPerSec = devFmt.nBlockAlign * devFmt.nSamplesPerSec;

	auto err = waveOutOpen(&hDev, WAVE_MAPPER, &devFmt, 0, 0, CALLBACK_NULL);
	if (err != MMSYSERR_NOERROR) hDev = nullptr;

	wprintf(L"Wave format: %u bits %s @ %u Hz WAVE_MAPPER\n", this->bitsPerSample, (this->channelCount == 1) ? L"Mono" : ((this->channelCount == 2) ? L"Stereo" : L"Surround sound"), this->sampleRate);
}

WaveOutDevice::~WaveOutDevice()
{
	if (hDev != nullptr) waveOutClose(hDev);
}

bool WaveOutDevice::valid() const {
	return hDev != nullptr;
}
