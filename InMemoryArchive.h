#pragma once
#include <7zip/Archive/IArchive.h>
#include <Common/MyCom.h>
#include "InMemoryFileSystem.h"

class InMemoryArchive {
public:
	CMyComPtr<IArchiveUpdateCallback> getUpdateCallback();

	InMemoryArchive();

	InMemoryFileSystem FileSystem;
};