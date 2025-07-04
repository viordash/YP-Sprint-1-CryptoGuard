#include "cmd_options.h"

namespace CryptoGuard {
namespace po = boost::program_options;

ProgramOptions::ProgramOptions() : desc_("Allowed options") {
    desc_.add_options()                                                                         //
        ("help,h", "список доступных опций")                                                    //
        ("command,c", po::value<int>()->required(), "команда encrypt, decrypt или checksum")    //
        ("input,i", po::value<std::string>(&inputFile_)->required(), "путь до входного файла")  //
        ("output,o", po::value<std::string>(&outputFile_)->required(),
         "путь до файла, в котором будет сохранён результат")  //
        ("password,p", po::value<std::string>(&password_)->required(), "пароль для шифрования и дешифрования");
}

ProgramOptions::~ProgramOptions() = default;

void ProgramOptions::Parse(int argc, char *argv[]) {}

}  // namespace CryptoGuard
