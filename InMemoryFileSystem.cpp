#include "InMemoryFileSystem.h"

#include <Windows.h>
#include <bcrypt.h>
#include <cassert>

#include <fmt/format.h>
//#include <fmt/ranges.h>


// https://learn.microsoft.com/en-us/windows/win32/seccng/creating-a-hash-with-cng#creating-a-hashing-object

static auto calculateSHA256(std::span<const std::byte> Data) -> std::array<std::byte, 32>
{
	const auto handleBcryptError = [](std::string_view FunctionName, NTSTATUS Status)
	{
		// It returns a native API NTSTATUS value, not a Win32 error code. So it's not really a usual "system_error".
		throw std::runtime_error(fmt::format("{} failed with NTSTATUS: {:#x}.",FunctionName, *reinterpret_cast<unsigned*>(&Status)));
	};

	BCRYPT_ALG_HANDLE AlgHandle = nullptr;
	if (NTSTATUS Status = BCryptOpenAlgorithmProvider(&AlgHandle, BCRYPT_SHA256_ALGORITHM, nullptr, 0); Status < 0)
		handleBcryptError("BCryptOpenAlgorithmProvider", Status);
	
	std::array<std::byte, 32> Hash;
	if (NTSTATUS Status = BCryptHash(AlgHandle, nullptr, 0, (PUCHAR)Data.data(), Data.size(), reinterpret_cast<PUCHAR>(Hash.data()), 32); Status < 0)
	{
		BCryptCloseAlgorithmProvider(AlgHandle,0);
		handleBcryptError("BCryptHash", Status);
	}

	if (NTSTATUS Status = BCryptCloseAlgorithmProvider(AlgHandle,0))
		handleBcryptError("BCryptCloseAlgorithmProvider", Status);
	return Hash;
}


InMemoryFile InMemoryFileSystem::createFile(PathType Path, FileView BinaryContent)
{
	std::string_view ImplicitUtf8Path{ reinterpret_cast<const char*>(Path.data()), Path.size() };
	std::array<std::byte, 32> Hash = calculateSHA256(BinaryContent);
	fmt::print("SHA256 of {}: {:0>2x}\n", ImplicitUtf8Path, fmt::join(Hash, ""));

	auto& NewFile = Files.emplace_back();
	NewFile.Path = Path;
	NewFile.Hash = Hash;
	BlobStorage.emplace(Hash, FileType{ std::ranges::begin(BinaryContent) , std::ranges::end(BinaryContent) });
	ModifiedTime = std::chrono::system_clock::now();
	return NewFile;
}

std::uint64_t InMemoryFileSystem::getFileSize(size_t Index) const noexcept
{
	if (Index >= Files.size())
		return 0;
	const auto InterFound = BlobStorage.find(Files[Index].Hash);
	return InterFound != BlobStorage.end() ? InterFound->second.size() : 0;
}

auto InMemoryFileSystem::getModifiedTime([[maybe_unused]] size_t Index) const noexcept -> TimePointType
{
	return ModifiedTime;
}

auto InMemoryFileSystem::getFileView(size_t Index) const noexcept -> FileView
{
	if (Index >= Files.size())
		return {};
	const auto InterFound = BlobStorage.find(Files[Index].Hash);
	return InterFound != BlobStorage.end() ? FileView{InterFound->second} : FileView{};
}

