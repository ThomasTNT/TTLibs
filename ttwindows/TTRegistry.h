#ifndef __TTREGISTRY_H_
#define __TTREGISTRY_H_

#include <string>
#include <vector>
#include <map>
#include <set>
#include <fstream>

#include "stringutil.h"

namespace ttwin{

//*****************************************************************
//*                         TTRegistry                            *
//*                                                               *
//*                                                               *
//*            abstract class for storing config data             *
//*                                                               *
//* there are two implementations below one with file and one     *
//* by using windows registry.                                    *
//*                                                               *
//*****************************************************************
class TTRegistry {

  public:

    //read the registry values to memory
    virtual bool ReadRegistryFromPersistence() = 0;

    //writes the registry values to persitent state
    virtual bool PersistRegistry() = 0;

    //function that gets a single config value (as string)
    virtual std::string GetValue(const std::string& key) = 0;

    //function that sets a single config value (as string)
    virtual void SetValue(const std::string& key, const std::string& value) = 0;

     //function that gets a vector of config values (as string)
    virtual std::vector<std::string> GetValues(const std::string& key) = 0;

    //function that sets aa vector of config values (as string)
    virtual void SetValues(const std::string& key, const std::vector<std::string>& values) = 0;

};


//*****************************************************************
//*                       TTRegistryFile                          *
//*                                                               *
//*                                                               *
//*        class for storing config data in an ini-file           *                            *
//*                                                               *
//*****************************************************************
struct TTRegEntry{
  TTRegEntry(){};
  std::vector<std::string> Values;
};

class TTRegistryFile : public  TTRegistry {

  public:

    //constructor
    TTRegistryFile(const std::string& filename);

    //destructor
    ~TTRegistryFile();

    //opens from file
    bool Open(const std::string& filename);

    //saves to file
    bool Save(const std::string& filename);

    //sets the filename used by ReadRegistryFromPersistence / PersistRegistry
    void SetFilename(const std::string& filename);

     //read the registry values to memory
    virtual bool ReadRegistryFromPersistence();

    //writes the registry values to persitent state
    virtual bool PersistRegistry();

    //function that gets a single config value (as string)
    virtual std::string GetValue(const std::string& key);

    //function that sets a single config value (as string)
    virtual void SetValue(const std::string& key, const std::string& value);

    //function that gets a vector of config values (as string)
    virtual std::vector<std::string> GetValues(const std::string& key);

    //function that sets aa vector of config values (as string)
    virtual void SetValues(const std::string& key, const std::vector<std::string>& values);

  private:

    //method for clearing
    void Clear();

    //map of config values
    std::map<std::string, TTRegEntry*> Values;

    //the filename
    std::string Filename;
};




} // end of namespace ttwin

#endif