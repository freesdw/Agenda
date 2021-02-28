#include "AgendaService.hpp"
AgendaService::AgendaService() {
  m_storage = nullptr;
  startAgenda();
}
AgendaService::~AgendaService() {
}

bool AgendaService::userLogIn(const std::string &userName, const std::string &password) {
  std::list<User> userList = listAllUsers();
  for(auto val : userList) {
    if(val.getName() == userName && val.getPassword() == password)
      return true;
  }
  return false;
}
bool AgendaService::userRegister(const std::string &userName, const std::string &password,
                  const std::string &email, const std::string &phone) {
  std::list<User> userList = listAllUsers();
  for(auto val : userList){
    if(val.getName()==userName)
      return false;
  }
  if(userName.size()==0 || password.size()==0 || email.size()==0 || phone.size()==0)
    return false;

  User user(userName,password,email,phone);
  m_storage->createUser(user);
  return true;
}
bool AgendaService::deleteUser(const std::string &userName, const std::string &password) {
  int i = m_storage->deleteUser([&](const User& user) {
    if(user.getName()==userName && user.getPassword()== password) 
      return true;
    return false;
  });
  if(i) {
    m_storage->deleteMeeting([&](const Meeting& meeting) {
      return meeting.getSponsor()==userName;
    });
    std::list<Meeting> meeting_p = listAllParticipateMeetings(userName);
    for(auto val : meeting_p) {
      std::string title(val.getTitle());
      quitMeeting(userName,title);
    }
    return true;
  }
  return false;
}
std::list<User> AgendaService::listAllUsers(void) const {
  std::list<User> userList = m_storage->queryUser([](const User& user) {
    return true;
  });
  return userList;
}

bool AgendaService::createMeeting(const std::string &userName, const std::string &title,
                   const std::string &startDate, const std::string &endDate,
                   const std::vector<std::string> &participator) {
  if(participator.size()==0)
    return false;
  std::list<User> userList = listAllUsers();
  int flag = 1;
  for(auto val : userList) {
    if(val.getName()==userName)
      flag = 0;
  }
  if(flag)
    return false;
  for(int i = 0; i < participator.size(); i++) {
    if(participator[i] == userName)
      return false;
    int isExit = 0;
    for(auto val : userList) {
      if(val.getName()==participator[i])   
        isExit = 1;
    }
    if(!isExit)
      return false;
    for(int j = 0; j < participator.size(); j++) {
      if(j != i && participator[i]==participator[j])
        return false;
    }
  }
  Date beginDate(startDate);
  Date finishDate(endDate);
  if(!(Date::isValid(beginDate)) || !(Date::isValid(finishDate)))
    return false;
  if(finishDate <= beginDate)
    return false;
  std::list<Meeting> meetingList = m_storage->queryMeeting([&](const Meeting& meeting) {
    if(meeting.getTitle()==title)
      return true;
    return false;
  });
  if(meetingList.size()>0)
    return false;
  std::list<Meeting> meeting_s = m_storage->queryMeeting([&](const Meeting& meeting) {
    if(meeting.getSponsor()==userName||meeting.isParticipator(userName)) {
      if(meeting.getStartDate()>beginDate&&finishDate>meeting.getStartDate()||meeting.getEndDate()>beginDate&&finishDate>meeting.getEndDate()||
        meeting.getStartDate()<startDate&&finishDate<meeting.getEndDate() || meeting.getStartDate()==beginDate || finishDate==meeting.getEndDate())
        return true;
    }
    return false;
  });
  if(meeting_s.size()>0)
    return false;
  for(auto val : participator) {
      std::list<Meeting> meeting_p = m_storage->queryMeeting([&](const Meeting& meeting) {
    if(meeting.getSponsor()==val||meeting.isParticipator(val)) {
      if(meeting.getStartDate()>beginDate&&finishDate>meeting.getStartDate()||meeting.getEndDate()>beginDate&&finishDate>meeting.getEndDate()||
        meeting.getStartDate()<startDate&&finishDate<meeting.getEndDate() || meeting.getStartDate()==beginDate || finishDate==meeting.getEndDate())
        return true;
    }
    return false;
  });
  if(meeting_p.size()>0)
    return false;
  } 
  m_storage->createMeeting(Meeting(userName,participator,startDate,endDate,title));
  return true;
}
bool AgendaService::addMeetingParticipator(const std::string &userName,
                            const std::string &title,
                            const std::string &participator) {
  std::list<User> userList = listAllUsers();
  int flag = 1;
  for(auto val : userList) {
    if(val.getName()==participator)
      flag = 0;
  }
  if(flag)
    return false;
  std::list<Meeting> meeting = meetingQuery(userName,title);
  if(meeting.size() != 1)
    return false;
  Date startDate = meeting.front().getStartDate();
  Date endDate = meeting.front().getEndDate();
  std::list<Meeting> meetings = listAllMeetings(participator);
  for(auto val : meetings) {
  if(val.getStartDate()>startDate&&endDate>val.getStartDate()||val.getEndDate()>startDate&&endDate>val.getEndDate()||
        val.getStartDate()<startDate&&endDate<val.getEndDate() || val.getStartDate()==startDate || endDate==val.getEndDate())
      return false;
  }
  m_storage->updateMeeting([&](const Meeting& meeting) {
    if(meeting.getSponsor()==userName && meeting.getTitle()==title)
      return true;
    return false;
  },[&](Meeting& meeting) {
    meeting.addParticipator(participator);
  });
  return true;
}
bool AgendaService::removeMeetingParticipator(const std::string &userName,
                               const std::string &title,
                               const std::string &participator) {
  int flag = m_storage->updateMeeting([&](const Meeting& meeting) {
    if((meeting.getSponsor()==userName&&meeting.isParticipator(participator))&&meeting.getTitle()==title)
      return true;
    return false;
  },[&](Meeting& meeting) {
    meeting.removeParticipator(participator);
  });
  int isValid = m_storage->deleteMeeting([&](const Meeting& meeting) {
    if(meeting.getParticipator().size()==0)
      return true;
    return false;
  });
  return isValid&&flag;
}

bool AgendaService::quitMeeting(const std::string &userName, const std::string &title) {
  int flag =  m_storage->updateMeeting([&](const Meeting& meeting) {
    if(meeting.getTitle()==title && meeting.isParticipator(userName))
      return true;
    return false;
  },[&](Meeting& meeting) {
    meeting.removeParticipator(userName);
  });
  int isValid = m_storage->deleteMeeting([&](const Meeting& meeting) {
    if(meeting.getParticipator().size()==0)
      return true;
    return false;
  });
  return isValid&&flag;
}

std::list<Meeting> AgendaService::meetingQuery(const std::string &userName,
                                const std::string &title) const {
  std::list<Meeting> meetings = m_storage->queryMeeting([&](const Meeting& meeting) {
    if((meeting.getSponsor()==userName|| meeting.isParticipator(userName))&&meeting.getTitle()==title )
      return true;
    return false;
  });
  return meetings;
}

std::list<Meeting> AgendaService::meetingQuery(const std::string &userName,
                                const std::string &startDate,
                                const std::string &endDate) const {
  Date beginDate(startDate),finishDate(endDate);
  if(!(Date::isValid(beginDate)) || !(Date::isValid(finishDate))) {
    std::list<Meeting> temp;
    return temp;
  }
  if (beginDate > finishDate) {
    std::list<Meeting> temp;
    return temp;
  }
  std::list<Meeting> select_meeting;
  std::list<Meeting> meetings = listAllMeetings(userName);
  for(auto val : meetings) {
    if(Date(startDate) > val.getStartDate() && Date(startDate) < val.getEndDate()||
     Date(endDate) > val.getStartDate() && Date(endDate) < val.getEndDate() ||
      Date(startDate) < val.getStartDate() && Date(endDate) > val.getEndDate() || Date(startDate)==val.getStartDate()||Date(endDate)==val.getEndDate()
      ||beginDate==val.getEndDate()||finishDate==val.getStartDate())
      select_meeting.push_back(val);
  };
  return select_meeting;
}

std::list<Meeting> AgendaService::listAllMeetings(const std::string &userName) const {
  std::list<Meeting> meetings = m_storage->queryMeeting([&](const Meeting& meeting) {
    if(meeting.isParticipator(userName)||meeting.getSponsor()==userName)
      return true;
    return false;
  });
  return meetings;
}

std::list<Meeting> AgendaService::listAllSponsorMeetings(const std::string &userName) const {
  std::list<Meeting> meetings = m_storage->queryMeeting([&](const Meeting& meeting) {
    if(meeting.getSponsor()==userName)
      return true;
    return false;
  });
  return meetings;
}

std::list<Meeting> AgendaService::listAllParticipateMeetings(
    const std::string &userName) const {
  std::list<Meeting> meetings = m_storage->queryMeeting([&](const Meeting& meeting) {
    if(meeting.isParticipator(userName))
      return true;
    return false;
  });
  return meetings;
}

bool AgendaService::deleteMeeting(const std::string &userName, const std::string &title) {
   return m_storage->deleteMeeting([&](const Meeting& meeting) {
    if(meeting.getSponsor()==userName && meeting.getTitle()==title)
      return true;
    return false;
  });
}

bool AgendaService::deleteAllMeetings(const std::string &userName) {
  int delete_num = m_storage->deleteMeeting([&](const Meeting& meeting) {
    if(meeting.getSponsor()==userName)
      return true;
    return false;
  });
  return delete_num > 0;
}

void AgendaService::startAgenda(void) {
  m_storage = Storage::getInstance();
}

void AgendaService::quitAgenda(void) {

}
