#pragma once
#include <string>
#include <vector>
#include <Common/MyCom.h>


class Archive7zProperties
{
public:
	struct CompressHeaders { bool Value; };
	struct CompressHeadersFull { bool Value; };
	struct EncryptHeaders { bool Value; };
	struct Level { std::uint32_t Value; };
	struct NumberOfThreads { std::uint32_t Value; };
	struct HashSize { std::uint32_t Value; };

	// No idea how to use that. It does not seem to have any effect on the archive.
	// It resets the solid extension to false, block size, and the number of solid files
	struct Solid { bool Value; };
	// format "e2g100f" = extension enabled, 2 GB block size, number of solid files = 100.
	// But it is still marked as "not solid" in the command line.
	struct SolidOptions { std::u8string_view Value; };

	template<typename... PropertyTypes>
	static void set(IUnknown* Archive, PropertyTypes... Values)
	{
		Archive7zProperties Props;
		(Props.set(Values), ...);
		Props.applyProperties(Archive);
	}

private:
	void applyProperties(IUnknown* Archive);
	Archive7zProperties& set(CompressHeaders Value);
	Archive7zProperties& set(CompressHeadersFull Value);
	Archive7zProperties& set(EncryptHeaders Value);
	Archive7zProperties& set(Level Value);
	Archive7zProperties& set(NumberOfThreads Value);
	Archive7zProperties& set(HashSize Value);
	Archive7zProperties& set(Solid Value);
	Archive7zProperties& set(SolidOptions Value);

	PROPVARIANT& emplace_back(std::wstring_view Name);
	Archive7zProperties& set(std::wstring_view Name, bool Value);
	Archive7zProperties& set(std::wstring_view Name, std::uint32_t Value);
	Archive7zProperties& set(std::wstring_view Name, std::wstring_view Value);

	std::vector<std::wstring> Properties;
	std::vector<PROPVARIANT> Values;
};