/*
    LightBaker3000 Frontend project,
    (c) 2022-2025 CrazyRussian
*/

#ifdef WINDOWS
#define _CRT_SECURE_NO_WARNINGS

#include <corecrt_io.h>
#include <direct.h>

#endif
#include <filesystem>

#include "../common.h"
#include "../text_utils.h"
#include "fs_core.h"
#include "fs_file_data.h"
#include "fs_file_handle.h"

#include "../application.h"
#include <unistd.h>



std::string FileData::BaseName()
{
    return Application::GetFileSystem()->BaseNameFromPath(m_Name);
}

std::string FileData::DirName()
{
    return Application::GetFileSystem()->BaseDirectoryFromPath(m_Name);
}

void FileData::FlagOwnsData()
{
    m_bOwnsData = true;
}


IArchive::IArchive(const char *fileName)
{
    std::filesystem::path p = std::filesystem::path(fileName);
    auto                  c = std::filesystem::canonical(p);

    m_FileName = c.string();
    m_BaseName = c.filename().string();
}

IArchive::~IArchive()
{
}

FileData *IArchive::LoadFile(const char *name)
{
    return nullptr;
}

std::string &IArchive::ArchiveFileName()
{
    return m_FileName;
}

const char *IArchive::BaseFileName()
{
    return m_BaseName.c_str();
}

const std::vector<const char *> IArchive::AllFiles()
{
    return m_lstFiles;
}

bool IArchive::IsHidden()
{
    return m_bHideFromUser;
}

FileData *FileSystem::LoadFileFromOS(const char *fileName)
{
    FILE *fp = fopen(fileName, "rb");

    if (!fp)
        return nullptr;

    size_t length = 0;
    byte * data   = 0;

    fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    data   = new byte[length + 1];
    memset(data, 0, length + 1);

    rewind(fp);

    fread(data, 1, length, fp);
    fclose(fp);

    std::filesystem::path p = std::filesystem::path(fileName);
    auto                  c = std::filesystem::canonical(p);

    FileData *pResult;
    pResult = new FileData(data, length, c.string().c_str());
    pResult->FlagOwnsData();

    return pResult;
}

FileSystem::FileSystem()
{
    // MountArchive(new Quake1Pak("res.pak"));    
    //MountArchive(new FolderMount("extra/"));
}

FileSystem::~FileSystem()
{
    for (auto &it : m_vecArchiveProviders)
        delete it;
}

FileSystem *FileSystem::Instance() 
{
    return Application::GetFileSystem();
}

FileData *FileSystem::LoadFile(const char *fileName)
{
    FileData *pResult = nullptr;

    for (auto &it : m_vecArchiveProviders)
    {
        if ((pResult = it->LoadFile(fileName)))
            return pResult;
    }

    pResult = LoadFileFromOS(fileName);

    return pResult;
}

FileData *FileSystem::LoadFile(std::string &fileName)
{
    return LoadFile(fileName.c_str());
}

std::string FileSystem::BaseNameFromPath(std::string &fullPath)
{
    std::filesystem::path p = std::filesystem::path(fullPath);
    auto                  c = std::filesystem::canonical(p);

    return c.stem().string();
}

void FileSystem::ChangeCurrentDirectoryToFileDirectory(const std::string &fileName)
{
    std::filesystem::path p    = std::filesystem::path(fileName);
    auto                  c    = std::filesystem::canonical(p);
    std::string           path = c.parent_path().string();

#ifdef WINDOWS
    _chdir(path.c_str());
#else
    chdir(path.c_str());
#endif
}

std::string FileSystem::BaseDirectoryFromPath(const std::string &fileName)
{
    std::filesystem::path p           = std::filesystem::path(fileName);
    auto                  c           = std::filesystem::canonical(p);
    std::string           parent_path = c.parent_path().string();

    return parent_path;
}

std::string FileSystem::ExtensionFromPath(const std::string &filePath)
{
    std::filesystem::path p    = std::filesystem::path(filePath);
    auto                  c    = std::filesystem::canonical(p);
    std::string           path = c.extension().string();

    return path;
}

bool FileSystem::FileExists(std::string fileName)
{
#ifdef WINDOWS
    return _access(fileName.c_str(), 0) == 0;
#else    
    return access(fileName.c_str(), 0) == 0;
#endif
}

int FileSystem::CopyFile(const char *srcPath, const char *dstPath)
{
    FILE *f1, *f2;

    f1 = fopen(srcPath, "rb");
    f2 = fopen(dstPath, "wb");

    char   buffer[BUFSIZ];
    size_t n;

    while ((n = fread(buffer, sizeof(char), sizeof(buffer), f1)) > 0)
    {
        if (fwrite(buffer, sizeof(char), n, f2) != n)
            return -1;
    }

    fclose(f1);
    fclose(f2);

    return 0;
}

std::string FileSystem::ExtractFilePath(const char *path)
{
    char *src;

    src = (char *)path + strlen(path) - 1;

    //
    // back up until a \ or the start
    //
    while (src != path && !PATHSEPARATOR(*(src - 1)))
        src--;

    std::string_view view = std::string_view(path, src - path);
    return std::string(view);
}

std::string FileSystem::ExtractFileBase(const char *path)
{
    char *src;

    src = (char *)path + strlen(path) - 1;

    while (src != path && !PATHSEPARATOR(*(src - 1)))
        src--;

    std::string result;

    while (*src && *src != '.')
    {
        result += *src++;
    }

    return result;
}

std::string FileSystem::ExtractFileExtension(const char *path)
{
    char *src;

    src = (char *)path + strlen(path) - 1;

    //
    // back up until a . or the start
    //
    while (src != path && *(src - 1) != '.')
        src--;

    if (src == path)
    {
        return "";
    }

    std::string result;

    while (*src)
        result += *src++;

    return result;
}

std::string FileSystem::ExtractFileName(const char *path)
{
    auto name = ExtractFileBase(path);
    auto ext  = ExtractFileExtension(path);

    return name + "." + ext;
}

FILE *FileSystem::OpenFileForWriting(std::string &fileName)
{
    FILE *fp = 0;

#ifdef WINDOWS
    fopen_s(&fp, fileName.c_str(), "wb");
#else    
    fp = fopen64(fileName.c_str(), "wb");
#endif

    if (!fp)
        return nullptr;

    return fp;
}

void FileSystem::MakeDir(std::string path)
{
    std::filesystem::create_directory(path);
}

std::string FileSystem::SanitizeFileName(std::string name)
{
    std::string result;

    result.reserve(name.size());

    // \ / : * ? " < > |

    for (auto &ch : name)
    {
        switch (ch)
        {
        case '\\':
        case '/':
        case ':':
        case '*':
        case '?':
        case '"':
        case '<':
        case '>':
        case '|':
            result = '#';
            break;
        default:
            result += ch;
        }
    }
    return result;
}

IFileHandle *FileSystem::OpenFileHandle(const char *filePath)
{
    // OS-only for now
    return new FileHandleOS(filePath);
}

std::string FileSystem::MakeCanonicalPath(const char *src)
{
    std::filesystem::path p = std::filesystem::path(src);
    auto                  c = std::filesystem::canonical(p);

    return c.string();
}

void FileSystem::MountArchive(IArchive * pArchive)
{
    if (!pArchive)
        Application::EPICFAIL("FileSystem::MountArchive(): nullptr input!");

    m_vecArchiveProviders.push_back(pArchive);
}

void FileSystem::UnmountArchive(IArchive *pArchive)
{
    m_vecArchiveProviders.remove_if([&](IArchive *pTest) { return pTest == pArchive; });
}

std::string FileSystem::MakeTemplatePath(const char *fileName, const char *templ)
{
#ifdef WINDOWS

    std::filesystem::path p = std::filesystem::path(fileName);
    auto                  c = std::filesystem::canonical(p);

    std::string directory = c.parent_path().string();
    std::string baseName  = c.stem().string();
    std::string extension = c.extension().string();

    std::string result = std::vformat(templ, std::make_format_args(directory, baseName, extension));
    return result;
#endif
    throw new std::string("Unimplemented");
}