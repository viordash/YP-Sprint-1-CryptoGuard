#include "cmd_options.h"
#include <iostream>
#include <map>

namespace CryptoGuard {
namespace po = boost::program_options;

ProgramOptions::ProgramOptions() : desc_("Allowed options") {
    desc_.add_options()                                                                         //
        ("help,h", "список доступных опций")                                                    //
        ("command,c", po::value<ProgramOptions::COMMAND_TYPE>()->required(),                    //
         "команда encrypt, decrypt или checksum")                                               //
        ("input,i", po::value<std::string>(&inputFile_)->required(), "путь до входного файла")  //
        ("output,o", po::value<std::string>(&outputFile_)->required(),                          //
         "путь до файла, в котором будет сохранён результат")                                   //
        ("password,p", po::value<std::string>(&password_)->required(), "пароль для шифрования и дешифрования");
}

ProgramOptions::~ProgramOptions() = default;

bool ProgramOptions::Parse(int argc, char *argv[]) {

    po::store(po::parse_command_line(argc, argv, desc_), vm_);

    try {
        po::notify(vm_);
    } catch (const po::required_option &e) {
        if (vm_.count("help") > 0) {
            std::cout << desc_ << "\n";
            return false;
        }
        throw;
    }
    return true;
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
