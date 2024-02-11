#pragma once
#include <7zip/Archive/IArchive.h>
#include <7zip/IPassword.h>
#include <Common/MyCom.h>
#include <string>
#include <optional>

class InMemoryArchiveOpenCallback Z7_final :
	public IArchiveOpenCallback,
	public ICryptoGetTextPassword,
	public CMyUnknownImp
{
	Z7_IFACES_IMP_UNK_2(IArchiveOpenCallback, ICryptoGetTextPassword)
public:
	explicit InMemoryArchiveOpenCallback();
	explicit InMemoryArchiveOpenCallback(std::u8string Password);
	InMemoryArchiveOpenCallback(const InMemoryArchiveOpenCallback& Other) = delete;
	InMemoryArchiveOpenCallback(InMemoryArchiveOpenCallback&& Other) noexcept = delete;
	InMemoryArchiveOpenCallback& operator=(const InMemoryArchiveOpenCallback& Other) = delete;
	InMemoryArchiveOpenCallback& operator=(InMemoryArchiveOpenCallback&& Other) noexcept = delete;
	virtual ~InMemoryArchiveOpenCallback();

private:
	std::optional<std::u8string> Password;
	bool PasswordRequired{};
};

