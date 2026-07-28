#ifndef TTDUMMY_H
#define TTDUMMY_H

#include "TTWin.h"

namespace ttwin{


//*****************************************************************************
//*                                  TTDummy                                  *
//*                                                                           *
//*                     place holder window during devolopnment               *
//*****************************************************************************

class TTDummy: public TTWin{

  public:

    //constructor 
    TTDummy() : TTWin("TTDummy"){};
 
};

} // end of namespace ttwin

#endif


