#ifndef __NOTIFICATOR_H__
#define __NOTIFICATOR_H__

#include <string>
#include <vector>

namespace ttutil{

//**********************************************************************
//*                             Notificator                            *
//*                                                                    *
//*           class for complex error and information handling         *
//*                                                                    *
//*                           used as singleton                        *
//**********************************************************************

enum NotificationType{
  UNDEF         = 0,   //mainly a placeholder for "all" 
  TRACE_INFO    = 1,   //more for tracing
  DETAILED_INFO = 2,   //simple informations which are not importend, just for info (hidable)
  INFORMATION   = 3,   //important information, that the user has to know (hidable)
  WARNING       = 4,   //something may went wrong or was missing, but can be corrected (->default) 
  NORMAL_ERROR  = 5,   //something went defenitely wrong, but does not effect the other 
  HARD_ERROR    = 6,   //function failed completely!
  FATAL_ERROR   = 7    //application has to be stopped
};


//this class represents notification entries
struct Notification{
  
  Notification(const NotificationType type, 
               const std::string& code,
               const std::string& msg,
               const std::string& info);

  Notification(const NotificationType type, const std::string& info);

  NotificationType Type;
  std::string Code; //e.g. error code
  std::string Msg;  //a short description
  std::string Info; //further informations
};

std::ostream& operator << (std::ostream& out, const Notification& notification);

//Notificator
class Notificator{
  
  public:

    // get instance method
    static Notificator& GetInstance();

    // clears the notification
    void Clear();

    // adds a notification
    void AddNotification(const Notification notification);
    
    // returns the notifications, equal and higher than type
    const std::vector<Notification> GetNotifications(const NotificationType type = UNDEF) const;
    
    // returns the numbers of notifications
    // equal and higher than type or exact this type
    unsigned int GetNumber(const NotificationType type = UNDEF, const bool exact = false) const;

    // returns the highes error level
    NotificationType GetHighLevel() const;

  private:

    // private constructor
    Notificator();

    // HELP: checks if type2 is equal or higher type1
    bool TypeEqualOrHigher(const NotificationType type1, const NotificationType type2, const bool exact) const;
    
    // reference to the singleton instance
    static Notificator* notificator;

    //list of sotifications
    std::vector<Notification> Notifications;

};


} //end of namespace util

std::string& operator + (std::string& s, const ttutil::Notification& notification);

#endif