#pragma once

#include <string>
#include <Windows.h>

namespace ttwin{

//*************************************************************************
//*                          TTContextMenu                                *
//*                                                                       *
//*   Lightweight right-click popup menu.                                 *
//*   Build with AddItem/AddSeparator, then call Show() at cursor pos.    *
//*   Show() returns the selected item id, or -1 if dismissed.            *
//*                                                                       *
//*************************************************************************
class TTContextMenu {

  public:

    TTContextMenu();
    ~TTContextMenu();

    //adds a labeled item with the given command id
    void AddItem(unsigned int id, const std::string& label);

    //adds a visual separator line
    void AddSeparator();

    //grays out a previously added item
    void DisableItem(unsigned int id);

    //shows the popup at screen coordinates and returns the chosen id, or -1
    int Show(HWND hwnd, int screenX, int screenY);

  private:

    HMENU HMenu = nullptr;

};

} // end of namespace ttwin
