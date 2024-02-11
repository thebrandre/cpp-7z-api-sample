#include "StdFileInStream.h"

#include <cassert>
#include <algorithm>

#include <fmt/format.h>
#include <fmt/std.h>

/*
ISequentialInStream::Read()
  The requirement for caller: (processedSize != NULL).
  The callee can allow (processedSize == NULL) for compatibility reasons.

  if (size == 0), this function returns S_OK and (*processedSize) is set to 0.

  if (size != 0)
  {
    Partial read is allowed: (*processedSize <= avail_size && *processedSize <= size),
      where (avail_size) is the size of remaining bytes in stream.
    If (avail_size != 0), this function must read at least 1 byte: (*processedSize > 0).
    You must call Read() in loop, if you need to read exact amount of data.
  }

  If seek pointer before Read() call was changed to position past the end of stream:
    if (seek_pointer >= stream_size), this function returns S_OK and (*processedSize) is set to 0.

  ERROR CASES:
    If the function returns error code, then (*processedSize) is size of
    data written to (data) buffer (it can be data before error or data with errors).
    The recommended way for callee to work with reading errors:
      1) write part of data before error to (data) buffer and return S_OK.
      2) return error code for further calls of Read().
*/

Z7_COM7F_IMF(StdFileInStream::Read(void* data, UInt32 size, UInt32* processedSize))
{
	if (size == 0 || InputStream.eof())
	{
		if (processedSize != nullptr)
			*processedSize = 0u;
		return S_OK;
	}

	constexpr UInt32 MaxChunkSize = 1 << 20;
	if (size > MaxChunkSize)
	{
		fmt::print("Large chunk {}\n", size);
	}
	size = std::min(size, MaxChunkSize);

	auto* Bytes = static_cast<std::fstream::char_type*>(data);
	const auto ChunkSize = static_cast<std::streamsize>(size);
	std::memset(data, 0, size);

	InputStream.read(Bytes, ChunkSize);
	if (processedSize != nullptr)
		*processedSize = (UInt32)InputStream.gcount();
	return S_OK;
}

/*
IInStream::Seek() / IOutStream::Seek()
  If you seek to position before the beginning of the stream,
  Seek() function returns error code:
      Recommended error code is __HRESULT_FROM_WIN32(ERROR_NEGATIVE_SEEK).
      or STG_E_INVALIDFUNCTION
  It is allowed to seek past the end of the stream.
  if Seek() returns error, then the value of *newPosition is undefined.
*/

Z7_COM7F_IMF(StdFileInStream::Seek(Int64 offset, UInt32 seekOrigin, UInt64* newPosition))
{
	if (seekOrigin == STREAM_SEEK_SET)
	{
		if (offset < 0) {
			InputStream.seekg(0, std::ios_base::beg);
			return ERROR_NEGATIVE_SEEK;
		}
		InputStream.seekg(offset, std::ios_base::beg);
		if (newPosition)
			*newPosition = InputStream.tellg();
		return S_OK;
	}

	if (seekOrigin == STREAM_SEEK_CUR)
	{
		const auto CurrentPosition = InputStream.tellg();
		if (CurrentPosition < 0)
		{
			InputStream.seekg(offset, std::ios_base::beg);
		}
		else if (offset < -(Int64)CurrentPosition) {
			InputStream.seekg(0, std::ios_base::beg);
			return ERROR_NEGATIVE_SEEK;
		}
		else
		{
			InputStream.seekg(offset, std::ios_base::cur);	
		}
		
		if (newPosition)
			*newPosition = InputStream.tellg();
		return S_OK;
	}
	if (seekOrigin == STREAM_SEEK_END)
	{
		const auto CurrentPosition = FileSize;
		if (offset < -(Int64)FileSize) {
			InputStream.seekg(0, std::ios_base::beg);
			return ERROR_NEGATIVE_SEEK;
		}
		if (offset == 0)
		{
			InputStream.seekg(0, std::ios_base::end);
			if (newPosition)
				*newPosition = FileSize;
			return S_OK;
		}

		InputStream.seekg(offset, std::ios_base::end);
		if (newPosition)
			*newPosition = InputStream.tellg();
		return S_OK;
	}

	return STG_E_INVALIDFUNCTION;
}

StdFileInStream::StdFileInStream(std::filesystem::path PathOfArchive)
	: InputStream(PathOfArchive, std::ios_base::in | std::ios::binary),
	FileSize(0)
{
	if (!std::filesystem::exists(PathOfArchive))
	{
		fmt::print("Archive at {} does not exist.\n", PathOfArchive);
		return;
	}

	std::error_code ErrorCode;
	FileSize = std::filesystem::file_size(PathOfArchive, ErrorCode);
	if (ErrorCode)
	{
		FileSize = 0;
		fmt::print("Unable to determine file size for {}! Error code: {}.\n", PathOfArchive, ErrorCode);
	}
}

StdFileInStream::~StdFileInStream()
{
}
