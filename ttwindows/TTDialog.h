#ifndef TTDIALOG_H
#define TTDIALOG_H

#include "TTWin.h"
#include "TTWinUtils.h"

namespace ttwin{

//*****************************************************************
//*                           TTDialog                            *
//*                                                               *
//*                                                               *
//*                     base class for dialogs                    *
//*                                                               *
//*****************************************************************

class TTDialog : public TTWin{

  public:

    //---------------------- construction and initialisation -----------------

    //constructor - doas nothing but initialisation
    TTDialog(const std::string classname, const int width, const int height);

    //destructor 
    virtual ~TTDialog();

    //-------------------------- public windows stuff ------------------------

    //gets the prefered window rect
    //normally calculated before calling create
    //returns default sizes
    virtual RECT GetPreferedWindowRect();

    //create control
    virtual bool Create(HWND parent, HINSTANCE hinst, int id, int x, int y, const std::string& label, bool modal);

   
    //called by child events - return 0 when direct return
    virtual int CheckControlEvent(UINT msg, WPARAM wParam, LPARAM lParam){ return 0; }

    //called when a child send a BN_CLICKED message
    virtual void ChildCommandBnClicked(const int item){};

    //called when a child send a WM_HSCROLL message
    virtual void ChildScroll(const HWND handle){};

    //called when a child send a EN_CHANGE message
    virtual void ChildEnChanged(const int item){};

    //called when the selection in a child listbox changes
    virtual void ChildListboxSelectionChanged(const int item){};

    //called when a child listbox item is double-clicked
    virtual void ChildListboxDoubleClicked(const int item){};


    //-------------------------- getter and setter ---------------------------

    //------------------------------- user methods ----------------------------

  protected:

    //------------------------------ windows stuff ---------------------------

    bool Register(const HINSTANCE hinst, const LPCSTR classname);
    virtual void Destroy();
    
  public:
  
    //windows message function
    virtual LRESULT Messagehandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
  
  protected:

    //intialisation and update
    virtual void Created(){};
    virtual void Init();
    virtual void PaintAll();
    
    //--------------------------------- fields -------------------------------

    //fixed sizes
    int Width;
    int Height;

    //modal or non modal dialog
    bool Modal;

};

} // end of namespace ttwin

#endif