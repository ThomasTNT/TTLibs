#pragma once

#include "TTWin.h"

namespace ttwin {

//******************************************************************************
//*                              TTScrollArea                                  *
//*                                                                            *
//*          scrollable container with TTFrame-style title bar;                *
//*          real Win32 window (WS_VSCROLL) — can host child TTWins            *
//*                                                                            *
//******************************************************************************
class TTScrollArea : public TTWin {

  public:

    TTScrollArea();
    virtual ~TTScrollArea();

    void SetTitle(const std::string& title);
    void SetVirtualHeight(const int height);

    int GetScrollY() const;
    int GetInnerTop() const;
    int GetInnerLeft() const;
    int GetInnerWidth() const;

  protected:

    virtual DWORD GetExtraWindowStyle() const override;
    virtual void  PaintAll() override;
    virtual void  ResizesFields() override;

    virtual void CM_VScroll(const int scrollCode, const int trackPos) override;
    virtual void CM_MouseWheel(const int delta) override;

  private:

    void UpdateScrollInfo();
    void NotifyParentScrolled();

    int VirtualHeight = 0;
    int ScrollY       = 0;
};

} // end of namespace ttwin
