#pragma once

#include <array>
#include <mutex>
#include <Windows.h>
#include "MidiInFile.hpp"

/**
 * \brief Opens a midi device so we can play a song once.
 */
class MidiOutDevice {
private:
	std::unique_ptr<MidiInFile> src; //< \brief The midi source file.

	HMIDISTRM dev;							//< \brief The midi device handle.
	std::array<MIDIHDR, 2> buffers;	//< \brief The double output buffer.

	std::mutex m;						//< \brief Mutex used to make the signal work.
	std::condition_variable cv;	//< \brief Conditional variable used by the signal to indicate that the song has finished palying.
	bool done;							//< \brief Varaible used to make the signal work.

public:
	/**
	 * \brief Constructor.
	 *
	 * \param[in]	src	The midi source.
	 */
	MidiOutDevice(std::unique_ptr<MidiInFile>&& src);

	/**
	 * \brief Destructor.
	 */
	~MidiOutDevice();

	/**
	 * \brief Starts playing the song.
	 *
	 * \post	Song has been played, It can't be restarted.
	 */
	void start();

private:
	/**
	 * \brief Static windows midi api callback.
	 *
	 * \param[in]	msg			The windows event.
	 * \param[in]	instance	The MidiOutDevice class that triggeredd the event.
	 * \param[in]	param1		The first parameter of the event.
	 * \param[in]	param2		The second parameter of the event.
	 */
	static void CALLBACK staticMidiProc(HMIDIOUT, UINT msg, DWORD_PTR instance, DWORD_PTR param1, DWORD_PTR param2);

	/**
	* \brief Windows midi api callback.
	*
	* \param[in]	msg			The windows event.
	* \param[in]	param1		The first parameter of the event.
	* \param[in]	param2		The second parameter of the event.
	*/
	void midiProc(UINT msg, DWORD_PTR param1, DWORD_PTR param2);

};

