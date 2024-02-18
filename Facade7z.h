#pragma once
#include <7zip/Archive/IArchive.h>
#include <7zip/ICoder.h>
#include <Common/MyCom.h>

#include "ArchiveProperties.h"

class Facade7z{
public:

	Facade7z();
	Facade7z(const Facade7z&) = delete;
	Facade7z(Facade7z&&) noexcept = delete;
	Facade7z& operator=(const Facade7z&) = delete;
	Facade7z& operator=(Facade7z&&) noexcept = delete;
	~Facade7z();

	auto createInArchive(unsigned FormatId = 7) const -> IInArchive*;
	auto createOutArchive(unsigned FormatId = 7) const -> IOutArchive*;
	auto createHasher(std::string_view Name) const -> IHasher*;

	static auto getFileExtensionFromFormatId(unsigned FormatId = 7) -> const char*;
	unsigned getNumberOfFormats() const;
	std::uint32_t getMajorVersion() const;
	std::uint32_t getMinorVersion() const;

private:
	struct FunctionTable
	{
		Func_CreateObject CreateObject{};

		Func_GetHandlerProperty GetHandlerProperty{};
		Func_GetNumberOfFormats GetNumberOfFormats{};
		Func_GetHandlerProperty2 GetHandlerProperty2{};
		Func_GetIsArc GetIsArc{};

		Func_GetNumberOfMethods GetNumberOfMethods{};
		Func_GetMethodProperty GetMethodProperty{};
		Func_CreateDecoder CreateDecoder{};
		Func_CreateEncoder CreateEncoder{};

		Func_GetHashers GetHashers{};

		Func_SetCodecs SetCodecs{};
		
		Func_SetLargePageMode SetLargePageMode{};
		Func_SetCaseSensitive SetCaseSensitive{};

		Func_GetModuleProp GetModuleProp{};
	};

	HMODULE Handle7zipDLL;
	FunctionTable Functions;
	std::uint32_t Version;
};
