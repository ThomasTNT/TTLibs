#ifndef __TTEDIT_h_
#define __TTEDIT_h_

#include "TTWin.h"

namespace ttwin{

//*****************************************************************
//*                            TTEdit                             *
//*                                                               *
//*                                                               *
//*               it is a wrapper around an edit control          *
//*                                                               *
//*****************************************************************
class TTEdit : public TTWin{

  public:

    //--------------------------- static stuff -------------------------------

    //static windows event callback function
    //its adress is given by AddressOfMainCallback()
    static LRESULT CALLBACK EditCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    //static function to get the address of MainCallback
    static WNDPROC AddressOfEditCallback();

    //---------------------- construction and initialisation -----------------

    //constructor - does nothing but initialisation
    TTEdit();

    //destructor 
    virtual ~TTEdit();

    //windows message function
    virtual LRESULT Messagehandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    //create control
    virtual bool Create(HWND parent, HINSTANCE hinst, int id, const std::string& label, 
                        int x, int y, int width, int height, TTControlUnit unit);

    //sets the font
    void SetFont(HFONT font);
    
    //configures the edit for numeric values
    void ConfigureNumeric(double editRangeMin, double editRangeMax, unsigned int editdigits);

    //-------------------------- getter and setter ---------------------------

    //sets the label text
    virtual void SetText(const std::string& label);

    //gets the label text
    virtual std::string GetText() const;

    //sets the label text
    virtual void SetNumericValue(const double v);

    //gets the label text
    virtual double GetNumericValue() const;

  protected:

    //pointer to the original edit control callback function
    WNDPROC OrgEditProcPtr;

    bool IsNumeric = false;
    double EditRangeMin = 0.0;
    double EditRangeMax = 1.0;
    unsigned int Editdigits = 1;

};

} // end of namespace ttwin

#endif
