#pragma once
#include <7zip/Archive/IArchive.h>
#include <Common/MyCom.h>


class ArchiveFactory{
public:

	ArchiveFactory();
	ArchiveFactory(const ArchiveFactory&) = delete;
	ArchiveFactory(ArchiveFactory&&) noexcept = delete;
	ArchiveFactory& operator=(const ArchiveFactory&) = delete;
	ArchiveFactory& operator=(ArchiveFactory&&) noexcept = delete;
	~ArchiveFactory();

	auto createInArchive(unsigned FormatId = 7) const -> IInArchive*;
	auto createOutArchive(unsigned FormatId = 7) const -> IOutArchive*;

	static auto getFileExtensionFromFormatId(unsigned FormatId = 7) -> const char*;
	unsigned getNumberOfFormats() const;

private:
	HMODULE Handle7zipDLL;
	Func_CreateObject CreateObject;
	Func_GetNumberOfFormats GetNumberOfFormats;
};
