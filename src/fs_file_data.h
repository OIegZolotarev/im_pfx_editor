/*
    LightBaker3000 Frontend project,
    (c) 2023 CrazyRussian
*/

#pragma once

// FileData - general file wrapper class
//
class FileData
{
    bool   m_bOwnsData = false;
    byte * m_pData     = 0;
    size_t m_szLength  = 0;

    std::string m_Name;

    int    m_nReferences = 1;
    size_t m_offset;

    friend class FileSystem;
    void FlagOwnsData();

public:
    FileData(byte *data, size_t length, const char *name);
    ~FileData();

    size_t Length();

    byte *       Data();
    std::string &Name();

    void Ref();
    void UnRef();

    template <class T> T *Read(size_t nElements);

    std::string BaseName();
    std::string DirName();
};