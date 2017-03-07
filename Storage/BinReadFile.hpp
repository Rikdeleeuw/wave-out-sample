#pragma once

#include <string>
#include <memory>

/**
 * \brief Reads a binary textfile.
 */
class BinReadFile
{
private:
	FILE* file;	//< \brief The file handle.

public:
	/**
	 * \brief Constructor that opens the file specified by path.
	 *
	 * \param[in]	path	Path to the binary file to be opened.
	 */
	BinReadFile(const std::wstring& path);

	/**
	 * \brief Deleted copyconstructor.
	 */
	BinReadFile(BinReadFile const&) = delete;

	/**
	 * \brief Destructor.
	 */
	virtual ~BinReadFile();

	/**
	 * \brief Deleted assignment operator.
	 */
	void operator =(BinReadFile const &) = delete;

	/**
	 * \brief Tries to read the structure from the file.
	 *
	 * \tparam			T		The type to read from the file.
	 * \param[in,out]	dst	The destination for the read data.
	 * \return						Wheter the structure was read successfully.
	 *
	 * \post The file pointer has moved.
	 */
	template <typename T>
	bool read(T& dst);

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

	/**
	 * \brief Returns the current offset in the file.
	 *
	 * \return	File pointer location.
	 */
	uint64_t getPos() const;

	/**
	 * \brief Set the current file pointer location.
	 *
	 * \param[in]	pos	The new file pointer position.
	 * \returns				If the position could be set.
	 *
	 * \post	File pointer has moved.
	 */
	bool setPos(uint64_t pos);

	/**
	 * \brief Skips over a number of bytes in the file.
	 *
	 * \param[in]	size	The number of bytes to skip.
	 * \return					If the positon could be set.
	 *
	 * \post file pointer has moved.
	 */
	bool skip(uint64_t size);
};

template <typename T>
bool BinReadFile::read(T& dst) {
	return fread_s(&dst, sizeof(T), sizeof(T), 1, this->file) == 1;
}

