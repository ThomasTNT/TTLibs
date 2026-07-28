#include "TTRegistry.h"


//*****************************************************************
//*                       TTRegistryFile                          *
//*                                                               *
//*                                                               *
//*        class for storing config data in an ini-file           *                            *
//*                                                               *
//*****************************************************************

//************
// constructor
//************
ttwin::TTRegistryFile::TTRegistryFile(const std::string& filename) : Filename(filename) {}

void ttwin::TTRegistryFile::SetFilename(const std::string& filename){
  Filename = filename;
}

//***********
// destructor
//***********
ttwin::TTRegistryFile::~TTRegistryFile(){
  Clear();
}

//******
// Clear
//******
void ttwin::TTRegistryFile::Clear(){
  //clean up all TTRegEntry's 
  for (std::map<std::string, TTRegEntry*>::iterator it = Values.begin(); it != Values.end(); ++it){
    delete (*it).second;
  }
  Values.clear();
}

//****************
// Open
// 
// opens from file
//****************
bool ttwin::TTRegistryFile::Open(const std::string& filename){
  std::ifstream fin(filename.c_str());
  if (!fin.good()){
    //Logger::getInstance().println("audioplay::ConcertParser::ParseFileContent could not parse: " + filename);
    //util::Notificator::GetInstance().AddNotification(util::Notification(util::NORMAL_ERROR, "3000", "Could not parse project file", "Could not open " + filename + "!"));  
    return false;
  }
  Clear();

  unsigned int linecount = 0;
  std::string buffer;
  while (fin.good()) {
    ++linecount;
    std::getline(fin, buffer, '\n');
    buffer = ttutil::StringUtil::trim(ttutil::StringUtil::removeComments(buffer));
    std::string key; 
    std::string value;
    ttutil::StringUtil::getKeyAndValue(buffer, key, value);
    SetValue(key, value);
  }
  fin.close();
  return true;
}

//**************
// Save
//
// saves to file
//**************
bool ttwin::TTRegistryFile::Save(const std::string& filename){
  std::ofstream fout(filename.c_str());
  if (!fout.good()){
    //Logger::getInstance().println("audioplay::ConcertParser::ParseFileContent could not parse: " + filename);
    //util::Notificator::GetInstance().AddNotification(util::Notification(util::NORMAL_ERROR, "3000", "Could not parse project file", "Could not open " + filename + "!"));  
    return false;
  }
  for (std::map<std::string, TTRegEntry*>::iterator it = Values.begin(); it != Values.end(); ++it){
    const std::string key = (*it).first;
    const TTRegEntry* const entry = (*it).second;
    for (std::vector<std::string>::const_iterator it = entry->Values.begin(); it != entry->Values.end(); ++it){
      if (!key.empty()){
        fout << key << "=" << (*it) << std::endl;
      }
    }
  }
  fout.close();
  return true;
}

//***********************************
// ReadRegistryFromPersistence
//
// read the registry values to memory
//***********************************
bool ttwin::TTRegistryFile::ReadRegistryFromPersistence(){
  return Open(Filename);
}

//**********************************************
// PersistRegistry
//
// writes the registry values to persitent state
//**********************************************
bool ttwin::TTRegistryFile::PersistRegistry(){
  return Save(Filename);
}

//****************************************************
// GetValue
//
// function that gets a single config value (as sring)
//****************************************************
std::string ttwin::TTRegistryFile::GetValue(const std::string& key){
  if (Values.find(key) == Values.end()) return "";
  const TTRegEntry* const entry = Values[key];
  if (!entry->Values.empty()) return entry->Values.front();
  return "";
}

//*****************************************************
// SetValue
//
// function that sets a single config value (as string)
//*****************************************************
void ttwin::TTRegistryFile::SetValue(const std::string& key, const std::string& value){
  if (Values.find(key) == Values.end()){
    TTRegEntry* entry = new TTRegEntry();
    entry->Values.push_back(value);
    Values[key] = entry;
  }
  else{
    TTRegEntry* entry = Values[key];
    for (const std::string& v : entry->Values){
      if (v == value) return;
    }
    entry->Values.push_back(value);
  }
}

//*********************************************************
// GetValues
//
// function that gets a vector of config values (as string)
//*********************************************************
std::vector<std::string> ttwin::TTRegistryFile::GetValues(const std::string& key){
  if (Values.find(key) == Values.end()) return std::vector<std::string>();
  const TTRegEntry* const entry = Values[key];
  return entry->Values;
}

//**********************************************************
// SetValues
//
// function that sets aa vector of config values (as string)
//**********************************************************
void ttwin::TTRegistryFile::SetValues(const std::string& key, const std::vector<std::string>& values){
  TTRegEntry* entry;
  if (Values.find(key) == Values.end()){
    entry = new TTRegEntry();
    Values[key] = entry;
  }
  else{
    entry = Values[key];
    entry->Values.clear();
  }
  for (unsigned int i = 0; i < values.size(); ++i){
    entry->Values.push_back(values[i]);
  }
}

