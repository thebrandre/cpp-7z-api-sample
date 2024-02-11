#pragma once

#define DEFINE_GUID_ARC(name, id) Z7_DEFINE_GUID(name, \
  0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, id, 0x00, 0x00);

enum
{
	kId_Zip = 1,
	kId_BZip2 = 2,
	kId_7z = 7,
	kId_Xz = 0xC,
	kId_Tar = 0xEE,
	kId_GZip = 0xEF
};

DEFINE_GUID_ARC(CLSID_Format, kId_7z)

