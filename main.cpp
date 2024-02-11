#include "ArchiveFactory.h"
#include <Shlwapi.h>
#include <7zip/ICoder.h>
#include <7zip/IPassword.h>
#include <7zip/Archive/IArchive.h>
#include <7zip/IProgress.h>
#include <Common/MyCom.h>
#include <fmt/format.h>
#include <fmt/std.h>
#include <boost/nowide/convert.hpp>

#include "InMemoryArchive.h"
#include "InMemoryArchiveOpenCallback.h"
#include "InMemoryExtractCallback.h"
#include "StdFileInStream.h"
#include "InMemoryOutStream.h"


size_t getUncompressedSizeOfFile(IInArchive* Archive, UInt32 Index)
{
	PROPVARIANT prop{};
	Archive->GetProperty(Index, kpidSize, &prop);
	if (prop.vt == VT_UI8)
		return prop.uhVal.QuadPart;

	throw std::runtime_error("");
}

std::string getFileName(IInArchive* Archive, UInt32 Index)
{
	PROPVARIANT prop{};
	Archive->GetProperty(Index, kpidPath, &prop);
	if (prop.vt == VT_BSTR)
		return boost::nowide::narrow(prop.bstrVal);
	else if (prop.vt != VT_EMPTY)
		return std::to_string(prop.vt);
	return {};
}


void extractStuff(ArchiveFactory& Factory)
{
	std::filesystem::path TestArchive("./Archive-Password2.7z");

	CMyComPtr<IInArchive> Archive = Factory.createIInArchive();
	CMyComPtr<IArchiveOpenCallback> OpenCallback(new InMemoryArchiveOpenCallback(u8"Password"));
	CMyComPtr<IInStream> InMemoryInStreamCallback(new StdFileInStream(TestArchive));

	const UInt64 scanSize = 1 << 23;
	if (const auto OpenResult = Archive->Open(InMemoryInStreamCallback, &scanSize, OpenCallback); OpenResult != S_OK)
	{
		if (OpenResult == S_FALSE)
			fmt::print("Open of {0} failed. Bad password?\n", TestArchive);
		else if (OpenResult == E_ABORT)
			fmt::print("Open of {0} failed. Password not defined?\n", TestArchive);
		else fmt::print("Open of {0} failed with error code {1:#x} ({1}).\n", TestArchive, *reinterpret_cast<const unsigned*>(&OpenResult));
	}

	UInt32 numItems = 0;
	Archive->GetNumberOfItems(&numItems);
	fmt::print("Archive contains {} items\n", numItems);
	for (UInt32 i = 0; i < numItems; i++)
	{
		fmt::print("{} of size {}\n", getFileName(Archive, i), getUncompressedSizeOfFile(Archive, i));
	}

	CMyComPtr<IArchiveExtractCallback> ExtractCallback(new InMemoryArchiveExtractCallback(u8"Password"));
	UInt32 ItemIndex = 68;
	Archive->Extract(&ItemIndex, 1, false, ExtractCallback);
}

void compressStuff(ArchiveFactory& Factory)
{
	CMyComPtr<IOutArchive> OutArchive = Factory.createIOutArchive();
	InMemoryArchive TempArchive;
	TempArchive.Password = u8"Password";
	unsigned char FileContent[] = "ASCII and stuff";
	const std::span<std::byte> FileContentView{ (std::byte*)FileContent, std::size(FileContent) - 1 };
	TempArchive.FileSystem.createFile(u8"SomeFile.txt", FileContentView);

	auto UpdateCallback = TempArchive.getUpdateCallback();
	std::vector<std::byte> Buffer;
	CMyComPtr<ISequentialOutStream> InMemoryOutStreamInstance(new InMemoryOutStream(Buffer));
	OutArchive->UpdateItems(InMemoryOutStreamInstance, 1, UpdateCallback);

	std::ofstream OutFile("generatedArchive.7z", std::ios_base::trunc | std::ios_base::binary);
	OutFile.write((const char*)Buffer.data(), Buffer.size());
}

int main()
{
	fmt::print("Running in directory {}.\n", std::filesystem::current_path());
	ArchiveFactory Factory;
	//extractStuff(Factory);
	compressStuff(Factory);
	return 0;
}
