#include "Storage.hpp"
#include "Path.hpp"
#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <list>
using namespace std;

shared_ptr<Storage> Storage::m_instance = nullptr;

Storage::Storage() {
    m_dirty = false;
    readFromFile();
}

bool Storage::readFromFile(void) {
    ifstream loadUser;

    loadUser.open("users.csv");//Path::userPath
    if(!loadUser)
        return false;
    string per;
    while(getline(loadUser, per)) {
        int index1 = per.find_first_of(',', 0);
        string t_name, t_password, t_email, t_phone;
        for(int i = 1; i < index1 - 1; i++)
            t_name.push_back(per[i]);
        int index2 = per.find_first_of(',', index1 + 1);
        for(int i = index1 + 2; i < index2 - 1; i++)
            t_password.push_back(per[i]);
        index1 = per.find_first_of(',', index2 + 1);
        for(int i = index2 + 2; i < index1 - 1; i++)
            t_email.push_back(per[i]);
        for(int i = index1 + 2; i < per.size() - 1; i++) 
            t_phone.push_back(per[i]);
        User user(t_name, t_password, t_email, t_phone);
        m_userList.push_back(user);
    }
    loadUser.close();

    ifstream loadMeeting;

    loadMeeting.open("meetings.csv");
    if(!loadMeeting)
        return false;
    while(getline(loadMeeting, per)) {
        string t_sponsor, t_startDate, t_endDate, t_title;
        int index1 = per.find_first_of(',', 0);
        for(int i = 1; i < index1 -1; i++) 
            t_sponsor.push_back(per[i]);
        int index2 = per.find_first_of(',', index1 + 1);
        vector<string> t_participator;
        string name;
        for(int i = index1 + 2; i < index2; i++) {
            if(per[i] == '&' || per[i] == '\"') {
                t_participator.push_back(name);
                name = "";
            }
            else {
                name.push_back(per[i]);
            }
        }
        index1 = per.find_first_of(',', index2 + 1);
        for(int i = index2 + 2; i < index1 - 1; i++) 
            t_startDate.push_back(per[i]);
        index2 = per.find_first_of(',', index1 + 1);
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

    saveUser.open("users.csv");
    if(!saveUser)
        return false;
    int index = 0;
    for(auto i : m_userList) {
        saveUser << "\"" << i.getName() << "\",\"" << i.getPassword() << "\",\"" << i.getEmail() << "\",\"" << i.getPhone() << "\"";
        if(index != m_userList.size() - 1)
            saveUser << endl;
        index++;
    }

    saveUser.close();

    ofstream saveMeeting;

    saveMeeting.open("meetings.csv");
    if(!saveMeeting)
        return false;
    index = 0;
    for(auto i : m_meetingList) {
        saveMeeting << "\"" << i.getSponsor() << "\",\"";
        auto temp = i.getParticipator();
        for(auto j : temp) {
            saveMeeting << j;
            if(j != temp.back())
                saveMeeting << '&';
        }
        saveMeeting << "\",\"" << Date::dateToString(i.getStartDate()) << "\",\"" <<  Date::dateToString(i.getEndDate()) << "\",\"" << i.getTitle() << "\"";
        if(index != m_meetingList.size() - 1)
             saveMeeting << endl;
        index++;
    }
    
    saveMeeting.close();
    return true;
}

shared_ptr<Storage> Storage::getInstance(void) {
    if(!m_instance) {
        shared_ptr<Storage> temp(new Storage());
        m_instance = temp;
    }
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
        if(filter(i))
            temp.push_back(i);
    }
    return temp;
}

int Storage::updateUser(function<bool(const User&)> filter, function<void(User&)> switcher) {
    int count = 0;
    for(auto i = m_userList.begin(); i != m_userList.end(); i++) {
        if(filter(*i)) {
            switcher(*i);
            count++;
            // break;
        }
    }
    m_dirty = true;
    return count;
}

int Storage::deleteUser(function<bool(const User&)> filter) {
    int count = 0;
    for(auto i = m_userList.begin(); i != m_userList.end(); i++) {
        if(filter(*i)) {
            i = m_userList.erase(i);
            count++;
            i--;
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
        if(filter(i))
            temp.push_back(i);
    }
    return temp;
}

int Storage::updateMeeting(function<bool(const Meeting&)> filter, function<void(Meeting&)> switcher) {
    int count = 0;
    for(auto i = m_meetingList.begin(); i != m_meetingList.end(); i++) {
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
    for(auto i = m_meetingList.begin(); i != m_meetingList.end(); i++) {
        if(filter(*i)) {
            i = m_meetingList.erase(i);
            i--;
            count++;
        }
    }
    m_dirty = true;
    return count;
}

bool Storage::sync(void) {
    if(m_dirty) {
        m_dirty = false;
        return writeToFile();
    }
    return false;
}