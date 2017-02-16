#pragma once

#include "BinReadFile.hpp"

/**
 * \brief Reads a *.wav file for use as audio source.
 */
class WaveInFile
{
private:
	uint16_t channelCount;	//< \brief Audio channel count in audio signal.
	uint32_t sampleRate;		//< \brief Sample rate of the audio signal in samples/second.
	uint16_t bitsPerSample;	//< \brief Sample width in bits.

	std::unique_ptr<BinReadFile> src; //< \brief Audio data source.

public:
	/**
	 * \brief Constructor that reads from file.
	 *
	 * \param[in]	src	The sound file.
	 *
	 * \post	The class takes ownership of the sound file.
	 */
	WaveInFile(std::unique_ptr<BinReadFile>&& src);

	/**
	 * \brief	Destructor.
	 */
	~WaveInFile();

	/**
	 * \brief The number of channels in the sound signal.
	 *
	 * \return Channel count.
	 */
	uint16_t getChannelCount() const;

	/**
	 * \brief The samplerate of the sound signal.
	 *
	 * \return Samplerate in samples/second.
	 */
	uint32_t getSampleRate() const;

	/**
	 * \brief The bit width of the sound signal.
	 *
	 * \return Width of each channels in bits.
	 */
	uint16_t getBitsPerSample() const;

	/**
	 * \brief Reads u number of bytes into a buffer.
	 *
	 * \param[in,out]	dst	The buffer to store the read data.
	 * \param[in]		size	The size of the buffer in bytes.
	 * \return						The number of bytes read.
	 *
	 * \post The file pointer has moved.
	 */
	size_t read(void* dst, size_t size);
};

