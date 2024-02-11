#pragma once
#include <7zip/Archive/IArchive.h>
#include <Common/MyCom.h>
#include <filesystem>
#include <fstream>

class StdFileInStream Z7_final :
	public IInStream,
	public CMyUnknownImp
{
	Z7_COM_UNKNOWN_IMP_1(IInStream)

		Z7_IFACE_COM7_IMP(ISequentialInStream)
		Z7_IFACE_COM7_IMP(IInStream)

public:
	explicit StdFileInStream(std::filesystem::path PathOfArchive);
	StdFileInStream(const StdFileInStream& Other) = delete;
	StdFileInStream(StdFileInStream&& Other) noexcept = delete;
	StdFileInStream& operator=(const StdFileInStream& Other) = delete;
	StdFileInStream& operator=(StdFileInStream&& Other) noexcept = delete;
	virtual ~StdFileInStream();

private:
	std::ifstream InputStream;
	std::uintmax_t FileSize;
};

