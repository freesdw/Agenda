#include "AgendaUI.hpp"
#include <string>
#include <iomanip>
using namespace std;

AgendaUI::AgendaUI() {
    startAgenda();
}

void printFirstMenu(void) {
    cout << "-----------------------Agenda--------------------------" << endl;
    cout << "Action :" << endl;
    cout << "  l     -- log in Agenda by name and password" << endl;
    cout << "  r     -- register an Agenda account" << endl;
    cout << "  q     -- quit Agenda" << endl;
    cout << "------------------------------------------------------\n" << endl;
}

void AgendaUI::OperationLoop(void) {
    string opera;
    
    while(opera != "q") {
        printFirstMenu();
        cout << "Agenda : ~$ ";
        cin >> opera;
        if(opera == "l") userLogIn();
        else if(opera == "r") userRegister();
        else if(opera == "q") quitAgenda();
        else {
            continue;
        }
    }
}

void AgendaUI::startAgenda(void) {
    m_agendaService.startAgenda();
}

string AgendaUI::getOperation() {
    string opera;
    cout << "-------------------------------------Agenda--------------------------------" << endl;
    cout << "  Action:" << endl;
    cout << "        o     --  log out Agenda" << endl;
    cout << "        dc    --  delete Agenda account" << endl;
    cout << "        lu    --  list all Agenda user" << endl;
    cout << "        cm    --  create a meeting" << endl;
    cout << "        amp   --  add meeting participator" << endl;
    cout << "        rmp   --  remove meeting participator" << endl;
    cout << "        rqm   --  request to quit meeting" << endl;
    cout << "        la    --  list all meetings" << endl;
    cout << "        las   --  list all sponsor meeting" << endl;
    cout << "        lap   --  list all participator meetings" << endl;
    cout << "        qm    --  query meeting by title" << endl;
    cout << "        qt    --  query meeting by time interval" << endl;
    cout << "        dm    --  delete meeting by title" << endl;
    cout << "        da    --  delete all meetings" << endl;
    cout << "---------------------------------------------------------------------------\n" << endl;

    cout << "\nAgenda@" << m_userName << " ~# ";
    cin >> opera;
    return opera;
}

bool AgendaUI::executeOperation(const string& opera) {
    if(opera == "o") {
        userLogOut();
        return false;
    } else if(opera == "dc") {
        deleteUser();
        return false;
    } else if(opera == "lu") {
        listAllUsers();
        return true;
    } else if(opera == "cm") {
        createMeeting();
        return true;
    } else if(opera == "amp") {
        addMeetingParticipator();
        return true;
    } else if(opera == "rmp") {
        removeMeetingParticipator();
        return true;
    } else if(opera == "rqm") {
        quitMeeting();
        return true;
    } else if(opera == "la") {
        listAllMeetings();
        return true;
    } else if(opera == "las") {
        listAllSponsorMeetings();
        return true;
    } else if(opera == "lap") {
        listAllParticipateMeetings();
        return true;
    } else if(opera == "qm") {
        queryMeetingByTitle();
        return true;
    } else if(opera == "qt") {
        queryMeetingByTimeInterval();
        return true;
    } else if(opera == "dm") {
        deleteMeetingByTitle();
        return true;
    } else if(opera == "da") {
        deleteAllMeetings();
        return true;
    } else return true;
}

void AgendaUI::userLogIn(void) {
    cout << endl;
    cout << "[log in] [username] [password]" << endl;
    cout << "[log in] ";

    string userName, password;
    cin >> userName >> password;

    if(m_agendaService.userLogIn(userName, password)) {
        m_userName = userName;
        m_userPassword = password;
        cout << "[log in] succeed!\n" << endl;
        while(executeOperation(getOperation())) {}
    } else {
        cout << "[log in] Password error of user doesn't exist\n" << endl;
    }
}

void AgendaUI::userRegister(void) {
    cout << endl;
    cout << "[register] [username] [password] [email] [phone]" << endl;
    cout << "[register] ";
    string userName, password, email, phone;
    cin >> userName >> password >> email >> phone;

    if(m_agendaService.userRegister(userName, password, email, phone)) {
        m_userName = userName;
        m_userPassword = password;
        cout << "[register] succeed!\n" << endl;
        while(executeOperation(getOperation())) {}
    } else {
        cout << "[register] This username has been registered!" << endl;
    }
}

void AgendaUI::userLogOut(void) {
    m_userName.clear();
    m_userPassword.clear();
    cout << endl;
}

void AgendaUI::quitAgenda(void) {
    m_agendaService.quitAgenda();
}

void AgendaUI::deleteUser(void) {
    cout << endl;
    if(m_agendaService.deleteUser(m_userName, m_userPassword)) {
        cout << "[delete agenda account] succeed!\n" << endl;
        userLogOut();
    }
}

void AgendaUI::listAllUsers(void) {
    cout << endl;
    cout << "[list all users]" << endl;
    cout << left << setw(35) << "name" << setw(20) << "email" << "phone" << endl;
    
    auto allUser = m_agendaService.listAllUsers();
    for(auto i : allUser) {
        cout << left << setw(35) << i.getName() << setw(20) << i.getEmail() << i.getPhone() << endl;
    }
    cout << endl;
}

void AgendaUI::createMeeting(void) {
    cout << endl;
    cout << "[create meeting] [the number of participators]" << endl;
    cout << "[create meeting] ";
    int number;
    string strNum;
    cin >> strNum;
    for(int i = 0; i < strNum.size(); i++) {
        if(strNum[i] > '9' || strNum[i] < '0')
            return;
    }
    number = atoi(strNum.c_str());
    vector<string> participator;
    for(int i = 1; i <= number; i++) {
        cout << "[create meeting] [please enter the participator " << i << " ]" << endl;
        string name;
        cout << "[create meeting] ";
        cin >> name;
        participator.push_back(name);
    }
    cout << "[create meeting] [title] [start tate(yyyy-mm-dd/hh:mm)] [endDate(yyyy-mm-dd/hh:mm)]" << endl;
    cout << "[create meeting] ";
    string title, startDate, endDate;
    cin >> title >> startDate >> endDate;

    if(m_agendaService.createMeeting(m_userName, title, startDate, endDate, participator)) {
        cout << "[create meeting] succeed!\n" << endl;;
    } else {
        cout << "[create meeting] error!\n" << endl;
    }
}

void AgendaUI::addMeetingParticipator(void) {
    cout << endl;
    cout << "[add participator] [meeting title] [participator username]" << endl;
    cout << "[add participator] ";
    string title, participator;
    cin >> title >> participator;

    if(m_agendaService.addMeetingParticipator(m_userName, title, participator)) {
        cout << "[add participator] succeed!\n" << endl;
    } else {
        cout << "[add participator] error!\n" << endl;
    }
}

void AgendaUI::removeMeetingParticipator(void) {
    cout << endl;
    cout << "[remove participator] [meeting title] [participator username]" << endl;
    cout << "[remove participator] ";
    string title, participator;
    cin >> title >> participator;

    if(m_agendaService.removeMeetingParticipator(m_userName, title, participator)) {
        cout << "[remove participator] succeed!\n" << endl;
    } else {
        cout << "[remove participator] error\n" << endl;
    }
}

void AgendaUI::quitMeeting(void) {
    cout << endl;
    cout << "[quit meeting] [meeting title]" << endl;
    cout << "[quit meeting] ";
    string title;
    cin >> title;

    if(m_agendaService.quitMeeting(m_userName, title)) {
        cout << "[quit meeting] succeed!" << endl;
    } else {
        cout << "[quit meeting] error!\n" << endl;
    }
}

void AgendaUI::listAllMeetings(void) {
    cout << endl;
    cout << "[list all meetings]\n" << endl;
    printMeetings(m_agendaService.listAllMeetings(m_userName));
}

void AgendaUI::listAllSponsorMeetings(void) {
    cout << endl;
    cout << "[list all sponsor meetings]\n" << endl;
    printMeetings(m_agendaService.listAllSponsorMeetings(m_userName));
}

void AgendaUI::listAllParticipateMeetings(void) {
    cout << endl;
    cout << "[list all participator meetings]\n" << endl;
    printMeetings(m_agendaService.listAllParticipateMeetings(m_userName));
}

void AgendaUI::queryMeetingByTitle(void) {
    cout << endl;
    cout << "[query meeting] [title]:" << endl;
    cout << "[query meeting] ";
    string title;
    cin >> title;
    cout << endl;

    printMeetings(m_agendaService.meetingQuery(m_userName, title));
}

void AgendaUI::queryMeetingByTimeInterval(void) {
    cout << endl;
    cout << "[query meetings] [start time(yyyy-mm-dd/hh:mm)] [end time(yyyy-mm-dd/hh:mm)]" << endl;
    cout << "[query meetings] ";
    string startDate, endDate;
    cin >> startDate >> endDate;
    cout << "\n[query meetings]\n" << endl;

    printMeetings(m_agendaService.meetingQuery(m_userName, startDate, endDate));
}

void AgendaUI::deleteMeetingByTitle(void) {
    cout << endl;
    cout << "[delete meeting] [title]" << endl;
    cout << "[delete meeting] ";
    string title;
    cin >> title;

    if(m_agendaService.deleteMeeting(m_userName, title)) {
        cout << "[delete meeting] succeed!\n" << endl;
    } else {
        cout << "[error] delete meeting fail!\n" << endl;
    }
}

void AgendaUI::deleteAllMeetings(void) {
    cout << endl;
    if(m_agendaService.deleteAllMeetings(m_userName)) {
        cout << "[delete all meeting] succeed!\n" << endl;
    } else {
        cout << "[rrror] Delete all meetings failed!\n" << endl;
    }
}

void AgendaUI::printMeetings(const list<Meeting>& meetings) {
    cout << left << setw(30) << "title" << setw(30) << "sponsor" << setw(20) << "start time" << setw(20) << "end time" << "participators" << endl;
    for(auto i : meetings) {
        cout << left << setw(30) << i.getTitle() << setw(30) << i.getSponsor() << setw(20) << Date::dateToString(i.getStartDate()) << setw(20) << Date::dateToString(i.getEndDate());
        for(auto j : i.getParticipator()) {
            cout << j;
            if(j != i.getParticipator().back())
                cout << ',';
        }
        cout << endl;
    } 
    cout << endl;
}