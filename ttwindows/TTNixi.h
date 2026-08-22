#pragma once

#include "TTWin.h"

namespace ttwin {

const int NIXI_BORDER    = 3;
const int NIXI_NATIVE_W  = 18;
const int NIXI_NATIVE_H  = 30;
const int NIXI_DEFAULT_H = NIXI_NATIVE_H + 2 * NIXI_BORDER;  // 36

const COLORREF NIXI_SHADOW_OUTER    = RGB( 21,  21,  21);
const COLORREF NIXI_SHADOW_MID      = RGB( 75,  75,  75);
const COLORREF NIXI_SHADOW_INNER    = RGB( 21,  21,  21);
const COLORREF NIXI_HIGHLIGHT_INNER = RGB( 75,  75,  75);
const COLORREF NIXI_HIGHLIGHT_MID   = RGB(131, 131, 131);
const COLORREF NIXI_HIGHLIGHT_OUTER = RGB(184, 184, 184);

//*****************************************************************************
//*                               TTNixi                                      *
//*                                                                           *
//*   Multi-digit Nixie tube display. Renders embedded BMP images on a       *
//*   black background with a sunken 3D border.                              *
//*   Images are self-contained — no external resources needed.              *
//*                                                                           *
//*****************************************************************************
class TTNixi : public TTWin {

  public:

    TTNixi();
    virtual ~TTNixi();

    // digit count, leading zeros, pixel spacing between digits (0 = adjacent,
    // positive = gap, negative = overlap), and decimal places (0 = integer);
    // call before Create()
    void Configure(const unsigned int digits, const bool leadingZeros = false, const int spacing = 0, const unsigned int decimalPlaces = 0);

    // total window width for a given window height, based on current configuration
    int GetCurrentTotalWidth(const int windowHeight) const;

    // update displayed value; rounding to DecimalPlaces is applied internally
    void SetValue(const double value);

    // show nixi_off for all digits
    void ClearValue();

  protected:

    virtual void PaintAll() override;

  private:

    void LoadImages();
    void DrawBorder(HDC hdc, const RECT& rect);
    void DrawDigit(HDC hdc, int x, int y, int w, int h, int imgIndex);

    unsigned int DigitCount    = 1;
    bool         LeadingZeros  = false;
    int          Spacing       = 0;
    unsigned int DecimalPlaces = 0;
    bool         HasValue      = false;
    double       Value         = 0.0;
    bool         ImagesLoaded  = false;
    HBITMAP      Images[11];
};

} // namespace ttwin
