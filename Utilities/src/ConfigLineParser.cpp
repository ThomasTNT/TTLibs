#include "ConfigLineParser.h"

#include "Logger.h"
#include "Notificator.h"


//******************************************************************
//*                        ConfigLineParser                        *
//*                                                                *
//*              this class provides methods for parsing           *
//*                                                                *                       
//******************************************************************

//************
// constructor
//************
ttutil::ConfigLineParser::ConfigLineParser(){}


//************
// destructor
//************
ttutil::ConfigLineParser::~ConfigLineParser(){}


//**************************
// InitParsing
//
// initalises  TagStack etc. 
//**************************
void ttutil::ConfigLineParser::InitParsing(){
  TagStack.clear();
}


//********************************************
// ParseGlobalTag
//
// parses a tag in the upper hierachie level. 
// the tag stack will be cleaned than
// NOTE: no inner tag must have the same name
//********************************************
bool ttutil::ConfigLineParser::ParseGlobalTag(const std::string& line, const unsigned int linenumber, const std::string& tagname, const bool toparend){
  const size_t pos = line.find(tagname, 0);
  if (pos == 0 && (line.length() == tagname.length() || IsWordEnd(line[tagname.length()]))){
    Logger::getInstance().println("start global tag " + tagname);
    TagStack.clear();
    TagStack.push_back(tagname);
    return true;
  }
  else{
    return false;
  }
}

//*************************************
// ParseSimpleChildTag
//
// parses a tag in the lower hierachie 
// does nothing but check the tag stack
// up to three parents, fill with ""
//*************************************
bool ttutil::ConfigLineParser::ParseSimpleChildTag(const std::string& line,
                                                   const unsigned int linenumber,
                                                   const std::string& tagname,
                                                   const std::string& parent0,
                                                   const std::string& parent1,
                                                   const std::string& parent2,
                                                   const std::string& parent3,
                                                   const bool toparend)
{
  const size_t pos = line.find(tagname, 0);
  if (pos == 0 && (line.length() == tagname.length() || IsWordEnd(line[tagname.length()]))){
    const bool tagOk = CheckTagHierachy(linenumber, tagname, parent0, parent1, parent2, parent3, toparend);
    return tagOk;
  }
  else{
    return false;
  }
}

//*************************************
// ParseNumberedTag
//
// checks numbered tag
//*************************************
bool ttutil::ConfigLineParser::ParseNumberedTag(const std::string& line,
                                                const std::string& tagname,
                                                unsigned int& number)
{
  number = 0;
  const size_t pos = line.find(tagname, 0);
  if (pos == 0){
    //check if tag is follow by a number an nothing else
    std::string numberStr;
    for (unsigned int i = tagname.length(); i < line.length(); ++i){
      const char c = line[i];
      if (c >= '0' && c <= '9'){
        numberStr += c;
      }
      else if (IsWordEnd(c)){
        //end of word
        break;
      }
      else{
        //unecpected character 
        return false;
      }
    }
    number = ttutil::StringUtil::string2int(numberStr);
    return true;
  }
  else{
    return false;
  }
}

//*********************************************************
// ParseEndTag
//
// parses a tag and checks if there it is a matching endtag
// this is the case if starttag is the last on the stack
//*********************************************************
bool ttutil::ConfigLineParser::ParseEndTag(const std::string& line,
                                           const unsigned int linenumber,
                                           const std::string& tagname,
                                           const std::string& starttagname,
                                           const std::string& parent0,
                                           const std::string& parent1,
                                           const std::string& parent2,
                                           const std::string& parent3)

{
  const size_t pos = line.find(tagname, 0);
  if (pos == 0 && (line.length() == tagname.length() || IsWordEnd(line[tagname.length()]))){
    const bool starttagOk = CheckTagHierachy(linenumber, tagname, starttagname, parent0, parent1, parent2, true);
    RemoveTag(starttagname); //removes tagname as well
    return starttagOk;
  }
  else{
    return false;
  }
}

//***********************************************
// IsInsideTag
//
// returns true if last tag on stack is starttag
// TODO: shall we check the whole hierachie?
//***********************************************
bool ttutil::ConfigLineParser::IsInsideTag(const std::string& line,
                                              const unsigned int linenumber,
                                              const std::string& starttagname)
{
  return TagStack.size() > 0 && TagStack.back() == starttagname;
}


//*************************************
// ParseParamChildTag
//
// parses a tag in the lower hierachie 
// expects a key value pair 
// "tagname = value"
//*************************************
bool ttutil::ConfigLineParser::ParseParamChildTag(const std::string& line,
                                                  const unsigned int linenumber,
                                                  const std::string& tagname,
                                                  const std::string& parent0,
                                                  const std::string& parent1,
                                                  const std::string& parent2,
                                                  const std::string& parent3,
                                                  double &value,
                                                  const bool toparend)
{
  const bool foundtag = ParseSimpleChildTag(line, linenumber, tagname, parent0, parent1, parent2, parent3, toparend);
  if (foundtag){
    std::string tagname1;
    std::string valueStr;
    ttutil::StringUtil::getKeyAndValue(line, tagname1, valueStr);
    value = ttutil::StringUtil::string2double(valueStr);
    Logger::getInstance().println("read " + tagname + " | " + ttutil::StringUtil::double2string(value) + " | ");
  }
  return foundtag;
}

//*************************************
// ParseParamChildTag
//
// parses a tag in the lower hierachie 
// expects a key value pair 
// "tagname = value"
//*************************************
bool ttutil::ConfigLineParser::ParseParamChildTag(const std::string& line,
                                                  const unsigned int linenumber,
                                                  const std::string& tagname,
                                                  const std::string& parent0,
                                                  const std::string& parent1,
                                                  const std::string& parent2,
                                                  const std::string& parent3,
                                                  unsigned int &value,
                                                  const bool toparend)
{
  const bool foundtag = ParseSimpleChildTag(line, linenumber, tagname, parent0, parent1, parent2, parent3, toparend);
  if (foundtag){
    std::string tagname1;
    std::string valueStr;
    ttutil::StringUtil::getKeyAndValue(line, tagname1, valueStr);
    value = ttutil::StringUtil::string2int(valueStr);
    Logger::getInstance().println("read " + tagname + " | " + ttutil::StringUtil::uint2string(value) + " | ");
  }
  return foundtag;
}

//*************************************
// ParseParamChildTag
//
// parses a tag in the lower hierachie 
// expects a key value pair 
// string
//*************************************
bool ttutil::ConfigLineParser::ParseParamChildTag(const std::string& line,
                                                  const unsigned int linenumber,
                                                  const std::string& tagname,
                                                  const std::string& parent0,
                                                  const std::string& parent1,
                                                  const std::string& parent2,
                                                  const std::string& parent3,
                                                  std::string &value,
                                                  const bool toparend)
{
  const bool foundtag = ParseSimpleChildTag(line, linenumber, tagname, parent0, parent1, parent2, parent3, toparend);
  if (foundtag){
    std::string tagname1;
    std::string valueStr;
    ttutil::StringUtil::getKeyAndValue(line, tagname1, valueStr);
    value = valueStr;
    Logger::getInstance().println("read " + tagname + " | " + value + " | ");
  }
  return foundtag;
}


//*************************************
// ParseParamChildTag
//
// parses a tag in the lower hierachie 
// expects a key value pair 
// "tagname = value"
// boolean
//*************************************
bool ttutil::ConfigLineParser::ParseParamChildTag(const std::string& line,
                                                  const unsigned int linenumber,
                                                  const std::string& tagname,
                                                  const std::string& parent0,
                                                  const std::string& parent1,
                                                  const std::string& parent2,
                                                  const std::string& parent3,
                                                  bool &value,
                                                  const bool toparend)
{
  const bool foundtag = ParseSimpleChildTag(line, linenumber, tagname, parent0, parent1, parent2, parent3, toparend);
  if (foundtag){
    std::string tagname1;
    std::string valueStr;
    ttutil::StringUtil::getKeyAndValue(line, tagname1, valueStr);
    value = valueStr == YES;
    Logger::getInstance().println("read " + tagname + " | " + ttutil::StringUtil::bool2string(value) + " | ");
  }
  return foundtag;
}


//*************************************
// ParseParamChildTag
//
// parses a tag in the lower hierachie 
// expects a key value pair 
// "tagname = value"
// vector of unsigned int
//*************************************
bool ttutil::ConfigLineParser::ParseParamChildTag(const std::string& line,
                                                  const unsigned int linenumber,
                                                  const std::string& tagname,
                                                  const std::string& parent0,
                                                  const std::string& parent1,
                                                  const std::string& parent2,
                                                  const std::string& parent3,
                                                  std::vector<unsigned int> &values,
                                                  const bool toparend)
{
  const bool foundtag = ParseSimpleChildTag(line, linenumber, tagname, parent0, parent1, parent2, parent3, toparend);
  if (foundtag){
    std::string tagname1;
    std::string valueStr;
    ttutil::StringUtil::getKeyAndValue(line, tagname1, valueStr);
    std::vector<std::string> vStrs = ttutil::StringUtil::split(valueStr, ',', false, true);
    values.clear();
    for (unsigned int i = 0; i < vStrs.size(); ++i){
      int number = ttutil::StringUtil::string2int(vStrs[i]);
      if (number < 0) number = -number;
      values.push_back(number);
      Logger::getInstance().println("use output: " + ttutil::StringUtil::uint2string(i) + " -> " + ttutil::StringUtil::uint2string(number));
    }
  }
  return foundtag;
}

//*************************************
// ParseParamChildTag
//
// parses a tag in the lower hierachie 
// expects a key value pair 
// "tagname = value"
// vector of double
//*************************************
bool ttutil::ConfigLineParser::ParseParamChildTag(const std::string& line,
                                                  const unsigned int linenumber,
                                                  const std::string& tagname,
                                                  const std::string& parent0,
                                                  const std::string& parent1,
                                                  const std::string& parent2,
                                                  const std::string& parent3,
                                                  std::vector<double> &values,
                                                  const bool toparend)
{
  const bool foundtag = ParseSimpleChildTag(line, linenumber, tagname, parent0, parent1, parent2, parent3, toparend);
  if (foundtag){
    std::string tagname1;
    std::string valueStr;
    ttutil::StringUtil::getKeyAndValue(line, tagname1, valueStr);
    std::vector<std::string> vStrs = ttutil::StringUtil::split(valueStr, ',', false, true);
    values.clear();
    for (unsigned int i = 0; i < vStrs.size(); ++i){
      const double number = ttutil::StringUtil::string2double(vStrs[i]);
      values.push_back(number);
      Logger::getInstance().println("use output: " + ttutil::StringUtil::uint2string(i) + " -> " + ttutil::StringUtil::double2string(number));
    }
  }
  return foundtag;
}

//*************************************
// ParseParamChildTag
//
// parses a tag in the lower hierachie 
// expects a key value pair 
// "tagname = value"
// vector of float
//*************************************
bool ttutil::ConfigLineParser::ParseParamChildTag(const std::string& line,
                                                  const unsigned int linenumber,
                                                  const std::string& tagname,
                                                  const std::string& parent0,
                                                  const std::string& parent1,
                                                  const std::string& parent2,
                                                  const std::string& parent3,
                                                  std::vector<float> &values,
                                                  const bool toparend)
{
  const bool foundtag = ParseSimpleChildTag(line, linenumber, tagname, parent0, parent1, parent2, parent3, toparend);
  if (foundtag) {
    std::string tagname1;
    std::string valueStr;
    ttutil::StringUtil::getKeyAndValue(line, tagname1, valueStr);
    std::vector<std::string> vStrs = ttutil::StringUtil::split(valueStr, ',', false, true);
    values.clear();
    for (unsigned int i = 0; i < vStrs.size(); ++i) {
      const float number = static_cast<float>(ttutil::StringUtil::string2double(vStrs[i]));
      values.push_back(number);
      Logger::getInstance().println("use output: " + ttutil::StringUtil::uint2string(i) + " -> " + ttutil::StringUtil::double2string(number));
    }
  }
  return foundtag;
}

//*************************************
// ParseParamChildTag
//
// parses a tag in the lower hierachie 
// expects a key value pair 
// "tagname = value"
// vector of string
//*************************************
bool ttutil::ConfigLineParser::ParseParamChildTag(const std::string& line,
                                                  const unsigned int linenumber,
                                                  const std::string& tagname,
                                                  const std::string& parent0,
                                                  const std::string& parent1,
                                                  const std::string& parent2,
                                                  const std::string& parent3,
                                                  std::vector<std::string> &values,
                                                  const bool toparend)
{
  const bool foundtag = ParseSimpleChildTag(line, linenumber, tagname, parent0, parent1, parent2, parent3, toparend);
  if (foundtag){
    std::string tagname1;
    std::string valueStr;
    ttutil::StringUtil::getKeyAndValue(line, tagname1, valueStr);
    std::vector<std::string> vStrs = ttutil::StringUtil::split(valueStr, ',', false, true);
    values.clear();
    for (unsigned int i = 0; i < vStrs.size(); ++i){
      const std::string value = ttutil::StringUtil::trim(vStrs[i]);
      values.push_back(value);
    }
  }
  return foundtag;
}


//***************************************
// ParseNumberedChildTag
//
// parses a tag in the lower hierachie 
// expects a tagname with number
// "tagnameN"
// sets 0 if no number could be extracted
//***************************************
bool ttutil::ConfigLineParser::ParseNumberedChildTag(const std::string& line,
                                                     const unsigned int linenumber,
                                                     const std::string& tagname,
                                                     const std::string& parent0,
                                                     const std::string& parent1,
                                                     const std::string& parent2,
                                                     const std::string& parent3,
                                                     unsigned int &number,
                                                     const bool toparend)
{
  const bool foundtag = ParseNumberedTag(line, tagname, number);
  if (foundtag){
    const bool tagOk = CheckTagHierachy(linenumber, tagname, parent0, parent1, parent2, parent3, toparend);
    return tagOk;
  }
  else{
    return false;
  }
}

//*************************************
// ParseNumberedParamChildTag
// 
// parses a tag in the lower hierachie 
// expects a key value pair with number
// "tagnameN = value"
// string
//*************************************
bool ttutil::ConfigLineParser::ParseNumberedParamChildTag(const std::string& line,
                                                          const unsigned int linenumber,
                                                          const std::string& tagname,
                                                          const std::string& parent0,
                                                          const std::string& parent1,
                                                          const std::string& parent2,
                                                          const std::string& parent3,
                                                          unsigned int &number,
                                                          std::string &value,
                                                          const bool toparend)
{
  const bool foundtag = ParseNumberedTag(line, tagname, number);
  if (foundtag){
    const bool tagOk = CheckTagHierachy(linenumber, tagname, parent0, parent1, parent2, parent3, toparend);
    if (tagOk){
      std::string tagname1;
      std::string valueStr;
      ttutil::StringUtil::getKeyAndValue(line, tagname1, valueStr);
      value = valueStr;
      Logger::getInstance().println("read " + tagname + " | " + value + " | ");
    }
    return tagOk;
  }
  else{
    return false;
  }
}

//*************************************
// ParseNumberedParamChildTag
// 
// parses a tag in the lower hierachie 
// expects a key value pair with number
// "tagnameN = value"
// int
//*************************************
bool ttutil::ConfigLineParser::ParseNumberedParamChildTag(const std::string& line,
                                                          const unsigned int linenumber,
                                                          const std::string& tagname,
                                                          const std::string& parent0,
                                                          const std::string& parent1,
                                                          const std::string& parent2,
                                                          const std::string& parent3,
                                                          unsigned int &number,
                                                          int &value,
                                                          const bool toparend)
{
  const bool foundtag = ParseNumberedTag(line, tagname, number);
  if (foundtag){
    const bool tagOk = CheckTagHierachy(linenumber, tagname, parent0, parent1, parent2, parent3, toparend);
    if (tagOk){
      std::string tagname1;
      std::string valueStr;
      ttutil::StringUtil::getKeyAndValue(line, tagname1, valueStr);
      value = ttutil::StringUtil::string2int(valueStr);
      Logger::getInstance().println("read " + tagname + " | " + ttutil::StringUtil::int2string(value) + " | ");
    }
    return tagOk;
  }
  else{
    return false;
  }
}


//******************************************
// CheckTagHierachy
//
// checks of tag is allowed on this position
// add it to the tag stack if yes
//******************************************
bool ttutil::ConfigLineParser::CheckTagHierachy(const unsigned int linenumber,
                                                const std::string& tagname,
                                                const std::string& parent0,
                                                const std::string& parent1,
                                                const std::string& parent2,
                                                const std::string& parent3,
                                                const bool toparend)
{
  return toparend ? CheckTagHierachyExact(linenumber, tagname, parent0, parent1, parent2, parent3)
                  : CheckTagHierachySuitable(linenumber, tagname, parent0, parent1, parent2, parent3);
}

//*************************************************
// CheckTagHierachyExact
//
// checks if tagstack matches exact the parent list
//*************************************************
bool ttutil::ConfigLineParser::CheckTagHierachyExact(const unsigned int linenumber,
                                                     const std::string& tagname,
                                                     const std::string& parent0,
                                                     const std::string& parent1,
                                                     const std::string& parent2,
                                                     const std::string& parent3) 
{
  unsigned int depth = 0;
  if (!parent0.empty()) ++depth;
  if (!parent1.empty()) ++depth;
  if (!parent2.empty()) ++depth;
  if (!parent3.empty()) ++depth;

  std::string errormessage = tagname + (parent0.empty() ? "must be a global tag!" : " must be a child of " + parent0);
  if (!parent1.empty()) errormessage += "->" + parent1;
  if (!parent2.empty()) errormessage += "->" + parent2;
  if (!parent3.empty()) errormessage += "->" + parent3;
  errormessage += "! (line: " + ttutil::StringUtil::uint2string(linenumber) + ")";

  if (TagStack.size() != depth){
    //ThrowParserError(errormessage); //no error can be valid anywhere else later
    return false;
  }
  else if (    (depth > 0 && TagStack[depth - 1] != parent0)
            || (depth > 1 && TagStack[depth - 2] != parent1 && (depth <= 2 || "*" != parent1))
            || (depth > 2 && TagStack[depth - 3] != parent2 && (depth <= 3 || "*" != parent2))
            || (depth > 3 && TagStack[depth - 4] != parent3))
  {
    //ThrowParserError(errormessage); //no error can be valid anywhere else later
    return false;
  }
  else{
    AddTag(tagname, depth, true);
    return true;
  }
}


//******************************************
// CheckTagHierachySuitable
//
// checks of tag is allowed on this position
// add it to the tag stack if yes
//******************************************
bool ttutil::ConfigLineParser::CheckTagHierachySuitable(const unsigned int linenumber,
                                                        const std::string& tagname,
                                                        const std::string& parent0,
                                                        const std::string& parent1,
                                                        const std::string& parent2,
                                                        const std::string& parent3)
{
  unsigned int depth = 0;
  if (!parent0.empty()) ++depth;
  if (!parent1.empty()) ++depth;
  if (!parent2.empty()) ++depth;
  if (!parent3.empty()) ++depth;

  std::string errormessage = tagname + (parent0.empty() ? "must be a global tag!" : " must be a child of " + parent0);
  if (!parent1.empty()) errormessage += "->" + parent1;
  if (!parent2.empty()) errormessage += "->" + parent2;
  if (!parent3.empty()) errormessage += "->" + parent3;
  errormessage += "! (line: " + ttutil::StringUtil::uint2string(linenumber) + ")";

  //check tagstack depth
  //HINT: may be greater is going up
  if (TagStack.size() < depth){
    //wrong tag placement
    ThrowParserError(errormessage);
    return false;
  }

  //global tag or something wrong
  if (parent0.empty()){
    //global tag? (this method should not be used for global tags, but we are tolerant)
    if (TagStack.size() == 0){
      AddTag(tagname, depth, false);
      return true;
    }
    else{
      //wrong tag placement
      ThrowParserError(errormessage);
      return false;
    }
  }
  //parent0 is not empty -> it must be on the right position
  else{
    if (TagStack[depth - 1] != parent0){
      //wrong tag placement
      ThrowParserError(errormessage);
      return false;
    }
    else{
      //********** recursively check parent1 ************
      if (parent1.empty()){
        AddTag(tagname, depth, false);
        return true;
      }
      else{
        if (TagStack[depth - 2] != parent1){
          //wrong tag placement
          ThrowParserError(errormessage);
          return false;
        }
        else{
          //************ recursively check parent2 ************
          if (parent2.empty()){
            AddTag(tagname, depth, false);
            return true;
          }
          else{
            if (TagStack[depth - 3] != parent2){
              //wrong tag placement
              ThrowParserError(errormessage);
              return false;
            }
            else{
              //************ recursively check parent3 ************
              if (parent3.empty()){
                AddTag(tagname, depth, false);
                return true;
              }
              else{
                if (TagStack[depth - 4] != parent3){
                  //wrong tag placement
                  ThrowParserError(errormessage);
                  return false;
                }
                else{
                  AddTag(tagname, depth, false);
                  return true;
                }
              }
            }
          }
        }
      }
    }
  }
}

//*************************************************
// AddTag
// 
// adds tag name to stack
// after this method tagstack is only depth+1 long
// all higher tags will be deleted
//*************************************************
void ttutil::ConfigLineParser::AddTag(const std::string& tagname, const unsigned int depth, const bool toparend){
  //fill up with dummys
  while (TagStack.size() < depth) TagStack.push_back("");
  //delete superfluous
  while (TagStack.size() > depth) TagStack.pop_back();
  //add it, if it does not belong directly to the parent
  if (!toparend) TagStack.push_back(tagname);
}

//*************************************************
// RemoveTag
// 
// removes tag and all children from stack
// all higher tags will be deleted
//*************************************************
void ttutil::ConfigLineParser::RemoveTag(const std::string& tagname){
  const unsigned int startsize = TagStack.size();
  unsigned int found = 0;
  for (unsigned int i = startsize; i > 0; --i){
    const std::string currTag = TagStack[i - 1];
    if (currTag == tagname){
      found = i;
      break;
    }
  }
  if (found > 0){
    for (unsigned int i = startsize; i >= found; --i){
      TagStack.pop_back();
    }
  }
}

//********************************
// ThrowParserError
//
// handels an error during parsing
//********************************
void ttutil::ConfigLineParser::ThrowParserError(const std::string& errormessage){
  Logger::getInstance().println(errormessage);
  ttutil::Notificator::GetInstance().AddNotification(ttutil::Notification(ttutil::NORMAL_ERROR, "3001", "Error parsing project file.", errormessage));
  for (unsigned int i = 0; i < TagStack.size(); ++i){
    Logger::getInstance().println("TagStack[" + ttutil::StringUtil::int2string(i) + "]: " + TagStack[i]);
  }
}

//****************************************************
// IsWordEnd
//
// returns true is char is a whitespace or "=" or ":" 
//****************************************************
bool ttutil::ConfigLineParser::IsWordEnd(const char c){
  return    c == ' '
         || c == '\t'
         || c == '\f'
         || c == '\v'
         || c == '\n'
         || c == '\r'
         || c == '=';
}

