#include "FileBrowser.h"

//************************************************************************
//*               CONSTRUCTORS / DESTRUCTORS / OPERATORS                 *
//************************************************************************

ttutil::FileBrowser::FileBrowser() : currDir("."), filelist() {}


//************************************************************************
//*                         PUBLIC METHODS                               *
//************************************************************************

void ttutil::FileBrowser::setCurrDir(const std::string& cwd){
  std::string cwdset = isOnlyDrive(cwd) ? cwd + SEPARATOR : cwd;
  if (!_chdir(cwdset.c_str()))
  { 
  	currDir = getCurrDir();
  }
}


std::string ttutil::FileBrowser::getCurrDir(){
  char* cwdch;
  cwdch = _getcwd(NULL, 256);
	if (cwdch){
  	std::string cwd(cwdch);
    free(cwdch);
    return cwd;
  }
	return "";
}


bool ttutil::FileBrowser::makeDir(const std::string& dirname){
  return _mkdir(dirname.c_str()) == 0;
}

bool ttutil::FileBrowser::makeDirRecursively(const std::string& dirname){
  //still existing?
  if (isDir(dirname) || isDrive(dirname)) return true;

  //try to make it
  if (makeDir(dirname)) return true;

  //if not make sure upper directory exists
  //if not try to make this directory
  //and try it again
  return makeDirRecursively(getPath(removeSep(dirname))) && makeDir(dirname);
}

std::vector<std::string> ttutil::FileBrowser::listFiles(){
	return filterFiles("*");
}


bool ttutil::FileBrowser::isFile(const std::string& filename){
  struct __stat64 fileinfo;
  int result = _stat64(filename.c_str(), &fileinfo);
  //std::cout << " isFile: " << filename << " " << std::ios::hex << fileinfo.st_mode << std::ios::dec << ":" << ((result == 0) && !((fileinfo.st_mode & _S_IFDIR) != 0)) <<  std::endl;
  return (result == 0) && !((fileinfo.st_mode & _S_IFDIR) != 0); 
}


bool ttutil::FileBrowser::isDir(const std::string& dirname){
  std::string name = removeSep(dirname);
  struct __stat64 fileinfo;
  int result = _stat64(name.c_str(), &fileinfo);
  //std::cout << " isDir: " << name << " " << std::ios::hex << fileinfo.st_mode << std::ios::dec << ":" << ((result == 0) && ((fileinfo.st_mode & _S_IFDIR) != 0)) << std::endl;
  return (result == 0) && ((fileinfo.st_mode & _S_IFDIR) != 0); 
}
 
bool ttutil::FileBrowser::isOnlyDrive(const std::string& dirname){
  return    dirname.length() == 2
         && ((dirname[0] > 'A' && dirname[0] < 'Z') || (dirname[0] > 'a' && dirname[0] < 'z')) 
         && dirname[1] == ':';
}         

bool ttutil::FileBrowser::isDrive(const std::string& drive){
  std::string driveonly = removeSep(drive);
  if (!isOnlyDrive(driveonly)) return false;
  char buffer[100]; 
  int size = ::GetLogicalDriveStringsA(100, buffer);
  for (int i=0; i<size; i+=4){
    if (drive[0] == buffer[i]) return true;
  }
  return false;
}

bool ttutil::FileBrowser::exists(const std::string& name){
  std::string nametmp = removeSep(name);
  struct __stat64 fileinfo;
  return _stat64(name.c_str(), &fileinfo) == 0;
}


bool ttutil::FileBrowser::isReadable(const std::string& name){
  // 4 read permission
  // 6 means read and write permission
  return _access(name.c_str(), 4) != -1;
}


timevar ttutil::FileBrowser::getTime(const std::string& filename){
  if (!exists(filename) ) return 0;
  struct __stat64 fileinfo;
  int result = _stat64(filename.c_str(), &fileinfo);
  return result == 0 ? fileinfo.st_mtime : 0;
}


std::string ttutil::FileBrowser::time2str(timevar time){
  //std::string s(26, ' ');
  char data[27] = {' '};
  //_ctime64_s(s.data(), s.size(), &time);
  _ctime64_s(data, 26, &time);
  return std::string(data);
}

timevar ttutil::FileBrowser::getSystemTime(){
  return time(NULL);
}


///removes the last "\"
std::string ttutil::FileBrowser::removeSep(const std::string& str){
  std::string tmp(str);
  if (hasSep(tmp)) tmp = tmp.substr(0, tmp.length() - 1);
  return tmp;
}

/** removes the first "\" so that the absolute path is converted in a relative one
 *  "C:\temp" turns to "C\temp"
 */
std::string ttutil::FileBrowser::makeRelative(const std::string& str){
  std::string result = str; 
#ifdef WIN32
  if (    ((str[0] > 'A' && str[0] < 'Z') || (str[0] > 'a' && str[0] < 'z')) 
       &&   str[1] == ':'
       &&   str[2] == SEPARATOR) 
  {
    //remove the ":"
    char c = str[0];
    result = ttutil::StringUtil::toupper(std::string(1, c)) + str.substr(2, str.length() - 2);    
  }
#endif   
  if (str[0] == SEPARATOR){
    result = result.substr(1, str.length() - 1);  
  }
  return result;
}

///adds the last "\"
/*std::string ttutil::FileBrowser::addSep(std::string& str){
  if (!hasSep(str)) str += SEPARATOR;
  return str;
}*/

///adds the last "\"
std::string ttutil::FileBrowser::addSep(const std::string& str){
  std::string tmp(str);
  if (!hasSep(tmp)) tmp += SEPARATOR;
  return tmp;
}

///checks if string ends with a separator
bool ttutil::FileBrowser::hasSep(const std::string& str){
  return ( str[str.length()-1] == SEPARATOR);
}

///checks if string is an absolute path
bool ttutil::FileBrowser::absolute(const std::string& str){
  if (str[0] == SEPARATOR) return true;
#ifdef WIN32
  if (str.length() < 2) return false;
  if (    ((str[0] > 'A' && str[0] < 'Z') || (str[0] > 'a' && str[0] < 'z')) 
       &&   str[1] == ':'
       //&&   str[2] == SEPARATOR
       ) 
       return true;
#endif   
  return false;
}

std::string ttutil::FileBrowser::getPath(const std::string& str){
  char path_buffer[_MAX_PATH];
  char drive[_MAX_DRIVE];
  char dir[_MAX_DIR];
  char fname[_MAX_FNAME];
  char ext[_MAX_EXT];

  _splitpath_s(str.c_str(), drive, dir, fname, ext );
  _makepath_s( path_buffer, drive, dir, "", "" );
  return std::string(path_buffer);

}

std::string ttutil::FileBrowser::removePath(const std::string& str){
  char path_buffer[_MAX_PATH];
  char drive[_MAX_DRIVE];
  char dir[_MAX_DIR];
  char fname[_MAX_FNAME];
  char ext[_MAX_EXT];

  _splitpath_s(str.c_str(), drive, dir, fname, ext );
  _makepath_s( path_buffer, "", "", fname, ext );
  return std::string(path_buffer);

}

std::vector<std::string> ttutil::FileBrowser::splitPath(const std::string& str){
  char path_buffer[_MAX_PATH];
  char drive[_MAX_DRIVE];
  char dir[_MAX_DIR];
  char fname[_MAX_FNAME];
  char ext[_MAX_EXT];

  _splitpath_s(str.c_str(), drive, dir, fname, ext);
  _makepath_s(path_buffer, drive, dir, "", "");
  const std::string directory(path_buffer);
  _makepath_s(path_buffer, "", "", fname, ext);
  const std::string filename(path_buffer);
  return std::vector < std::string > {directory, filename};

}

std::string ttutil::FileBrowser::removePathPart(const std::string& root, const std::string& str){
  if (root.empty() || str.empty() || root.length() > str.length() ) return str;
  if (str.find(root) != std::string::npos) return ttutil::FileBrowser::makeRelative(str.substr(root.length()));
  return str; 
}


std::string ttutil::FileBrowser::join(const std::string& path, const std::string& filename){
  if (path.empty() || absolute(filename)) return filename;
  if (filename.empty()) return path;
  return addSep(path) + filename;
}

std::vector<std::string> ttutil::FileBrowser::filterFiles(const std::string& mask){
  //clear the vector
  filelist.clear();
	return addFilterFiles(mask);
}

std::vector<std::string> ttutil::FileBrowser::addFilterFiles(const std::string& mask){
  struct _finddata_t fileinfo;
  long handle = _findfirst(mask.c_str(), &fileinfo);
  int result = (handle != -1L) ? 0 : -1 ;
  while(!result){
  	filelist.push_back(ttutil::FileBrowser::getPath(mask) + std::string(fileinfo.name));
    result = _findnext(handle, &fileinfo);
  }
  //if (handle != -1L) _findclose(handle);
  return filelist;
}

std::vector<std::string> ttutil::FileBrowser::fileList(){ return filelist; }

std::string ttutil::FileBrowser::addEnding(const std::string& filename, 
                                   const std::string& ending, 
                                   bool caseSensitiv)
{
  std::string pureEnding = (( ending[0] == '.') ? ending : "." + ending );                   
  if (    filename.rfind(pureEnding) == std::string::npos 
       && (    caseSensitiv 
            || ttutil::StringUtil::toupper(filename)
                .rfind(ttutil::StringUtil::toupper(pureEnding)) == std::string::npos 
          )
     )
  {
    return filename;
  } 
  return filename + pureEnding;             
}

std::string ttutil::FileBrowser::removeEnding(const std::string& filename){
  const size_t dotPos = filename.rfind('.');
  if (dotPos == std::string::npos) return filename;
  return filename.substr(0, dotPos);
}

bool ttutil::FileBrowser::renameFile(const std::string& src, const std::string& dst){
  return rename(src.c_str(), dst.c_str()) == 0;
}

bool ttutil::FileBrowser::moveDir(const std::string& src, const std::string& dst){
  if (!copyDir(src, dst)) return false;
  return deleteDir(src);
}

bool ttutil::FileBrowser::moveFile(const std::string& src, const std::string& dst, bool createDirs){

  //if createDirs make sure that directory exists
  if (createDirs){
    std::string path = getPath(dst);
    makeDirRecursively(path);
  }

  //std::cout << "Try to move to " << dst << std::endl;
  if (!MoveFile(src.c_str(), dst.c_str()))
  {
    if (!CopyFile(src.c_str(), dst.c_str(), false)) return false;
    deleteFile(src.c_str());
    return true;
  }

  return false;
}

bool ttutil::FileBrowser::copyDir(const std::string& src, const std::string& dst){
  std::cout << "Copy directory: " << src << "->" << dst <<  std::endl;
  bool ok = true;

  //if createDirs make sure that directory exists
  std::string path = getPath(dst);
  makeDirRecursively(path);
 
  //list all entries
  ttutil::FileBrowser filebrowser;
  unsigned int i;
  filebrowser.setCurrDir(src);
  std::vector<std::string> filelist = filebrowser.listFiles();
  for (i=0; i<filelist.size(); i++){
    if (filelist[i] != "." && filelist[i] != ".."){
      std::string srcName = filebrowser.join(src, filelist[i]);
      std::string dstName = filebrowser.join(dst, filelist[i]);
      if (filebrowser.isDir(srcName)){
        ok = ok && copyDir(srcName, dstName);
      }
      else if (filebrowser.isFile(srcName)){
        ok = ok && copyFile(srcName, dstName, true);  
      }
      else{
        std::cerr << "Error, no file nor dir...!!!" << std::endl; 
      }
    }
  }
  return ok;
}

bool ttutil::FileBrowser::copyFile(const std::string& src, const std::string& dst, bool createDirs){
  std::cout << "Copy File: " << src << "->" << dst << std::endl;
  //if createDirs make sure that directory exists
  if (createDirs){
    std::string path = getPath(dst);
    makeDirRecursively(path);
  }

  //use the windows API function
  bool success = CopyFile(src.c_str(), dst.c_str(), false);
  if (!success){ 
    DWORD errorcode = GetLastError();
    std::cerr << "Error while copy file (Errorcode: " << errorcode << ")!" << std::endl;
  }
  return success; 
}

bool ttutil::FileBrowser::deleteDir(const std::string& src){
  std::cout << "Try to delete directory " << src << std::endl;
  bool ok = true;
  //list all entries
  ttutil::FileBrowser filebrowser;
  unsigned int i;
  filebrowser.setCurrDir(src);
  std::vector<std::string> filelist = filebrowser.listFiles();
  for (i=0; i<filelist.size(); i++){
    if (filelist[i] != "." && filelist[i] != ".."){
      std::string srcName = filebrowser.join(src, filelist[i]);
      if (filebrowser.isDir(srcName)){
        ok = ok && deleteDir(srcName);
      }
      else if (filebrowser.isFile(srcName)){
        ok = ok && deleteFile(srcName);  
      }
      else{
        std::cerr << "deleteDir: Error, no file nor dir...!!!" << std::endl; 
      }
    }
  }
  if (ok){
    filebrowser.setCurrDir(src + "\\..");
    if (!::RemoveDirectoryA(src.c_str())){
      DWORD errorcode = GetLastError();
      std::cerr << "deleteDir: Could not delete directory " << src << ": " << errorcode << "!" << std::endl; 
      std::cerr << "CWD: " << filebrowser.getCurrDir() << std::endl;
      ok = false;
    }
  }
  return ok;
}

bool ttutil::FileBrowser::deleteFile(const std::string& src){
  std::cout << "Delete File: " << src << std::endl;
  return remove(src.c_str()) == 0;
}

void ttutil::FileBrowser::BackupChain(const std::string& filename, const int maxBackups){
  char drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT], path_buffer[_MAX_PATH];
  _splitpath_s(filename.c_str(), drive, dir, fname, ext);
  auto backupPath = [&](const int n) -> std::string {
    _makepath_s(path_buffer, drive, dir, (std::string(fname) + "_backup_" + StringUtil::int2string(n)).c_str(), ext);
    return std::string(path_buffer);
  };
  const std::string highest = backupPath(maxBackups);
  if (exists(highest)) deleteFile(highest);
  for (int n = maxBackups - 1; n >= 1; --n){
    const std::string from = backupPath(n);
    if (exists(from)) renameFile(from, backupPath(n + 1));
  }
  if (exists(filename)) renameFile(filename, backupPath(1));
}

bool ttutil::FileBrowser::IsBackupFilename(const std::string& filename, const int maxBackups, std::string& outOrgName){
  char drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
  _splitpath_s(filename.c_str(), drive, dir, fname, ext);
  const std::string base(fname);
  const std::string tag("_backup_");
  const size_t tagPos = base.rfind(tag);
  if (tagPos == std::string::npos) return false;
  const std::string nStr = base.substr(tagPos + tag.size());
  if (nStr.empty()) return false;
  for (const char c : nStr){ if (c < '0' || c > '9') return false; }
  const int n = std::stoi(nStr);
  if (n < 1 || n > maxBackups) return false;
  outOrgName = base.substr(0, tagPos) + "_" + nStr;
  return true;
}


long long ttutil::FileBrowser::getSize(const std::string& filename){
  struct __stat64 fileinfo;
  int result = _stat64(filename.c_str(), &fileinfo);
  return result == 0 ? fileinfo.st_size : 0;
}