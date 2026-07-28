#ifndef __TTMENU_h_
#define __TTMENU_h_

#include <string>
#include <vector>

#include <Windows.h>

#include "TTWin.h"

namespace ttwin{

//*****************************************************************************
//*                                  TTMenu                                   *
//*                                                                           *
//*                                                                           *
//*                    class for menu creation and handling                   *
//*                                                                           *
//*****************************************************************************


enum TTMenuType { TTMENU_MAINMENU, 
                  TTMENU_SUBMENU, 
};

//-------------------------------- TTMenuItem ---------------------------------
class TTMenu;

class TTMenuItem {
  
  friend class TTMenu;

  protected: 
      
    //defualt constructor
    TTMenuItem();

    //constructor
    TTMenuItem(const unsigned int id);

  public:

    //destructor 
    virtual ~TTMenuItem();

    //getter
    unsigned int GetId() const;

    //getter
    std::string GetLabel() const;

    //getter for lpString
    char* GetLpString();

    //gets the lpString array prefilled with label
    char* GetLabelAsLpString();

    //enable or disables this item
    bool Enable(const bool enable);

    //returns the windows handle of the menu
    virtual HMENU GetMenuHandle();

  protected:

    //own id
    unsigned int Id;

    //constant for lpString
    static const unsigned int MAX_TEXTVALUE_LENGTH = 256;

    //buffer for textvalues for windows
    char lpString[MAX_TEXTVALUE_LENGTH];

    //the label (not used for main menus)
    std::string Label;

    //the parent menu 
    TTMenu* ParentMenu;


};

//------------------------------- TTMenuItemString -------------------------------
class TTMenuItemString : public TTMenuItem {
  
  public: 
      
    //constructor
    TTMenuItemString(const unsigned int id, const std::string& label);

    //destructor 
    virtual ~TTMenuItemString();

};

//------------------------------- TTMenuItemSeparator -------------------------------
class TTMenuItemSeparator : public TTMenuItem {
  
  public: 
      
    //constructor
    TTMenuItemSeparator();

    //destructor 
    virtual ~TTMenuItemSeparator();
};

//------------------------------------- TTMenu ------------------------------------
class TTMenu : public TTMenuItem{
  
  public:

    //constructor 1
    //you have to call Create() later manually
    TTMenu();

    //constructor 2
    //you have to call Create() later manually or add it to an parent menu
    //by calling parent.AddMenuItem()
    TTMenu(const unsigned int id, const std::string& label);

    //destructor 
    virtual ~TTMenu();

    //create
    //call this only for main menus
    //for submenus this is called in AddMenuItem() of its parent
    bool Create(TTMenu* parent, const unsigned int id, const std::string& label);

    //adds a menu item
    bool AddMenuItem(TTMenuItem * item);

    //attaches the menu to the window
    //calls setMenu so call this after creation of the menu
    bool AttachToWindow(HWND Hwnd);

    //attaches the menu to the window
    //calls setMenu so call this after creation of the menu
    bool AttachToWindow(ttwin::TTWin& win);

    //gets an item by its name
    TTMenuItem* GetItemByName(const std::string& name);

    //gets an item by its id
    TTMenuItem* GetItemById(const unsigned int id);

    //returns the windows handle of the menu
    virtual HMENU GetMenuHandle();

  protected:

    //list of child menus for deleting
    std::vector<TTMenuItem*> ChildMenuItems;

    //the window menu handle
    HMENU HMenu;

};


} // end of namespace ttwin

#endif