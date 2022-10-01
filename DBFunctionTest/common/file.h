/*
 * =====================================================================================
 *
 *       Filename:  file.h
 *    Description:  文件操作相关函数封装（丁富来：2014-4-30）
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

#ifndef _WIND_FILE_H_
#define _WIND_FILE_H_

#include <string>
#include <vector>

#ifndef _WIN32
#ifndef __linux__
#error "Unknown os platform, _WIN32 or __linux__ not defined."
#endif
#endif

using namespace::std;

namespace windcom
{
    // 查找指定路径下匹配指定模式的所有文件,mode最多可包含一个"*"通配符，例如"*.h","*Msg.db"
    // 成功返回0，否则-1
    int FindDirFile(const std::string& path, const string& mode, vector<string>& files);

    //查询子目录，只取当前目录，不递归遍历
    int FindSubDir(const string& path,  vector<string>& files);

    long long GetFileSize(const string& file); // 获取文件大小

    bool FileExist(const string& file);
    bool DirExist(const string& dir);
    bool IsFile(const string& file);
    bool IsDir(const string& dir);
    
    bool CreateDirs(const string& path); // 创建多层目录
    
    int CountFiles(const string& dir); // 获取指定目录下文件数量，跳过目录
    int GetFiles(const string& dir, vector<string> &files); // 获取指定目录下的所有文件全路径
    int GetFileNames(const string& dir, vector<string> &filenames); // 获取指定目下的所有文件名，不包括路径
    int DeleteFiles(const string& dir); // 删除指定目录下所有文件
    
    string GetProcessFile(); // 获取进程文件全路径，包括路径和文件
    string GetProcessName(); // 获取进程文件名，不包括路径
    string GetProcessPath(); // 获取进程文件路径，不包括文件名

    
    time_t GetLastModifyTime(const std::string &fileName);

}

#endif
