#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <unordered_set>
#include <algorithm>
#include <chrono>
#include <intrin.h>
#include <mutex>

#include "stringutil.h"

//*************************************************************
//*                                                           *
//*             class for logging into a file                 *
//*                                                           *
//*                      a singleton                          *
//*************************************************************

//define it, if there should be logging even in case of Release
//#define LOG_ALWAYS

static const std::string LOGFILE("C:\\temp\\plugin.log");

class Logger{

  public: 
    
    // get instance method
    static Logger& getInstance(); 

    // sets the themes to print
    void SetThemes(const std::vector<std::string>& themes);

    // adds a theme to print
    void AddTheme(const std::string& theme);

    // removes a theme to print
    void RemoveTheme(const std::string& theme);

    // logging function
    void print(const std::string& msg, const std::string& theme = "");

    // logging function
    void println(const std::string& msg, const std::string& theme = "");

    // logging function
    void println(const std::string& msg, int i1, const std::string& theme = "");

    // logging function
    void println(const std::string& msg, int i1, int i2, const std::string& theme = "");

    // logging function
    void println(const std::string& msg, unsigned int i1, unsigned int i2, unsigned int i3, const std::string& theme = "");

    // logging function
    void printDelay(const std::string& msg, const std::string& theme = "");

    // logging function
    void printlnDelay(const std::string& msg, const std::string& theme = "");

    // destructor
    ~Logger();

  private:
    
    // private constructor
    Logger();
    
    // HELP: checks is we should print
    bool PrintThis(const std::string& theme);

    // reference to the singleton instance
    static Logger* logger;

    // file to write to
    std::ofstream* outfile = nullptr;

    // cache for delayed writing
    std::string cache;

    // list of themes that are to print
    std::unordered_set<std::string> Themes; 

    // mutex for log
    static std::mutex LogMutex;

};

//*************************************************************
//*                                                           *
//*             class for profiling into logfile              *
//*                                                           *
//*                      a singleton                          *
//*************************************************************
unsigned __int64 __cdecl rdtsc();

class Profiler{
  
  public:

    // get instance method
    static Profiler& getInstance(); 

    // creates a local instance
    static Profiler getLocalInstance(); 
  
    // logging function
    void printTime(const std::string& msg);

    // start adding to sum
    void startSum();

    // start adding to sum
    void stopSum();

    // logging function
    void printSum(const std::string& msg);

    // destructor
    ~Profiler();

    // resets the profiler for a new meassurement
    void reset();

  private:

    // last timestamp
    std::chrono::time_point<std::chrono::system_clock> last;

    // sum
    std::chrono::time_point<std::chrono::system_clock> sum;

    // private constructor
    Profiler();
    
    // reference to the singleton instance
    static Profiler* profiler;

};
