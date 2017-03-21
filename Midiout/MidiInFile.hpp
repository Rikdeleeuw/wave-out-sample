#pragma once
#include <vector>
#include <array>
#include "Storage/BinReadFile.hpp" 
#include "MidiTrack.hpp"

/**
 * \brief Parser for midi files.
 */
class MidiInFile
{
	friend MidiTrack;

private:
	std::unique_ptr<BinReadFile> src;	//< \brief Source file.
	
	std::vector<MidiTrack> trackPtrs;	//< \brief Individual tracks are handled by these classes.

	int16_t tickCount;							//< \brief Tick speed configured in the file.
	int32_t curTickCount;						//< \brief Playback stream current tick count.

public:
	/**
	 * \brief Constructor.
	 *
	 * \param[in]	src	The file source for this midi song.
	 */
	MidiInFile(std::unique_ptr<BinReadFile>&& src);

	/**
	 * \brief Destructor.
	 */
	~MidiInFile();

	/**
	 * \brief Returns the current tick count.
	 *
	 * \return The current tick count in the song.
	 */
	int16_t getTickCount() const;

	/**
	 * \brief Reads a couple of events from a midi track in the file.
	 *
	 * \param[in,out]	dst				The buffer in which midievents will be stored.
	 * \param[in]		count			The number of midi events to read.
	 * \param[in]		trackIndex	The track index to read from.
	 * \return									The amount of events read.
	 *
	 * \post	The file offset for the track has moved to the first new midi event.
	 */
	size_t read(midiEvent_t* dst, size_t count, unsigned int trackIndex);

	/**
	 * \brief Reads the combined events from all tracks (the song) from the file.
	 *
	 * \param[in,out]	dst		The bufffer that will store the midievents.
	 * \param[in]		count	The number of events to read.
	 * \return							The number of events styored in the buffer.
	 *
	 * \post	The file offset for one or mor tracks will have moved.
	 */
	size_t read(midiEvent_t* dst, size_t count);

private:
	/**
	 * \brief Reads the type form the file.
	 *
	 * \tparam			T		The type to read.
	 * \param[in,out]	dst	The buffer to store the read type.
	 * \return						If we succeeded in reading the type.
	 *
	 * \post	File pointer will have moved.
	 */
	template <typename T>
	bool read(T& dst);

	/**
	 * \brief Advances the file pointer a couple of bytes.
	 *
	 * \param	size	The number of bytes to advance the file pointer.
	 * \return			If we did not reach the end of the file while moving.
	 *
	 * \post	The file pointer will have moved.
	 */
	bool skip(uint64_t size);

	/**
	 * \brief Set the file pointer to a given location
	 *
	 * \param	pos	The new position of the filepointer.
	 * \return			Wether the given poistion was in the file.
	 *
	 * \post	The file pointer will have moved.
	 */
	bool setPos(uint64_t pos);
};

template <typename T>
bool MidiInFile::read(T& dst) {
	return this->src->read<T>(dst);
}
