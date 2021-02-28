#include "AgendaUI.hpp"
#include "myException.h"
#include <string>
#include <conio.h>
#include <csignal>
#include <iomanip>
using namespace std;

string input_password() {
	string password;
	char pwords[50] = {0},ch = 0;
    int i = 0;
	while((ch = getch()) != '\r') {
		if(ch == '\b' && i > 0) {
			printf("\b \b");
			--i;
		} else if(ch != '\n') {
			pwords[i++] = ch;
			printf("*");
		}
	}
	password = pwords;
	cout << endl;
	return password;
}

bool check_phone(string phone) {
    if(phone.size() != 11)
        return false;
    for(int i = 0; i < phone.size(); i++) {
        if(phone[i] > '9' || phone[i] < '0')
            return false;
    }
    return true;
}

bool check_email(string email) {
    int damp = false;
    for(int i = 0; i < email.size(); i++) {
        if(email[i] == '@')
            damp = true;
    }
    if(!damp)
        return false;
    int index = email.find_first_of('@', 0);
    string temp = email.substr(index, email.size() - index);

    if(temp != "@126.com" && temp != "@163.com" && temp != "@sina.com" && temp != "@21cn.com"
        && temp != "@sohu.com" && temp != "@tom.com" && temp != "@qq.com" && temp != "@eyou.com"
        && temp != "@56.com" && temp != "@chinaren.com" && temp != "@sogou.com" && temp != "@citiz.com")
        return false;
    if(temp == "@qq.com") {
        for(int i = 0; i < index; i++) {
            if(email[i] > '9' || email[i] < '0')
                return false;
        }
    }
    return true;
}

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
    printFirstMenu();
    cout << "Agenda : ~$ ";
    while(opera != "q") {
        if(opera != "") {
            printFirstMenu();
            cout << "Agenda : ~$ ";
        }
        getline(cin, opera);
        if(opera == "l") userLogIn();
        else if(opera == "r") userRegister();
        else if(opera == "q") quitAgenda();
        else if(opera != "") {
            cout << "\a[error] Please input \"l\",\"r\" or \"q\"!" << endl;
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
    cout << "        cf    --  change personal information" << endl;
    cout << "        cp    --  change password" << endl;
    cout << "---------------------------------------------------------------------------\n" << endl;

    cout << "\nAgenda@" << m_userName << " ~# ";
    getline(cin, opera);
    return opera;
}

bool AgendaUI::executeOperation(const string& opera) {
    if(opera == "")
        return true;
    if(opera == "o") {
        userLogOut();
        return false;
    } else if(opera == "dc") {
        deleteUser();
        return false;
    } else if(opera == "cp") {
        changePassword();
        return false;
    }
    try {
        if(opera == "lu") {
            listAllUsers();
        } else if(opera == "cm") {
            createMeeting();
        } else if(opera == "amp") {
            addMeetingParticipator();
        } else if(opera == "rmp") {
            removeMeetingParticipator();
        } else if(opera == "rqm") {
            quitMeeting();
        } else if(opera == "la") {
            listAllMeetings();
        } else if(opera == "las") {
            listAllSponsorMeetings();
        } else if(opera == "lap") {
            listAllParticipateMeetings();
        } else if(opera == "qm") {
            queryMeetingByTitle();
        } else if(opera == "qt") {
            queryMeetingByTimeInterval();
        } else if(opera == "dm") {
            deleteMeetingByTitle();
        } else if(opera == "da") {
            deleteAllMeetings();
        } else if(opera == "cf") {
            changeInformation();
        }
    }
    catch(myException e) {
        cout << "\a[error] " << e.what() << endl;
    }
    return true;
}

void AgendaUI::userLogIn(void) {
    cout << endl;
    cout << "[log in] [username]" << endl;
    cout << "[log in] ";

    string userName, password;
    getline(cin, userName);
    if(userName == "")
        return;

    cout << "[log in] [password]" << endl;
    cout << "[log in] ";
    password = input_password();
    if(password == "")
        return;

    try {
        if(m_agendaService.userLogIn(userName, password)) {
            m_userName = userName;
            m_userPassword = password;
            cout << "[log in] succeed!\n" << endl;
            while(executeOperation(getOperation())) {}
        }
    }
    catch(myException e) {
        cout << "\a[error] " << e.what() << endl;
    }
}

void AgendaUI::userRegister(void) {

    cout << endl;
    cout << "[register] [username]" << endl;
    cout << "[register] ";
    string userName, password, email, phone;
    getline(cin, userName);
    if(userName == "")
        return;

    cout << "[register] [password]" << endl;
    cout << "[register] ";
    password = input_password();
    if(password == "")
        return;
    while(true) {
        cout << "[register] [email]" << endl;
        cout << "[register] ";
        getline(cin, email);
        if(email == "")
            return;
        if(check_email(email))
            break;
        else {
            cout << "\a[error] The email is invalid! Try again!" << endl;
        }
    }
    while(true) {
        cout << "[register] [phone]" << endl;
        cout << "[register] ";
        getline(cin, phone);
        if(phone == "")
            return;
        if(check_phone(phone))
            break;
        else 
            cout << "\a[error] The phone number is invalid! Try again!" << endl;
    }    

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
    getline(cin, strNum);
    if(strNum.size() == 0)
        return;

    for(int i = 0; i < strNum.size(); i++) {
        if(strNum[i] > '9' || strNum[i] < '0') {
            cout << "\a[error] The number is invalid!" << endl;
            return;
        }
    }
    number = atoi(strNum.c_str());
    vector<string> participator;
    for(int i = 1; i <= number; i++) {
        cout << "[create meeting] [please enter the participator " << i << " ]" << endl;
        string name;
        cout << "[create meeting] ";
        getline(cin, name);
        if(name == "")
            return;

        participator.push_back(name);
    }
    cout << "[create meeting] [title]" << endl;
    cout << "[create meeting] ";
    string title, startDate, endDate;
    getline(cin, title);
    if(title == "")
        return;
    while(true) {
        cout << "[create meeting] [start tate(yyyy-mm-dd/hh:mm)]" << endl;
        cout << "[create meeting] ";
        getline(cin, startDate);
        if(startDate == "")
            return;
        if(Date::isValid(Date::stringToDate(startDate))) 
            break;
        else 
            cout << "\a[error] The date is invalid! Please try again!" << endl;
    }

    while(true) {
        cout << "[create meeting] [endDate(yyyy-mm-dd/hh:mm)]" << endl;
        cout << "[create meeting] ";
        getline(cin, endDate);
        if(endDate == "")
            return;
        if(Date::isValid(Date::stringToDate(endDate)))
            break;
        else 
            cout << "\a[error] The date is invalid! Please try again!" << endl;
    }

    if(m_agendaService.createMeeting(m_userName, title, startDate, endDate, participator)) {
        cout << "[create meeting] succeed!\n" << endl;;
    } else {
        cout << "[create meeting] error!\n" << endl;
    }
}

void AgendaUI::addMeetingParticipator(void) {
    cout << endl;
    cout << "[add participator] [meeting title]" << endl;
    cout << "[add participator] ";
    string title, participator;
    getline(cin, title);
    if(title == "")
        return;

    cout << "[add participator] [participator username]" << endl;
    cout << "[add participator] ";
    getline(cin, participator);
    if(participator == "")
        return;

    if(m_agendaService.addMeetingParticipator(m_userName, title, participator)) {
        cout << "[add participator] succeed!\n" << endl;
    } else {
        cout << "[add participator] error!\n" << endl;
    }
}

void AgendaUI::removeMeetingParticipator(void) {
    cout << endl;
    cout << "[remove participator] [meeting title]" << endl;
    cout << "[remove participator] ";
    string title, participator;
    getline(cin, title);
    if(title == "")
        return;

    cout << "[remove participator] [participator username]" << endl;
    cout << "[remove participator] ";
    getline(cin, participator);
    if(participator == "")
        return;

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
    getline(cin, title);
    if(title == "")
        return;

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
    getline(cin, title);
    if(title == "")
        return;

    cout << endl;

    printMeetings(m_agendaService.meetingQuery(m_userName, title));
}

void AgendaUI::queryMeetingByTimeInterval(void) {
    cout << endl;
    string startDate, endDate;
    while(true) {
        cout << "[query meetings] [start time(yyyy-mm-dd/hh:mm)]" << endl;
        cout << "[query meetings] ";
        
        getline(cin, startDate);
        if(startDate == "")
            return;
        if(Date::isValid(Date::stringToDate(startDate)))
            break;
        else
            cout << "\a[error] The date is invalid! Please try again!" << endl;
    }

    while(true) {
        cout << "[query meetings] [end time(yyyy-mm-dd/hh:mm)]" << endl;
        cout << "[query meetings] ";
        getline(cin, endDate);
        if(endDate == "")
            return;
        if(Date::isValid(Date::stringToDate(endDate)))
            break;
        else 
            cout << "\a[error] The date is invalid! Please try again!" << endl;
    }

    cout << "\n[query meetings]\n" << endl;

    printMeetings(m_agendaService.meetingQuery(m_userName, startDate, endDate));
}

void AgendaUI::deleteMeetingByTitle(void) {
    cout << endl;
    cout << "[delete meeting] [title]" << endl;
    cout << "[delete meeting] ";
    string title;
    getline(cin, title);
    if(title == "")
        return;

    if(m_agendaService.deleteMeeting(m_userName, title)) {
        cout << "[delete meeting] succeed!\n" << endl;
    } else {
        cout << "\a[error] delete meeting fail!\n" << endl;
    }
}

void AgendaUI::deleteAllMeetings(void) {
    cout << endl;
    if(m_agendaService.deleteAllMeetings(m_userName)) {
        cout << "[delete all meeting] succeed!\n" << endl;
    } else {
        cout << "\a[rrror] Delete all meetings failed!\n" << endl;
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

void AgendaUI::endAgenda(void) {
    quitAgenda();
}

void AgendaUI::changeInformation(void) {

    string email, phone;
    while(true) {
        cout << "[change information] [new email]" << endl;
        cout << "[change information] ";
        getline(cin, email);
        if(email == "")
            return;
        if(check_email(email))
            break;
        else {
            cout << "\a[error] Invalid email! Please try again!" << endl;
        }
    }
    
    while(true) {
        cout << "[change information] [new phone number]" << endl;
        cout << "[change information] ";
        getline(cin, phone);
        if(phone == "")
            return;
        if(check_phone(phone))
            break;
        else {
            cout << "\a[error] Invalid phone number! Please try agian!" << endl;
        }
    }
    if(m_agendaService.changeInformation(m_userName, m_userPassword, email, phone))
        cout << "[change information] succeed!\n" << endl;
    else {
        cout << "\a[error] change information failed!\n" << endl;
    }
}

void AgendaUI::changePassword(void) {
    string oldPassword, newPassword;
    for(int i = 2; i >= 0; i--) {
        cout << "[change password] [old password]" << endl;
        cout << "[change password] ";
        oldPassword = input_password();
        if(oldPassword == "")
            return;
        if(oldPassword == m_userPassword)
            break;
        else {
            cout << "\a[error] Wrong password! You still have " << i << "chances!" << endl;
        }
    }
    while(true) {
        cout << "[change password] [new password]" << endl;
        cout << "[change password] ";
        newPassword = input_password();
        if(newPassword == "")
            return;
        cout << "[change password] [Message: please input again]" << endl;
        cout << "[change password] [new password]" << endl;
        cout << "[change password] ";
        string temp = input_password();
        if(temp == "")
            return;
        if(temp == newPassword)
            break;
        else {
            cout << "\a[error] The inputs are different! Please try again!" << endl;
        }
    }
    if(m_agendaService.changePassword(m_userName, m_userPassword, newPassword))
        cout << "[change password] succeed!\n" << endl;
    else 
        cout << "\a[error] change password failed!\n" << endl;
}