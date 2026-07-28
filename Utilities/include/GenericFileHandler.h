#ifndef __FILECHECK_GENERICFILEHANDLER__
#define __FILECHECK_GENERICFILEHANDLER__

#include <iostream>
#include <string>
#include <vector>

#include <FileBrowser.h>

namespace ttutil{

class GenericFileHandler{

  public: 

    /** contructor1 */
    GenericFileHandler(const std::vector<std::string>& filelist,
                       const std::vector<std::string>& pattern = std::vector<std::string>(),
                       const std::vector<std::string>& forbidden = std::vector<std::string>());

    /** contructor2 */
    GenericFileHandler(const std::string& dirname,
                       const std::vector<std::string>& pattern = std::vector<std::string>(),
                       const std::vector<std::string>& forbidden = std::vector<std::string>());

    /** starts the recursiv search process */
    void run(); 

    /** sets the filepattern */
    void setFilePattern(const std::vector<std::string>& pattern);

    /** sets the filter */
    void setForbidden(const std::vector<std::string>& forbidden);

  protected:

    /** called before runnning */
    virtual void initRun(){};

    /** called after runnning */
    virtual void endRun(){};

    /** overwrite this function when you want to do something with this file
     *  @param filename
     */
    virtual void processFile(const std::string &filename);

    /** overwrite this function when you want to do something with this directory
     *  @param dirname
     */
    virtual void processDir(const std::string &dirname);

    /** checks if the filename is allowed 
     *  @param filename
     */
    virtual bool fileAllowed(const std::string &name);

    /** checks if the path is allowed 
     *  @param path
     */
    virtual bool pathAllowed(const std::string &name);

  private:

    /** looks into the given directory and processes all files in it 
     *  @param dirname
     */
    void workDir(const std::string &dirname);

    /** processes the given file 
     *  @param dirname
     */
    void workFile(const std::string &filename);

    std::vector<std::string> filePattern;
    std::vector<std::string> forbiddenFiles;
    std::vector<std::string> files;
};

}

#endif