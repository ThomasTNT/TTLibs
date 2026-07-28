#pragma once

#include <string>
#include <iostream>
#include <fstream>

#include "stringutil.h"
#include "FileBrowser.h"


namespace ttutil{

//******************************************************************
//*                        ConfigLineParser                        *
//*                                                                *
//*              this class provides methods for parsing           *
//*                                                                *                       
//******************************************************************

const std::string YES = "yes";
const std::string NO = "no";


class ConfigLineParser {

  protected:

    //-------------------- INITIALISATION AND CO. ------------------

    //constructor
    ConfigLineParser();

  public:    

    //destructor
    virtual ~ConfigLineParser();

    //----------------------- PUBLIC METHODS --------------------


  protected:

    //----------------------- PARSING METHODS ----------------------

    //initalises  TagStack etc. 
    void InitParsing();

    //parses a tag in the upper hierachie level. 
    //the tag stack will be cleaned than
    //NOTE: no inner tag must have the same name
    bool ParseGlobalTag(const std::string& line, const unsigned int linenumber, const std::string& tagname, const bool toparend);

    //parses a tag in the lower hierachie 
    //does nothing but check the tag stack
    //up to three parents, fill with ""
    bool ParseSimpleChildTag(const std::string& line,
                             const unsigned int linenumber,
                             const std::string& tagname,
                             const std::string& parent0,
                             const std::string& parent1,
                             const std::string& parent2,
                             const std::string& parent3, 
                             const bool toparend);

    //checks numbered tag
    bool ParseNumberedTag(const std::string& line,
                          const std::string& tagname,
                          unsigned int& number);

    //parses a tag and checks if there it is a matching endtag
    //this is the case if starttag is the last on the stack
    bool ParseEndTag(const std::string& line,
                     const unsigned int linenumber,
                     const std::string& tagname,
                     const std::string& starttagname,
                     const std::string& parent0,
                     const std::string& parent1,
                     const std::string& parent2,
                     const std::string& parent3);

    //returns true if last tag on stack is starttag
    //TODO: shall we check the whole hierachie?
    bool IsInsideTag(const std::string& line,
                     const unsigned int linenumber,
                     const std::string& starttagname);

    //parses a tag in the lower hierachie 
    //expects a key value pair 
    //"tagname = value"
    //double
    bool ParseParamChildTag(const std::string& line,
                            const unsigned int linenumber,
                            const std::string& tagname,
                            const std::string& parent0,
                            const std::string& parent1,
                            const std::string& parent2,
                            const std::string& parent3,
                            double &value,
                            const bool toparend);

    //parses a tag in the lower hierachie 
    //expects a key value pair 
    //"tagname = value"
    //unsigned int
    bool ParseParamChildTag(const std::string& line,
                            const unsigned int linenumber,
                            const std::string& tagname,
                            const std::string& parent0,
                            const std::string& parent1,
                            const std::string& parent2,
                            const std::string& parent3,
                            unsigned int &value,
                            const bool toparend);

    //parses a tag in the lower hierachie 
    //expects a key value pair 
    //"tagname = value"
    //string
    bool ParseParamChildTag(const std::string& line,
                            const unsigned int linenumber,
                            const std::string& tagname,
                            const std::string& parent0,
                            const std::string& parent1,
                            const std::string& parent2,
                            const std::string& parent3,
                            std::string &value,
                            const bool toparend);

    //parses a tag in the lower hierachie 
    //expects a key value pair 
    //"tagname = value"
    //boolean
    bool ParseParamChildTag(const std::string& line,
                            const unsigned int linenumber,
                            const std::string& tagname,
                            const std::string& parent0,
                            const std::string& parent1,
                            const std::string& parent2,
                            const std::string& parent3,
                            bool &value,
                            const bool toparend);

    //parses a tag in the lower hierachie 
    //expects a key value pair 
    //"tagname = value"
    //vector of unsigned int
    bool ParseParamChildTag(const std::string& line,
                            const unsigned int linenumber,
                            const std::string& tagname,
                            const std::string& parent0,
                            const std::string& parent1,
                            const std::string& parent2,
                            const std::string& parent3,
                            std::vector<unsigned int> &values,
                            const bool toparend);

    //parses a tag in the lower hierachie 
    //expects a key value pair 
    //"tagname = value"
    //vector of double
    bool ParseParamChildTag(const std::string& line,
                            const unsigned int linenumber,
                            const std::string& tagname,
                            const std::string& parent0,
                            const std::string& parent1,
                            const std::string& parent2,
                            const std::string& parent3,
                            std::vector<double> &values,
                            const bool toparend);

    //parses a tag in the lower hierachie 
    //expects a key value pair 
    //"tagname = value"
    //vector of float
    bool ParseParamChildTag(const std::string& line,
                            const unsigned int linenumber,
                            const std::string& tagname,
                            const std::string& parent0,
                            const std::string& parent1,
                            const std::string& parent2,
                            const std::string& parent3,
                            std::vector<float> &values,
                            const bool toparend);

    //parses a tag in the lower hierachie 
    //expects a key value pair 
    //"tagname = value"
    //vector of string
    bool ParseParamChildTag(const std::string& line,
                            const unsigned int linenumber,
                            const std::string& tagname,
                            const std::string& parent0,
                            const std::string& parent1,
                            const std::string& parent2,
                            const std::string& parent3,
                            std::vector<std::string> &values,
                            const bool toparend);

    //parses a tag in the lower hierachie 
    //expects a tagname with number
    //"tagnameN"
    //sets 0 if no number could be extracted
    bool ParseNumberedChildTag(const std::string& line,
                               const unsigned int linenumber,
                               const std::string& tagname,
                               const std::string& parent0,
                               const std::string& parent1,
                               const std::string& parent2,
                               const std::string& parent3,
                               unsigned int &number,
                               const bool toparend);

    //parses a tag in the lower hierachie 
    //expects a key value pair with number
    //"tagnameN = value"
    //string
    bool ParseNumberedParamChildTag(const std::string& line,
                                    const unsigned int linenumber,
                                    const std::string& tagname,
                                    const std::string& parent0,
                                    const std::string& parent1,
                                    const std::string& parent2,
                                    const std::string& parent3,
                                    unsigned int &number,
                                    std::string &value,
                                    const bool toparend);

    //parses a tag in the lower hierachie 
    //expects a key value pair with number
    //"tagnameN = value"
    //int
    bool ParseNumberedParamChildTag(const std::string& line,
                                    const unsigned int linenumber,
                                    const std::string& tagname,
                                    const std::string& parent0,
                                    const std::string& parent1,
                                    const std::string& parent2,
                                    const std::string& parent3,
                                    unsigned int &number,
                                    int &value,
                                    const bool toparend);

    //----------------------- PARSING HELP METHODS ----------------------

  private:

    //checks of tag is allowed on this position
    //add it to the tag stack if yes
    bool CheckTagHierachy(const unsigned int linenumber,
                          const std::string& tagname,
                          const std::string& parent0,
                          const std::string& parent1,
                          const std::string& parent2,
                          const std::string& parent3,
                          const bool toparend);

    //checks if tagstack matches exact the parent list
    bool CheckTagHierachyExact(const unsigned int linenumber,
                               const std::string& tagname,
                               const std::string& parent0,
                               const std::string& parent1,
                               const std::string& parent2,
                               const std::string& parent3);

    //checks of tag is allowed on this position
    //add it to the tag stack if yes
    bool CheckTagHierachySuitable(const unsigned int linenumber,
                                  const std::string& tagname,
                                  const std::string& parent0,
                                  const std::string& parent1,
                                  const std::string& parent2,
                                  const std::string& parent3);

    //adds tag name to stack
    //after this method tagstack is only depth+1 long
    //all higher tags will be deleted
    void AddTag(const std::string& tagname, const unsigned int depth, const bool toparend);

    //removes tag and all children from stack
    void RemoveTag(const std::string& tagname);

    // handels an error during parsing
    void ThrowParserError(const std::string& errormessage);

    //returns true is char is a whitespace or "=" or ":" 
    bool IsWordEnd(const char c);
    

    //-------------------------- FIELDS -----------------------

  private:

    std::vector<std::string> TagStack;

  };

} //end of namespace audioplay
