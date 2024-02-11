

#pragma once
#include "7zip/Archive/IArchive.h"
#include "Common/MyCom.h"

class ArchiveFactory{
public:
	ArchiveFactory();
	ArchiveFactory(const ArchiveFactory&) = delete;
	ArchiveFactory(ArchiveFactory&&) noexcept = delete;
	ArchiveFactory& operator=(const ArchiveFactory&) = delete;
	ArchiveFactory& operator=(ArchiveFactory&&) noexcept = delete;
	~ArchiveFactory();

	CMyComPtr<IInArchive> createIInArchive();
	CMyComPtr<IOutArchive> createIOutArchive();


private:
	HMODULE Handle7zipDLL;
	Func_CreateObject CreateObject;
};

