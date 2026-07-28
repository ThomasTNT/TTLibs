#ifndef __TTFILEDIALOG_H_
#define __TTFILEDIALOG_H_

#include <string>
#include <vector>

#include <windows.h>

#include "TTWinUtils.h"
#include "stringutil.h"

namespace ttwin{

//*****************************************************************************
//*                               TTFileDialog                                *
//*                                                                           *
//*                                                                           *
//*                 class encapsulating windows API file dialogs              *
//*                                                                           *
//*****************************************************************************

class TTFileDialog{

  public:

    //constructor
    TTFileDialog(const HWND hwnd, const HINSTANCE hinst, const std::string& title);

    //opens a file open dialog and returns the result
    std::string FileOpenSingle(const std::string& directory, const std::string& ending, const std::vector<std::string>& filter);

    //opens a file open dialog and returns the result
    std::vector<std::string> FileOpenMultiple(const std::string& directory, const std::string& ending, const std::vector<std::string>& filter);

    //opens a file save dialog and returns the result
    std::string FileSaveSingle(const std::string& directory, const std::string& ending, const std::vector<std::string>& filter);

  private:

    //dialog title
    std::string Title;

    //handle of the owner window
    HWND Hwnd; 

    //instance
    HINSTANCE Hinst;
};



} // end of namespace ttwin

#endif