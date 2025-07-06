#include "cmd_options.h"
#include "handled_exception.h"
#include <iostream>
#include <map>

namespace CryptoGuard {
namespace po = boost::program_options;

ProgramOptions::ProgramOptions() : desc_("Allowed options") {
    desc_.add_options()                                                                         //
        ("help,h", "список доступных опций")                                                    //
        ("command,c", po::value<ProgramOptions::COMMAND_TYPE>(&command_)->required(),           //
         "команда encrypt, decrypt или checksum")                                               //
        ("input,i", po::value<std::string>(&inputFile_)->required(), "путь до входного файла")  //
        ("output,o", po::value<std::string>(&outputFile_),                                      //
         "путь до файла, в котором будет сохранён результат")                                   //
        ("password,p", po::value<std::string>(&password_), "пароль для шифрования и дешифрования");
}

ProgramOptions::~ProgramOptions() = default;

void ProgramOptions::Parse(int argc, char *argv[]) {
    po::store(po::parse_command_line(argc, argv, desc_), vm_);

    try {
        po::notify(vm_);
        switch (command_) {
        case ProgramOptions::COMMAND_TYPE::ENCRYPT:
        case ProgramOptions::COMMAND_TYPE::DECRYPT:
            if (vm_.count("output") == 0) {
                throw po::required_option("--output");
            }
            if (vm_.count("password") == 0) {
                throw po::required_option("--password");
            }
            break;
        case ProgramOptions::COMMAND_TYPE::CHECKSUM:
            outputFile_.clear();
            password_.clear();
            break;
        }

    } catch (const po::required_option &e) {
        if (vm_.count("help") > 0) {
            std::ostringstream oss;
            oss << desc_;
            throw HandledException(oss.str());
        }
        throw;
    }
}

std::istream &operator>>(std::istream &in, ProgramOptions::COMMAND_TYPE &command) {
    static const std::map<std::string, ProgramOptions::COMMAND_TYPE> mapping = {
        {"encrypt", ProgramOptions::COMMAND_TYPE::ENCRYPT},
        {"decrypt", ProgramOptions::COMMAND_TYPE::DECRYPT},
        {"checksum", ProgramOptions::COMMAND_TYPE::CHECKSUM},
    };

    std::string token;
    in >> token;
    auto it = mapping.find(token);
    if (it == mapping.end()) {
        in.setstate(std::ios_base::failbit);
    } else {
        command = it->second;
    }

    return in;
}

}  // namespace CryptoGuard
