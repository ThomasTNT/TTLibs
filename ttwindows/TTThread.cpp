#include "TTThread.h"


//*****************************************************************************
//*                                SynchMethod                                *
//*                                                                           *
//*             class makes sure that a function runs only once               *
//*                                                                           *
//*                                                                           *
//*****************************************************************************

//***********
// static map
//***********
std::map<unsigned int, ttutil::SynchMethod*> ttutil::SynchMethod::SynchMethodMap;

//********************
// private constructor
//********************
ttutil::SynchMethod::SynchMethod(const unsigned int id) : Id(id) {
  InitializeCriticalSection(&CriticalSection);
}

//********************
// GetInstance
// 
// get instance method
//********************
ttutil::SynchMethod& ttutil::SynchMethod::GetInstance(const unsigned int id){
  if (SynchMethodMap.find(id) == SynchMethodMap.end()){
    SynchMethod* sm = new SynchMethod(id);
    SynchMethodMap[id] = sm;
  }
  return *(SynchMethodMap[id]);
}
    
//**************************************
// Enter
// 
// STATIC enter the critical part
// returns true when entering is allowed
// it does not wait until it is.
//**************************************
bool ttutil::SynchMethod::Enter(const unsigned int id){
  return SynchMethod::GetInstance(id).Enter();
}

//*******************************
// EnterWait
//
// STATIC enter the critical part
// waits until it is allowed
//*******************************
void ttutil::SynchMethod::EnterWait(const unsigned int id){
  SynchMethod::GetInstance(id).EnterWait();
}

//************************************
// Leave
// 
// STATIC exits the critical part
// releases the lock for other threads
//************************************
void ttutil::SynchMethod::Leave(const unsigned int id){
  SynchMethod::GetInstance(id).Leave();
}

//**************************************
// Enter
// 
// enter the critical part
// returns true when entering is allowed
// it does not wait until it is.
//**************************************
bool ttutil::SynchMethod::Enter(){
  const BOOL gainAccess = TryEnterCriticalSection(&CriticalSection);
  return gainAccess != FALSE;
}

//**************************
// EnterWait
//
// enter the critical part
// waits until it is allowed
//**************************
void ttutil::SynchMethod::EnterWait(){
  EnterCriticalSection(&CriticalSection);
}

//************************************
// Leave
// 
// exits the critical part
// releases the lock for other threads
//************************************
void ttutil::SynchMethod::Leave(){
  LeaveCriticalSection(&CriticalSection);
}



//*****************************************************************************
//*                                 SyncLock                                  *
//*                                                                           *
//*               locks a special funtionality for other thread               *
//*                                                                           *
//*                                                                           *
//*****************************************************************************

//***********
// static map
//***********
std::map<unsigned int, ttutil::SyncLock*> ttutil::SyncLock::SyncLockMap;

//********************
// private constructor
//********************
ttutil::SyncLock::SyncLock(const unsigned int id) : Id(id) {
  
}

//********************
// GetInstance
// 
// get instance method
//********************
ttutil::SyncLock& ttutil::SyncLock::GetInstance(const unsigned int id){
  if (SyncLockMap.find(id) == SyncLockMap.end()){
    SyncLock* sl = new SyncLock(id);
    SyncLockMap[id] = sl;
  }
  return *(SyncLockMap[id]);
}
    
//**************************************************
// Lock
// 
// STATIC enter the critical part
// if lokced wait until the other thread releases it
//**************************************************
void ttutil::SyncLock::Lock(const unsigned int id){
  SyncLock::GetInstance(id).Lock();
}

//**************************************************
// Release
// 
// STATIC exits the critical part
// releases the lock for other threads
//**************************************************
void ttutil::SyncLock::Release(const unsigned int id){
  SyncLock::GetInstance(id).Release();
}

//**************************************************
// Lock
// 
// enter the critical part
// if lokced wait until the other thread releases it
//**************************************************
void ttutil::SyncLock::Lock(){

}

//**************************************************
// Release
// 
// exits the critical part
// releases the lock for other threads
//**************************************************
void ttutil::SyncLock::Release(){

}

//*****************************************************************************
//*                                 TTThread                                  *
//*                                                                           *
//*     a class inpired by Java Thread to encapsulating win32 functionality   *
//*     abstract!                                                             *
//*****************************************************************************

//************
// constructor
//************
ttutil::TTThread::TTThread(){

}

//***********
// destructor
//***********
ttutil::TTThread::~TTThread(){

}

//*************************
// Start
//
// call start thread to run
//*************************
//void ttutil::TTThread::Start(){
//  CreateThread();
//}

//***************************************
// StartWait
// 
// call start thread to run, 
// waits if there is sychronized conflict
//***************************************
void ttutil::TTThread::StartWait(){
  CreateThread(false);
} 

//*******************************************
// StartOptional
//
// call start thread to run
// cancels if there is a sychronized conflict
//*******************************************
void ttutil::TTThread::StartOptional(){
  CreateThread(true);
}

//******************************************
// ThreadProc
//
// static function as win32 thread functions
// parameter is 
//******************************************
DWORD WINAPI ttutil::TTThread::ThreadProc(LPVOID lpParameter){
  //cast lpParameter back to TTThread and call user method Process()
  ThreadThisWrapper* ttw = static_cast<ThreadThisWrapper*>(lpParameter); 
  TTThread* ttthread = ttw->Self;
  const bool optional = ttw->Optional;
  delete ttw;
  return ttthread->Process(optional);
}

//****************************************
//
//
// private method to create a win32 thread
//****************************************
HANDLE ttutil::TTThread::CreateThread(const bool optional){
  //create warpper to handle this pointer
  ThreadThisWrapper* ttw = new ThreadThisWrapper(this, optional);
  return ::CreateThread(NULL, //lpThreadAttributes (NULL -> default security descriptor)
                        0,    //dwStackSize      (0 -> default size)
                        static_cast<LPTHREAD_START_ROUTINE>(&ThreadProc), // lpStartAddress, pointer of global thread function
                        static_cast<LPVOID>(ttw), //lpParameter, containing this pointer
                        0, //dwCreationFlags  (0 -> The thread runs immediately after creation)
                        NULL); //lpThreadId   (NULL -> the thread identifier is not returned)

}



