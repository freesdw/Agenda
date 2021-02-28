#include "AgendaService.hpp"
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
    auto filter = [userName, password](const User& user) {
        return userName == user.getName() && user.getPassword() == password;
    };
    return m_storage->queryUser(filter).size() > 0;
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
            return false;
    }
    for(int i = 0; i < participator.size(); i++) {
        for(int j = 0; j < participator.size(); j++) {
            if(j != i && participator[i] == participator[j])
                return false;
        }
    }
    if(Date::stringToDate(startDate) >= Date::stringToDate(endDate))
        return false;
    if(!(Date::isValid(Date::stringToDate(startDate)) && Date::isValid(Date::stringToDate(endDate))))
        return false;
    auto filter1 = [title](const Meeting& meeting) {
        return meeting.getTitle() == title;
    };
    if(m_storage->queryMeeting(filter1).size() > 0)
        return false;
    for(auto i : participator) {
        auto filter2 = [i](const User& user) {
            return user.getName() == i;
        };
        if(m_storage->queryUser(filter2).size() <= 0)
            return false;
    }
    bool freeSponsor = true;;
    for(auto i : listAllMeetings(userName)) {
        if(Date::stringToDate(startDate) >= i.getStartDate() && Date::stringToDate(startDate) < i.getEndDate()
            || Date::stringToDate(endDate) > i.getStartDate() && Date::stringToDate(endDate) <= i.getEndDate()
            || Date::stringToDate(startDate) <= i.getStartDate() && Date::stringToDate(endDate) >= i.getEndDate())
            freeSponsor = false;
        }
    bool freeParticipator = true;
    for(auto i : participator) {
        for(auto j : listAllMeetings(i)) {
            if(Date::stringToDate(startDate) >= j.getStartDate() && Date::stringToDate(startDate) < j.getEndDate()
                || Date::stringToDate(endDate) > j.getStartDate() && Date::stringToDate(endDate) <= j.getEndDate()
                || Date::stringToDate(startDate) <= j.getStartDate() && Date::stringToDate(endDate) >= j.getEndDate())
                    freeParticipator = false;
        }
    }
    if(!freeSponsor || !freeParticipator)
        return false;
    Meeting newMeeting(userName, participator, Date::stringToDate(startDate), Date::stringToDate(endDate), title);
    m_storage->createMeeting(newMeeting);
    return m_storage->sync();
}

bool AgendaService::addMeetingParticipator(const string& userName,
                                           const string& title,
                                           const string& participator) {
    if(participator == userName)
        return false;
    auto theMeeting = meetingQuery(userName, title);
    if(theMeeting.size() <= 0)
        return false;
    auto myMeeting = theMeeting.begin();
    vector<string> meetingParticipator = myMeeting->getParticipator();
    if(myMeeting->isParticipator(participator))
        return false;
    auto filter = [participator](const User& user) {
        return user.getName() == participator;
    };
    if(m_storage->queryUser(filter).size() <= 0)
        return false;
    Date startDate = myMeeting->getStartDate();
    Date endDate = myMeeting->getEndDate();
    bool freeParticipator = true;
    for(auto i : listAllMeetings(participator)) {
        if(startDate >= i.getStartDate() && startDate < i.getEndDate()
            || endDate > i.getStartDate() && endDate <= i.getEndDate()
            || startDate <= i.getStartDate() && endDate >= i.getEndDate())
                freeParticipator = false;
    }
    if(!freeParticipator)
        return false;
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
        return false;
    auto myMeeting = meeting.begin();
    vector<string> allParticipators = myMeeting->getParticipator();
    bool hasA = myMeeting->isParticipator(participator);
    if(!hasA)
        return false;
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
        return false;
    if(deleteMeeting(userName, title)) {
        return true;
    }
    auto filter = [userName, title](const Meeting& meeting) {
        bool hasA = meeting.isParticipator(userName);
        return (hasA && meeting.getTitle() == title);
    };
    auto switcher = [userName](Meeting& meeting) {
        vector<string> temp = meeting.getParticipator();
        for(int i = 0; i < temp.size(); i++) {
            if(temp[i] == userName) {
                temp.erase(temp.begin() + i);
                break;
            }
        }
        meeting.setParticipator(temp);
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
        return temp;
    }
    if(!Date::isValid(Date::stringToDate(startDate)) || !Date::isValid(Date::stringToDate(endDate))) 
        return temp;
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
        // for(auto i : meeting.getParticipator()) {
        //     if(i == userName)
        //         hasA = true;
        // }
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
    return m_storage->sync();
}

void AgendaService::startAgenda(void) {
    m_storage = Storage::getInstance();
}

void AgendaService::quitAgenda(void) {
    m_storage->sync();
}