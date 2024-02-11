#include "ArchiveFactory.h"
#include "ExportGUID.h"

#include <stdexcept>



ArchiveFactory::ArchiveFactory()
{
	Handle7zipDLL = LoadLibraryW(L"7zip.dll");
	if (!Handle7zipDLL)
		throw std::runtime_error("7zip DLL not loaded");

	CreateObject = Z7_GET_PROC_ADDRESS(Func_CreateObject, Handle7zipDLL, "CreateObject");

	if (!CreateObject)
		throw std::runtime_error("CreateObject not found");

}

ArchiveFactory::~ArchiveFactory()
{
	FreeLibrary(Handle7zipDLL);
}

CMyComPtr<IInArchive> ArchiveFactory::createIInArchive()
{

	CMyComPtr<IInArchive> archive;
	if (CreateObject(&CLSID_Format, &IID_IInArchive, (void**)&archive) != S_OK)
	{
		throw std::runtime_error("CreateObject not found");
	}
	return archive;

}

CMyComPtr<IOutArchive> ArchiveFactory::createIOutArchive()
{
	CMyComPtr<IOutArchive> archive;
	if (CreateObject(&CLSID_Format, &IID_IOutArchive, (void**)&archive) != S_OK)
	{
		throw std::runtime_error("CreateObject not found");
	}
	return archive;
}
