#include "Facade7z.h"
#include "ExportGUID.h"
#include <boost/nowide/convert.hpp>
#include <fmt/format.h>
#include <stdexcept>
#include <string>
#include <cassert>

class ArchiveException : public std::runtime_error
{
public:
	ArchiveException(const std::string& fcr_message) : std::runtime_error(fcr_message) {}
};


static auto getArchiveClassIdFromFormatId(unsigned f_formatId) -> const GUID*
{
	switch (f_formatId)
	{
	case 0x01:
		return &CLSID_Zip;
	case 0x02:
		return &CLSID_BZip2;
	case 0x03:
		return &CLSID_Rar;
	case 0x07:
		return &CLSID_7z;
	case 0xC:
		return &CLSID_Xz;
	case 0xCC:
		return &CLSID_Rar5;
	case 0xEE:
		return &CLSID_Tar;
	case 0xEF:
		return &CLSID_GZip;
	default:
		return nullptr;
	}
}


Facade7z::Facade7z()
{
	Handle7zipDLL = LoadLibraryW(L"7zip.dll");
	if (!Handle7zipDLL)
		throw ArchiveException("7zip DLL not loaded");

	const auto loadFunction = [&]<typename T_Func>(std::string_view f_name, T_Func& fr_functionPointer)
	{
		fr_functionPointer = Z7_GET_PROC_ADDRESS(T_Func, Handle7zipDLL, f_name.data());
		if (!fr_functionPointer)
			throw ArchiveException(fmt::format("{} not found in 7zip DLL", f_name));
	};

	loadFunction("CreateObject", Functions.CreateObject);

	loadFunction("GetHandlerProperty", Functions.GetHandlerProperty);
	loadFunction("GetNumberOfFormats", Functions.GetNumberOfFormats);
	loadFunction("GetHandlerProperty2", Functions.GetHandlerProperty2);
	loadFunction("GetIsArc", Functions.GetIsArc);

	loadFunction("GetNumberOfMethods", Functions.GetNumberOfMethods);
	loadFunction("GetMethodProperty", Functions.GetMethodProperty);
	loadFunction("CreateDecoder", Functions.CreateDecoder);
	loadFunction("CreateEncoder", Functions.CreateEncoder);
	
	loadFunction("GetHashers", Functions.GetHashers);
	
	loadFunction("SetCodecs", Functions.SetCodecs);

	loadFunction("SetLargePageMode", Functions.SetLargePageMode);
	loadFunction("SetCaseSensitive", Functions.SetCaseSensitive);

	loadFunction("GetModuleProp", Functions.GetModuleProp);

	{
		PROPVARIANT l_versionProp{};
		PropVariantClear(&l_versionProp);
		if (Functions.GetModuleProp(NModulePropID::kVersion, &l_versionProp) == S_OK && l_versionProp.vt == VT_UI4)
			Version = l_versionProp.ulVal;
	}
}

Facade7z::~Facade7z()
{
	FreeLibrary(Handle7zipDLL);
}

auto Facade7z::createInArchive(unsigned FormatId) const -> IInArchive*
{
	const auto ArchiveClassGuid = ::getArchiveClassIdFromFormatId(FormatId);
	if (ArchiveClassGuid == nullptr)
		throw ArchiveException(fmt::format("Format ID {} is not supported!", FormatId));

	void* Archive{};
	if (Functions.CreateObject(ArchiveClassGuid, &IID_IInArchive, &Archive) != S_OK)
		throw ArchiveException(fmt::format("Unable to create read archive with format ID {}!", FormatId));

	return static_cast<IInArchive*>(Archive);
}

auto Facade7z::createOutArchive(unsigned FormatId) const -> IOutArchive*
{
	const auto ArchiveClassGuid = ::getArchiveClassIdFromFormatId(FormatId);
	if (ArchiveClassGuid == nullptr)
		throw ArchiveException(fmt::format("Format ID {} is not supported!", FormatId));

	void* Archive{};
	if (Functions.CreateObject(ArchiveClassGuid, &IID_IOutArchive, &Archive) != S_OK) [[unlikely]]
		throw ArchiveException(fmt::format("Unable to create read archive with format ID {}!", FormatId));

	return static_cast<IOutArchive*>(Archive);
}

auto Facade7z::createHasher(std::string_view Name) const -> IHasher*
{
	CMyComPtr<IHashers> Hashers{};
	if (Functions.GetHashers(&Hashers) != S_OK) [[unlikely]]
		throw ArchiveException("GetHashers failed!");
	const UInt32 NumberOfHashers = Hashers->GetNumHashers();

	UInt32 HasherIndex = NumberOfHashers;
	for (UInt32 i = 0; i < NumberOfHashers; ++i)
	{
		PROPVARIANT HasherProp{};
		PropVariantClear(&HasherProp);
		Hashers->GetHasherProp(i, NMethodPropID::kName, &HasherProp);
		assert(HasherProp.vt == VT_BSTR);
		const auto HasherName = boost::nowide::narrow(HasherProp.bstrVal);
		PropVariantClear(&HasherProp);
		Hashers->GetHasherProp(i, NMethodPropID::kDigestSize, &HasherProp);
		assert(HasherProp.vt == VT_UI4);
		std::uint32_t DigestSize = HasherProp.ulVal;
		fmt::print("Hash method {} with digest size {} at index {}.\n", HasherName, DigestSize, i);
		if (Name == HasherName)
			HasherIndex = i;
	}
	if (HasherIndex == NumberOfHashers)
		throw ArchiveException(fmt::format("No Hasher called '{}' found!", Name));

	IHasher* Hasher{};
	Hashers->CreateHasher(HasherIndex, &Hasher);
	if (Hasher == nullptr)
		throw ArchiveException(fmt::format("Unable to create hasher '{}' with index {}!", Name, HasherIndex));
	return Hasher;
}

auto Facade7z::getFileExtensionFromFormatId(unsigned FormatId) -> const char*
{
	switch (FormatId)
	{
	case 0x01:
		return "zip";
	case 0x02:
		return "bz2";
	case 0x03:
		return "rar";
	case 0x07:
		return "7z";
	case 0x0C:
		return ".xz";
	case 0xCC:
		return "rar";
	case 0xEE:
		return "tar";
	case 0xEF:
		return "gz";
	default:
		return "";
	}
}

unsigned Facade7z::getNumberOfFormats() const
{
	UInt32 NumberOfFormats{};
	if (Functions.GetNumberOfFormats(&NumberOfFormats) != S_OK) [[unlikely]]
		throw ArchiveException("GetNumberOfFormats failed!");
	return NumberOfFormats;
}

std::uint32_t Facade7z::getMajorVersion() const
{
	return Version >> 16;
}

std::uint32_t Facade7z::getMinorVersion() const
{
	return Version & 0xFFFFu;
}