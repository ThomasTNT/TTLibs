#ifndef __TTCOMBOBOX_H_
#define __TTCOMBOBOX_H_

#include "TTWin.h"

namespace ttwin{

//*****************************************************************
//*                          TTCombobox                           *
//*                                                               *
//*                                                               *
//*           it is a wrapper around an combobox control          *
//*                                                               *
//*****************************************************************
class TTCombobox : public TTWin{

  public:

    //--------------------------- static stuff -------------------------------

    //static windows event callback function
    //its adress is given by AddressOfMainCallback()
    static LRESULT CALLBACK ComboCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    //static function to get the address of MainCallback
    static WNDPROC AddressOfComboCallback();

    //---------------------- construction and initialisation -----------------

    //constructor - doas nothing but initialisation
    TTCombobox();

    //destructor 
    virtual ~TTCombobox();

    //windows message function
    virtual LRESULT Messagehandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    //event notification 
    void SendClickMessage();

    //create control
    virtual bool Create(HWND parent, HINSTANCE hinst, int id, const std::string& label, 
                        int x, int y, int width, int height, TTControlUnit unit);

    //sets the font
    void SetFont(HFONT font);
    
    //-------------------------- getter and setter ---------------------------

    //adds an entry (counting from 1)
    void SetItem(const unsigned int pos, const std::string& itemtext);

    //gets the entry at (counting from 1)
    std::string GetItem(const unsigned int pos);

    //sets selected index, counting from 1
    void SetSelectedItemPos(const unsigned int pos);

    //sets selected item
    void SetSelectedItemString(const std::string& itemtext);

    //gets selected index, counting from 1
    unsigned int GetSelectedItemPos();

    //gets selected item
    std::string GetSelectedItemString();

    //clears all entries
    void Clear();

  protected:

    //pointer to the original combobox control callback function
    WNDPROC OrgComboProcPtr;

};

} // end of namespace ttwin

#endif
