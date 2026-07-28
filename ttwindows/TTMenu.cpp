#include "TTMenu.h"


//*****************************************************************************
//*                                  TTMenu                                   *
//*                                                                           *
//*                    class for menu creation and handling                   *
//*                                                                           *
//*****************************************************************************

//-------------------------------- TTMenuItem ---------------------------------

//****************************
// private defualt constructor
//****************************
ttwin::TTMenuItem::TTMenuItem() : Id(0), ParentMenu(NULL) {}

//********************
// private constructor
//********************
ttwin::TTMenuItem::TTMenuItem(const unsigned int id) : Id(id), Label(""), ParentMenu(NULL) {}

//***********
// destructor
//***********
ttwin::TTMenuItem::~TTMenuItem(){}

//**************
// getter for id
//**************
unsigned int ttwin::TTMenuItem::GetId() const{
  return Id;
}

//*****************
// getter for Label
//*****************
std::string ttwin::TTMenuItem::GetLabel() const{
  return Label;
}

//********************
// getter for lpString
//********************
char* ttwin::TTMenuItem::GetLpString(){
  return lpString;
}

//*********************************************
// GetLabelAsLpString
//
// gets the lpString array prefilled with label
//*********************************************
char* ttwin::TTMenuItem::GetLabelAsLpString(){
  return LpStringByStdString(Label, lpString, MAX_TEXTVALUE_LENGTH);
}

//*****************************
// Enable
//
// enable or disables this item
//*****************************
bool ttwin::TTMenuItem::Enable(const bool enable){
  return EnableMenuItem(GetMenuHandle(), Id, enable ? MF_ENABLED : MF_GRAYED) == MF_ENABLED;
}
 
//***************************************
// GetMenuHandle
//
// returns the windows handle of the menu
//***************************************
HMENU ttwin::TTMenuItem::GetMenuHandle(){
  return ParentMenu ? ParentMenu->GetMenuHandle() : 0;
}

//------------------------------ TTMenuItemString ------------------------------

//************
// constructor
//************
ttwin::TTMenuItemString::TTMenuItemString(const unsigned int id, const std::string& label) : TTMenuItem(id){
  Label = label;
}

//***********
// destructor
//***********
ttwin::TTMenuItemString::~TTMenuItemString(){}

//------------------------------- TTMenuItemSeparator -------------------------------

//************
// constructor
//************
ttwin::TTMenuItemSeparator::TTMenuItemSeparator() : TTMenuItem(0){}

//***********
// destructor
//***********
ttwin::TTMenuItemSeparator::~TTMenuItemSeparator(){}


//----------------------------------- TTMenu ----------------------------------

//*****************************************
// constructor 1
//
// you have to call Create() later manually
//*****************************************
ttwin::TTMenu::TTMenu() : HMenu(0){}

//*********************************************************************
// constructor 2
// you have to call Create() later manually or add it to an parent menu
// by calling parent.AddMenuItem()
//*********************************************************************
ttwin::TTMenu::TTMenu(const unsigned int id, const std::string& label) : HMenu(0){
  Label = label;
  Id = id;
}

//***********
// destructor
//***********
ttwin::TTMenu::~TTMenu(){
  for (unsigned int i = 0; i < ChildMenuItems.size(); ++i){
    delete ChildMenuItems[i];
  }
}

//***********************************************************
// create
//
// call this only for main menus
// for submenus this is called in AddMenuItem() of its parent
//***********************************************************
bool ttwin::TTMenu::Create(TTMenu* parent, const unsigned int id, const std::string& label){

  Label = label;
  Id = id;
  ParentMenu = parent;

  //main menu
  if (!ParentMenu){
    HMenu = ::CreateMenu();
    return HMenu != 0;
  }
  //sub menu 
  else{
    HMenu = ::CreatePopupMenu();

    ::MENUITEMINFO mii;
	  memset(&mii, 0, sizeof(MENUITEMINFO));
	  mii.cbSize	= sizeof(MENUITEMINFO);
	  mii.fMask	= MIIM_STRING | MIIM_FTYPE | MIIM_ID | MIIM_SUBMENU;
	  mii.fType	= MFT_STRING;
    mii.dwTypeData = TEXT(const_cast<char*>(Label.c_str()));
	  mii.cch		= Label.length();
	  mii.wID		= id;
	  mii.hSubMenu	= HMenu;

	  const BOOL success = ::InsertMenuItem(ParentMenu->HMenu, ParentMenu->ChildMenuItems.size(), TRUE, &mii);
    return success != FALSE;
  }
    
  return true;
}


//*****************
// AddMenuItem
//
// adds a menu item
//*****************
bool ttwin::TTMenu::AddMenuItem(TTMenuItem * item){
  ChildMenuItems.push_back(item);
  item->ParentMenu = this;

  TTMenu * const                    subMenu    = dynamic_cast<TTMenu *>(item); 
  const TTMenuItemString * const    itemString = dynamic_cast<TTMenuItemString *>(item); 
  const TTMenuItemSeparator * const itemSep    = dynamic_cast<TTMenuItemSeparator *>(item); 

  //submenu
  if (subMenu){
    return subMenu->Create(this, subMenu->Id, subMenu->Label);
  }
  //string item
  else if (itemString){
    ::MENUITEMINFO menuiteminfo;
	  memset(&menuiteminfo, 0, sizeof(MENUITEMINFO));
	  menuiteminfo.cbSize	= sizeof(MENUITEMINFO);
	  menuiteminfo.fMask	= MIIM_STRING | MIIM_FTYPE | MIIM_ID | MIIM_SUBMENU;
	  menuiteminfo.fType	= MFT_STRING;
    char * const lpString = item->GetLabelAsLpString();
    menuiteminfo.dwTypeData = TEXT(lpString);
	  menuiteminfo.cch		= itemString->GetLabel().length();
    menuiteminfo.wID		= item->GetId();
	  menuiteminfo.hSubMenu = NULL;
    menuiteminfo.dwItemData = item->GetId();
    const BOOL success = ::InsertMenuItem(HMenu, ChildMenuItems.size() - 1, TRUE, &menuiteminfo);
    return success != FALSE;
  }
  //separator
  else if (itemSep){
    ::MENUITEMINFO menuiteminfo;
	  memset(&menuiteminfo, 0, sizeof(MENUITEMINFO));
	  menuiteminfo.cbSize	= sizeof(MENUITEMINFO);
	  menuiteminfo.fMask	= MIIM_STRING | MIIM_FTYPE | MIIM_ID | MIIM_SUBMENU;
	  menuiteminfo.fType	= MFT_SEPARATOR;
    char * const lpString = item->GetLabelAsLpString();
    menuiteminfo.dwTypeData = NULL;
	  menuiteminfo.cch		= 0;
    menuiteminfo.wID		= 0;
	  menuiteminfo.hSubMenu = NULL;
    menuiteminfo.dwItemData = item->GetId();
	  const BOOL success = ::InsertMenuItem(HMenu, ChildMenuItems.size() - 1, TRUE, &menuiteminfo);
    return success != FALSE;
  }

  return false;
}


//******************************************************
// AttachToWindow
//
// attaches the menu to the window
// calls setMenu so call this after creation of the menu
//******************************************************
bool ttwin::TTMenu::AttachToWindow(HWND Hwnd){
  return ::SetMenu(Hwnd, HMenu) != 0;
}

//******************************************************
// AttachToWindow
//
// attaches the menu to the window
// calls setMenu so call this after creation of the menu
//******************************************************
bool ttwin::TTMenu::AttachToWindow(ttwin::TTWin& win){
  return ::SetMenu(win.Handle(), HMenu) != 0;
}

//*************************
// GetItemByName
//
// gets an item by its name
//*************************
ttwin::TTMenuItem* ttwin::TTMenu::GetItemByName(const std::string& name){
  for (unsigned int i = 0; i < ChildMenuItems.size(); ++i){
    TTMenuItem* child = ChildMenuItems[i];
    TTMenu* menu = dynamic_cast<TTMenu*>(child);
    if (menu){
      TTMenuItem* found = menu->GetItemByName(name);
      if (found) return found;
    }
    else{
      if (child->GetLabel() == name){
        return child;
      } 
    }
  }
  return NULL;
}

//***********************
// GetItemByName
//
// gets an item by its id
//***********************
ttwin::TTMenuItem* ttwin::TTMenu::GetItemById(const unsigned int id){
  for (unsigned int i = 0; i < ChildMenuItems.size(); ++i){
    TTMenuItem* child = ChildMenuItems[i];
    TTMenu* menu = dynamic_cast<TTMenu*>(child);
    if (menu){
      TTMenuItem* found = menu->GetItemById(id);
      if (found) return found;
    }
    else{
      if (child && child->GetId() == id){
        return child;
      } 
    }
  }
  return nullptr;
}

//***************************************
// GetMenuHandle
//
// returns the windows handle of the menu
//***************************************
HMENU ttwin::TTMenu::GetMenuHandle(){
  return HMenu;
}


