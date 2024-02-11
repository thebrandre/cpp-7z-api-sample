#pragma once
#include <variant>
#include <vector>
#include <string>
#include <span>
#include <algorithm>
#include <array>
#include <chrono>

#include <unordered_map>

class InMemoryFileSystem;

class InMemoryFile {
	friend class InMemoryFileSystem;
	using PathType = std::u8string;
public:
	using PathView = std::u8string_view;
	using HashType = std::array<std::byte, 32>;

	PathView getPath() const {
		return Path;
	}
	PathView getFileName() const {
		const auto LastSeparator = Path.find_last_of(u8'/');
		return PathView(Path).substr(LastSeparator != PathType::npos ? LastSeparator : 0);
	}

	PathType Path;
	HashType Hash;
};

class InMemoryFileSystem
{
	friend class InMemoryFile;
	using FileType = std::vector<std::byte>;
public:
	using PathType = InMemoryFile::PathType;
	using PathView = InMemoryFile::PathView;
	using HashType = InMemoryFile::HashType;
	using FileView = std::span<const std::byte>;
	using TimePointType = std::chrono::time_point<std::chrono::system_clock>;

	InMemoryFile createFile(PathType Path, FileView BinaryContent);
	std::uint64_t getFileSize(size_t Index) const noexcept;
	auto getModifiedTime(size_t Index) const noexcept -> TimePointType;
	auto getFileView(size_t Index) const noexcept -> FileView;

	std::vector<InMemoryFile> Files;

private:
	struct GetTruncatedHash
	{
		std::size_t operator()(const HashType& HashValue) const
		{
			return *reinterpret_cast<const std::size_t*>(HashValue.data());
		}
	};

	std::unordered_map<HashType, FileType, GetTruncatedHash> BlobStorage;
	TimePointType ModifiedTime;
};