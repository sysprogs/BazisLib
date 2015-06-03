#include "stdafx.h"
#include "ramdisk.h"

using namespace BazisLib;

RamDisk::RamDisk(unsigned MegabyteCount)
{
	m_SectorCount = MegabyteCount * (1024 * 1024 / RAMDISK_SECTOR_SIZE);
	m_TotalSize = MegabyteCount * 1024 * 1024;
	m_pBuffer = (char *)bulk_malloc((unsigned)m_TotalSize);
}

RamDisk::~RamDisk()
{
	if (m_pBuffer)
		bulk_free(m_pBuffer, (size_t)m_TotalSize);
}

ULONGLONG RamDisk::GetSectorCount()
{
	return m_SectorCount;
}

unsigned RamDisk::Read(ULONGLONG ByteOffset, void *pBuffer, unsigned Length)
{
	if (!m_pBuffer)
		return 0;
	LONGLONG MaxSize = m_TotalSize - ByteOffset;
	if (MaxSize < 0)
		MaxSize = 0;
	if (Length > MaxSize)
		Length = (unsigned)MaxSize;
	memcpy(pBuffer, m_pBuffer + ByteOffset, Length);
	return Length;
}

unsigned RamDisk::Write(ULONGLONG ByteOffset, const void *pBuffer, unsigned Length)
{
	if (!m_pBuffer)
		return 0;
	LONGLONG MaxSize = m_TotalSize - ByteOffset;
	if (MaxSize < 0)
		MaxSize = 0;
	if (Length > MaxSize)
		Length = (unsigned)MaxSize;
	memcpy(m_pBuffer + ByteOffset, pBuffer, Length);
	return Length;
}

