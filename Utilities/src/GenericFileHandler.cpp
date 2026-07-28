#include <GenericFileHandler.h>


/** contructor1 */
ttutil::GenericFileHandler::GenericFileHandler(const std::vector<std::string>& filelist,
                                               const std::vector<std::string>& pattern,
                                               const std::vector<std::string>& forbidden)

{
  //set search params 
  filePattern.clear();
  forbiddenFiles.clear();

  setFilePattern(pattern);
  setForbidden(forbidden);

  //init start path
  files.clear();
  std::string cwd = ttutil::FileBrowser::getCurrDir(); 
  for (unsigned int i=0; i<filelist.size(); i++){
    files.push_back( ttutil::FileBrowser::join( cwd, ttutil::FileBrowser::removeSep(filelist[i]) ) );
  }
}

/** contructor2 */
ttutil::GenericFileHandler::GenericFileHandler(const std::string& dirname,
                                               const std::vector<std::string>& pattern,
                                               const std::vector<std::string>& forbidden)
{
   //set search params 
  filePattern.clear();
  forbiddenFiles.clear();

  setFilePattern(pattern);
  setForbidden(forbidden);

  //init start path
  files.clear();
  std::string cwd = ttutil::FileBrowser::getCurrDir(); 
  files.push_back( ttutil::FileBrowser::join( cwd, ttutil::FileBrowser::removeSep(dirname) ) );                   
}

/** sets the filepattern */
void ttutil::GenericFileHandler::setFilePattern(const std::vector<std::string>& pattern){
  if (pattern.size() > 0) filePattern = pattern;  
  else filePattern.clear();                       
  if (filePattern.size() < 1) filePattern.push_back("*");
}

/** sets the filter */
void ttutil::GenericFileHandler::setForbidden(const std::vector<std::string>& forbidden){
  if (forbidden.size() > 0) forbiddenFiles = forbidden;
  else forbiddenFiles.clear();
  forbiddenFiles.push_back("."); 
  forbiddenFiles.push_back(".."); 
}

/** overwrite this function when you want to do something with this file
 *  @param dirname
 */
void ttutil::GenericFileHandler::processFile(const std::string &filename){
  std::cout << "Process file: " << filename << std::endl;
}

/** overwrite this function when you want to do something with this file
 *  @param dirname
 */
void ttutil::GenericFileHandler::processDir(const std::string &dirname){
  std::cout << "Process dir: " << dirname << std::endl;
}


/** starts the recursiv search process 
 *  @param dirname
 *  @param filepattern
 *  @param forbidden dirs and files
 */
void ttutil::GenericFileHandler::run(){
  
  //call user initialization
  initRun();
  
  //starting the process 
  for (unsigned int i=0; i<files.size(); i++){
    std::string& currname = files[i];
    if (ttutil::FileBrowser::isDir(currname)) workDir(currname);
    else if (ttutil::FileBrowser::isFile(currname)) workFile(currname);
  } 
  
  //call user function after running
  endRun();          
} 
 
/** looks into the given directory and processes all files in it 
 *  @param dirname
 */
void ttutil::GenericFileHandler::workDir(const std::string &dirname){
  
  if (!pathAllowed(dirname)) return;

  processDir(dirname);

  ttutil::FileBrowser filebrowser;
  filebrowser.setCurrDir(dirname);
  std::vector<std::string> filelist = filebrowser.listFiles();
  for (unsigned int i=0; i<filelist.size(); i++){
    std::string currname = filebrowser.join(dirname, filelist[i]);
    if (filebrowser.isDir(currname)) workDir(currname);
    else if (filebrowser.isFile(currname)) workFile(currname);
    else{
        //std::cout << currname << " does not exist!"<< std::endl;
    }
  }

}

/** processes the given file 
 *  @param dirname
 */
void ttutil::GenericFileHandler::workFile(const std::string &filename){
  if (!fileAllowed(filename)) return;
  processFile(filename);  
}


/** checks if the filename is allowed 
 *  @param filename
 */
bool ttutil::GenericFileHandler::fileAllowed(const std::string &name){
  if (name.empty()) return false;
  for (unsigned int i=0; i<forbiddenFiles.size(); i++){
    if (name == forbiddenFiles[i]) return false;
  }
  return true;
}


/** checks if the path is allowed 
 *  @param path
 */
bool ttutil::GenericFileHandler::pathAllowed(const std::string &name){
  std::string dirName = ttutil::FileBrowser::removePath(name);
  if (dirName.empty()) return true;
  for (unsigned int i=0; i<forbiddenFiles.size(); i++){
    if (dirName == forbiddenFiles[i]) return false;
  }
  return true;
}