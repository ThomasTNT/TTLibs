#ifndef TTCALLBACK_H
#define TTCALLBACK_H

#include <map>

namespace ttwin{

//*****************************************************************
//*                          TTCallback                           *
//*                                                               *
//*                                                               *
//*                abstract class for callbacks                   *
//*                                                               *
//*   this has to be implemented by classes that are called from  *
//*   TTCallbackProcess instances.                                *
//*****************************************************************
class TTCallback{


};



//*****************************************************************
//*                        TTCallbackProcess                      *
//*                                                               *
//*                                                               *
//*  this abstract class represensts a running process            * 
//*  this can be a simple process, a master process with sub      *
//*  processes or anything else.                                  *
//*                                                               *
//*  a TTCallbackProcess informs the callback about the status    *
//*                                                               *
//*  Any thread must have it's own TTCallbackProcess!             *
//*  A call to the TTCallbackProcess is done there where the      *
//*  Information is. Means in the class in the method.            *
//*  When a method is called by a different thread then it has to *
//*  call the correct TTCallbackProcess.                          *
//*****************************************************************
enum TTCallbackProcessState { TTCBPS_UNKNOWN, TTCBPS_STARTING, TTCBPS_WAITING, TTCBPS_RUNNING, TTCBPS_ERROR, TTCBPS_CANCELED, TTCBPS_FINISHED };
class TTCallbackProcess{

  public: 

    //constructor
    TTCallbackProcess();

    //return the current status
    virtual TTCallbackProcessState GetStatus() const = 0;

    //gets how much is completed
    //normally 0 ... 1.0, but can be larger than 1.0 in some circumstances
    //don't know which, but I don't forbid it 
    //but its always postive. 
    virtual float GetCompletionRatio() const = 0;

  protected:

    
};


//*****************************************************************
//*                   TTCallbackRunningProcess                    *
//*                                                               *
//*                                                               *
//*           this class encapsulates a running process           *
//*                                                               *
//*****************************************************************
class TTCallbackMainProcess : public TTCallbackProcess {

  public:

    //constructor
    TTCallbackMainProcess(TTCallbackProcess * const parent);

    //constructor
    TTCallbackMainProcess(TTCallback * const callback);

    //defines steps by id and relative time
    void DefineStepsTimes(const std::map<unsigned int, float> steps);

    //return the current status
    virtual TTCallbackProcessState GetStatus() const;

    //gets how much is completed
    //normally 0 ... 1.0, but can be larger than 1.0 in some circumstances
    //don't know which, but I don't forbid it 
    //but its always postive. 
    virtual float GetCompletionRatio() const;

  protected:

    TTCallbackProcess * ParentProcess = nullptr;

    TTCallback * Callback = nullptr;

    std::map<unsigned int, float> StepsTime;

};


//*****************************************************************
//*                      TTCallbackCount                          *
//*                                                               *
//*                                                               *
//*                abstract class for callbacks                   *
//*                                                               *
//*****************************************************************
//class TTCallbackCount : public TTCallback{


//};


//*****************************************************************
//*                    TTCallbackComplexSteps                     *
//*                                                               *
//*                                                               *
//*          abstract class for callbacks with many steps         *
//*                                                               *
//*****************************************************************
//class TTCallbackComplexSteps : public TTCallback{





//};


} //end of namespace ttwin

#endif
