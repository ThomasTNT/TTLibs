#pragma once

#include <string>
#include <vector>
#include <map>
#include <Date.h>
#include <cstdint>

namespace ttutil{

  /**
   * This class is used 
   * interpet a line of config file
   */
  class StringUtil {

  public:
    /// default constructur
    StringUtil();

    /// copy constructor
    StringUtil(const StringUtil& sutil);

    /// destructor
    ~StringUtil();

    /// get key and value
    static bool getKeyAndValue(const std::string& src, std::string& key, std::string& val);

    /**
     * get key and value
     * @param src command line
     * @param host specified host name
     * @param program name of program
     * @param key key of parameter
     * @param value of paramater
     */
    static bool getKeyAndValue2(
				     const std::string& src, 
				     const std::string& host, 
				     const std::string& program,
				     std::string& key, 
				     std::string& val
				     );
    


    /**
     * get key and value
     * @param src command line
     * @param host name
     * @param program name
     * @param key   key
     * @param value value
     */
static bool getKeyAndValue3(
				 const std::string& src, 
				 std::string& host, 
				 std::string& program,
				 std::string& key, 
				std::string& val);

    /// remove white spaces
    static std::string trim(const std::string& src);

    /// remove comments
    static std::string removeComments(const std::string& src);


    /// detects whitespace
    static bool isWhiteSpace(char ch);

    /// detects start of comment
    static bool isStartOfComment(char ch);



    /**
     * left formatting of a string
     */
    static std::string formatLeft(const std::string& src, int size, char fillchar) ;


    /**
     * right formatting of a string
     */
    static std::string formatRight(const std::string& src, int size, char fillchar);


    /**
     * centered formatting of a string
     */
    static std::string formatCenter(const std::string& src, int size, char fillchar);


    /**
     * flexible formatting of a string (
     * direction < 0 : left;
     * direction = 0 : centered;
     * direction > 0 : right;
     */
    static std::string formatString(
			     const std::string& src, 
			     int size, 
			     int direction, 
			     char fillchar
			     );

    /**
     * separate name of program
     */
    static std::string getProgramName(const std::string& prgwithpath); 

    /**
     * parses arguments 
     */
    static std::map<std::string, std::string> parseArgs(char const * const * const argv, const unsigned int argc);

    /**
     * conversion of string to integer
     */
    static int strin2int(const std::string& src) {
      int rc=0;
      return rc;
    };

    /// conversion from bool to string
    static std::string bool2string(bool val);

    /// conversion from long to string
    static std::string int2string(long val);

    /// conversion from long to string
    static std::string uint2string(unsigned long val);

    /// conversion from long to hex string
    static std::string uint2hexstring(unsigned long val);

    /// conversion from long given as four chars to string
    static std::string long2charstring(unsigned long val);

    /// conversion from double to string
    static std::string double2string(double val);

    /// conversion from double to string, pos decimal places; fixedPoint=true forces trailing zeros (e.g. "0.0")
    static std::string double2string(double val, unsigned int pos, bool fixedPoint = true);

    /// conversion from string to bool
    static bool string2bool(const std::string& val);

    /// conversion from string to long integer
    static long string2int(const std::string& val);

    /// conversion from string to integer
    static double string2double(const std::string&  val);
  
    /// conversion from date to string
    static std::string date2string(const ttutil::Date& val);

    /// conversion from date to string, day only
    static std::string date2stringDay(const ttutil::Date& val);
  
    /// conversion from string to date
    static ttutil::Date string2date(const std::string&  val); 

    /**
     * split
     * @param str Source
     * @param delimiter
     * @param emptystring if true split returns always an vector 
     *                    with at least one element (empty string) 
     *                    if false and strSrc is empty result is 
     *                    empty, too.
     * @param totrim      if true the splitted strings will be trimed
     * @return Vector of splitted elements (OUT)
     */
    static std::vector<std::string> split(const std::string& strSrc, 
                                          char  cDelimiter, 
                                          bool  emptystring = false,
                                          bool  totrim = true);

    /**
     * extendet split
     * @param str Source
     * @param strDelimiters String of delimiters
     * @return Vector of splitted elements (OUT)
     * @return 
     */
    static std::vector<std::string> splitEx(const std::string& str, 
						                                const std::string& strDelimiters,
					                                  bool bTrim = true);

  /// ret the upper case of string
  static std::string toupper(const std::string& src);

  /**
   * ands a suffix to the name
   * filename.end -> filenamesuffix.end
   *
   */
  static std::string addSuffixAndKeepEnding(const std::string& src, const std::string& suffix);

  /**
  * ands a suffix to the name
  * filename_0000.end -> filenamesuffix_0000.end
  *
  */
  static std::string addSuffixAndKeepNumberAndEnding(const std::string& src, const std::string& suffix);

  /**
  * endsWith
  */
  static bool endsWith(std::string const &fullString, std::string const &ending);

  private:

    /**
     * basic method for this other methods
     */
    static std::string formatStringBase(
				 const std::string& src, 
				 int size, 
				 bool fill_right_start, 
				 bool fill_alternating, 
				 char fill_char
				 );
  };

};
