#include <exception>
#include <string>

class HandledException : public std::exception {
private:
    std::string message;

public:
    explicit HandledException(const std::string &msg) : message(msg) {}

    const char *what() const noexcept override { return message.c_str(); }
};