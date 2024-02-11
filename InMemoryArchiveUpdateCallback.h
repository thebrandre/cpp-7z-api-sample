#pragma once
#include <7zip/Archive/IArchive.h>
#include <7zip/IPassword.h>
#include <Common/MyCom.h>
#include <string>


class InMemoryArchive;

class InMemoryArchiveUpdateCallback Z7_final :
	public IArchiveUpdateCallback2,
	public ICryptoGetTextPassword2,
	public CMyUnknownImp
{
	Z7_IFACES_IMP_UNK_2(IArchiveUpdateCallback2, ICryptoGetTextPassword2)
	Z7_IFACE_COM7_IMP(IProgress)
	Z7_IFACE_COM7_IMP(IArchiveUpdateCallback)

public:
	explicit InMemoryArchiveUpdateCallback(InMemoryArchive* Archive);
	InMemoryArchiveUpdateCallback(const InMemoryArchiveUpdateCallback& Other) = delete;
	InMemoryArchiveUpdateCallback(InMemoryArchiveUpdateCallback&& Other) noexcept = delete;
	InMemoryArchiveUpdateCallback& operator=(const InMemoryArchiveUpdateCallback& Other) = delete;
	InMemoryArchiveUpdateCallback& operator=(InMemoryArchiveUpdateCallback&& Other) noexcept = delete;
	virtual ~InMemoryArchiveUpdateCallback();

private:
	InMemoryArchive* Archive{};
};
