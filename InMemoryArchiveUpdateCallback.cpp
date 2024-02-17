#include "InMemoryArchiveUpdateCallback.h"
#include "InMemoryArchive.h"
#include "InMemoryInStream.h"
#include <iostream>
#include <fmt/format.h>
#include <7zip/CPP/Common/MyWindows.h>

#include "boost/nowide/convert.hpp"


UInt64 getFileTimeFromUnixTime(UInt32 unixTime)
{
	static constexpr UInt32 kNumTimeQuantumsInSecond = 10000000;
	static constexpr UInt32 kFileTimeStartYear = 1601;
	static constexpr UInt32 kUnixTimeStartYear = 1970;
	static constexpr UInt64 kUnixTimeOffset =
		(UInt64)60 * 60 * 24 * (89 + 365 * (kUnixTimeStartYear - kFileTimeStartYear));
	return (kUnixTimeOffset + (UInt64)unixTime) * kNumTimeQuantumsInSecond;
}


InMemoryArchiveUpdateCallback::~InMemoryArchiveUpdateCallback()
{
	fmt::print("Cleanup InMemoryArchiveUpdateCallback");
}

InMemoryArchiveUpdateCallback::InMemoryArchiveUpdateCallback(InMemoryArchive* Archive) : Archive(Archive)
{}

Z7_COM7F_IMF(InMemoryArchiveUpdateCallback::SetTotal(UInt64 size))
{
	fmt::print("InMemoryArchiveUpdateCallback::SetTotal({})\n", size);
	return S_OK;
}

Z7_COM7F_IMF(InMemoryArchiveUpdateCallback::SetCompleted(const UInt64* completeValue))
{
	fmt::print("InMemoryArchiveUpdateCallback::SetCompleted({})\n", completeValue != nullptr ? std::to_string(*completeValue) : "nullptr");
	return S_OK;
}

Z7_COM7F_IMF(InMemoryArchiveUpdateCallback::GetUpdateItemInfo(UInt32 index,
	Int32* newData, Int32* newProperties, UInt32* indexInArchive))
{
	//	GetUpdateItemInfo outs:
	//	*newData  *newProps
	//   0        0      - Copy data and properties from archive
	//   0        1      - Copy data from archive, request new properties
	//   1        0      - that combination is unused now
	//   1        1      - Request new data and new properties. It can be used even for folders

	  //indexInArchive = -1 if there is no item in archive, or if it doesn't matter.

	fmt::print("InMemoryArchiveUpdateCallback::GetUpdateItemInfo({})\n", index);
	if (newData)
		*newData = 1;
	if (newProperties)
		*newProperties = 1;
	if (indexInArchive)
		*indexInArchive = static_cast<UInt32>(-1);

	return S_OK;
}

Z7_COM7F_IMF(InMemoryArchiveUpdateCallback::GetProperty(UInt32 index, PROPID propID, PROPVARIANT* value))
{
	fmt::print("InMemoryArchiveUpdateCallback::GetProperty({}, {})\n", index, propID);
	PropVariantClear(value);

	if (index >= Archive->FileSystem.Files.size())
		return S_FALSE;

	bool IsDirectory = false;
	bool IsAnti = false;
	const auto& CurrentFile = Archive->FileSystem.Files[index];

	switch (propID)
	{
	case kpidAttrib:
		value->vt = VT_UI4;
		value->ulVal = IsDirectory ? FILE_ATTRIBUTE_DIRECTORY : 0;
		break;
	case kpidCTime:
	case kpidMTime:
	{
		const auto ModifiedTime = Archive->FileSystem.getModifiedTime(index);
		const auto FileTime64 = getFileTimeFromUnixTime(std::chrono::duration_cast<std::chrono::seconds>(ModifiedTime.time_since_epoch()).count());
		value->vt = VT_FILETIME;
		value->filetime = {};
		value->filetime.dwLowDateTime = (DWORD)FileTime64;
		value->filetime.dwHighDateTime = (DWORD)(FileTime64 >> 32);

		//FILETIME Filetime;
		//GetSystemTimeAsFileTime(&Filetime);
		//value->vt = VT_FILETIME;
		//value->filetime = Filetime;
		break;
	}
	case kpidHardLink:
		value->vt = VT_BOOL;
		value->boolVal = VARIANT_FALSE;
		break;
	case kpidPath:
	{
		const auto PathW = boost::nowide::widen(CurrentFile.Path);
		value->vt = VT_BSTR;
		value->bstrVal = ::SysAllocStringLen(PathW.c_str(), PathW.size());
		break;
	}
	case kpidIsDir:
		value->vt = VT_BOOL;
		value->boolVal = IsDirectory ? VARIANT_TRUE : VARIANT_FALSE;
		break;
	case kpidIsAnti:
		value->vt = VT_BOOL;
		value->boolVal = IsAnti ? VARIANT_TRUE : VARIANT_FALSE;
		break;
	case kpidSize:
		value->vt = VT_UI8;
		value->uhVal.QuadPart = Archive->FileSystem.getFileSize(index);
		break;
	case kpidATime:  break;
	case kpidPosixAttrib:  break;
	}

	return S_OK;
}

Z7_COM7F_IMF(InMemoryArchiveUpdateCallback::GetStream(UInt32 index, ISequentialInStream** inStream))
{
	fmt::print("InMemoryArchiveUpdateCallback::GetStream({})\n", index);
	*inStream = nullptr;

	if (index >= Archive->FileSystem.Files.size())
		return S_FALSE;

	CMyComPtr<ISequentialInStream> InStream(new InMemoryInStream(Archive->FileSystem.getFileView(index)));
	*inStream = InStream.Detach();
	return S_OK;
}

Z7_COM7F_IMF(InMemoryArchiveUpdateCallback::SetOperationResult(Int32 operationResult))
{
	fmt::print("InMemoryArchiveUpdateCallback::SetOperationResult({})\n", operationResult);
	return S_OK;
}

Z7_COM7F_IMF(InMemoryArchiveUpdateCallback::GetVolumeSize(UInt32 index, UInt64* size))
{
	*size = 1u << 31;
	fmt::print("InMemoryArchiveUpdateCallback::GetVolumeSize({})\n", index);
	return S_OK;
}

Z7_COM7F_IMF(InMemoryArchiveUpdateCallback::GetVolumeStream(UInt32 index, ISequentialOutStream** volumeStream))
{
	*volumeStream = nullptr;
	fmt::print("InMemoryArchiveUpdateCallback::GetVolumeStream({})\n", index);
	return S_OK;
}


Z7_COM7F_IMF(InMemoryArchiveUpdateCallback::CryptoGetTextPassword2(Int32* passwordIsDefined, BSTR* password))
{
	fmt::print("InMemoryArchiveUpdateCallback::CryptoGetTextPassword2()\n");
	const bool PasswordIsDefined = !Archive->Password.empty();
	if (PasswordIsDefined && password != nullptr)
	{
		const auto PasswordW = boost::nowide::widen(Archive->Password);
		*password = ::SysAllocStringLen(PasswordW.c_str(), PasswordW.size());
	}
	if (passwordIsDefined)
		*passwordIsDefined = PasswordIsDefined;
	return S_OK;
}