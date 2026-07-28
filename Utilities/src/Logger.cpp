#include "Logger.h"

//*************************************************************
//*                                                           *
//*             class for logging into a file                 *
//*                                                           *
//*                      a singleton                          *
//*************************************************************

//static pointer to the instance
Logger* Logger::logger = nullptr; 

std::mutex Logger::LogMutex;

//#undef LOG_TT_DEBUG

//********************
// getInstance
//
// get instance method
//********************
Logger& Logger::getInstance(){
  if (!logger) logger = new Logger();
  return *logger;
}

//*************************
// SetThemes
//
// sets the themes to print
//*************************
void Logger::SetThemes(const std::vector<std::string>& themes) {
  Themes.clear();
  std::for_each(themes.begin(), themes.end(), [this](const std::string& theme) { Themes.insert(theme); });
}

//**********************
// AddTheme
//
// adds a theme to print
//**********************
void Logger::AddTheme(const std::string& theme) {
  Themes.insert(theme);
}

//*************************
// RemoveTheme
//
// removes a theme to print
//*************************
void Logger::RemoveTheme(const std::string& theme) {
  Themes.erase(theme);
}

//*********************************
// print
//
// logging function with no newline
//*********************************
void Logger::print(const std::string& msg, const std::string& theme){
  #if defined LOG_TT_DEBUG || defined LOG_ALWAYS
    std::lock_guard<std::mutex> lock(LogMutex);
    if (outfile && outfile->good()){
      if (cache.length() > 0){ 
        *outfile << cache;
        cache.clear();
      }
      *outfile << msg << std::flush; 
    }
  #endif
} 

//******************************
// println
//
// logging function with newline
//******************************
void Logger::println(const std::string& msg, const std::string& theme){
  #if defined LOG_TT_DEBUG || defined LOG_ALWAYS
    std::lock_guard<std::mutex> lock(LogMutex);
    if (PrintThis(theme) && outfile && outfile->good()){
      if (cache.length() > 0){ 
        *outfile << cache;
        cache.clear();
      }
      *outfile << msg << std::endl; 
    }
  #endif
} 

//******************************
// println
//
// logging function with newline
//******************************
void Logger::println(const std::string& msg, int i1, const std::string& theme){
  println(msg + ": " + ttutil::StringUtil::int2string(i1));
}

//******************************
// println
//
// logging function with newline
//******************************
void Logger::println(const std::string& msg, int i1, int i2, const std::string& theme) {
  println(msg + ": " + ttutil::StringUtil::int2string(i1) + ", " + ttutil::StringUtil::uint2string(i2));
}

//******************************
// println
//
// logging function with newline
//******************************
void Logger::println(const std::string& msg, unsigned int i1, unsigned int i2, unsigned int i3, const std::string& theme){
  println(msg + ": " + ttutil::StringUtil::uint2string(i1) + ", " + ttutil::StringUtil::uint2string(i2) + ", " + ttutil::StringUtil::uint2string(i3));
}

//*********************************
// print
//
// logging function with no newline
//*********************************
void Logger::printDelay(const std::string& msg, const std::string& theme){
  #if defined LOG_TT_DEBUG || defined LOG_ALWAYS
    if (PrintThis(theme)) {
      cache += msg;
    }
  #endif
} 

//******************************
// println
//
// logging function with newline
//******************************
void Logger::printlnDelay(const std::string& msg, const std::string& theme){
  #if defined LOG_TT_DEBUG || defined LOG_ALWAYS
  if (PrintThis(theme)) {
    cache += msg + '\n';
  }
  #endif
} 

//********************
// private constructor
//********************
Logger::Logger() {
  #if defined LOG_TT_DEBUG || defined LOG_ALWAYS
  outfile = new std::ofstream(LOGFILE.c_str());
  #endif
}

//***********
// destructor
//***********
Logger::~Logger(){
  if (outfile){
    outfile->close();
    delete outfile;
  }
}

//********************************
// PrintThis
//
// HELP: checks is we should print
//********************************
bool Logger::PrintThis(const std::string& theme) {
  if (!Themes.empty() && !theme.empty()) {
    if (Themes.find(theme) == Themes.end()) {
      return false;
    }
  }
  return true;
}


//*************************************************************
//*                                                           *
//*             class for profiling into logfile              *
//*                                                           *
//*                      a singleton                          *
//*************************************************************

//***************************
// reads internal CPU counter
//***************************
#ifndef TTLOGGER_NO_PROFILE 
  #ifdef _WIN64
  #pragma intrinsic(__rdtsc)
  unsigned __int64 __cdecl rdtsc() {
    return __rdtsc();
  }
  #else
  __declspec(naked) unsigned __int64 __cdecl rdtsc() {
    __asm
    {
      rdtsc
      ret; return value at EDX : EAX
    }
  }
  #endif  
#else
unsigned __int64 __cdecl rdtsc() {
  return 0;
}
#endif

//*********************
// the static singleton
//*********************
Profiler* Profiler::profiler = NULL; 

//******************** 
// private constructor
//********************
Profiler::Profiler(){
  reset();
}

//***********
// destructor
//***********
Profiler::~Profiler(){}

//******************** 
// getInstance
// 
// get instance method
//******************** 
Profiler& Profiler::getInstance(){
  if (!profiler) profiler = new Profiler();
  return *profiler;
}

//************************* 
// getLocalInstance
// 
// creates a local instance
//************************* 
Profiler Profiler::getLocalInstance(){
  Profiler p;
  return p;
} 

//*****************
// printTime
// 
// logging function
//*****************
void Profiler::printTime(const std::string& msg){
  auto now = std::chrono::system_clock::now();
  auto diff = now - last;
  last = now;
  double milliseconds = static_cast<double>(static_cast<unsigned int>(diff.count())) / 3000000.0;
  Logger::getInstance().println(std::string("###PROFILE### ") + msg + " diff: " + ttutil::StringUtil::double2string(milliseconds));
}

//*******************************************
// reset
//
// resets the profiler for a new meassurement
//*******************************************
void Profiler::reset(){
  last = std::chrono::system_clock::now(); // static_cast<time_t>(rdtsc());
}

//*********************
// startSum
//
// start adding to sum
//*********************
void Profiler::startSum(){
  reset();
}

//****************************
// stopSum
//
// adds time difference to sum
//****************************
void Profiler::stopSum(){
  auto now = std::chrono::system_clock::now();
  auto diff = now - last;
  last = now;
  sum += diff;
}

//*****************
// printSum
//
// logging function 
//*****************
void Profiler::printSum(const std::string& msg){
  #pragma warning(suppress : 4244)
  long milliseconds = sum.time_since_epoch().count();
  Logger::getInstance().println(std::string("###PROFILE SUM ### ") + msg + " sum: " + ttutil::StringUtil::int2string(milliseconds));
}
