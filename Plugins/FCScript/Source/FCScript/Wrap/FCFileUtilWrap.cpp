#include "FCFileUtilWrap.h"

#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "IPlatformFilePak.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "HAL/PlatformFilemanager.h"
#include "HAL/PlatformApplicationMisc.h"

void FCFileUtilWrap::Register(lua_State* L)
{
    luaL_requiref(L, "FileUtil", LibOpen_wrap, 1);
}

int FCFileUtilWrap::LibOpen_wrap(lua_State* L)
{
    static constexpr LuaRegFunc LibFuncs[] = {
        {"GetAllFiles", GetAllFiles_wrap},
        {"CopyFile", CopyFile_wrap},
        {"DeleteFile", DeleteFile_wrap},
        {"ReadText", ReadText_wrap},
        {"WriteText", WriteText_wrap},
        {"FileExists", FileExists_wrap},
        {"DirectoryExists", DirectoryExists_wrap},
        {NULL, NULL}
    };
    const char* ClassName = lua_tostring(L, 1);
    int Ret = FCExportedClass::RegisterLibClass(L, ClassName, (const LuaRegFunc*)LibFuncs);
    return Ret;
}

FPakPlatformFile  *GetPlatformFileInterface()
{
    FPakPlatformFile* PakPlatformFile = (FPakPlatformFile*)(FPlatformFileManager::Get().FindPlatformFile(FPakPlatformFile::GetTypeName()));
    if (!PakPlatformFile)
    {
        static FPakPlatformFile* GPakPlatformFile = nullptr;
        if (!GPakPlatformFile)
        {
            GPakPlatformFile = new FPakPlatformFile();
        }
        GPakPlatformFile->Initialize(&FPlatformFileManager::Get().GetPlatformFile(), TEXT(""));
        FPlatformFileManager::Get().SetPlatformFile(*GPakPlatformFile);
    }
    return PakPlatformFile;
}

class FLuaFillArrayDirectoryVisitor : public IPlatformFile::FDirectoryVisitor
{
public:
    virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override
    {
        if (bIsDirectory)
        {
            Directories.Add(FilenameOrDirectory);
        }
        else
        {
            Files.Add(FilenameOrDirectory);
        }
        return true;
    }

    TArray<FString> Directories;
    TArray<FString> Files;
};

void PushStringArray(lua_State* L, const TArray<FString> &Files)
{
    lua_createtable(L, 0, 0);
    int nTableIdx = lua_gettop(L);
    int Numb = Files.Num();
    for (int Index = 0; Index < Numb; ++Index)
    {
        const FString &FileName = Files[Index];
        lua_pushstring(L, TCHAR_TO_UTF8(*FileName));
        lua_rawseti(L, nTableIdx, Index + 1);
    }
}

int FCFileUtilWrap::GetAllFiles_wrap(lua_State* L)
{
    // file table GetAllFiles(FilePath, bool bLoopChild)
    const char *InFilePath = lua_tostring(L, 1);
    bool bLoopChildDir = (bool)lua_toboolean(L, 2);
    if(!InFilePath)
    {
        lua_pushnil(L);
        lua_pushnil(L);
        return 2;
    }
    const TCHAR* RelativePath = UTF8_TO_TCHAR(InFilePath);

    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    FLuaFillArrayDirectoryVisitor FallArrayDirVisitor;
    if (PlatformFile.DirectoryExists(RelativePath))
    {
        if (bLoopChildDir)
        {
            PlatformFile.IterateDirectoryRecursively(RelativePath, FallArrayDirVisitor);
        }
        else
        {
            PlatformFile.IterateDirectory(RelativePath, FallArrayDirVisitor);
        }
    }
    PushStringArray(L, FallArrayDirVisitor.Files);
    PushStringArray(L, FallArrayDirVisitor.Directories);
    return 2;
}

int FCFileUtilWrap::CopyFile_wrap(lua_State* L)
{
    // bool CopyFile(DesFilePath, SrcFilePath)
    const char* InDesFilePath = lua_tostring(L, 1);
    const char* InSrcFilePath = lua_tostring(L, 2);
    if (!InDesFilePath || !InSrcFilePath)
    {
        lua_pushboolean(L, false);
        return 1;
    }
    const TCHAR* DesFilename = UTF8_TO_TCHAR(InDesFilePath);
    const TCHAR* SrcFilename = UTF8_TO_TCHAR(InSrcFilePath);
    FPakPlatformFile* PakPlatformFile = GetPlatformFileInterface();
    bool bSuccess = PakPlatformFile->MoveFile(DesFilename, SrcFilename);
    lua_pushboolean(L, bSuccess);
    return 1;
}

int FCFileUtilWrap::DeleteFile_wrap(lua_State* L)
{
    const char* InFilePath = lua_tostring(L, 1);
    if(!InFilePath)
    {
        lua_pushboolean(L, false);
        return 1;
    }
    const TCHAR* Filename = UTF8_TO_TCHAR(InFilePath);
    bool bSuccess = false;

    if (!FPaths::FileExists(Filename))
    {
        lua_pushboolean(L, true);
        return 1;
    }
    FPakPlatformFile* PakPlatformFile = GetPlatformFileInterface();
    bSuccess = PakPlatformFile->DeleteFile(Filename);
    lua_pushboolean(L, bSuccess);
    return 1;
}

int FCFileUtilWrap::ReadText_wrap(lua_State* L)
{
    // string ReadText(FilePath)
    const char* InFilePath = lua_tostring(L, 1);
    if (!InFilePath)
    {
        lua_pushnil(L);
        return 1;
    }
    const TCHAR* Filename = UTF8_TO_TCHAR(InFilePath);
    FPakPlatformFile* PakPlatformFile = GetPlatformFileInterface();
    IFileHandle  *FileHandle = PakPlatformFile->OpenRead(Filename, false);
    if(FileHandle)
    {
        int64 FileSize = FileHandle->Size();
        uint8  *FileBuffer = new uint8[FileSize + 1];
        FileBuffer[FileSize] = 0;
        FileHandle->Read(FileBuffer, FileSize);        
        delete FileHandle;

        lua_pushlstring(L, (const char *)FileBuffer, FileSize);
        delete []FileBuffer;
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}

int FCFileUtilWrap::WriteText_wrap(lua_State* L)
{
    const char* InFilePath = lua_tostring(L, 1);
    if (!InFilePath)
    {
        lua_pushboolean(L, false);
        return 1;
    }
    const TCHAR* Filename = UTF8_TO_TCHAR(InFilePath);
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    IFileHandle* FileHandle = PlatformFile.OpenWrite(Filename);
    if(FileHandle)
    {
        size_t  FileLen = 0;
        const char *FileContent = lua_tolstring(L, 2, &FileLen);
        if(FileLen > 0 && FileContent)
        {
            FileHandle->Write((const uint8*)FileContent, FileLen);
            FileHandle->Flush();
        }
        delete FileHandle;
        lua_pushboolean(L, true);
    }
    else
    {
        lua_pushboolean(L, false);
    }
    return 1;
}

int FCFileUtilWrap::FileExists_wrap(lua_State* L)
{
    const char* InFilePath = lua_tostring(L, 1);
    if (!InFilePath)
    {
        lua_pushboolean(L, false);
        return 1;
    }
    const TCHAR* Filename = UTF8_TO_TCHAR(InFilePath);
    bool bExist = FPaths::FileExists(Filename);
    lua_pushboolean(L, bExist);
    return 1;
}

int FCFileUtilWrap::DirectoryExists_wrap(lua_State* L)
{
    const char* InFilePath = lua_tostring(L, 1);
    if (!InFilePath)
    {
        lua_pushboolean(L, false);
        return 1;
    }
    const TCHAR* Pathname = UTF8_TO_TCHAR(InFilePath);
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    bool bExist = PlatformFile.DirectoryExists(Pathname);
    lua_pushboolean(L, bExist);
    return 1;
}
