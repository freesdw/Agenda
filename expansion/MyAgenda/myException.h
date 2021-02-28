#ifndef MYEXCEPTION_H
#define MYEXCEPTION_H
#include <string>

class myException {
    public:
        myException(std::string message) : m_message(message) {}
        std::string what() {
            return m_message;
        }
    private:
        std::string m_message;
};

#endif