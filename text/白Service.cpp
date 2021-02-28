bool AgendaService::createMeeting(const std::string &userName, const std::string &title,
                   const std::string &startDate, const std::string &endDate,
                   const std::vector<std::string> &participator) {
  std::list<User> userList = listAllUsers();
  int flag = 0;
  for(auto val:userList) {
    if(val.getName()==userName)
      flag = 1;
  }
  if(!flag)
    return false;
  for(int i = 0; i < participator.size(); i++) {
    int isExit = 0;
    for(auto val : userList) {
      if(val.getName()==participator[i])
        isExit = 1;
    }
    if(!isExit)
      return false;
  }
  Date beginDate(startDate);
  Date finishDate(endDate);
  if((!Date::isValid(beginDate)) || (!Date::isValid(finishDate)))
    return false;
  if(finishDate <= beginDate)
    return false;
  std::list<Meeting> meetingList = m_storage->queryMeeting([](const Meeting& meeting) {
    return true;
  });
  for(auto val : meetingList) {
    if(val.getTitle()==title)
      return false;
    if((beginDate>=val.getStartDate()&&beginDate<val.getEndDate())||(finishDate>val.getStartDate()&&finishDate<=val.getEndDate())||(beginDate<=val.getStartDate()&&finishDate>=val.getEndDate()))
      return false;
  }
  m_storage->createMeeting(Meeting(userName,participator,startDate,endDate,title));
  return true;
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
/**
 * search a meeting by username, time interval
 * @param uesrName all the users's userName
 * @param startDate time interval's start date
 * @param endDate time interval's end date
 * @return a meeting list result
 */
std::list<Meeting> AgendaService::meetingQuery(const std::string &userName,
                                const std::string &startDate,
                                const std::string &endDate) const {
  Date beginDate(startDate),finishDate(endDate);
  if(!Date::isValid(beginDate) || !Date::isValid(finishDate)) {
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
    if(Date(startDate) >= val.getStartDate() && Date(startDate) <= val.getEndDate()||
     Date(endDate) >= val.getStartDate() && Date(endDate) <= val.getEndDate() ||
      Date(startDate) <= val.getStartDate() && Date(endDate) >= val.getEndDate())
      select_meeting.push_back(val);
  };
  return select_meeting;
}