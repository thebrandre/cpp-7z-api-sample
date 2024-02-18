#include <cassert>
#include <ranges>

#include "ArchiveFactory.h"
#include "ArchiveProperties.h"
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
	const unsigned ArchiveFormatId = 0x07;
	//std::filesystem::path TestArchive("./Archive-Password2.7z");
	std::filesystem::path TestArchive("./Archive.7z");

	CMyComPtr<IInArchive> Archive = Factory.createInArchive(ArchiveFormatId);
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

	PROPVARIANT Method{};
	Archive->GetArchiveProperty(kpidMethod, &Method);
	if (Method.vt == VT_BSTR)
	{
		fmt::print("Method: {}\n", boost::nowide::narrow(Method.bstrVal));
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
	const unsigned ArchiveFormatId = 0x07;
	CMyComPtr<IOutArchive> OutArchive = Factory.createOutArchive(ArchiveFormatId);
	Archive7zProperties::set(OutArchive,
		//Archive7zProperties::EncryptHeaders{ true },
		Archive7zProperties::NumberOfThreads{ 1 },
		Archive7zProperties::SolidOptions{ u8"e2g" }
		//Archive7zProperties::Solid{ true }
	);

	InMemoryArchive TempArchive;
	TempArchive.Password = u8"Password";
	unsigned char FileContent[] = "ASCII and stuff";
	const std::span<std::byte> FileContentView{ (std::byte*)FileContent, std::size(FileContent) - 1 };
	TempArchive.FileSystem.createFile(u8"SomeFile.txt", FileContentView);

	auto UpdateCallback = TempArchive.getUpdateCallback();
	std::vector<std::byte> Buffer;
	CMyComPtr<ISequentialOutStream> InMemoryOutStreamInstance(new InMemoryOutStream(Buffer));
	OutArchive->UpdateItems(InMemoryOutStreamInstance, 1, UpdateCallback);


	// dump buffer to file
	const auto OutFileName = [&]()-> std::filesystem::path
	{
		for (const int GeneratedArchiveIndex : std::views::iota(1, 100))
		{
			const std::filesystem::path Candidate = std::filesystem::current_path() / fmt::format("generatedArchive_{}.{}", GeneratedArchiveIndex, ArchiveFactory::getFileExtensionFromFormatId(ArchiveFormatId));
			if (!std::filesystem::exists(Candidate))
				return Candidate;
		}
		throw std::runtime_error("Please delete some of the generated files!");
	}();
	fmt::print("Generated archive {}.\n", OutFileName);
	std::ofstream OutFile(OutFileName, std::ios_base::trunc | std::ios_base::binary);
	OutFile.write(reinterpret_cast<const char*>(Buffer.data()), Buffer.size());
}

void hashStuff(ArchiveFactory& Factory)
{
	CMyComPtr<IHasher> Hasher = Factory.createHasher("SHA256");
	Hasher->Init();
	const char* TestValue = "Test";
	Hasher->Update(TestValue, std::strlen(TestValue));
	std::array<std::byte, 32> Hash;
	Hasher->Final(reinterpret_cast<Byte*>(Hash.data()));
	assert("532eaabd9574880dbf76b9b8cc00832c20a6ec113d682299550d7a6e0f345e25", fmt::format("{:0>2x}", fmt::join(Hash, "")));
	fmt::print("SHA256 of {}: {:0>2x}\n", TestValue, fmt::join(Hash, ""));
}

int main()
{
	fmt::print("Running in directory {}.\n", std::filesystem::current_path());
	ArchiveFactory Factory;
	fmt::print("Number of supported formats {}.\n", Factory.getNumberOfFormats());
	extractStuff(Factory);
	//compressStuff(Factory);
	hashStuff(Factory);
	return 0;
}
