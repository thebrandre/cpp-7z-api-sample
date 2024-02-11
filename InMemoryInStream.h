#pragma once
#include <7zip/Archive/IArchive.h>
#include <Common/MyCom.h>
#include <span>

class InMemoryInStream Z7_final :
	public IInStream,
	public CMyUnknownImp
{
	Z7_IFACES_IMP_UNK_1(IInStream)
	Z7_IFACE_COM7_IMP(ISequentialInStream)

public:
	explicit InMemoryInStream(std::span<const std::byte> Data);
	InMemoryInStream(const InMemoryInStream& Other) = delete;
	InMemoryInStream(InMemoryInStream&& Other) noexcept = delete;
	InMemoryInStream& operator=(const InMemoryInStream& Other) = delete;
	InMemoryInStream& operator=(InMemoryInStream&& Other) noexcept = delete;
	virtual ~InMemoryInStream();

private:
	std::span<const std::byte> Data;
	size_t Position{};
};