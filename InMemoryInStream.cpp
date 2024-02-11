#include "InMemoryInStream.h"

#include <span>
#include <fmt/format.h>

InMemoryInStream::InMemoryInStream(std::span<const std::byte> Data): Data(Data)
{}

InMemoryInStream::~InMemoryInStream() = default;

Z7_COM7F_IMF(InMemoryInStream::Read(void *data, UInt32 size, UInt32 *processedSize))
{
	const auto TruncatedNextPosition = std::min(Position + static_cast<std::size_t>(size), Data.size());
	const auto TruncatedSize = TruncatedNextPosition - Position;
	std::memcpy(data, Data.data() + Position, TruncatedSize);
	fmt::print("InMemoryInStream::Read {} of {} bytes at {}.\n", TruncatedSize, size, Position);
	Position = TruncatedNextPosition;
	if (processedSize)
		*processedSize = static_cast<UInt32>(TruncatedSize);
	return S_OK;
}


Z7_COM7F_IMF(InMemoryInStream::Seek(Int64 offset, UInt32 seekOrigin, UInt64* newPosition))
{
	if (seekOrigin == STREAM_SEEK_SET)
	{
		if (offset < 0)
		{
			Position = 0;
			return ERROR_NEGATIVE_SEEK;
		}
		Position = std::min(Data.size(), (size_t)offset);
		if (newPosition)
			*newPosition = Position;
		return S_OK;
	}

	if (seekOrigin == STREAM_SEEK_CUR)
	{
		if (-offset > Position)
		{
			Position = 0;
			return ERROR_NEGATIVE_SEEK;
		}
		Position = std::min(Data.size(), Position + offset);

		if (newPosition)
			*newPosition = Position;
		return S_OK;
	}
	if (seekOrigin == STREAM_SEEK_END)
	{
		if (-offset > Data.size())
		{
			Position = 0;
			return ERROR_NEGATIVE_SEEK;
		}
		std::size_t Last = Data.empty() ? Data.size() - 1 : 0;
		Position = std::min(Data.size(), Last + offset);
		if (newPosition)
			*newPosition = Position;
		return S_OK;
	}

	return STG_E_INVALIDFUNCTION;
}
