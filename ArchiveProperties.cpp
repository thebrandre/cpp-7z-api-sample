#include "ArchiveProperties.h"

#include <algorithm>
#include <7zip/Archive/IArchive.h>

void Archive7zProperties::applyProperties(IUnknown* Archive)
{
	CMyComPtr<ISetProperties> SetProperties = [&]
	{
		void* Interface{};
		Archive->QueryInterface(IID_ISetProperties, &Interface);
		return static_cast<ISetProperties*>(Interface);
	}();

	std::vector<const wchar_t*> PropKeys(Properties.size());
	std::ranges::transform(Properties, PropKeys.begin(), &std::wstring::c_str);

	SetProperties->SetProperties(PropKeys.data(), Values.data(), PropKeys.size());
}

Archive7zProperties& Archive7zProperties::set(CompressHeaders Value)
{
	return set(L"hc", Value.Value);
}

Archive7zProperties& Archive7zProperties::set(CompressHeadersFull Value)
{
	return set(L"hcf", Value.Value);
}

Archive7zProperties& Archive7zProperties::set(EncryptHeaders Value)
{
	return set(L"he", Value.Value);
}

Archive7zProperties& Archive7zProperties::set(Level Value)
{
	return set(L"x", Value.Value);
}

Archive7zProperties& Archive7zProperties::set(NumberOfThreads Value)
{
	return set(L"mt", Value.Value);
}

Archive7zProperties& Archive7zProperties::set(HashSize Value)
{
	return set(L"crc", Value.Value);
}

Archive7zProperties& Archive7zProperties::set(const wchar_t* Name, bool Value)
{
	Properties.emplace_back(Name);
	auto& PropValue = Values.emplace_back();
	PropVariantClear(&PropValue);
	PropValue.vt = VT_BOOL;
	PropValue.boolVal = Value ? VARIANT_TRUE : VARIANT_FALSE;
	return *this;
}

Archive7zProperties& Archive7zProperties::set(const wchar_t* Name, std::uint32_t Value)
{
	Properties.emplace_back(Name);
	auto& PropValue = Values.emplace_back();
	PropVariantClear(&PropValue);
	PropValue.vt = VT_UI4;
	PropValue.ulVal = Value;
	return *this;
}
