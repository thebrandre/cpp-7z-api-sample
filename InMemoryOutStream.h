#pragma once
#include <7zip/Archive/IArchive.h>
#include <Common/MyCom.h>
#include <vector>

class InMemoryOutStream Z7_final :
	public IOutStream,
	public CMyUnknownImp
{
	Z7_IFACES_IMP_UNK_1(IOutStream)
	Z7_IFACE_COM7_IMP(ISequentialOutStream)

public:
	explicit InMemoryOutStream(std::vector<std::byte>& Data);
	InMemoryOutStream(const InMemoryOutStream& Other) = delete;
	InMemoryOutStream(InMemoryOutStream&& Other) noexcept = delete;
	InMemoryOutStream& operator=(const InMemoryOutStream& Other) = delete;
	InMemoryOutStream& operator=(InMemoryOutStream&& Other) noexcept = delete;
	virtual ~InMemoryOutStream();

private:
	std::vector<std::byte>* Data;
	size_t Position{};
};