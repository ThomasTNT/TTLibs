#ifndef __TTWIN_h_
#define __TTWIN_h_

#include <map>
#include <string>

#include <windows.h>
#include <Windowsx.h>
#include <commctrl.h>

#include "TTTooltip.h"
#include "TTBackgroundImage.h"
namespace ttwin{

//unit for dialog placement
enum TTControlUnit { PIXEL, DIALOGUNIT, PERCENT };

//own window messages 
const UINT TTM_CHILD_CLICKED        = WM_APP + 1;
const UINT TTM_CHILD_CLICKEDCHANGED = TTM_CHILD_CLICKED + 1;
const UINT TTWIN_MESSAGE_END        = TTM_CHILD_CLICKEDCHANGED;

const unsigned int MAX_TEXTVALUE_LENGTH = 256;

//*****************************************************************************
//*                                     TTWin                                 *
//*                                                                           *
//*                                                                           *
//*                            base class for own windows                     *
//*                                                                           *
//*****************************************************************************

const COLORREF BACKGROUND_COLOR_DEFAULT = RGB(255, 110, 110);

const int TTWIN_TIMER_ID1 = 1;

// color scheme for edit controls and combo boxes (white text on black)
const COLORREF TTWIN_EDIT_TEXT_COLOR = RGB(255, 255, 255);
// background: use GetStockObject(BLACK_BRUSH)

class TTWin{

  public:

    //--------------------------- static stuff -------------------------------

    //map to store the handels and the pointes to the real control objects
    static std::map<HWND, TTWin*> TTWinCallBackMap;

    //static windows event callback function
    //its adress is given by AddressOfMainCallback()
    static LRESULT CALLBACK MainCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    //static function to get the address of MainCallback
    static WNDPROC AddressOfMainCallback();

    //returns the TTWin registered for the given HWND, or nullptr if not found
    static TTWin* LookupByHandle(HWND hwnd);

    // draws one item for an owner-drawn combo box (black bg, white text);
    // call from WM_DRAWITEM handler; returns TRUE when handled
    static LRESULT OnDrawComboItem(LPARAM lParam);

    // draws one item for an owner-drawn listbox; item color read from LB_GETITEMDATA (0 = default white);
    // call from WM_DRAWITEM handler; returns TRUE when handled
    static LRESULT OnDrawListboxItem(LPARAM lParam);

    // sets a reasonable item height for owner-drawn combo boxes and listboxes;
    // call from WM_MEASUREITEM handler
    static void OnMeasureComboItem(LPARAM lParam);

    //---------------------- construction and initialisation -----------------

    //constructor - does nothing but initialisation
    TTWin(const std::string& classname);

    //constructor - does nothing but initialisation
    //TTWin(const std::wstring& classname);

    //destructor 
    virtual ~TTWin();

    //NOTE:
    //to configure your window you should write a method configure()
    //Configure should get all parameter to configure your window behaviour.
    //E.g. type, exponential vs. linear etc.
    //But not Font, Color. And even no model object. 


    //-------------------------- public windows stuff ------------------------

    //create control
    virtual bool Create(HWND parent, HINSTANCE hinst, int id, const std::string& label, 
                        int x, int y, int width, int height, TTControlUnit unit);

    //create control
    bool CreateAsChild(TTWin& control, HINSTANCE hinst, int id, const std::string& label, 
                       int x, int y, int width, int height, TTControlUnit unit);

    //creates a tool tip
    virtual void CreateToolTip(const std::string& tooltip);

    //sets the tool tip text
    void SetToolTip(const std::string& tooltip);

    //resizes the control
    void Resize(int x, int y, int width, int height, TTControlUnit unit);
      
    //invalidates the whole client area
    void Invalidate();

    //invalidates the given client area
    void Invalidate(int x1, int y1, int x2, int y2);

    //invalidates if necessary
    //call this when something has changed
    virtual void CheckForInvalidate();

    //updates the window immediately
    void Update();

    //adds one or more style bits to the window (e.g. WS_CLIPCHILDREN)
    void AddWindowStyle(LONG style);

    //shows or hides the control
    void Show(const bool show);

    //Enables or disbales the control
    virtual void Enable(const bool enable);

    //sets keyboard focus to this control
    void Focus();

    //bring to top
    void ToTop();

     //returns visibility
    bool IsVisible();

    //returns true if window is enabled
    bool IsEnabled();

    //sets the background image
    virtual void SetBackImage(TTBackgroundImage * const backgroundImage);

    //if upperRight is true, the back image will be arranged to (0,0) 
    //not relative to the parent
    void SetBackImageUpperLeft(bool upperLeft);

    //sets the background color
    virtual void SetBackColor(const COLORREF color);

    //can be called directly or via sending TTM_CHILD_CLICKED message
    virtual void ChildWasClicked(TTWin* child, const int value1, const int value2);

    //can be called directly or via sending TTM_CHILD_CLICKEDCHANGED message
    virtual void ChildWasClickedAndChanged(TTWin* child, const int value1, const int value2);

    //sends the-child-was-clicked-message
    void SendChildWasClickedMsg(const int value);

    // sends the child was clicked message
    // the difference to SendChildWasClickedMsg is that the child calles for changing
    // e.g. after mouse input. Where as SendChildWasClickedMsg is only a click.
    void SendChildWasClickedAndChangedMsg(const int value);

    //called by child events - return 0 when direct return
    virtual int CheckControlEvent(UINT msg, WPARAM wParam, LPARAM lParam){ return 0; }

    //-------------------------- getter and setter ---------------------------

    //adds this window as parent, for direct messaging
    void SetParentTTWin(TTWin* parent);

    //return the parent window
    const TTWin* GetParentTTWin() const;

    //return the most upper parent window
    const TTWin* GetGrandParentTTWin() const;

    //returns the handle of this window
    HWND Handle() const;

    //gets the client rect
    RECT GetClientRect() const;

    //gets the window rect
    RECT GetWindowRect() const;

    //gets offset x
    int OffsetToOtherWinX(const TTWin* const win) const;

    //gets offset y
    int OffsetToOtherWinY(const TTWin* const win) const;

    //gets the prefered window rect
    //normally calculated before calling create
    //default is (0, 0, 0, 0);
    virtual RECT GetPreferedWindowRect() const;

    //sets the label text
    virtual void SetText(const std::string& label);

    //gets the label text
    virtual std::string GetText();

    //gets the id
    virtual unsigned int GetID();

    //sets the select status
    virtual void SetSelected(const bool selected);

    //gets the select status
    virtual bool GetSelected();

    //------------------------------- user methods ----------------------------

    //refreshes the model or the content of the window
    //shall call Invalidate() or Update() then
    //Used to refresh level meters etc.
    virtual void UpdateContent();

    //sets font height and weight used in PaintAll
    void SetFontDimensions(const int height, const int weight);

    //starts the timer 1
    void StartTimer1(const unsigned int milliseconds);

    //stops the time 1
    void StopTimer1();

  protected:

    //------------------------------ windows stuff ---------------------------

    bool Register(const HINSTANCE hinst, const LPCSTR classname);
    bool Unregister(const HINSTANCE hinst, const LPCSTR classname);
    virtual void Destroy();
    
  public:
  
    //windows message function
    virtual LRESULT Messagehandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
  
  protected:

    //intialisation and update
    virtual void Created(){};
    virtual void Init(){};
    virtual BOOL EditColor(WPARAM wparam);
    virtual void InitGraphic(){};
    virtual void PaintAll();
    virtual void ResizesFields(){};
    virtual DWORD GetExtraWindowStyle() const { return 0; }

    //sets the background image
    virtual void SetBackImageToChildren(){};

    // can be called in PaintAll
    virtual void DrawBackground(HDC hdc, const bool drawDefaultBackground);
    virtual void CreateBackDrawDC(HDC hdc, const RECT winRect);

    //event functions
    virtual void CM_Mouse_LeftDown(int x, int y, bool shift, bool ctrl, bool alt){};
    virtual void CM_Mouse_RightDown(int x, int y, bool shift, bool ctrl, bool alt){};
    virtual void CM_Mouse_LeftDouble(int x, int y, bool shift, bool ctrl, bool alt){};
    virtual void CM_Mouse_LeftUp(int x, int y, bool shift, bool ctrl, bool alt){};
    virtual void CM_Mouse_RightUp(int x, int y, bool shift, bool ctrl, bool alt){};
    virtual void CM_Mouse_Move(int x, int y, bool shift, bool ctrl, bool alt){};
    virtual void CM_Mouse_Leave(int x, int y, bool shift, bool ctrl, bool alt){};

    virtual void CM_VScroll(const int scrollCode, const int trackPos){};
    virtual void CM_MouseWheel(const int delta){};

    //timer
    void TimerEvent1();

    //overwrite this to handle a periodical timer event
    //return false for stop
    virtual bool TimerHandler1() { return true; };

    //help functions
    void PixelsFromBaseUnits(int &x, int &y, int &width, int &height);
    POINT CalcOffsetToMainWin() const;

    std::string GetLastErrorString();
    static std::string WinMessageToString(const UINT msg);

    //--------------------------------- fields -------------------------------

    //selected
    bool Selected = false;

    //Classname 
    //IMPORTANT: THIS IS USED DURING REGISTER. register() is called during create()
    std::string Classname; 

    //handle for this window
    HWND Hwnd = 0; 

    //instance
    HINSTANCE Hinst = 0;

    //ID for this window
    int ID = 0; 

    //handle of the parent window
    HWND Parent = 0; 

    //a TTWin parent (not necessary the same like Parent
    TTWin* TTWinParent = nullptr;

    //tool tip
    TTTooltip* Tooltip = nullptr;

    //background image
    TTBackgroundImage* BackgroundImage = nullptr;

    //fix placement of background image
    bool BackgroundImageUpperLeft = false;

    //backgroud color 
    COLORREF BackColor = BACKGROUND_COLOR_DEFAULT;

    //handle for memory device context for background drawing
    HDC BackDrawDC = 0;   
    HBITMAP BackDrawBitmap = 0;
    HGDIOBJ BackDrawOld = 0; //do we need this really? Because DC will destroyed anyway. 

    bool DrawOutline = true;

    //stored to recontruct them to prevent that own objects are selected
    //useful for drawing
    HGDIOBJ OrgPen = 0;
    HGDIOBJ OrgBrush = 0;
    HGDIOBJ OrgFont = 0;

    int FontHeight = 12;
    int FontWeight = FW_NORMAL;

    //the label or title
    //not all windows will show this label
    mutable std::string Label;

    //buffer for textvalues from windows
    mutable char lpString[MAX_TEXTVALUE_LENGTH];

    //true when an event function has to be skiped because it is called
    //from another event function
    bool SkipControlFunction = false;

    //timeot for timer
    unsigned int TimerTime1 = 0;


   
};

} // end of namespace ttwin

#endif


