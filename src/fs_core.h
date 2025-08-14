/*
    LightBaker3000 Frontend project,
    (c) 2022 CrazyRussian
*/

#pragma once
#include "fs_file_handle.h"
#include "fs_file_data.h"


class IArchive
{
protected:
    std::string m_FileName;
    std::string m_BaseName;

    std::vector<const char *> m_lstFiles;

    bool m_bHideFromUser = false;

public:
    IArchive(const char *fileName);
    virtual ~IArchive();

    virtual FileData *LoadFile(const char *name);
    std::string &     ArchiveFileName();

    const char *                    BaseFileName();
    const std::vector<const char *> AllFiles();

    bool IsHidden();
};

class FileSystem
{
    std::list<IArchive *> m_vecArchiveProviders;

private:

    friend IArchive;
    friend class FolderMount;

    static FileData *LoadFileFromOS(const char* fileName);

public:
    FileSystem();
    ~FileSystem();
        
    void MountArchive(IArchive * pArchive);
    void UnmountArchive(IArchive * pArchive);

    static FileSystem *Instance();

    [[nodiscard]] FileData *LoadFile(const char *fileName);
    [[nodiscard]] FileData *LoadFile(std::string &fileName);

    void ChangeCurrentDirectoryToFileDirectory(const std::string &fileName);

    /*
     Return file name without extension from path, file must exist!
     Examples:
    		/home/user/path/to/file.txt -> file
    		C:\User\Documents\file.txt -> file
    */
    std::string BaseNameFromPath(std::string &path);

    /*/
     Returns parent path from full file path, file must exist!
     Examples:
          /home/user/path/to/file.txt -> /home/user/path/to/
          C:\User\Documents\file.txt -> C:\User\Documents\ 
    */
    std::string BaseDirectoryFromPath(const std::string &fileName);

    /* 
      Return file extension with dot,
        file must exist !
      Examples:
          /home/user/path/to/file.txt -> .txt
          C:\User\Documents\file.bsp -> .bsp
    */
    std::string ExtensionFromPath(const std::string &path);

    /*
     ��������� ��� ����� �� �������
     ����������� ��� ���� ������� � ��� �� �������� ��� � �������� ����
     ��������� �������
    	{0} - ������� � ������� ��������� ����
    	{1} - ��� ����� ��� ����������
    	{2} - ���������� ����� (� ������ - ".txt")
    
    	������:
    		MakeTemplatePath("/home/user/file.obj","{0}.lm.png") -> /home/user/file.lm.png
    */

    std::string MakeTemplatePath(const char *fileName, const char *templ);

    static bool FileExists(std::string fileName);

    int CopyFile(const char *srcPath, const char *dstPath);

// Версии для несуществующих путей
#define PATHSEPARATOR(c) ((c) == '\\' || (c) == '/')

    static std::string ExtractFilePath(const char *path); // /home/user/file.txt -> /home/user/ 

    static std::string ExtractFileBase(const char *path); // /home/user/file.txt -> file

    static std::string ExtractFileExtension(const char *path); // /home/user/file.txt -> txt

    
    static std::string         ExtractFileName(const char *path); // /home/user/file.txt - > file.txt
    static FILE *OpenFileForWriting(std::string &fileName);
    static void                MakeDir(std::string path);

    static std::string SanitizeFileName(std::string result);


    IFileHandle *OpenFileHandle(const char *filePath);

    static std::string MakeCanonicalPath(const char* src);

};
