#pragma once

#define _HAS_STD_BYTE 0

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include <stringutil.h>


using namespace std;
#include <DIRECT.H>
#include <IO.H>
#include <time.h>
#include <windows.h>
#include <sys/types.h>
#include <sys/stat.h>
//#define timevar time_t
#define timevar __time64_t  
const char SEPARATOR = '\\';

namespace ttutil{

//********************************************************************
//*                         FileBrowser                              *
//*                                                                  *
//*   This class provides filehandling, currently only for windows   *
//*   but you can add POSIX file and directory functionality         *
//*   when you like...                                               *
//*                                                                  *
//*   Written originally for wavefile conversion                     *
//*   by Thomas Thiele in 2003                                       *
//*                                                                  *
//*   The author does not allow the use for other projects           *
//*   without his express permission!                                * 
//*                                                                  *
//*   The author wants to be payed for it. Monthly salary is ok.     *
//*                                                                  *
//******************************************************************** 

class FileBrowser{
	public:
  	FileBrowser();

		void setCurrDir(const std::string& cwd);
    static std::string getCurrDir();
    
    static bool makeDir(const std::string& dirname);
    static bool makeDirRecursively(const std::string& dirname);
    static bool isFile(const std::string& filename);
    static bool isDir(const std::string& dirname);
    static bool isDrive(const std::string& drive);
    static bool exists(const std::string& name);
    static bool isReadable(const std::string& name);
    
    static bool hasSep(const std::string& str);
    static bool isOnlyDrive(const std::string& dirname);
    static bool absolute(const std::string& str);
    //static std::string addSep(std::string& str);
    static std::string addSep(const std::string& str);
    //static std::string removeSep(std::string& str);
    static std::string removeSep(const std::string& str);
    static std::string makeRelative(const std::string& str);
    static std::string join(const std::string& path, const std::string& filename);

    static std::string getPath(const std::string& str);
    static std::string removePath(const std::string& str);
    static std::vector<std::string> splitPath(const std::string& str);
    static std::string removePathPart(const std::string& root, const std::string& str);

    static timevar getTime(const std::string& filename);
    static std::string time2str(timevar time);
    static timevar getSystemTime();

    static long long getSize(const std::string& filename);

    static std::string addEnding(const std::string& filename,
                                 const std::string& ending,
                                 bool caseSensitiv = false);
    static std::string removeEnding(const std::string& filename);
    static std::string getEnding(const std::string& filename);
    static bool hasEnding(const std::string& filename, const std::string& ending);

    static bool renameFile(const std::string& src, const std::string& dst);
    static bool moveFile(const std::string& src, const std::string& dst, bool createDirs = true);
    static bool moveDir(const std::string& src, const std::string& dst);
    static bool copyFile(const std::string& src, const std::string& dst, bool createDirs = true);
    static bool copyDir(const std::string& src, const std::string& dst);
    static bool deleteFile(const std::string& src);
    static bool deleteDir(const std::string& src);
    static void BackupChain(const std::string& filename, int maxBackups);
    static bool IsBackupFilename(const std::string& filename, int maxBackups, std::string& outOrgName);
    
    std::vector<std::string> listFiles();
    std::vector<std::string> filterFiles(const std::string& mask);
    std::vector<std::string> addFilterFiles(const std::string& mask);
    std::vector<std::string> fileList();

  private:
   	std::vector<std::string> filelist;
   	std::string currDir;
};

}

