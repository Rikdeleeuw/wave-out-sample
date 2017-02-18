#include <exception>
#include "WaveOutDevice.hpp"

using namespace std;

WaveOutDevice::WaveOutDevice(unique_ptr<WaveInFile>&& src) : src(move(src)), dev(nullptr), tickCount(0) {
	// prepare wave format for audio device from the parameters found in the file
	WAVEFORMATEX devFmt;
	devFmt.nSamplesPerSec = this->src->getSampleRate();
	devFmt.wBitsPerSample = this->src->getBitsPerSample();
	devFmt.nChannels = this->src->getChannelCount();

	// prepare the rest form the header by calculating values
	devFmt.cbSize = 0;
	devFmt.wFormatTag = WAVE_FORMAT_PCM;
	devFmt.nBlockAlign = (devFmt.wBitsPerSample / 8) * devFmt.nChannels;
	devFmt.nAvgBytesPerSec = devFmt.nBlockAlign * devFmt.nSamplesPerSec;

	// open hardware device
	if (waveOutOpen(&dev, WAVE_MAPPER, &devFmt, reinterpret_cast<DWORD_PTR>(staticWaveProc), reinterpret_cast<DWORD_PTR>(this), CALLBACK_FUNCTION | WAVE_ALLOWSYNC) != MMSYSERR_NOERROR) throw exception("Can't open default wave device");
}

WaveOutDevice::~WaveOutDevice() {
	if (dev != nullptr) waveOutClose(dev);
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
		// reschedule buffer with new audio data
		LPWAVEHDR buffer = reinterpret_cast<LPWAVEHDR>(param1);
		waveOutUnprepareHeader(this->dev, buffer, sizeof(WAVEHDR));
		size_t wsize = this->src->read(buffer->lpData, buffer->dwBufferLength);
		// end of audio signal reached
		if (wsize == 0) {
			// signal the main thread so it can continue
			unique_lock<mutex> l(this->m);
			done = true;
			cv.notify_all();
			break;
		}
		// do we need to pad the buffer
		else if (wsize != buffer->dwBufferLength) ZeroMemory(buffer->lpData + wsize, buffer->dwBufferLength - wsize);
		waveOutPrepareHeader(dev, buffer, sizeof(WAVEHDR));
		waveOutWrite(dev, buffer, sizeof(WAVEHDR));
	}
		break;
	case WOM_CLOSE:
		break;
	}
}

void WaveOutDevice::start() {
	// prepare buffers and issue initial buffers to the audio device
	for (auto& buffer : buffers) {
		ZeroMemory(&buffer, sizeof(buffer));
		buffer.dwBufferLength = (this->src->getBitsPerSample() * this->src->getChannelCount() * this->src->getSampleRate()) / 8;
		buffer.lpData = new CHAR[buffer.dwBufferLength];
		size_t wsize = this->src->read(buffer.lpData, buffer.dwBufferLength);
		if (wsize != buffer.dwBufferLength) ZeroMemory(buffer.lpData + wsize, buffer.dwBufferLength - wsize);
		waveOutPrepareHeader(dev, &buffer, sizeof(WAVEHDR));
		waveOutWrite(dev, &buffer, sizeof(WAVEHDR));
	}
	// wait for audio to stop
	unique_lock<mutex> l(this->m);
	this->cv.wait(l, [this]() {return this->done; });
	for (auto& buffer : buffers) {
		waveOutUnprepareHeader(this->dev, &buffer, sizeof(WAVEHDR));
		delete [] buffer.lpData;
	}
}
