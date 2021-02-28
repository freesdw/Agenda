#include "AgendaService.hpp"
#include "myException.h"
#include "Meeting.hpp"
#include <list>
#include <string>
using namespace std;

AgendaService::AgendaService() {
    startAgenda();
}

AgendaService::~AgendaService() {
    quitAgenda();
}

bool AgendaService::userLogIn(const string& userName, const string& password) {
    auto filter1 = [userName](const User& user) {
        return userName == user.getName();
    };
    if(m_storage->queryUser(filter1).size() <= 0) {
        throw myException("The user doesn't exit!");
    }
    auto filter = [userName, password](const User& user) {
        return userName == user.getName() && user.getPassword() == password;
    };
    if(m_storage->queryUser(filter).size() <= 0) {
        throw myException("Wrong password!");
    }
    return true;
}

bool AgendaService::userRegister(const string& userName, const string& password,
                                 const string& email, const string& phone) {
    auto filter = [userName](const User& user) {
        return user.getName() == userName;
    };
    if(m_storage->queryUser(filter).size() == 0) {
        User newUser(userName, password, email, phone);
        m_storage->createUser(newUser);
        return m_storage->sync();
    }
    return false;
}

bool AgendaService::deleteUser(const string& userName, const string& password) {
    auto filter = [userName, password](const User& user) {
        return userName == user.getName() && user.getPassword() == password;
    };
    if(m_storage->deleteUser(filter) > 0) {
        for(auto i : listAllParticipateMeetings(userName)) {
            string title = i.getTitle();
            auto filter = [title](const Meeting& meetings) {
                return meetings.getTitle() == title;
            };
            auto switcher = [userName](Meeting& meetings) {
                meetings.removeParticipator(userName);
            };
            m_storage->updateMeeting(filter, switcher);
        }
        deleteAllMeetings(userName);
        return m_storage->sync();
    } else return false;
}

list<User> AgendaService::listAllUsers(void) const {
    auto filter = [](const User& user) {
        return true;
    };
    return m_storage->queryUser(filter);
}

bool AgendaService::createMeeting(const string& userName, const string& title,
                                  const string& startDate, const string& endDate,
                                  const vector<string>& participator) {
    for(auto i : participator) {
        if(i == userName)
            throw myException("The participators inputed shouldn't include sponsor!");
    }
    for(int i = 0; i < participator.size(); i++) {
        string temp = participator[i];
        auto filter = [temp](const User& user) {
            return user.getName() == temp;
        };
        if(m_storage->queryUser(filter).size() <= 0) {
            string str;
            str = "\"" + temp + "\" are not the user of Agenda!";
            throw myException(str);
        }
        for(int j = 0; j < participator.size(); j++) {
            if(j != i && participator[i] == participator[j])
                throw myException("Some participators are repeated!");
        }
    }
    if(Date::stringToDate(startDate) >= Date::stringToDate(endDate))
        throw myException("The date is invalid!");
    if(!(Date::isValid(Date::stringToDate(startDate)) && Date::isValid(Date::stringToDate(endDate))))
        throw myException("The date is invalid!");
    auto filter1 = [title](const Meeting& meeting) {
        return meeting.getTitle() == title;
    };
    if(m_storage->queryMeeting(filter1).size() > 0)
        throw myException("The meeting's name has been used!");
    for(auto i : participator) {
        auto filter2 = [i](const User& user) {
            return user.getName() == i;
        };
        if(m_storage->queryUser(filter2).size() <= 0)
            throw myException("Some of the participators are not exist!");
    }
    bool freeSponsor = true;;
    for(auto i : listAllMeetings(userName)) {
        if(Date::stringToDate(startDate) >= i.getStartDate() && Date::stringToDate(startDate) < i.getEndDate()
            || Date::stringToDate(endDate) > i.getStartDate() && Date::stringToDate(endDate) <= i.getEndDate()
            || Date::stringToDate(startDate) <= i.getStartDate() && Date::stringToDate(endDate) >= i.getEndDate())
            freeSponsor = false;
        }
    if(!freeSponsor)
        throw myException("The sponsor is busy at that time!");
    bool freeParticipator = true;
    for(auto i : participator) {
        for(auto j : listAllMeetings(i)) {
            if(Date::stringToDate(startDate) >= j.getStartDate() && Date::stringToDate(startDate) < j.getEndDate()
                || Date::stringToDate(endDate) > j.getStartDate() && Date::stringToDate(endDate) <= j.getEndDate()
                || Date::stringToDate(startDate) <= j.getStartDate() && Date::stringToDate(endDate) >= j.getEndDate())
                    freeParticipator = false;
        }
    }
    if(!freeParticipator)
        throw myException("Some of the participators are busy at that time!");
    Meeting newMeeting(userName, participator, Date::stringToDate(startDate), Date::stringToDate(endDate), title);
    m_storage->createMeeting(newMeeting);
    return m_storage->sync();
}

bool AgendaService::addMeetingParticipator(const string& userName,
                                           const string& title,
                                           const string& participator) {
    if(participator == userName)
        throw myException("The new participator shouldn't be the sponsor!");
    auto theMeeting = meetingQuery(userName, title);
    if(theMeeting.size() <= 0)
        throw myException("The meeting isn't exist!");
    auto myMeeting = theMeeting.begin();
    auto filter = [participator](const User& user) {
        return user.getName() == participator;
    };
    if(m_storage->queryUser(filter).size() <= 0)
        throw myException("The participator isn't user of Agenda!");
    vector<string> meetingParticipator = myMeeting->getParticipator();
    if(myMeeting->isParticipator(participator))
        throw myException("This participator inputed would have been one of the participators!");
    bool freeParticipator = true;
    for(auto i : listAllMeetings(participator)) {
        if(i.getStartDate() >= myMeeting->getEndDate() || i.getEndDate() <= myMeeting->getStartDate())
            continue;
        else freeParticipator = false;
    }
    if(!freeParticipator) {
        string str = "\"" + participator + "\" is busy at that time!";
        throw myException(str); 
    }
    auto filter1 = [title, userName](const Meeting& meeting) {
        return (meeting.getTitle() == title && meeting.getSponsor() == userName);
    };
    auto switcher = [participator](Meeting& meeting) {
        meeting.addParticipator(participator);
    };
    m_storage->updateMeeting(filter1, switcher);
    return m_storage->sync();
}

bool AgendaService::removeMeetingParticipator(const string& userName,
                                              const string& title,
                                              const string& participator) {
    auto meeting = meetingQuery(userName, title);
    if(meeting.size() <= 0)
        throw myException("The meeting isn't exist!");
    auto myMeeting = meeting.begin();
    vector<string> allParticipators = myMeeting->getParticipator();
    bool hasA = myMeeting->isParticipator(participator);

    if(!hasA)
        throw myException("The name inputed isn't one of the participators!");
    auto filter = [title](const Meeting& meetings) {
        return meetings.getTitle() == title;
    };
    auto switcher = [participator](Meeting& meetings) {
        meetings.removeParticipator(participator);
    };
    m_storage->updateMeeting(filter, switcher);
    auto filter1 = [title](const Meeting& meeting) {
        return (meeting.getTitle() == title && meeting.getParticipator().size() == 0);
    };
    m_storage->deleteMeeting(filter1);
    return m_storage->sync();
}          

bool AgendaService::quitMeeting(const string& userName, const string& title) {
    if(meetingQuery(userName, title).size() <= 0)
        throw myException("You do not participate in this meeting!");
    if(deleteMeeting(userName, title)) {
        return true;
    }
    auto filter = [userName, title](const Meeting& meeting) {
        bool hasA = meeting.isParticipator(userName);
        return (hasA && meeting.getTitle() == title);
    };
    auto switcher = [userName](Meeting& meeting) {
        meeting.removeParticipator(userName);
    };
    m_storage->updateMeeting(filter, switcher);
    auto filter1 = [title](const Meeting& meeting) {
        return (meeting.getTitle() == title && meeting.getParticipator().size() == 0);
    };
    m_storage->deleteMeeting(filter1);
    return m_storage->sync();
}

list<Meeting> AgendaService::meetingQuery(const string& userName, const string& title) const {
    auto filter = [userName, title](const Meeting& meeting) {
        bool hasA = meeting.isParticipator(userName);
        return ((hasA || userName == meeting.getSponsor()) && title == meeting.getTitle());
    };
    return m_storage->queryMeeting(filter);
}

list<Meeting> AgendaService::meetingQuery(const string& userName, 
                                          const string& startDate,
                                          const string& endDate) const {
    list<Meeting> temp;
    if(Date::stringToDate(startDate) > Date::stringToDate(endDate)) {
        throw myException("The date is invalid!");
    }
    if(!Date::isValid(Date::stringToDate(startDate)) || !Date::isValid(Date::stringToDate(endDate))) 
        throw myException("The date is invalid!");
    auto filter = [userName, startDate, endDate](const Meeting& meeting) {
        bool hasA = meeting.isParticipator(userName);
        return ((hasA || userName == meeting.getSponsor()) && 
                ((meeting.getStartDate() <= Date::stringToDate(endDate) && meeting.getStartDate() >= Date::stringToDate(startDate)) ||
                (meeting.getEndDate() <= Date::stringToDate(endDate) && meeting.getEndDate() >= Date::stringToDate(startDate)) ||
                (meeting.getStartDate() <= Date::stringToDate(startDate) && meeting.getEndDate() >= Date::stringToDate(endDate)))
                );
    };
    return m_storage->queryMeeting(filter);
}

list<Meeting> AgendaService::listAllMeetings(const string& userName) const {
    auto filter = [userName](const Meeting& meeting) {
        bool hasA = meeting.isParticipator(userName);
        return (hasA || userName == meeting.getSponsor());
    };
    return m_storage->queryMeeting(filter);
}

list<Meeting> AgendaService::listAllSponsorMeetings(const string& userName) const {
    auto filter = [userName](const Meeting& meeting) {
        return meeting.getSponsor() == userName;
    };
    return m_storage->queryMeeting(filter);
}

list<Meeting> AgendaService::listAllParticipateMeetings(const string& userName) const {
    auto filter = [userName](const Meeting& meeting) {
        bool hasA = meeting.isParticipator(userName);
        return hasA;
    };
    return m_storage->queryMeeting(filter);
}

bool AgendaService::deleteMeeting(const string& userName, const string& title) {
    auto meetings = listAllSponsorMeetings(userName);
    if(meetings.size() <= 0)
        throw myException("You are not the sponsor of this meeting or the meeting isn't exist!");
    auto filter = [userName, title](const Meeting& meeting) {
        return (meeting.getSponsor() == userName && meeting.getTitle() == title);
    };
    m_storage->deleteMeeting(filter);
    return m_storage->sync();
}

bool AgendaService::deleteAllMeetings(const string& userName) {
    auto filter = [userName](const Meeting& meeting) {
        return (meeting.getSponsor() == userName);
    };
    m_storage->deleteMeeting(filter);
    m_storage->sync();
    return true;
}

void AgendaService::startAgenda(void) {
    m_storage = Storage::getInstance();
}

void AgendaService::quitAgenda(void) {
    m_storage->sync();
}

bool AgendaService::changeInformation(const string& userName, const string& password, const string& email, const string& phone) {
   // deleteUser(userName, password);
   auto filter1 = [userName, password](const User& user) {
        return userName == user.getName() && user.getPassword() == password;
    };
    m_storage->deleteUser(filter1);

    return userRegister(userName, password, email, phone);
}

bool AgendaService::changePassword(const string& userName, const string& oldPassword, const string& newPassword) {
    auto filter = [userName](const User& user) {
        return user.getName() == userName;
    };
    auto temp = m_storage->queryUser(filter).begin();
    string email = temp->getEmail();
    string phone = temp->getPhone();
    //deleteUser(userName, oldPassword);
    auto filter1 = [userName, oldPassword](const User& user) {
        return userName == user.getName() && user.getPassword() == oldPassword;
    };
    m_storage->deleteUser(filter1);
    
    return userRegister(userName, newPassword, email, phone);
}