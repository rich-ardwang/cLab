/*
 * =====================================================================================
 *
 *       Filename:  file.cpp
 *    Description:  文件操作相关函数实现（丁富来：2014-4-30）
 *        Version:  1.0
 *        Created:  2014-4-30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  dingfulai
 *        Company:  wind, Inc.
 *
 * =====================================================================================
 */

#include "common/file.h"

#include <cstdio>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <direct.h>
#define S_ISDIR(mode) ((mode) & _S_IFDIR)
#define S_ISREG(mode) ((mode) & _S_IFREG)
#elif defined __linux__
#include <sys/io.h>
#include <dirent.h>
#include <errno.h>
#endif

#include <sys/stat.h>
#include <errno.h>

namespace windcom
{
#ifndef _WIN32
    static inline bool IsDir(const char* path);
    static bool IsMatch(const string& path, const string& findFormat);
#endif

    int FindDirFile(const string& path, const string& mode, vector<string>& files)
    {
#ifdef _WIN32
        string strFindFormat = path + "/" + mode;

        _finddata_t data;
        intptr_t handle = _findfirst(strFindFormat.c_str(), &data);
        if (handle != -1)
        {
            do{
                if (data.attrib == _A_NORMAL || data.attrib == _A_RDONLY || data.attrib == _A_ARCH)
                {
                    string strFullPath = path + "/" + data.name;
                    files.push_back(strFullPath);
                }
                else if (data.attrib == _A_SUBDIR)
                {
                    if (!(strcmp(data.name,".") == 0 || strcmp(data.name,"..") == 0))
                    {
                        //子目录，递归
                        string subPath = path + "/" + data.name;
                        FindDirFile(subPath, mode, files);
                    }
                }
            }while(_findnext(handle, &data) != -1);
            _findclose(handle);
        }
        else
            return -1;

        return 0;
#else
        char childpath[512];

        DIR *pDir = opendir(path.c_str());
        if(!pDir)
            return -1;

        struct dirent* ent;

        while((ent=readdir(pDir)) != NULL)
        {
            snprintf(childpath, sizeof(childpath), "%s/%s", path.c_str(), ent->d_name);

            if((ent->d_type & DT_DIR) || (ent->d_type == DT_UNKNOWN && IsDir(childpath)))
            {
                //把当前目录.，上一级目录..及隐藏文件都去掉，避免死循环遍历目录
                if(strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0)
                    continue;

                FindDirFile(childpath, mode, files);
            }
            else
            {
                if(IsMatch(ent->d_name,mode.c_str())) files.push_back(childpath);
            }
        }
        closedir(pDir);
        return 0;
#endif
    }

    //查询子目录，只取当前目录，不递归遍历
    int FindSubDir(const string& path, vector<string>& files)
    {
#ifdef _WIN32
        string strFindFormat = path + "/*";

        _finddata_t data;
        intptr_t handle = _findfirst(strFindFormat.c_str(), &data);
        if (handle != -1)
        {
            do{
                if (data.attrib == _A_SUBDIR)
                {
                    if (!(strcmp(data.name,".") == 0 || strcmp(data.name,"..") == 0))
                    {
                        //子目录，不递归
                        files.push_back(data.name);
                    }
                }
            }while(_findnext(handle, &data) != -1);
            _findclose(handle);
        }
        else
            return -1;

        return 0;
#else
        char childpath[512];

        DIR *pDir = opendir(path.c_str());
        if(!pDir)
            return -1;

        struct dirent* ent;

        while((ent=readdir(pDir)) != NULL)
        {
            snprintf(childpath, sizeof(childpath), "%s/%s", path.c_str(), ent->d_name);

            if((ent->d_type & DT_DIR) || (ent->d_type == DT_UNKNOWN && IsDir(childpath)))
            {
                //把当前目录.，上一级目录..及隐藏文件都去掉，避免死循环遍历目录
                if(strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0)
                    continue;

                if(IsMatch(ent->d_name, "*")) 
                    files.push_back(ent->d_name);
            }
        }
        closedir(pDir);
        return 0;
#endif
    }


    bool CreateDirs(const std::string & sPath)
    {
        std::string path = sPath;
        std::replace(path.begin(), path.end(), '\\', '/');

        // 根据sPath从左向右扫描,并逐一建立目录
        string tmp;
        for(string::size_type pos = 0; ;++pos)
        {
            pos = path.find( '/', pos);
            tmp = path.substr(0, pos);
#ifdef _WIN32
            if(_access(tmp.c_str(), 0) != 0 && !tmp.empty())
#else
            if(access(tmp.c_str(), 0) != 0 && !tmp.empty())
#endif
            {
#ifdef _WIN32
                if (0 != _mkdir(tmp.c_str())) // 网络共享的地址如 192.168.0.1 所以该处不返回，继续执行
                {
                    //return false;
                }
#else
                if (0 != mkdir(tmp.c_str(), 0755)) // 网络共享的地址如 192.168.0.1 所以该处不返回，继续执行
                {                
                    //return false;
                }
#endif
            }
            if(pos == string::npos)
            {
                break;
            }
        }
        return true;
    }

#ifndef _WIN32
    static inline bool IsDir(const char* path)
    {
        struct stat filestat;
        lstat(path, &filestat);
        return S_ISDIR(filestat.st_mode) != 0;
    }

    static bool IsMatch(const string& path, const string& findFormat)
    {   
        bool found = false;

        string::size_type pos = findFormat.find('*');
        if (string::npos != pos)
        {
            string s1 = findFormat.substr(0, pos);
            string s2 = findFormat.substr(pos + 1, findFormat.length() - pos - 1);

            string::size_type pos1 = string::npos;
            string::size_type pos2 = string::npos;
            if(s1.empty())
            {
                pos2 = path.find(s2);
                if (pos2 != string::npos && pos2 + s2.length() == path.length())
                    found = true;
            }
            else if(s2.empty())
            {
                pos1 = path.find(s1);
                if (0 == pos1)
                    found = true;
            }
            else
            {
                pos1 = path.find(s1);
                pos2 = path.find(s2);
                if (pos1 == 0 && (pos2 != string::npos && pos2 + s2.length() == path.length()))
                    found = true;
            }
        }
        else
        {
            found = true;
        }

        return found;
    }
#endif

    bool IsFile(const string& file)
    {
#ifdef _WIN32
        struct _stat st;
        int ret = _stat(file.c_str(),&st);
#else
        struct stat st;
        int ret = stat(file.c_str(), &st);
#endif
        return (!ret && S_ISREG(st.st_mode));
    }

    bool IsDir(const string& dir)
    {
#ifdef _WIN32
        struct _stat st;
        int ret = _stat(dir.c_str(),&st);
#else
        struct stat st;
        int ret = stat(dir.c_str(), &st);
#endif
        return (!ret && S_ISDIR(st.st_mode));
    }

    string GetProcessPath()
    {
        string str = GetProcessFile();
        string::size_type pos = str.find_last_of("/\\");
        return(string::npos == pos)?str:str.substr(0, pos);
    }


    //获取文件路径的程序名部分，取最后一个'/'或'\\'之后的部分
    string GetProcessName()
    {
        string str = GetProcessFile();
        string::size_type pos = str.find_last_of("/\\");
        return (string::npos == pos)?"":str.substr(pos + 1, str.length() + pos - 1);
    }

    string GetProcessFile()
    {
        string strPath;
        char fullPath[1024] = {0};

#ifdef _WIN32
        if(GetModuleFileNameA(NULL, fullPath, sizeof(fullPath)))
        {
            strPath = fullPath;
        }
        else
        {
            printf("GetProcessFile failed, error %d\n", GetLastError());
        }
#else
        char sysfile[] = "/proc/self/exe";

        if ( -1 != readlink( sysfile, fullPath, sizeof(fullPath)))
        {
            strPath = fullPath;
        }
        else
        {
            printf("GetProcessFile failed, error %d\n", errno);
        }
#endif
        return strPath;
    }

    long long GetFileSize(const std::string &fileName)
    {
#ifdef _WIN32
        struct _stat fileInfo;
        if (0 == _stat(fileName.c_str(), &fileInfo))
        {
            return fileInfo.st_size;
        }
        else
        {
            return 0;
        }
#else
        struct stat fileInfo;
        if (stat(fileName.c_str(), &fileInfo) < 0)
        {
            return 0;
        }
        else
        {
            return fileInfo.st_size;
        }
#endif
        return 0;
    }


    int GetFiles(const std::string & rootPath, std::vector<std::string> &fileList )
    {
#ifdef _WIN32
        string strFindFormat = rootPath + "/*.*";

        _finddata_t data;
        intptr_t handle = _findfirst(strFindFormat.c_str(), &data);
        if (handle == -1)
        {
            return -1;
        }

        do
        {
            if (data.attrib == _A_SUBDIR)
            {
                if ( strcmp(data.name,".") == 0 || strcmp(data.name,"..") == 0 )
                {
                    continue;
                }
                string strFullPath = rootPath + "/" + data.name;
                GetFiles(strFullPath,fileList);
            }
            else if (data.attrib == _A_NORMAL || data.attrib == _A_RDONLY || data.attrib == _A_ARCH )
            {
                string strFullPath = rootPath + "/" + data.name;
                fileList.push_back(strFullPath);
            }
            else
            {

            }
        }while(_findnext(handle, &data) != -1);
        _findclose(handle);


        return 0;
#else
        char childpath[512];

        DIR *pDir = opendir(rootPath.c_str());
        if(!pDir)
        {
            return -1;
        }

        struct dirent* ent;

        while((ent=readdir(pDir)) != NULL)
        {
            snprintf(childpath, sizeof(childpath), "%s/%s", rootPath.c_str(), ent->d_name);

            if((ent->d_type & DT_DIR) || (ent->d_type == DT_UNKNOWN && DirExist(childpath)))
            {
                //把当前目录.，上一级目录..及隐藏文件都去掉，避免死循环遍历目录
                if(strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0)
                {
                    continue;
                }
                GetFiles(childpath, fileList);
            }
            else
            {
                fileList.push_back(childpath);
            }
        }
        closedir(pDir);
        return 0;
#endif
    }


    int DeleteFiles(const std::string& dir)
    {
#ifdef _WIN32
        std::string strFindFormat = dir + "/*.*";

        _finddata_t data;
        intptr_t handle = _findfirst(strFindFormat.c_str(), &data);
        if (handle == -1)
        {
            return -1;
        }

        do
        {
            if (data.attrib == _A_SUBDIR)
            {
                if ( strcmp(data.name,".") == 0 || strcmp(data.name,"..") == 0 )
                {
                    continue;
                }
                std::string strFullPath = dir + "/" + data.name;
                DeleteFiles(strFullPath);
            }
            else
            {
                std::string strFullPath = dir + "/" + data.name;
                remove(strFullPath.c_str());
            }
        }while(_findnext(handle, &data) != -1);
        _findclose(handle);

        return 0;
#else
        char childpath[512];

        DIR *pDir = opendir(dir.c_str());
        if(!pDir)
        {
            return -1;
        }

        struct dirent* ent;

        while((ent=readdir(pDir)) != NULL)
        {
            snprintf(childpath, sizeof(childpath), "%s/%s", dir.c_str(), ent->d_name);

            if((ent->d_type & DT_DIR) || (ent->d_type == DT_UNKNOWN && DirExist(childpath)))
            {
                //把当前目录.，上一级目录..及隐藏文件都去掉，避免死循环遍历目录
                if(strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0)
                {
                    continue;
                }
                DeleteFiles(childpath);
            }
            else
            {
                if ( unlink(childpath) != 0 )
                {
                    //删除文件出错了，怎么办呢？
                }
            }
        }
        closedir(pDir);
        return 0;
#endif
    }

    static bool _FileDirExist(const std::string &f,bool DirFlag = true )
    {
#ifdef _WIN32
        struct _stat st;
#define S_ISDIR(mode) ((mode) & _S_IFDIR)
#define S_ISREG(mode) ((mode) & _S_IFREG)

        int ret = _stat(f.c_str(),&st);
#else
        struct stat st;
        int ret = stat(f.c_str(),&st);
#endif
        if ( ret < 0 )
        {
            if ( errno == ENOENT )
            {
                return false;
            }
            /* 其他错误怎么办？这个函数只能返回bool值是不够的，暂时先返回false吧*/
            return false;
        }
        if ( DirFlag ) //对目录进行判断
        {
            if (S_ISDIR(st.st_mode) )
            {
                return true;
            }
            return false;
        }
        if ( S_ISREG(st.st_mode) ) //对普通文件进行判断
        {
            return true;
        }
        return false;
    }

    bool FileExist(const std::string &path)
    {
        return _FileDirExist(path, false);
    }

    bool DirExist(const std::string &path)
    {
        return _FileDirExist(path, true);
    }


    time_t GetLastModifyTime(const std::string &fileName)
    {
#ifdef _WIN32
        struct _stat64 fileInfo;
        if (0 == _stat64(fileName.c_str(), &fileInfo))
            return fileInfo.st_mtime;
        else
            return 0;
#else
        struct stat fileInfo;
        if (stat(fileName.c_str(), &fileInfo) < 0)
        {
            return 0;
        }
        else
        {
            return fileInfo.st_mtime;
        }
#endif
    }

}
