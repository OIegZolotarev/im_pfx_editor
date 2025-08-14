/*
    LightBaker3000 Frontend project,
    (c) 2022 CrazyRussian
*/

#define _CRT_SECURE_NO_WARNINGS

#include "common.h"
#include "fs_file_handle.h"


FileHandleOS::FileHandleOS(const char *fileName)
{
#ifdef WINDOWS
    m_pFileHandle = fopen(fileName, "rb");
#else
    m_pFileHandle = fopen64(fileName, "rb");
#endif
    m_Name        = fileName;
}

FileHandleOS::~FileHandleOS()
{
    if (m_pFileHandle)
        fclose(m_pFileHandle);
}

size_t FileHandleOS::TotalSize()
{
    fseek(m_pFileHandle, 0, SEEK_END);
    size_t len = ftell(m_pFileHandle);
    fseek(m_pFileHandle, m_Offset, SEEK_SET);

    return len;
}

size_t FileHandleOS::Position()
{
    return m_Offset;
}

size_t FileHandleOS::Read(size_t elementSize, size_t elementsCount, void *destBuffer)
{
    size_t r = fread(destBuffer, elementSize, elementsCount, m_pFileHandle);
    m_Offset += r;
    return r;
}

bool FileHandleOS::Seek(size_t position, SeekOrigin origin)
{
    int r = 0;

    switch (origin)
    {
    case SeekOrigin::Start:
        r = fseek(m_pFileHandle, (long)position, SEEK_SET);
        break;
    case SeekOrigin::Relative:
        r = fseek(m_pFileHandle, (long)position, SEEK_CUR);
        break;
    case SeekOrigin::End:
        r = fseek(m_pFileHandle, (long)position, SEEK_END);
        break;
    default:
        break;
    }

    return !r;
}

FileHandleUncompressedArchive::FileHandleUncompressedArchive(FILE *fpArchive, size_t dataStart, size_t dataLength)
{
    m_pArchiveFile = fpArchive;
    m_DataStart    = dataStart;
    m_DataLength   = dataLength;

#ifdef WINDOWS
    _fseeki64(m_pArchiveFile, m_DataStart, SEEK_SET);
#else
    fseeko64(m_pArchiveFile, m_DataStart, SEEK_SET);
#endif
}

FileHandleUncompressedArchive::~FileHandleUncompressedArchive()
{
}

size_t FileHandleUncompressedArchive::Position()
{
    return m_Offset;
}

size_t FileHandleUncompressedArchive::Read(size_t elementSize, size_t elementsCount, void *destBuffer)
{
    size_t elementsRead = elementsCount;
    size_t bytesToRead  = elementSize * elementsCount;
    size_t bytesLeft    = m_DataLength - (m_Offset - m_DataStart);

    if (bytesLeft < bytesToRead)
    {
        bytesToRead  = bytesLeft;
        elementsRead = bytesToRead / elementSize;
    }

    fread(destBuffer, elementSize, elementsRead, m_pArchiveFile);

    return elementsRead;
}

bool FileHandleUncompressedArchive::Seek(size_t position, SeekOrigin origin)
{
    if (position > m_DataLength)
        return false;

    switch (origin)
    {
    case SeekOrigin::Start:
        m_Offset = position;
        break;
    case SeekOrigin::Relative:
        m_Offset += position;
        break;
    case SeekOrigin::End:
        m_Offset = m_DataLength - 1 - position;
        break;
    default:
        break;
    }

    fseek(m_pArchiveFile, m_Offset, SEEK_SET);

    return true;
}

size_t FileHandleUncompressedArchive::TotalSize()
{
    return m_DataLength;
}

const char *IFileHandle::FileName()
{
    return m_Name.c_str();
}
