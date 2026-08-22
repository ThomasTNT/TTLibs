#pragma once

#include "TTSlider.h"

namespace ttwin {

//*************************************************************
//*                     TTDoubleSlider                        *
//*                                                           *
//*   Horizontal slider with two handles.                     *
//*   The highlighted zone always spans between the two       *
//*   handle positions.                                       *
//*                                                           *
//*************************************************************
class TTDoubleSlider : public AbstractDoubleSlider {

  public:

    TTDoubleSlider();

  protected:

    virtual void PaintAll() override;
};

} // end of namespace ttwin
