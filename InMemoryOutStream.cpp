#include "InMemoryOutStream.h"

#include <cassert>
#include <span>
#include <fmt/format.h>

InMemoryOutStream::InMemoryOutStream(std::vector<std::byte>& Data) : Data(&Data)
{}

InMemoryOutStream::~InMemoryOutStream() = default;

static std::size_t writeBytesToBuffer(auto& Buffer, std::size_t Position, std::span<const std::byte> BytesToWrite)
{
	const auto BytesToOverwrite = BytesToWrite.subspan(0, std::min(std::ranges::size(Buffer) - Position, BytesToWrite.size()));
	const auto BytesToAppend = BytesToWrite.subspan(BytesToOverwrite.size());
	fmt::print("InMemoryOutStream::Write overwrite {} of {} bytes of current buffer of size {}.\n", BytesToOverwrite.size(), BytesToWrite.size(), std::ranges::size(Buffer));
	const auto CurrentPositionIterator = std::ranges::copy(BytesToOverwrite, std::ranges::next(std::ranges::begin(Buffer), Position)).out;
	if (!BytesToAppend.empty())
	{
		assert(CurrentPositionIterator == std::ranges::end(Buffer));
		fmt::print("InMemoryOutStream::Write append remaining {} of {} bytes of current buffer of size {}.\n", BytesToAppend.size(), BytesToWrite.size(), std::ranges::size(Buffer));
		std::ranges::copy(BytesToAppend, std::back_inserter(Buffer));
		return std::ranges::size(Buffer);
	}

	const auto NewPosition = std::ranges::distance(std::ranges::begin(Buffer), CurrentPositionIterator);
	fmt::print("InMemoryOutStream::Write move position from {} to {}.\n", Position, NewPosition);
	return NewPosition;
}


Z7_COM7F_IMF(InMemoryOutStream::Write(const void* data, UInt32 size, UInt32* processedSize))
{
	const auto BytesToWrite = std::span{ static_cast<const std::byte*>(data), static_cast<size_t>(size) };
	Position = ::writeBytesToBuffer(*Data, Position, BytesToWrite);
	if (processedSize)
		*processedSize = size;
	return S_OK;
}


Z7_COM7F_IMF(InMemoryOutStream::Seek(Int64 offset, UInt32 seekOrigin, UInt64* newPosition))
{
	if (seekOrigin == STREAM_SEEK_SET)
	{
		fmt::print("InMemoryOutStream::Seek(): set position to {} of size {}.\n", offset, Data->size());
		if (offset < 0)
		{
			Position = 0;
			return ERROR_NEGATIVE_SEEK;
		}
		Position = std::min(Data->size(), (size_t)offset);
		if (newPosition)
			*newPosition = Position;
		return S_OK;
	}

	if (seekOrigin == STREAM_SEEK_CUR)
	{
		fmt::print("InMemoryOutStream::Seek(): move {} bytes from {} in buffer of size {}.\n", offset, Position, Data->size());
		if (-offset > Position)
		{
			Position = 0;
			return ERROR_NEGATIVE_SEEK;
		}
		Position = std::min(Data->size(), Position + offset);

		if (newPosition)
			*newPosition = Position;
		return S_OK;
	}
	if (seekOrigin == STREAM_SEEK_END)
	{
		fmt::print("InMemoryOutStream::Seek(): move {} bytes from end in buffer of size {}.\n", offset, Position, Data->size());
		if (-offset > Data->size())
		{
			Position = 0;
			return ERROR_NEGATIVE_SEEK;
		}
		std::size_t Last = Data->empty() ? Data->size() - 1 : 0;
		Position = std::min(Data->size(), Last + offset);
		if (newPosition)
			*newPosition = Position;
		return S_OK;
	}

	return STG_E_INVALIDFUNCTION;
}

Z7_COM7F_IMF(InMemoryOutStream::SetSize(UInt64 newSize))
{
	Data->resize(newSize);
	return S_OK;
}

