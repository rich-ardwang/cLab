/*
 * =====================================================================================
 *
 *       Filename:  file.h
 *    Description:  �ļ�������غ�����װ����������2014-4-30��
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
    // ����ָ��·����ƥ��ָ��ģʽ�������ļ�,mode���ɰ���һ��"*"ͨ���������"*.h","*Msg.db"
    // �ɹ�����0������-1
    int FindDirFile(const std::string& path, const string& mode, vector<string>& files);

    //��ѯ��Ŀ¼��ֻȡ��ǰĿ¼�����ݹ����
    int FindSubDir(const string& path,  vector<string>& files);

    long long GetFileSize(const string& file); // ��ȡ�ļ���С

    bool FileExist(const string& file);
    bool DirExist(const string& dir);
    bool IsFile(const string& file);
    bool IsDir(const string& dir);
    
    bool CreateDirs(const string& path); // �������Ŀ¼
    
    int CountFiles(const string& dir); // ��ȡָ��Ŀ¼���ļ�����������Ŀ¼
    int GetFiles(const string& dir, vector<string> &files); // ��ȡָ��Ŀ¼�µ������ļ�ȫ·��
    int GetFileNames(const string& dir, vector<string> &filenames); // ��ȡָ��Ŀ�µ������ļ�����������·��
    int DeleteFiles(const string& dir); // ɾ��ָ��Ŀ¼�������ļ�
    
    string GetProcessFile(); // ��ȡ�����ļ�ȫ·��������·�����ļ�
    string GetProcessName(); // ��ȡ�����ļ�����������·��
    string GetProcessPath(); // ��ȡ�����ļ�·�����������ļ���

    
    time_t GetLastModifyTime(const std::string &fileName);

}

#endif
