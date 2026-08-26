
#include "stringutil.h"

#include <iostream>
#include <string>

//strstream and sstream seem to be to heavy and not thread safe
//#define USE_OWN_ALGORITHMS

#ifdef USE_OWN_ALGORITHMS
  //import nothing
#else
  #ifndef GPP_STL_OLD
  #include <sstream>
  #include <iomanip>
  #else
  #include <stdio.h>
  #include <strstream>
  #endif
#endif

// default constructur
ttutil::StringUtil::StringUtil() {  };

// copy constructor
ttutil::StringUtil::StringUtil(const StringUtil& sutil) {  };

// destructor
ttutil::StringUtil::~StringUtil() {  };


// get key and value
bool ttutil::StringUtil::getKeyAndValue(const std::string& src, std::string& key, std::string& val) {
  bool result = false;
  const std::string DELIMITER("=");

  std::string tmp = trim ( removeComments( src ) );

  std::string::size_type pos = tmp.find_first_of( DELIMITER );
   
  if ( (std::string::npos != pos) && (pos > 0) ) {

    key = trim(tmp.substr(0, pos));

    if ( (pos + 1) < tmp.length() )
      val = trim( tmp.substr(pos + 1,  tmp.length() - (pos+1) ) );

    result = true;
  };

  return result;
};


// get key and value
bool ttutil::StringUtil::getKeyAndValue2(
				 const std::string& src, 
				 const std::string& host, 
				 const std::string& program,
				 std::string& key, std::string& val) {
    
  bool result = false;
  const std::string DELIMITER("=");
std::string host_tmp;
std::string program_tmp;

// get key and value
if (getKeyAndValue3( src, host_tmp, program_tmp, key, val)) {
    

    if (!( ( (host_tmp == "-") || (host_tmp == host) ) ) ) {
      if (false)
	std::cerr 
	  << "StringUtil::getKeyAndValue2 : "
	  << " hostname does not match host='" << host << "' "
	  << " line = '" << src << "' "
	  << std::endl;
      result = false;
    } else {
      if (!( ( (program_tmp == "-") || (program_tmp == program) ) ) ) {
	if (false)
	  std::cerr 
	    << "StringUtil::getKeyAndValue2 : "
	    << " processname does not match process='" << program << "' "
	    << " line = '" << src << "' "
	    << std::endl;
	result = false;
      } else {
	// O.K.
	result = !key.empty();
      };
    };
  } else {
    result = false; 
  };
  return result;
};



// get key and value
bool ttutil::StringUtil::getKeyAndValue3(
				 const std::string& src, 
				 std::string& host, 
				 std::string& program,
				 std::string& key, std::string& val) {
    
  bool rc = false;
  const std::string DELIMITER("=");

  host    = "";
  program = "";
  std::string tmp = trim ( removeComments( src ) );

#if !defined GPP_STL_OLD && !defined USE_OWN_ALGORITHMS 
  std::istringstream is( tmp );

  is 
    >> host >> std::ws
    >> program >> std::ws
    >> key >> std::ws;
	char c;
	val = "";
	while(is.get(c))
	{
		val += c;
	}
	val = StringUtil::trim(val);
	//std::cout << val << "\n";
	//is.getline(val);
#else
  std::string valx[5];
  unsigned int i=0;
  unsigned int j=0;
  bool is_white_space = false;
  bool was_white_space= false;
  while ( (j < 4) && (i < tmp.size()) ) {
    is_white_space = (tmp[i] == ' ') || (tmp[i] == '\t');

    if ( is_white_space ) {
      if ( !was_white_space) {
	++j; was_white_space = true;
      };
    } else {
      valx[j] += tmp[i];
      was_white_space = false;
    };
    ++i;
  };
  host = valx[0];
  program = valx[1];
  key = valx[2];
  val = valx[3];
#endif
  
  host = trim (host);
  program  = trim ( program );
  key  = trim ( key );
  val  = trim ( val );

  if (
      (!host.empty()) 
      && 
      (!program.empty()) 
      && 
      (!key.empty())
&& (!val.empty())
      ) {
    rc = true;
  } else {
    rc = false; 
  };
  return rc;
};



std::string ttutil::StringUtil::trim(const std::string& src) {
  if (src.empty()) return src;
  if (src.length() == 1 && isWhiteSpace(src[0])) return "";

  std::string result = "";

  size_t anf = 0;
  size_t end = src.length() - 1;

  while ( (anf < end) &&  isWhiteSpace( src[anf] ) ) anf++;
  while ( (anf <= end) && isWhiteSpace( src[end] ) ) end--;

  size_t len = end - anf + 1;
  if (len < 0) len = 0;

  result = src.substr(anf, len);

  return result;
};

std::string ttutil::StringUtil::removeComments(const std::string& src){
  std::string result;
  size_t anf = 0;
  size_t end = src.length();

  while ( (anf < end) &&  ( !isStartOfComment( src[anf] ) ) ) anf++;
  
  end = anf;
  anf = 0;

  size_t len = end - anf;
  if (len < 0) len = 0;

  result =  src.substr(anf, len);

  return result;
};


bool ttutil::StringUtil::isWhiteSpace(char ch) {
  return 
    '\t' == ch ||
    ' '  == ch ||
    ','  == ch
    ;
};


bool ttutil::StringUtil::isStartOfComment(char ch) {
  return '#' == ch;// || ';' == ch
// ||  '/'  == ch
    ;
};


std::string ttutil::StringUtil::formatStringBase(
					 const std::string& src, 
					 int size, 
					 bool fill_right_start, 
					 bool fill_alternating, 
					 char fill_char
					 ) {
  std::string tmp = src;
  std::string tmp2;
  bool fill_right = fill_right_start;;

  if (static_cast<int>(tmp.size()) < size) {
    // allocate memory
    if ( static_cast<int>(tmp.capacity()) < size )
         
      tmp.reserve(size+1);

    // fill with fillchar until the size is reached
    while (static_cast<int>(tmp.size()) < size) {
      if (fill_right) {
	// fill at end
	tmp += fill_char;
      } else {
	// fill at begin
	tmp2  = tmp;
	tmp   = fill_char;
	tmp  += tmp2;
      };
      if (fill_alternating) fill_right = !fill_right;
    };
  } else {
    tmp2 = tmp;
    tmp = tmp2.substr(0, size);
  };
  return tmp;
};


std::string ttutil::StringUtil::formatLeft(const std::string& src, int size, char fillchar) {
  return formatStringBase(src, size, true, false, fillchar);
};

std::string ttutil::StringUtil::formatRight(const std::string& src, int size, char fillchar) {
  return formatStringBase(src, size, false, false, fillchar);
};


std::string ttutil::StringUtil::formatCenter(const std::string& src, int size, char fillchar) {
  return formatStringBase(src, size, true, true, fillchar);
};


std::string ttutil::StringUtil::formatString(const std::string& src, int size, int direction,char fillchar) {
  std::string result;
  
  if (direction <  0) result = formatLeft(  src, size, fillchar);
  if (direction >  0) result = formatRight( src, size, fillchar);
  if (direction == 0) result = formatCenter(src, size, fillchar);
  
  return result;
};



   
/**
   * separate name of program
   */
std::string ttutil::StringUtil::getProgramName(const std::string& prgwithpath){
  char separator = '/';
   
#ifndef WINDOWS 
  separator = '\\';   
#endif
   
  size_t pos = prgwithpath.find_last_of(prgwithpath, 0);
  if (pos < static_cast<int>(prgwithpath.size())) 
    pos = static_cast<int>(prgwithpath.size());
  if (pos < 0) pos = 0;
  std::string result = prgwithpath.substr(pos); 
   
  return result;
};

 /**
  * parses arguments 
  */
std::map<std::string, std::string> ttutil::StringUtil::parseArgs(char const * const * const argv, const unsigned int argc){
  std::map<std::string, std::string> argMap;
  std::string currKey;
  std::string currValue;
  for (unsigned int i = 1; i < argc; ++i){
    const std::string as(argv[i]);
    //new key?
    if (as[0] == '-'){
      if (!currKey.empty()){
        argMap[currKey] = currValue;
      }
      currKey = as;
      currValue.clear();
    }
    //add to value
    else{
      if (!currValue.empty()) currValue += ", ";
      currValue += as;
    }
  }
  if (!currKey.empty()){
    argMap[currKey] = currValue;
  }
  return argMap;
}

/**
 * conversion from bool to string
 */
std::string ttutil::StringUtil::bool2string(bool val){
  return val ? std::string("1") : std::string("0");
}

/**
 * conversion from int to string
 */
std::string ttutil::StringUtil::int2string(long val) {

 std::string result = "";

#ifdef USE_OWN_ALGORITHMS 
  result = val >= 0 ? uint2string(val) : "-" + uint2string(-val);
#elif GPP_STL_OLD
  // needed for older compilers
  const int CHAR_SIZE= 50;
  char buff[CHAR_SIZE];
  char* pBuff = &buff[0];
  for (int i=0; i < CHAR_SIZE; i++) buff[i] = '\0';
  sprintf(buff, "%ld", val);
  while ( *pBuff++ ) result += *pBuff;
#else
  std::ostringstream os;
  os << val; 
  result = os.str();
#endif

  return result;
}

/**
   * conversion from unsigend int to string
   */
std::string ttutil::StringUtil::uint2string(unsigned long val) {

  std::string result = "";

#ifdef  USE_OWN_ALGORITHMS 
  if (val == 0) return "0";
  const int CHAR_SIZE = 20;
  char buff[CHAR_SIZE];
  unsigned int i = 0;
  while (val > 0)
  {
    buff[i] = val % 10 + 48;
    val /= 10;
    ++i;
  }
  for (unsigned int j = i; j > 0; --j){
    result += buff[j - 1];
  }
#elif GPP_STL_OLD
 result = os.str();
  // needed for older compilers
  const int CHAR_SIZE= 50;
  char buff[CHAR_SIZE];
  char* pBuff = &buff[0];
  for (int i=0; i < CHAR_SIZE; i++) buff[i] = '\0';
  sprintf(buff, "%ld", val);
  while ( *pBuff++ ) result += *pBuff;
#else
   std::ostringstream os;
   os << val; 
   result = os.str();
#endif

  return result;
}

/// conversion from long to hex string
std::string ttutil::StringUtil::uint2hexstring(unsigned long val) {
  std::string result = "";
#ifdef  USE_OWN_ALGORITHMS 
  //TODO
  return uint2string(val);

#elif GPP_STL_OLD
  // needed for older compilers
  const int CHAR_SIZE= 50;
  char buff[CHAR_SIZE];
  char* pBuff = &buff[0];
  for (int i=0; i < CHAR_SIZE; i++) buff[i] = '\0';
  sprintf(buff, "%lx", val);
  while ( *pBuff++ ) result += *pBuff;
#else
  std::ostringstream os;
  os << std::hex << val; 
  result = os.str();
#endif

  return result;
};
  
/// conversion from long given as four chars to string
std::string ttutil::StringUtil::long2charstring(unsigned long val){
  std::string result;
  char c;
  c = static_cast<char>((val & 0xff000000) >> 24);
  result += (c >= 0x20 && c <= 0x7E) ? c : '_';
  c = static_cast<char>((val & 0x00ff0000) >> 16);  
  result += (c >= 0x20 && c <= 0x7E) ? c : '_';
  c = static_cast<char>((val & 0x0000ff00) >> 8);  
  result += (c >= 0x20 && c <= 0x7E) ? c : '_';
  c = static_cast<char>((val & 0x000000ff));
  result += (c >= 0x20 && c <= 0x7E) ? c : '_';
  return result;
}

/// conversion from double to string, pos decimal places; fixedPoint=true forces trailing zeros
std::string ttutil::StringUtil::double2string(const double val, const unsigned int pos, const bool fixedPoint){
  if (fixedPoint){
    std::ostringstream os;
    os << std::fixed << std::setprecision(static_cast<int>(pos)) << val;
    return os.str();
  }
  double positiveValue = val >= 0 ? val : -val;
  for (unsigned int i = 0; i < pos; i++) positiveValue = positiveValue * 10;
  positiveValue = static_cast<double>(static_cast<int>(positiveValue + 0.5));
  for (unsigned int i = 0; i < pos; i++) positiveValue = positiveValue / 10;
  return double2string(val >= 0 ? positiveValue : -positiveValue);
}

/// conversion from double to string
std::string ttutil::StringUtil::double2string(double val) {
  std::string result = "";
  //val = 0.1;
#ifdef  USE_OWN_ALGORITHMS 
 const int CHAR_SIZE= 50;
  char buff[CHAR_SIZE];
  char* pBuff = &buff[0];
  for (int i=0; i < CHAR_SIZE; i++) buff[i] = '\0';
  sprintf_s(buff, "%8.1f", val);
  while ( *pBuff++ ) result += *pBuff;

#elif GPP_STL_OLD
  // needed for older compilers
  const int CHAR_SIZE= 50;
  char buff[CHAR_SIZE];
  char* pBuff = &buff[0];
  for (int i=0; i < CHAR_SIZE; i++) buff[i] = '\0';
  sprintf(buff, "%8.3f", val);
  while ( *pBuff++ ) result += *pBuff;
#else
  std::ostringstream os;
  os << val;
  result = os.str();
#endif
  return result;
};


/// conversion from string to bool
bool ttutil::StringUtil::string2bool(const std::string& val){
  int i = string2int(val);
  return i != 0;
}

/**
 * conversion from string to integer
 */
long ttutil::StringUtil::string2int(const std::string& val) {
  long result = 0;

#if !defined GPP_STL_OLD && !defined USE_OWN_ALGORITHMS 
  std::istringstream is( val );
  is >> result;
#else   
  char * buff = new char[val.size()+1];
  for (int i = 0; i <  static_cast<int>(val.size()); i++) buff[i] = val[i];
  buff[val.size()] = '\0';
  result = atoi(buff);
  delete[] buff;
#endif
  return result;
}


/**
  * conversion from string to integer
  */
double ttutil::StringUtil::string2double(const std::string& val) {
  double result = 0;

#if !defined GPP_STL_OLD && !defined USE_OWN_ALGORITHMS 
  std::istringstream is( val );
  is >> result;
#else   
  char * buff = new char[val.size()+1];
  for (unsigned int i = 0; i < static_cast<int>(val.size()); i++) buff[i] = val[i];
  buff[val.size()] = '\0';
  result = atof(buff);
  delete[] buff;
#endif

  return result;
}



/**
  * conversion from date to string
  * uses int2string
  */
std::string ttutil::StringUtil::date2string(const ttutil::Date& val){
	int year = 0; 
	unsigned int month = 0, day = 0, hour = 0, minute = 0, second = 0;   
	
	bool ok = val.getLocalTime(year, month, day, hour, minute, second);   
	if (!ok) return "-- invalid date --";
	
	std::string result("");
	if (day < 10) result.append("0");
	result.append(int2string(day));
	result.append(".");
	if (month < 10) result.append("0");
	result.append(int2string(month));
	result.append(".");
	result.append(int2string(year));
	result.append(" ");
	if (hour < 10) result.append("0");
	result.append(int2string(hour));
	result.append(":");
	if (minute < 10) result.append("0");
	result.append(int2string(minute));
	result.append(":");
	if (second < 10) result.append("0");
	result.append(int2string(second));
	return result;
}
  
/**
  * conversion from date to string
  * uses int2string
  */
std::string ttutil::StringUtil::date2stringDay(const ttutil::Date& val) {
  int year = 0;
  unsigned int month = 0, day = 0, hour = 0, minute = 0, second = 0;

  bool ok = val.getLocalTime(year, month, day, hour, minute, second);
  if (!ok) return "-- invalid date --";

  std::string result("");
  if (day < 10) result.append("0");
  result.append(int2string(day));
  result.append(".");
  if (month < 10) result.append("0");
  result.append(int2string(month));
  result.append(".");
  result.append(int2string(year));
  return result;
}

  
/**
  * conversion from string to date
  * we assume we have the german format ("day.month.year hour:min:sec")
  * the conversion stops at the end of the input string
  * uses string2int
  */
ttutil::Date ttutil::StringUtil::string2date(const std::string&  val){
	//parsing the string
	const std::string delims(":.|- ");
	int year = 0;
	unsigned int month = 0, day = 0, hour = 0, minute = 0, second = 0;
	std::string::size_type start = 0, ende = 0;
	
	// day
	if ( (ende = val.find_first_of(delims, start) ) == std::string::npos ) ende = val.length();
	day = string2int(val.substr(start, start - ende));
	
  // month
  if ( ende <  val.length() ){
  	start = ende + 1;
		if ( (ende = val.find_first_of( delims, start) ) == std::string::npos )  ende = val.length();
		month = string2int(val.substr(start, start - ende));
	}
	
  // year
  if ( ende <  val.length() ){
  	start = ende + 1;
		if ( (ende = val.find_first_of( delims, start) ) == std::string::npos )  ende = val.length();
		year = string2int(val.substr(start, start - ende));
	}
	
	// hour
	if ( ende <  val.length() ){
  	start = ende + 1;
		if ( (ende = val.find_first_of( delims, start) ) == std::string::npos )  ende = val.length();
		hour = string2int(val.substr(start, start - ende));
	}
	
	// minute
	if ( ende <  val.length() ){
  	start = ende + 1;
		if ( (ende = val.find_first_of( delims, start) ) == std::string::npos )  ende = val.length();
		minute = string2int(val.substr(start, start - ende));
	}
	
	// second
	if ( ende <  val.length() ){
  	start = ende + 1;
		if ( (ende = val.find_first_of( delims, start) ) == std::string::npos )  ende = val.length();
		second = string2int(val.substr(start, start - ende));
	}
	
	return ttutil::Date(year, month, day, hour, minute, second); 
} 


// splits the strSrc string into strings by delimiter cDelimiter
std::vector<std::string> ttutil::StringUtil::split(const std::string& strSrc, 
                                                 char cDelimiter,
                                                 bool emptystring,
                                                 bool totrim){

#if !defined GPP_STL_OLD && !defined USE_OWN_ALGORITHMS 
  std::istringstream is(strSrc);
#endif
	std::vector<std::string> vSplit;
	char c;
	std::string buff;

#if !defined GPP_STL_OLD && !defined USE_OWN_ALGORITHMS 
  while(is.get(c)) {
#else
  for (unsigned int i = 0; i < strSrc.size(); ++i){
    c = strSrc[i];
#endif
	
		if (cDelimiter == c) {
      if (totrim) buff = trim(buff);
      if (emptystring || !buff.empty()) vSplit.push_back(buff);
		  buff = "";
		} 
    else {
			buff += c;
		}
	}

	if (totrim) buff = trim(buff);
	if (emptystring || !buff.empty()) vSplit.push_back(buff);
	return vSplit;
}

/**
  * extendet split
  * @param str Source
  * @param strDelimiters String of delimiters
  * @return vSplit Vector of splitted elements (OUT)
  */
std::vector<std::string> ttutil::StringUtil::splitEx(const std::string& str, const std::string& strDelimiters, bool bTrim)
{
  std::vector<std::string> vSplit;
	// get first non delim
	std::string::size_type lastPos = str.find_first_not_of(strDelimiters, 0);
	// skip leading delim
	std::string::size_type pos  = str.find_first_of(strDelimiters, lastPos);

	while (std::string::npos != pos)
	  {
	    vSplit.push_back(trim(str.substr(lastPos, pos - lastPos)));
	    // Skip delimiters
	    lastPos = str.find_first_not_of(strDelimiters, pos);
	    // Find next "non-delimiter"
	    pos = str.find_first_of(strDelimiters, lastPos);
	  }
	//get the rest
	vSplit.push_back(trim(str.substr(lastPos, str.length())));
	return vSplit;
}



std::string ttutil::StringUtil::toupper(const std::string& src) {
  std::string tmp;
  for (unsigned int i=0; i < src.size(); i++) {
#ifndef _WIN32
  tmp += std::toupper( src[i]);
#else
  tmp += (src[i] >= 97 && src[i] <= 122) ? (src[i] - 32) : src[i];
#endif
  }
  return std::string(tmp);
}

std::string ttutil::StringUtil::tolower(const std::string& src) {
  std::string tmp;
  for (unsigned int i=0; i < src.size(); i++) {
#ifndef _WIN32
  tmp += std::tolower( src[i]);
#else
  tmp += (src[i] >= 65 && src[i] <= 90) ? (src[i] + 32) : src[i];
#endif
  }
  return std::string(tmp);
}


std::string ttutil::StringUtil::addSuffixAndKeepEnding(const std::string& src, const std::string& suffix) {
  const std::string::size_type pos = src.find_last_of('.');
  if (pos == std::string::npos){
    return src + suffix;
  }
  return src.substr(0, pos) + suffix + src.substr(pos);
}

std::string ttutil::StringUtil::addSuffixAndKeepNumberAndEnding(const std::string& src, const std::string& suffix) {
  const std::string::size_type pos = src.find_last_of('_');
  if (pos == std::string::npos) {
    return src + suffix;
  }
  return src.substr(0, pos) + suffix + src.substr(pos);
}

bool ttutil::StringUtil::endsWith(std::string const &fullString, std::string const &ending) {
  if (fullString.length() >= ending.length()) {
    return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
  }
  else {
    return false;
  }
}
