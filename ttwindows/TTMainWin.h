#ifndef TTMAINWIN_h
#define TTMAINWIN_h

#include "TTWin.h"

namespace ttwin{

//*****************************************************************************
//*                                TTMainWin                                  *
//*                                                                           *
//*                                                                           *
//*                      class that represents a main window                  *
//*                                                                           *
//*****************************************************************************
class TTMainWin : public TTWin{

  public:

    //---------------------- construction and initialisation -----------------

    //constructor - does nothing but initialisation
    TTMainWin(const std::string classname);

    //destructor 
    virtual ~TTMainWin();

    //create control inheritance from TTWin 
    //dont use it
    virtual bool Create(HWND parent, HINSTANCE hinst, int id, const std::string& label,
                        int x, int y, int width, int height, TTControlUnit unit);


    //create
    virtual bool Create(HWND parent, HINSTANCE hinst, int id, const std::string& label,
                        int x, int y, int width, int height, TTControlUnit unit, int nCmdShow);

    //mainloop 
    WPARAM MainLoop();

    //sets the title
    virtual void SetTitle(const std::string& title);
    


};






} // end of namespace ttwin
#endif

