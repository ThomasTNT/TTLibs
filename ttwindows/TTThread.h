#ifndef __TTTHREAD_h__
#define __TTTHREAD_h__

#include <windows.h>

#include <map>

namespace ttutil{

//*****************************************************************************
//*                                SynchMethod                                *
//*                                                                           *
//*             class makes sure that a function runs only once               *
//*                                                                           *
//*****************************************************************************
class SynchMethod{

  private:
    
    //constructor
    SynchMethod(const unsigned int id);

  public:

    //get instance method
    static SynchMethod& GetInstance(const unsigned int id);
    
    //enter the critical part
    //returns true when entering is allowed
    //it does not wait until it is.
    static bool Enter(const unsigned int id);

    //enter the critical part
    //waits until it is allowed
    static void EnterWait(const unsigned int id);

    //exits the critical part
    //releases the lock for other threads
    static void Leave(const unsigned int id);

    //enter the critical part
    //returns true when entering is allowed
    //it does not wait until it is.
    bool Enter();

    //enter the critical part
    //waits until it is allowed
    void EnterWait();

    //exits the critical part
    //releases the lock for other threads
    void Leave();

  private:

    //references to the instances
    static std::map<unsigned int, SynchMethod*> SynchMethodMap;

    //id
    unsigned int Id;

    //CRITICAL_SECTION windows object
    CRITICAL_SECTION CriticalSection;

};



//*****************************************************************************
//*                                 SyncLock                                  *
//*                                                                           *
//*               locks a special funtionality for other thread               *
//*                                                                           *
//*                                                                           *
//*****************************************************************************
class SyncLock{

  private:
    
    //constructor
    SyncLock(const unsigned int id);

  public:

    //get instance method
    static SyncLock& GetInstance(const unsigned int id);
    
    //enter the critical part
    //if lokced wait until the other thread releases it
    static void Lock(const unsigned int id);

    //exits the critical part
    //releases the lock for other threads
    static void Release(const unsigned int id);

    //enter the critical part
    //if lokced wait until the other thread releases it
    void Lock();

    //exits the critical part
    //releases the lock for other threads
    void Release();

  private:

    //reference to the singleton instance
    static std::map<unsigned int, SyncLock*> SyncLockMap;

     //id
    unsigned int Id;

};


//*****************************************************************************
//*                                 TTThread                                  *
//*                                                                           *
//*     a class inpired by Java Thread to encapsulating win32 functionality   *
//*     abstract!                                                             *
//*****************************************************************************

class TTThread{
  
  public:

    //constructor
    TTThread();

    //destructor
    virtual ~TTThread();

    //call start thread to run, 
    //void Start(); 

    //call start thread to run, 
    //waits if there is sychronized conflict
    void StartWait(); 

    //call start thread to run
    //cancels if there is a sychronized conflict
    void StartOptional();

  protected:

    //overwrite this in your own thread class
    virtual int Process(const bool optional) = 0;

  private:

    //static function as win32 thread functions
    static unsigned long WINAPI ThreadProc(void* lpParameter);

    //private method to create a win32 thread
    void* CreateThread(const bool optional);

};

//stores the this pointer
//necessary due to C++ standard for valid conversions to void* and back
struct ThreadThisWrapper{
  ThreadThisWrapper(TTThread* self, const bool optional) : Self(self), Optional(optional){};
	TTThread* Self;
  const bool Optional;
};




} // end of namespace ttwin

#endif
