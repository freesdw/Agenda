#include "Storage.hpp"
#include "Path.hpp"
#include <string>
#include <fstream>
#include <list>
using namespace std;

shared_ptr<Storage> m_instance = Storage();

Storage::Storage() {
    m_dirty = false;
}

bool Storage::readFromFile(void) {
    ifstream loadUser;
    loadUser.open(Path::userPath);
    if(!loadUser)
        return false;
    string per;
    while(getline(loadUser, per)) {
        int index1 = find_first_of(per, ',', 0);
        string t_name, t_password, t_email, t_phone;
        for(int i = 1; i < index1 - 1; i++)
            t_name.push_back(per[i]);
        int index2 = find_first_of(per, ',', index1 + 1);
        for(int i = index1 + 2; i < index2 - 1; i++)
            t_password.push_back(per[i]);
        index1 = find_first_of(per, ',', index2 + 1);
        for(int i = index2 + 2; i < index1 - 1; i++)
            t_email.push_back(per[i]);
        for(int i = index1 + 2; i < per.size() - 1; i++) 
            t_phone.push_back(per[i]);
        User user(t_name, t_password, t_email, t_phone);
        m_userList.push_back(user);
    }
    loadUser.close();

    ifstream loadMeeting;
    loadMeeting.open(Path::meetingPath);
    if(!loadMeeting)
        return false;
    while(getline(loadMeeting, per)) {
        string t_sponsor, t_startDate, t_endDate, t_title;
        int index1 = find_first_of(per, ',', 0);
        for(int i = 1; i < index1 -1; i++) 
            t_sponsor.push_back(per[i]);
        int index2 = find_first_of(per, ',', index1 + 1);
        vector<string> t_participator;
        string name;
        for(int i = index1 + 2; i < index2 - 1; i++) {
            if(per[i] != '&')
                name.push_back(per[i]);
            else {
                t_participator.push_back(name);
                name = "";
            }
        }
        index1 = find_first_of(per, ',', index2 + 1);
        for(int i = index2 + 2; i < index1 - 1; i++) 
            t_startDate.push_back(per[i]);
        index2 = find_first_of(per, ',', index1 + 1);
        for(int i = index1 + 2; i < index2 - 1; i++)
            t_endDate.push_back(per[i]);
        for(int i = index2 + 2; i < per.size() - 1; i++) 
            t_title.push_back(per[i]);
        Meeting meeting(t_sponsor, t_participator, t_startDate, t_endDate, t_title);
        m_meetingList.push_back(meeting);
    }
    return true;
}

bool Storage::writeToFile(void) {
    ofstream saveUser;
    saveUser.open(Path::userPath);
    if(!saveUser)
        return false;
    for(auto i : m_userList) {
        saveUser << "\"" << i->getName() << "\",\"" << i->getPassword() << "\",\"" << i->getEmail() << "\",\"" << i->getPhone() << "\"";
        if(i != m_userList.end() - 1)
            saveUser << endl;
    }
    saveUser.close();

    ofstream saveMeeting;
    saveMeeting.open(Path::meetingPath);
    if(!saveMeeting)
        return false;
    for(auto i : m_meetingList) {
        saveMeeting << "\"" << i->getSponsor() << "\",\"";
        auto temp = i->getParticipator();
        for(auto j : temp) {
            saveMeeting << j->getName();
            if(j != temp.end() - 1)
                saveMeeting << '&';
        }
        saveMeeting << "\",\"" << i->getStartDate() << "\",\"" <<  i->getEndDate() << "\",\"" << i->getTitle() << "\"";
        if(i != m_meetingList.end())
            saveMeeting << endl;
    }
    saveMeeting.close();
    return true;
}

shared_ptr<Storage> Storage::getInstance(void) {
    return m_instance;
}

Storage::~Storage() {
    sync();
}

void Storage::createUser(const User& t_user) {
    m_userList.push_back(t_user);
    m_dirty = true;
}

list<User> Storage::queryUser(function<bool(const User&)> filter) const {
    list<User> temp;
    for(auto i : m_userList) {
        if(filter(*i))
            temp.push_back(*i);
    }
    return temp;
}

int Storage::updateUser(function<bool(const User&)> filter, function<void(User&)> switcher) {
    int count = 0;
    for(auto i : m_userList) {
        if(filter(*i)) {
            switcher(*i);
            count++;
        }
    }
    m_dirty = true;
    return count;
}

int Storage::deleteUser(function<bool(const User&)> filter) {
    int count = 0;
    for(auto i : m_userList) {
        if(filter(*i) {
            m_userList.erase(i);
            count++;
        }
    }
    m_dirty = true;
    return count;
}

void Storage::createMeeting(const Meeting& t_meeting) {
    m_meetingList.push_back(t_meeting);
    m_dirty = true;
}

list<Meeting> Storage::queryMeeting(function<bool(const Meeting&)> filter) const {
    list<Meeting> temp;
    for(auto i : m_meetingList) {
        if(filter(*i))
            temp.push_back(*i);
    }
    return temp;
}

int Storage::updateMeeting(function<bool(const Meeting&)> filter, function<void(Meeting&)> switcher) {
    int count = 0;
    for(auto i : m_meetingList) {
        if(filter(*i)) {
            switcher(*i);
            count++;
        }
    }
    m_dirty = true;
    return count;
}

int Storage::deleteMeeting(function<bool(const Meeting&)> filter) {
    int count = 0;
    for(auto i : m_meetingList) {
        if(filter(*i)) {
            m_meetingList.erase(i);
            count++;
        }
    }
    m_dirty = true;
    return count;
}

bool Storage::sync(void) {
    if(m_dirty)
        return writeToFile();
    else return false;
}