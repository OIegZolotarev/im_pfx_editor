/*
    LightBaker3000 Frontend project,
    (c) 2023 CrazyRussian
*/


#include "../common.h"
#include "fs_file_data.h"

FileData::FileData(byte *data, size_t length, const char *name)
{
    if (data)
        m_pData = data;
    else
    {
        m_bOwnsData = true;
        m_pData     = new byte[length];
    }

    m_szLength = length;

    m_Name = name;
}

FileData::~FileData()
{
    if (m_bOwnsData)
        delete[] m_pData;
}

size_t FileData::Length()
{
    return m_szLength;
}

byte *FileData::Data()
{
    return m_pData;
}

std::string &FileData::Name()
{
    return m_Name;
}

void FileData::Ref()
{
    m_nReferences++;
}

void FileData::UnRef()
{
    m_nReferences--;
    if (!m_nReferences)
        delete this;
}