#include <exception>
#include "WaveOutDevice.hpp"

using namespace std;

WaveOutDevice::WaveOutDevice(unique_ptr<WaveInFile>&& src) : src(move(src)), hDev(nullptr) {
	WAVEFORMATEX devFmt;
	devFmt.nSamplesPerSec = this->src->getSampleRate();
	devFmt.wBitsPerSample = this->src->getBitsPerSample();
	devFmt.nChannels = this->src->getChannelCount();

	devFmt.cbSize = 0;
	devFmt.wFormatTag = WAVE_FORMAT_PCM;
	devFmt.nBlockAlign = (devFmt.wBitsPerSample / 8) * devFmt.nChannels;
	devFmt.nAvgBytesPerSec = devFmt.nBlockAlign * devFmt.nSamplesPerSec;

	if (waveOutOpen(&hDev, WAVE_MAPPER, &devFmt, reinterpret_cast<DWORD_PTR>(staticWaveProc), reinterpret_cast<DWORD_PTR>(this), CALLBACK_FUNCTION | WAVE_ALLOWSYNC) != MMSYSERR_NOERROR) throw exception("Can't open WAVE_MAPPER");
}

WaveOutDevice::~WaveOutDevice() {
	if (hDev != nullptr) waveOutClose(hDev);
}

void WaveOutDevice::staticWaveProc(HWAVEOUT, UINT msg, DWORD_PTR instance, DWORD_PTR param1, DWORD_PTR param2) {
	reinterpret_cast<WaveOutDevice* const>(instance)->waveProc(msg, param1, param2);
}

void WaveOutDevice::waveProc(UINT msg, DWORD_PTR param1, DWORD_PTR param2) {
	switch (msg) {
	case WOM_OPEN:
		break;
	case WOM_DONE:
	{
		LPWAVEHDR buffer = reinterpret_cast<LPWAVEHDR>(param1);
		waveOutUnprepareHeader(this->hDev, buffer, sizeof(WAVEHDR));
		size_t wsize = this->src->read(buffer->lpData, buffer->dwBufferLength);
		if (wsize == 0) {
			unique_lock<mutex> l(this->m);
			done = true;
			cv.notify_all();
			break;
		}
		else if (wsize != buffer->dwBufferLength) ZeroMemory(buffer->lpData + wsize, buffer->dwBufferLength - wsize);
		waveOutPrepareHeader(hDev, buffer, sizeof(WAVEHDR));
		waveOutWrite(hDev, buffer, sizeof(WAVEHDR));
	}
		break;
	case WOM_CLOSE:
		break;
	}
}

void WaveOutDevice::start() {
	for (auto& buffer : buffers) {
		ZeroMemory(&buffer, sizeof(buffer));
		buffer.dwBufferLength = (this->src->getBitsPerSample() * this->src->getChannelCount() * this->src->getSampleRate()) / 80;
		buffer.lpData = new CHAR[buffer.dwBufferLength];
		size_t wsize = this->src->read(buffer.lpData, buffer.dwBufferLength);
		if (wsize != buffer.dwBufferLength) ZeroMemory(buffer.lpData + wsize, buffer.dwBufferLength - wsize);
		waveOutPrepareHeader(hDev, &buffer, sizeof(WAVEHDR));
		waveOutWrite(hDev, &buffer, sizeof(WAVEHDR));
	}
	unique_lock<mutex> l(this->m);
	this->cv.wait(l, [this]() {return this->done; });
}
