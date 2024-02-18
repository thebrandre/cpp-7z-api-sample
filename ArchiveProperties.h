#pragma once
#include <string>
#include <vector>
#include <Common/MyCom.h>


class Archive7zProperties
{
public:
	struct CompressHeaders {bool Value;};
	struct CompressHeadersFull {bool Value;};
	struct EncryptHeaders {bool Value;};
	struct Level {std::uint32_t Value;};
	struct NumberOfThreads {std::uint32_t Value;};
	struct HashSize {std::uint32_t Value;};

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

	Archive7zProperties& set(const wchar_t* Name, bool Value);
	Archive7zProperties& set(const wchar_t* Name, std::uint32_t Value);

	std::vector<std::wstring> Properties;
	std::vector<PROPVARIANT> Values;
};