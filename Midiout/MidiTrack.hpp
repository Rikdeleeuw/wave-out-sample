#pragma once

#include <array>
#include <memory>
#include <Windows.h>
#include "Storage/BinReadFile.hpp"

class MidiInFile;

/**
 * \brief Holds all information about a single midi event.
 */
struct midiEvent_t {
	DWORD deltaTime;	//< \brief Delay in seconds since the last event in the stream.
	DWORD streamID;	//< \brief The number of the owning stream.
	DWORD event;			//< \brief The event to be processed.
};

/**
 * \brief A single track/stream/instrument in a midi file.
 */
class MidiTrack
{
private:
	uint64_t trackEnd;	//< \brief The end of the track within the file as file pointer.
	uint64_t trackPos;	//< \brief The current read position of the track in the file as file pointer.
	uint8_t lastCmd;		//< \brief The previous command from the stream. We need it incase it gets repeated.
	MidiInFile& src;		//< \brief This class is used for reading from the file.

	uint32_t curTickCount;							//< \brief The current time in ticks for this track.
	std::array<midiEvent_t, 1024> buffer;		//< \brief The buffer for midi read events.
	uint32_t index;										//< \brief The current offset in the midi buffer.
	uint32_t endIndex;									//< \brief The end of the buffer, can be set if buffer is shorter then preallocated.
	bool ended;												//< \brief If both the buffer end the file are completely used.

public:
	/**
	 * \brief Constructor.
	 *
	 * \param[in]	src			The midi source which contains this midi track.
	 * \param[in]	fileOffset	The offset in the file where the data for this track is stored.
	 * \param[in]	size			The size of the track in bytes.
	 */
	MidiTrack(MidiInFile& src, uint64_t fileOffset, uint32_t size);

	/**
	 * \brief Destructor.
	 */
	~MidiTrack();

	/**
	 * \brief If the track has ended and there are no events in the buffer to read.
	 *
	 * \return	If the track contains no more midi events.
	 */
	bool hasEnded() const;

	/**
	 * \brief Reads the next midi event form the buffer and advances the pointer.
	 *
	 * \remark if no event can be read it wil load an new buffer from file.
	 *
	 * \param[in]	songTickCount	The current tick count of the whole song.
	 * \param[in]	src						The file to read from in case we need to read more midi events.
	 * \return										The next event in the buffer.
	 *
	 * \post	The file point could have moved and the buffer refiled. The buffer pointer will have moved to the next event.
	 */
	midiEvent_t getNextEvent(uint32_t songTickCount, const BinReadFile* src);

	/**
	* \brief Reads the next midi event form the buffer but does not advance the pointer.
	*
	* \remark if no event can be read it wil load an new buffer from file.
	*
	* \param[in]	songTickCount	The current tick count of the whole song.
	* \param[in]	src						The file to read from in case we need to read more midi events.
	* \return									The next event in the buffer.
	*
	* \post	The file point could have moved and the buffer refiled.
	*/
	midiEvent_t peekNextEvent(uint32_t songTickCount, const BinReadFile* src);

private:
	/**
	 * \brief Read a new buffer from file.
	 *
	 * \param[in]	src	The source file.
	 *
	 * \post	The file pointer will have moved.
	 */
	void readBuffer(const BinReadFile* src);

	/**
	 * \brief Reads the next event from the file.
	 *
	 * \post	The file pointer will have moved.
	 */
	void readEvent();

	/**
	 * \brief Reads a number in the variable length format from the file.
	 *
	 * \return	The number read form the file.
	 *
	 * \post The file pointer will have moved.
	 */
	uint32_t readVariableLength();

	/**
	 * \brief Skips the given amount of bytes.
	 *
	 * \param[in]	size	The number of bytes to skip.
	 * \return					If the skipping was successfully performed.
	 *
	 * \post	The file pointer will have moved.
	 */
	bool skip(uint64_t size);

	/**
	 * \brief Reads a special type form the file.
	 *
	 * \tparam			T		The type to read.
	 * \param[inout]	dst	The allocated memory for the read type
	 * \return						If the read was succesful.
	 *
	 * \post the file pointer will have moved.
	 */
	template <typename T>
	bool read(T& dst);
};

template <typename T>
bool MidiTrack::read(T& dst) {
	bool res = false;
	if ((this->trackPos + sizeof(T)) <= this->trackEnd) {
		res = this->src.read(dst);
		this->trackPos += sizeof(T);
		if (!res || (this->trackPos == this->trackEnd)) this->ended = true;
	}
	else {
		this->ended = true;
	}
	return res;
}

#include "MidiInFile.hpp"

