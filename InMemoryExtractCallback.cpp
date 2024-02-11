#include "InMemoryExtractCallback.h"
#include <boost/nowide/convert.hpp>

#include "InMemoryOutStream.h"
#include "fmt/core.h"


InMemoryArchiveExtractCallback::InMemoryArchiveExtractCallback() = default;

InMemoryArchiveExtractCallback::InMemoryArchiveExtractCallback(std::u8string Password) : Password(Password)
{}

InMemoryArchiveExtractCallback::~InMemoryArchiveExtractCallback() = default;

Z7_COM7F_IMF(InMemoryArchiveExtractCallback::SetTotal(UInt64 /* size */))
{
	return S_OK;
}

Z7_COM7F_IMF(InMemoryArchiveExtractCallback::SetCompleted(const UInt64* /* completeValue */))
{
	return S_OK;
}

Z7_COM7F_IMF(InMemoryArchiveExtractCallback::GetStream(UInt32 index,
	ISequentialOutStream** outStream, Int32 askExtractMode))
{
	if (askExtractMode != NArchive::NExtract::NAskMode::kExtract)
	{
		*outStream = nullptr;
		return S_OK;
	}

	OutStream.Release();
	OutStream = new InMemoryOutStream(Buffer);
	Index = index;

	*outStream = OutStream.Detach();

	return S_OK;
}

Z7_COM7F_IMF(InMemoryArchiveExtractCallback::PrepareOperation(Int32 askExtractMode))
{
	return S_OK;
}

Z7_COM7F_IMF(InMemoryArchiveExtractCallback::SetOperationResult(Int32 operationResult))
{
	if (operationResult != NArchive::NExtract::NOperationResult::kOK)
	{
		fmt::print("SetOperationResult reported an error. Operation result: {}\n", operationResult);
		return S_OK;
	}
	if (!Buffer.empty())
	{
		fmt::print("\nBinary content of item at index {}:\n{}\n", Index, std::string_view((const char*)Buffer.data(), Buffer.size()));
		Buffer.clear();
	}
	return S_OK;
}

Z7_COM7F_IMF(InMemoryArchiveExtractCallback::CryptoGetTextPassword(BSTR* password))
{
	if (!Password)
		return E_ABORT;

	return StringToBstr(boost::nowide::widen(Password.value()).c_str(), password);
}