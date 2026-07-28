#pragma once

#include "TTWin.h"

namespace ttwin{

//*****************************************************************
//*                          TTListbox                            *
//*                                                               *
//*           wrapper around a Windows LISTBOX control            *
//*                                                               *
//*****************************************************************
class TTListbox : public TTWin{

  public:

    //--------------------------- static stuff -------------------------------

    static LRESULT CALLBACK ListboxCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static WNDPROC AddressOfListboxCallback();

    //---------------------- construction and initialisation -----------------

    TTListbox();
    virtual ~TTListbox();

    virtual LRESULT Messagehandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    virtual bool Create(HWND parent, HINSTANCE hinst, int id, const std::string& label,
                        int x, int y, int width, int height, TTControlUnit unit);

    void SetFont(HFONT font);

    //-------------------------- item management -----------------------------

    // appends an item at the end; color 0 = default white
    void AddItem(const std::string& text, COLORREF color = 0);

    // inserts an item at pos (0-based); color 0 = default white
    void InsertItem(int pos, const std::string& text, COLORREF color = 0);

    // changes the text color of an existing item (0 = default white)
    void SetItemColor(int pos, COLORREF color);

    // removes the item at pos (0-based)
    void RemoveItem(int pos);

    // removes all items
    void Clear();

    //-------------------------- selection -----------------------------------

    // returns item count
    int GetCount() const;

    // returns selected index (0-based), -1 if nothing selected
    int GetSelectedPos() const;

    // returns selected item text, "" if nothing selected
    std::string GetSelectedItemString() const;

    // returns text of item at pos (0-based)
    std::string GetItem(int pos) const;

    // selects item at pos (0-based); -1 deselects
    void SetSelectedPos(int pos);

  protected:

    WNDPROC OrgListboxProcPtr = nullptr;

  private:

    char lpString[512] = {};

};

} // end of namespace ttwin
