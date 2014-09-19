#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_

#include "convert.h"

#ifdef _WIN32
#include <direct.h>
#include <sys/stat.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

#ifdef _WIN32

class FileSystem {
public:
    static bool IsFileExists(const std::string& file) {
        struct _stat fileStat;
        return (_stat(dir.c_str(), &fileStat) == 0) && !(fileStat.st_mode & _S_IFDIR));
    }
    
    static bool IsFileExists(const std::wstring& file) {
        struct _stat fileStat;
        return (_wstat(wdir.c_str(), &fileStat) == 0) && !(fileStat.st_mode & _S_IFDIR));
    }
    
    static bool IsDirExists(const std::string& dir) {
        struct _stat fileStat;
        return (_stat(dir.c_str(), &fileStat) == 0) && (fileStat.st_mode & _S_IFDIR));
    }
    
    static bool IsDirExists(const std::wstring& wdir) {
        struct _stat fileStat;
        return (_wstat(wdir.c_str(), &fileStat) == 0) && (fileStat.st_mode & _S_IFDIR));
    }
    
    static bool MakeDir(const std::string& dir) {
        return _mkdir(dir.c_str()) == 0;
    }
    
    static bool MakeDir(const std::wstring& wdir) {
        return _wmkdir(dir.c_str()) == 0;
    }
    
    static void TraversalDir(const std::string& path, const std::function<void (const std::string&, bool)>& cb) {
        WIN32_FIND_DATAA fdata;
        HANDLE fh = FindFirstFileA("*", &fdata);
        if(fh == INVALID_HANDLE_VALUE)
            return;
        do {
            std::string file = fdata.cFileName;
            cb(file, (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY));
        } while(FindNextFile(fh, &fdata));
        FindClose(fh);
    }
    
    static void TraversalDir(const std::wstring& wpath, const std::function<void (const std::wstring&, bool)>& cb) {
        WIN32_FIND_DATAW fdataw;
        HANDLE fh = FindFirstFileW(L"*", &fdataw);
        if(fh == INVALID_HANDLE_VALUE)
            return;
        do {
            std::wstring file = fdataw.cFileName;
            cb(file, (fdataw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY));
        } while(FindNextFileW(fh, &fdataw));
        FindClose(fh);
    }
    
};

#else

class FileSystem {
public:
    
    static bool IsFileExists(const std::string& file) {
        struct stat fileStat;
        return (stat(file.c_str(), &fileStat) == 0) && !S_ISDIR(fileStat.st_mode);
    }
    
    static bool IsFileExists(const std::wstring& wfile) {
        return IsFileExists(To<std::string>(wfile));
    }
    
    static bool IsDirExists(const std::string& dir) {
        struct stat fileStat;
        return (stat(dir.c_str(), &fileStat) == 0) && S_ISDIR(fileStat.st_mode);
    }
    
    static bool IsDirExists(const std::wstring& wdir) {
        return IsDirExists(To<std::string>(wdir));
    }
    
    static bool MakeDir(const std::string& dir) {
        return mkdir(dir.c_str(), 0x775) == 0;
    }
    
    static bool MakeDir(const std::wstring& wdir) {
        return MakeDir(To<std::string>(wdir));
    }
    
    static void TraversalDir(const std::string& path, const std::function<void (const std::string&, bool)>& cb) {
        DIR* dir = nullptr;
        struct dirent* dirp = nullptr;
        if((dir = opendir(path.c_str())) == nullptr)
            return;
        struct stat fileStat;
        while((dirp = readdir(dir)) != nullptr) {
            std::string fname = path + '/' + dirp->d_name;
            stat(fname.c_str(), &fileStat);
            cb(dirp->d_name, S_ISDIR(fileStat.st_mode));
        }
        closedir(dir);
    }
    
    static void TraversalDir(const std::wstring& wpath, const std::function<void (const std::wstring&, bool)>& cb) {
        DIR* dir = nullptr;
        struct dirent* dirp = nullptr;
        std::string path = To<std::string>(wpath);
        if((dir = opendir(path.c_str())) == nullptr)
            return;
        struct stat fileStat;
        while((dirp = readdir(dir)) != nullptr) {
            std::string fname = path + '/' + dirp->d_name;
            stat(fname.c_str(), &fileStat);
            std::string name = dirp->d_name;
            cb(To<std::wstring>(name), S_ISDIR(fileStat.st_mode));
        }
        closedir(dir);
    }
    
};

#endif

#endif