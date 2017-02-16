#pragma once

#include <array>
#include <condition_variable>
#include <mutex>
#include <Windows.h>
#include "WaveInFile.hpp"

/**
 * \brief Opens a waveOut device to play the audio data.
 */
class WaveOutDevice
{
private:
	std::unique_ptr<WaveInFile> src; //< \brief Audio source.

	HWAVEOUT dev;								//< \brief Audio device handle.
	std::array<WAVEHDR, 2> buffers;	//< \brief Audio device buffers.

	std::mutex m;						//< \brief Mutex for blocking the main thread during audio playback.
	std::condition_variable cv;	//< \brief Condition variable for signaling the main thread at the end of audio playback.
	bool done;							//< \brief Variable used to indicate playback is finished.

public:
	/**
	 * \brief Constructor that prepares wave data for playback on the default audio device.
	 *
	 * \param[in]	src	The sound source.
	 *
	 * \post	The class takes ownership of the sound source.
	 */
	WaveOutDevice(std::unique_ptr<WaveInFile>&& src);

	/**
	 * \brief Deleted copyconstructor.
	 */
	WaveOutDevice(WaveOutDevice const&) = delete;
	
	/**
	 * \brief Destructor.
	 */
	~WaveOutDevice();

	/**
	 * \brief Deleted assignment operator.
	 */
	void operator =(WaveOutDevice const&) = delete;

	/**
	 * \brief Starts the playback of the sound.
	 *
	 * \pre		It must be the first time the sound is going to play.
	 * \post	The sound has played.
	 */
	void start();

private:
	/**
	 *	\brief Static callback function for Windows.
	 *
	 * \param[in]	msg		The message send by Windows.
	 * \param[in]	param1	The first parameter of the message.
	 * \param[in]	param2	The second parameter of the message.
	 */
	static void staticWaveProc(HWAVEOUT, UINT msg, DWORD_PTR instance, DWORD_PTR param1, DWORD_PTR param2);

	/**
	 * \brief Processes windows messages received through the callback.
	 *
	 * \param[in]	msg		The message send by Windows.
	 * \param[in]	param1	The first parameter of the message.
	 * \param[in]	param2	The second parameter of the message.
	 */
	void waveProc(UINT msg, DWORD_PTR param1, DWORD_PTR param2);
};

