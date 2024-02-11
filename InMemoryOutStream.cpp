#include "InMemoryOutStream.h"

#include <span>
#include <fmt/format.h>

InMemoryOutStream::InMemoryOutStream(std::vector<std::byte>& Data): Data(&Data)
{}

InMemoryOutStream::~InMemoryOutStream() = default;

Z7_COM7F_IMF(InMemoryOutStream::Write(const void* data, UInt32 size, UInt32* processedSize))
{
	const auto BytesToWrite = static_cast<size_t>(size);
	const auto OverWriteBytes = std::min(Data->size() - Position, BytesToWrite);
	std::memcpy(Data->data() + Position, data, OverWriteBytes);
	fmt::print("InMemoryOutStream::Write overwrite {} of {} bytes of current buffer of size {}.\n", OverWriteBytes, BytesToWrite, Data->size());
	Position += OverWriteBytes;

	const auto RemainingBytes = BytesToWrite - OverWriteBytes;
	if (RemainingBytes != 0)
	{
		const auto RemainingBytesView = std::span{ static_cast<const std::byte*>(data) + OverWriteBytes, RemainingBytes };
		Data->insert(Data->end(), RemainingBytesView.begin(), RemainingBytesView.end());
		fmt::print("InMemoryOutStream::Write append remaining {} of {} bytes of current buffer of size {}.\n", RemainingBytes, BytesToWrite, Data->size());
		Position = Data->size();
	}

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

