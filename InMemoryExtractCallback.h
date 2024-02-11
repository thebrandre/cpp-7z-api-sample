#pragma once
#include <7zip/Archive/IArchive.h>
#include <7zip/IPassword.h>
#include <Common/MyCom.h>
#include<optional>
#include<string>
#include <vector>


class InMemoryArchiveExtractCallback Z7_final :
	public IArchiveExtractCallback,
	public ICryptoGetTextPassword,
	public CMyUnknownImp
{
	Z7_IFACES_IMP_UNK_2(IArchiveExtractCallback, ICryptoGetTextPassword)
		Z7_IFACE_COM7_IMP(IProgress)

public:
	InMemoryArchiveExtractCallback();
	explicit InMemoryArchiveExtractCallback(std::u8string Password);
	virtual ~InMemoryArchiveExtractCallback();
	InMemoryArchiveExtractCallback(const InMemoryArchiveExtractCallback& Other) = delete;
	InMemoryArchiveExtractCallback(InMemoryArchiveExtractCallback&& Other) noexcept = delete;
	InMemoryArchiveExtractCallback& operator=(const InMemoryArchiveExtractCallback& Other) = delete;
	InMemoryArchiveExtractCallback& operator=(InMemoryArchiveExtractCallback&& Other) noexcept = delete;

private:
	std::optional<std::u8string> Password;
	CMyComPtr<ISequentialOutStream> OutStream;
	std::vector<std::byte> Buffer;
	UInt32 Index;
};
