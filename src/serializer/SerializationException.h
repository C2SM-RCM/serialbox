#pragma once

#include <exception>
#include <string>

namespace ser {
    class SerializationException : public std::exception
    {
    public:
        SerializationException() throw() { }
        ~SerializationException() throw() { }

        void Init(const std::string& errormsg)
        {
            message_ = errormsg;
        }

        const char* what() const throw()
        {
            return message_.c_str();
        }

        const std::string& message() const { return message_; }

    private:
        std::string message_;
    };

}//namespace ser
