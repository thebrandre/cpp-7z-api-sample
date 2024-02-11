#include "InMemoryArchive.h"

#include <memory>

#include "InMemoryArchiveUpdateCallback.h"
#include "InMemoryFileSystem.h"

CMyComPtr<IArchiveUpdateCallback> InMemoryArchive::getUpdateCallback()
{
	return new InMemoryArchiveUpdateCallback(this);
}

InMemoryArchive::InMemoryArchive()
{
}
