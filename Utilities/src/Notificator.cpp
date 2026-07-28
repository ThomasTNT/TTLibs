#include "Notificator.h"

//**********************************************************************
//*                             Notificator                            *
//*                                                                    *
//*           class for complex error and information handling         *
//*                                                                    *
//*                           used as singleton                        *
//**********************************************************************

enum NotificationType{
  UNDEF         = 0,  //mainly a placeholder for "all" 
  DETAILED_INFO = 1,  //simple informations which are not importend, just for info (hidable)
  INFORMATION   = 2,  //important information, that the user has to know (hidable)
  WARNING       = 3,  //something may went wrong or was missing, but can be corrected (->default) 
  ERROR         = 4,  //something went defenitely wrong, but does not effect the other 
  HARD_ERROR    = 5,  //function failed completely!
  FATAL_ERROR   = 6   //application has to be stopped
};

//*******************************************
// 1. constructor of Notification
//
// this class represents notification entries
//*******************************************
ttutil::Notification::Notification(const NotificationType type, 
                                 const std::string& code,
                                 const std::string& msg,
                                 const std::string& info)
                    :Type(type), Code(code), Msg(msg), Info(info)
{
}

//*******************************************
// 2. constructor of Notification
//
// this class represents notification entries
//*******************************************
ttutil::Notification::Notification(const NotificationType type, const std::string& info)
                     :Type(type), Code(""), Msg(""), Info(info)
{
}

//*******************************************
// operator <<
//
// "print operator" for Notification
//*******************************************
std::ostream& ttutil::operator << (std::ostream& out, const ttutil::Notification& notification)
{
  switch (notification.Type){
    case UNDEF:         out << "UNDEF"; break;
    case DETAILED_INFO: out << "DETAILED_INFO"; break;
    case INFORMATION:   out << "INFORMATION"; break;
    case WARNING:       out << "WARNING";  break;
    case NORMAL_ERROR:  out << "ERROR"; break;
    case HARD_ERROR:    out << "HARD_ERROR"; break;
    case FATAL_ERROR:   out << "FATAL_ERROR"; break;
  }
  out << " "; 
  out << (notification.Code.length() > 0 ? notification.Code : "---");
  out << " "; 
  out << (notification.Msg.length() > 0 ? notification.Msg : "---");
  out << " "; 
  out << (notification.Info.length() > 0 ? notification.Info : "---");
  return out;
}

//*******************************************
// operator +
//
// "print operator" for strings
//*******************************************
std::string& operator + (std::string& s, const ttutil::Notification& notification)
{
  switch (notification.Type){
    case ttutil::UNDEF:         s += "UNDEF"; break;
    case ttutil::DETAILED_INFO: s += "DETAILED_INFO"; break;
    case ttutil::INFORMATION:   s += "INFORMATION"; break;
    case ttutil::WARNING:       s += "WARNING";  break;
    case ttutil::NORMAL_ERROR:  s += "ERROR"; break;
    case ttutil::HARD_ERROR:    s += "HARD_ERROR"; break;
    case ttutil::FATAL_ERROR:   s += "FATAL_ERROR"; break;
  }
  s +=  " "; 
  s +=  (notification.Code.length() > 0 ? notification.Code : "---");
  s +=  " "; 
  s +=  (notification.Msg.length() > 0 ? notification.Msg : "---");
  s +=  " "; 
  s +=  (notification.Info.length() > 0 ? notification.Info : "---");
  return s;
}

//*******************
// singleton instance
//*******************
ttutil::Notificator* ttutil::Notificator::notificator = NULL;

//***********************************
// private constructor of Notificator
//***********************************
ttutil::Notificator::Notificator(){}

//********************
// GetInstance
//
// get instance method
//********************
ttutil::Notificator& ttutil::Notificator::GetInstance(){
  if (!notificator) notificator = new Notificator();
  return *notificator;
}

//************************
// Clear
//
// clears the notification
//************************
void ttutil::Notificator::Clear(){
  Notifications.clear();
}

//********************
// AddNotification
//
// adds a notification
//********************
void ttutil::Notificator::AddNotification(const Notification notification){
  Notifications.push_back(notification);
}
    
//******************************************************
// GetNotifications
// 
// returns the notifications, equal and higher than type
//******************************************************
 const std::vector<ttutil::Notification> ttutil::Notificator::GetNotifications(const NotificationType type) const{
  std::vector<ttutil::Notification> result;
  for (unsigned int i = 0; i < Notifications.size(); ++i){
    if (TypeEqualOrHigher(type, Notifications[i].Type, false)){
      result.push_back(Notifications[i]);
    }
  }
  return result;
}
    
//**********************************************
// GetNumber
// 
// returns the numbers of notifications
// equal and higher than type or exact this type
//**********************************************
unsigned int ttutil::Notificator::GetNumber(const NotificationType type, const bool exact) const{
  unsigned int number = 0;
  for (unsigned int i = 0; i < Notifications.size(); ++i){
    if (TypeEqualOrHigher(type, Notifications[i].Type, exact)){
      ++number;
    }
  }
  return number;
}

//*******************************
// GetHighLevel
// 
// returns the highes error level
//*******************************
ttutil::NotificationType ttutil::Notificator::GetHighLevel() const{
  NotificationType type = UNDEF;
  for (unsigned int i = 0; i < Notifications.size(); ++i){
    if (Notifications[i].Type > type){
      type = Notifications[i].Type; 
    }
  }
  return type;
}

//**********************************************
// TypeEqualOrHigher
//
// HELP: checks if type2 is equal or higher type1
//**********************************************
bool ttutil::Notificator::TypeEqualOrHigher(const NotificationType type1, const NotificationType type2, const bool exact) const{
  if (type1 == type2) return true; 
  if (exact) return type1 == type2;
  if ((type1 == UNDEF && type2 != UNDEF) || (type1 != UNDEF && type2 == UNDEF)) return false;
  return type2 >= type1;
}
