#include "Date.hpp"
using namespace std;

Date::Date() {
    m_year = 0;
    m_month = 0;
    m_day = 0;
    m_hour = 0;
    m_minute = 0;
}

Date::Date(int t_year, int t_month, int t_day, int t_hour, int t_minute) {
    m_year = t_year;
    m_month = t_month;
    m_day = t_day;
    m_hour = t_hour;
    m_minute = t_minute;
}

Date::Date(const string& dateString) {
    bool valid = true;
    if(dateString.size() != 16)
        valid = false;
    if(dateString[4] != '-' || dateString[7] != '-' || dateString[10] != '/' || dateString[13] != ':')
        valid = false;
    for(int i = 0; i < 16; i++) {
        if(i == 4 || i == 7 || i == 10 || i == 13)
            continue;
        if(dateString[i] < '0' || dateString[i] > '9')
            valid = false;
    }
    if(valid) {
        sscanf(dateString.c_str(), "%d-%d-%d/%d:%d", &m_year, &m_month, &m_day, &m_hour, &m_minute);
        if(!isValid(*this)) {
            *this = Date();
        }
    } else {
        m_year = 0;
        m_month = 0;
        m_day = 0;
        m_hour = 0;
        m_minute = 0;
    }
}

int Date::getYear(void) const {
    return m_year;
}

void Date::setYear(const int t_year) {
    m_year = t_year;
}

int Date::getMonth(void) const {
    return m_month;
}

void Date::setMonth(const int t_month) {
    m_month = t_month;
}

int Date::getDay(void) const {
    return m_day;
}

void Date::setDay(const int t_day) {
    m_day = t_day;
}

int Date::getHour(void) const {
    return m_hour;
}

void Date::setHour(const int t_hour) {
    m_hour = t_hour;
}

int Date::getMinute(void) const {
    return m_minute;
}

void Date::setMinute(const int t_minute) {
    m_minute = t_minute;
}

bool Date::isValid(const Date& t_date) {
    if(t_date.m_year < 1000 || t_date.m_year > 9999)
        return false;
    if(t_date.m_month < 1 || t_date.m_month > 12 || t_date.m_day < 1)
        return false;
    int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if(t_date.m_year % 400 == 0 || t_date.m_year % 4 == 0 && t_date.m_year % 100 != 0)
        days[1] = 29;
    if(t_date.m_day > days[t_date.m_month - 1])
        return false;
    if(t_date.m_hour < 0 || t_date.m_hour > 23 || t_date.m_minute < 0 || t_date.m_minute > 59)
        return false;
    return true;
}

Date Date::stringToDate(const string& t_dateString) {
    // if(t_dateString.size() != 16)
    //     return Date();
    // if(t_dateString[4] != '-' || t_dateString[7] != '-' || t_dateString[10] != '/' || t_dateString[13] != ':')
    //     return Date();
    // for(int i = 0; i < 16; i++) {
    //     if(i == 4 || i == 7 || i == 10 || i == 13)
    //         continue;
    //     if(t_dateString[i] < '0' || t_dateString[i] > '9')
    //         return Date();
    // }
    Date temp(t_dateString);
    if(isValid(temp)) {
        return temp;
    } else return Date();
}

string Date::dateToString(const Date& t_date) {
    char str[16] = {0};
    sprintf(str, "%04d-%02d-%02d/%02d:%02d", t_date.m_year, t_date.m_month, t_date.m_day, t_date.m_hour, t_date.m_minute);
    if(!isValid(t_date))
        return "0000-00-00/00:00";
    else return string(str);
}

Date& Date::operator=(const Date& t_date) {
    m_year = t_date.m_year;
    m_month = t_date.m_month;
    m_day = t_date.m_day;
    m_hour = t_date.m_hour;
    m_minute = t_date.m_minute;
}

bool Date::operator==(const Date& t_date) const {
    return (m_year == t_date.m_year && m_month == t_date.m_month && m_day == t_date.m_day && m_hour == t_date.m_hour && m_minute == t_date.m_minute);
}

bool Date::operator>(const Date& t_date) const {
    if(m_year > t_date.m_year) return true;
    if(m_year < t_date.m_year) return false;
    if(m_month > t_date.m_month) return true;
    if(m_month < t_date.m_month) return false;
    if(m_day > t_date.m_day) return true;
    if(m_day < t_date.m_day) return false;
    if(m_hour > t_date.m_hour) return true;
    if(m_hour < t_date.m_hour) return false;
    if(m_minute > t_date.m_minute) return true;
    else return false;
}

bool Date::operator<(const Date& t_date) const {
    return !(*this > t_date || *this == t_date);
}

bool Date::operator>=(const Date& t_date) const {
    return !(*this < t_date);
}

bool Date::operator<=(const Date& t_date) const {
    return !(*this > t_date);
}