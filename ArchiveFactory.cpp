#include "ArchiveFactory.h"
#include "ExportGUID.h"
#include <stdexcept>
#include <string>

#include "fmt/format.h"

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


ArchiveFactory::ArchiveFactory()
{
	Handle7zipDLL = LoadLibraryW(L"7zip.dll");
	if (!Handle7zipDLL)
		throw ArchiveException("7zip DLL not loaded");

	CreateObject = Z7_GET_PROC_ADDRESS(Func_CreateObject, Handle7zipDLL, "CreateObject");

	if (!CreateObject)
		throw ArchiveException("CreateObject not found");

}

ArchiveFactory::~ArchiveFactory()
{
	FreeLibrary(Handle7zipDLL);
}

auto ArchiveFactory::createInArchive(unsigned FormatId) const -> IInArchive*
{
	const auto ArchiveClassGuid = ::getArchiveClassIdFromFormatId(FormatId);
	if (ArchiveClassGuid == nullptr)
		throw ArchiveException(fmt::format("Format ID {} is not supported!", FormatId));

	void* Archive{};
	if (CreateObject(ArchiveClassGuid, &IID_IInArchive, &Archive) != S_OK)
		throw ArchiveException(fmt::format("Unable to create read archive with format ID {}!", FormatId));

	return static_cast<IInArchive*>(Archive);
}

auto ArchiveFactory::createOutArchive(unsigned FormatId) const -> IOutArchive*
{
	const auto ArchiveClassGuid = ::getArchiveClassIdFromFormatId(FormatId);
	if (ArchiveClassGuid == nullptr)
		throw ArchiveException(fmt::format("Format ID {} is not supported!", FormatId));

	void* Archive{};
	if (CreateObject(ArchiveClassGuid, &IID_IOutArchive, &Archive) != S_OK)
		throw ArchiveException(fmt::format("Unable to create read archive with format ID {}!", FormatId));

	return static_cast<IOutArchive*>(Archive);
}

auto ArchiveFactory::getFileExtensionFromFormatId(unsigned FormatId) -> const char*
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
