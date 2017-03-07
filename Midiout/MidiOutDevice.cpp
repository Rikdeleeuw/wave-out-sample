#include "MidiOutDevice.hpp"

using namespace std;

MidiOutDevice::MidiOutDevice(std::unique_ptr<MidiInFile>&& src) : src(move(src)), dev(nullptr), done(false) {
	UINT deviceId = 0;
	if (midiStreamOpen(&this->dev, &deviceId, 1, reinterpret_cast<DWORD_PTR>(MidiOutDevice::staticMidiProc), reinterpret_cast<DWORD_PTR>(this), CALLBACK_FUNCTION) != MMSYSERR_NOERROR)  throw exception("Can't open default media device");
	MIDIPROPTIMEDIV prop;
	prop.cbStruct = sizeof(MIDIPROPTIMEDIV);
	prop.dwTimeDiv = this->src->getTickCount();
	midiStreamProperty(this->dev, reinterpret_cast<LPBYTE>(&prop), MIDIPROP_SET | MIDIPROP_TIMEDIV);
}

MidiOutDevice::~MidiOutDevice() {
	if (this->dev != nullptr) midiStreamClose(this->dev);
}

void MidiOutDevice::start() {
	midiStreamRestart(this->dev);
	for (auto& buffer: this->buffers) {
		ZeroMemory(&buffer, sizeof(buffer));
		buffer.dwBufferLength = 1024 * sizeof(midiEvent_t);
		buffer.lpData = new CHAR[buffer.dwBufferLength];
		ZeroMemory(buffer.lpData, buffer.dwBufferLength);
		DWORD eventCount = static_cast<DWORD>(this->src->read(reinterpret_cast<midiEvent_t*>(buffer.lpData), 1024));
		buffer.dwBytesRecorded = eventCount * sizeof(midiEvent_t);
		midiOutPrepareHeader(reinterpret_cast<HMIDIOUT>(dev), &buffer, sizeof(MIDIHDR));
		midiStreamOut(this->dev, &buffer, sizeof(MIDIHDR));
	}
	unique_lock<mutex> l(this->m);
	this->cv.wait(l, [this]() {return this->done; });
	midiStreamStop(this->dev);
	for (auto& buffer : this->buffers) {
		midiOutUnprepareHeader(reinterpret_cast<HMIDIOUT>(dev), &buffer, sizeof(MIDIHDR));
		delete [] buffer.lpData;
	}
}

void MidiOutDevice::staticMidiProc(HMIDIOUT, UINT msg, DWORD_PTR instance, DWORD_PTR param1, DWORD_PTR param2) {
	reinterpret_cast<MidiOutDevice* const>(instance)->midiProc(msg, param1, param2);
}

void MidiOutDevice::midiProc(UINT msg, DWORD_PTR param1, DWORD_PTR param2) {
	if (msg == MOM_DONE) {
		if (done) {
			cv.notify_all();
			return;
		}
		LPMIDIHDR buffer = reinterpret_cast<LPMIDIHDR>(param1);
		midiOutUnprepareHeader(reinterpret_cast<HMIDIOUT>(this->dev), buffer, sizeof(MIDIHDR));
		DWORD eventCount = static_cast<DWORD>(this->src->read(reinterpret_cast<midiEvent_t*>(buffer->lpData), 1024));
		buffer->dwBytesRecorded = eventCount * sizeof(midiEvent_t);
		if (eventCount == 0) {
			unique_lock<mutex> l(this->m);
			done = true;
			return;
		}
		midiOutPrepareHeader(reinterpret_cast<HMIDIOUT>(dev), buffer, sizeof(MIDIHDR));
		midiStreamOut(this->dev, buffer, sizeof(MIDIHDR));
	}
}