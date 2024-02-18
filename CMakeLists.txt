cmake_minimum_required (VERSION 3.25)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if (POLICY CMP0077)
	set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
	set(X_VCPKG_APPLOCAL_DEPS_INSTALL ON)
	set(VCPKG_APPLOCAL_DEPS_INSTALL ON)
endif()

project ("7zip-examples")

find_package(7zip CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(Boost REQUIRED)

add_executable (7zip-examples)

target_sources(7zip-examples
	PRIVATE 
	"main.cpp"
	"InMemoryArchiveUpdateCallback.h"
	"InMemoryArchiveUpdateCallback.cpp"
	"Facade7z.h"
	"Facade7z.cpp"
	"ArchiveProperties.h"
	"ArchiveProperties.cpp"
	"ExportGUID.cpp"
	"ExportGUID.h"
	"InMemoryArchiveOpenCallback.cpp"
	"InMemoryExtractCallback.h"
	"InMemoryExtractCallback.cpp"
	"StdFileInStream.h"
	"StdFileInStream.cpp"
	"InMemoryOutStream.h"
	"InMemoryOutStream.cpp"
	"InMemoryFileSystem.h"
	"InMemoryArchive.h"
	"InMemoryArchive.cpp"
	"InMemoryFileSystem.cpp"
	"InMemoryInStream.h"
	"InMemoryInStream.cpp"
)

set_property(TARGET 7zip-examples PROPERTY CXX_STANDARD 23)
target_link_libraries(7zip-examples
	PRIVATE
	7zip::7zip
	Boost::boost
	fmt::fmt
	Bcrypt.lib
)

if (WIN32)
	target_compile_definitions(7zip-examples
		PRIVATE
		UNICODE_
		UNICODE
		NOMINMAX
		_WIN32_WINNT=0x0A00
	)
endif()

