#include "InMemoryArchiveOpenCallback.h"

#include <fmt/format.h>
#include <boost/nowide/convert.hpp>

InMemoryArchiveOpenCallback::InMemoryArchiveOpenCallback() = default;

InMemoryArchiveOpenCallback::InMemoryArchiveOpenCallback(std::u8string Password) : Password(Password)
{}

InMemoryArchiveOpenCallback::~InMemoryArchiveOpenCallback()
= default;

Z7_COM7F_IMF(InMemoryArchiveOpenCallback::SetTotal(const UInt64* files, const UInt64* bytes))
{
	fmt::print("SetTotal {} files and {} bytes.", files != nullptr ? *files : 0, bytes != nullptr ? *bytes : 0);

	return S_OK;
}

Z7_COM7F_IMF(InMemoryArchiveOpenCallback::SetCompleted(const UInt64* files, const UInt64* bytes))
{
	fmt::print("SetCompleted {} files and {} bytes.", files != nullptr ? *files : 0, bytes != nullptr ? *bytes : 0);
	return S_OK;
}

Z7_COM7F_IMF(InMemoryArchiveOpenCallback::CryptoGetTextPassword(BSTR* password))
{
	PasswordRequired = true;
	if (!Password)
		return E_ABORT;

	return StringToBstr(boost::nowide::widen(Password.value()).c_str(), password);
}

