/*
    LightBaker3000 Frontend project,
    (c) 2023 CrazyRussian
*/

#pragma once

enum class SeekOrigin
{
    Start,
    Relative,
    End
};

class IFileHandle
{
protected:
    size_t m_Offset = 0;
    std::string m_Name;

public:
    IFileHandle()         = default;
    virtual ~IFileHandle() = default;

    virtual size_t TotalSize()                                                      = 0;
    virtual size_t Read(size_t elementSize, size_t elementsCount, void *destBuffer) = 0;
    virtual bool   Seek(size_t position, SeekOrigin origin) = 0;
    virtual size_t Position() = 0;

        const char *FileName();
};

class FileHandleOS: public IFileHandle
{
    FILE *m_pFileHandle = nullptr;

public:
    FileHandleOS(const char* fileName);
    ~FileHandleOS();

    size_t TotalSize() override;
    size_t Position() override;
    size_t Read(size_t elementSize, size_t elementsCount, void *destBuffer) override;
    bool Seek(size_t position, SeekOrigin origin) override;


};

class FileHandleUncompressedArchive: public IFileHandle
{
    FILE *m_pArchiveFile = nullptr;
    
    size_t m_DataStart = 0;
    size_t m_DataLength = 0;
public:
    FileHandleUncompressedArchive(FILE *fpArchive, size_t dataStart, size_t dataLength);
    ~FileHandleUncompressedArchive();

    size_t Position() override;

    size_t Read(size_t elementSize, size_t elementsCount, void *destBuffer) override;

    bool Seek(size_t position, SeekOrigin origin) override;

    size_t TotalSize() override;
};

