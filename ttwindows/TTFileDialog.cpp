#include "TTFileDialog.h"


//*****************************************************************************
//*                               TTFileDialog                                *
//*                                                                           *
//*                                                                           *
//*                 class encapsulating windows API file dialogs              *
//*                                                                           *
//*****************************************************************************

//************
// constructor
//************
ttwin::TTFileDialog::TTFileDialog(const HWND hwnd, const HINSTANCE hinst, const std::string& title) 
                    : Hwnd(hwnd), Hinst(hinst), Title(title) {
}

//************************************************
// FileOpenDialogSingle
//
// opens a file open dialog and returns the result
//************************************************
std::string ttwin::TTFileDialog::FileOpenSingle(const std::string& directory, 
                                                const std::string& ending,
                                                const std::vector<std::string>& filter)
{
  char buffer[MAX_PATH + 1];

  OPENFILENAME openDptions;
  ZeroMemory(&openDptions, sizeof(openDptions));
  openDptions.lStructSize = sizeof(OPENFILENAME);
  openDptions.hwndOwner = Hwnd;
  openDptions.hInstance = Hinst;

  char lpStringFilter[MAX_PATH + 1];
  openDptions.lpstrFilter = LpStringZeroByStdStringVector(filter, lpStringFilter, 256);

  openDptions.lpstrCustomFilter = NULL;
  openDptions.nMaxCustFilter = 0;
  openDptions.nFilterIndex = 0;
  openDptions.lpstrFile = buffer;
  openDptions.lpstrFile[0] = '\0';
  openDptions.nMaxFile = sizeof(buffer);
  openDptions.lpstrFileTitle = NULL;
  openDptions.nMaxFileTitle = 0;

  char lpStringDir[MAX_PATH + 1];
  openDptions.lpstrInitialDir = LpStringByStdString(directory, lpStringDir, 256);
  
  char lpString[256];
  openDptions.lpstrTitle = LpStringByStdString(Title, lpString, 256);

  openDptions.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
  openDptions.nFileOffset = 0;
  openDptions.nFileExtension = 0;
  openDptions.lpstrDefExt = NULL;
  openDptions.lCustData = NULL;
  openDptions.lpfnHook = NULL;
  openDptions.lpTemplateName = NULL;

  const BOOL success = ::GetOpenFileName(&openDptions);
  if (success == TRUE){
    return std::string(buffer);
  }
  else{
    const DWORD errorcode = CommDlgExtendedError();
    return "";
  }
}

//************************************************
// FileOpenDialogMulti
//
// opens a file open dialog and returns the result
//************************************************
std::vector<std::string> ttwin::TTFileDialog::FileOpenMultiple(const std::string& directory, 
                                                               const std::string& ending,
                                                               const std::vector<std::string>& filter)
{

  const unsigned int BUFFER_LENGTH = 1024;
  char buffer[BUFFER_LENGTH];

  OPENFILENAME openDptions;
  ZeroMemory(&openDptions, sizeof(openDptions));
  openDptions.lStructSize = sizeof(OPENFILENAME);
  openDptions.hwndOwner = Hwnd;
  openDptions.hInstance = Hinst;

  char lpStringFilter[MAX_PATH + 1];
  openDptions.lpstrFilter = LpStringZeroByStdStringVector(filter, lpStringFilter, 256);

  openDptions.lpstrCustomFilter = NULL;
  openDptions.nMaxCustFilter = 0;
  openDptions.nFilterIndex = 0;
  openDptions.lpstrFile = buffer;
  openDptions.lpstrFile[0] = '\0';
  openDptions.nMaxFile = sizeof(buffer);
  openDptions.lpstrFileTitle = NULL;
  openDptions.nMaxFileTitle = 0;

  char lpStringDir[MAX_PATH + 1];
  openDptions.lpstrInitialDir = LpStringByStdString(directory, lpStringDir, 256);
  
  char lpString[256];
  openDptions.lpstrTitle = LpStringByStdString(Title, lpString, 256);

  openDptions.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
  openDptions.nFileOffset = 0;
  openDptions.nFileExtension = 0;
  if (ending.empty()){
    openDptions.lpstrDefExt = NULL;
  }
  else{
    openDptions.lpstrDefExt = ending.c_str();
  }
  openDptions.lCustData = NULL;
  openDptions.lpfnHook = NULL;
  openDptions.lpTemplateName = NULL;

  const BOOL success = ::GetOpenFileName(&openDptions);
  if (success == TRUE){
    const std::vector<std::string> strs = SplitCharZeroString(buffer, sizeof(buffer));

    std::string filename(buffer);
    std::vector<std::string> result;
    return result;
  }
  else{
    const DWORD errorcode = CommDlgExtendedError();
    return std::vector<std::string>();
  }

}

//************************************************
// FileSaveSingle
//
// opens a file open dialog and returns the result
//************************************************
std::string ttwin::TTFileDialog::FileSaveSingle(const std::string& directory,
                                                const std::string& ending,
                                                const std::vector<std::string>& filter)
{
 
  OPENFILENAME saveOptions;
  char buffer[MAX_PATH] = "";

  ZeroMemory(&saveOptions, sizeof(saveOptions));

  saveOptions.lStructSize = sizeof(saveOptions);

  saveOptions.hwndOwner = Hwnd;
  saveOptions.hInstance = Hinst;
  char lpStringFilter[MAX_PATH + 1];
  saveOptions.lpstrFilter = LpStringZeroByStdStringVector(filter, lpStringFilter, 256);

  saveOptions.lpstrFile = buffer;
  saveOptions.nMaxFile = MAX_PATH;
  saveOptions.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

  if (ending.empty()){
    saveOptions.lpstrDefExt = NULL;
  }
  else{
    saveOptions.lpstrDefExt = ending.c_str();
  }

  const BOOL success = ::GetSaveFileName(&saveOptions);

  if (success == TRUE){
    return std::string(buffer);
  }
  else{
    const DWORD errorcode = CommDlgExtendedError();
    return "";
  }


}