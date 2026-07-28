#include "TTContextMenu.h"

//************
// constructor
//************
ttwin::TTContextMenu::TTContextMenu(){
  HMenu = ::CreatePopupMenu();
}

//***********
// destructor
//***********
ttwin::TTContextMenu::~TTContextMenu(){
  if (HMenu){
    ::DestroyMenu(HMenu);
    HMenu = nullptr;
  }
}

//*********
// AddItem
//*********
void ttwin::TTContextMenu::AddItem(unsigned int id, const std::string& label){
  if (!HMenu) return;
  ::AppendMenuA(HMenu, MF_STRING, static_cast<UINT_PTR>(id), label.c_str());
}

//**************
// AddSeparator
//**************
void ttwin::TTContextMenu::AddSeparator(){
  if (!HMenu) return;
  ::AppendMenuA(HMenu, MF_SEPARATOR, 0, nullptr);
}

//***************
// DisableItem
//***************
void ttwin::TTContextMenu::DisableItem(unsigned int id){
  if (!HMenu) return;
  ::EnableMenuItem(HMenu, id, MF_BYCOMMAND | MF_GRAYED);
}

//******
// Show
//******
int ttwin::TTContextMenu::Show(HWND hwnd, int screenX, int screenY){
  if (!HMenu) return -1;
  const int selected = static_cast<int>(
    ::TrackPopupMenu(HMenu,
                     TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD | TPM_NONOTIFY,
                     screenX, screenY,
                     0, hwnd, nullptr));
  return (selected == 0) ? -1 : selected;
}
