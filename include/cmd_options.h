#pragma once

#include <boost/program_options.hpp>
#include <string>

namespace CryptoGuard {
namespace po = boost::program_options;

class ProgramOptions {
public:
    ProgramOptions();
    ~ProgramOptions();

    enum class COMMAND_TYPE {
        ENCRYPT,
        DECRYPT,
        CHECKSUM,
    };

    void Parse(int argc, char *argv[]);

    COMMAND_TYPE GetCommand() const { return command_; }
    std::string GetInputFile() const { return inputFile_; }
    std::string GetOutputFile() const { return outputFile_; }
    std::string GetPassword() const { return password_; }

private:
    COMMAND_TYPE command_;
    std::string inputFile_;
    std::string outputFile_;
    std::string password_;

    po::options_description desc_;
    po::variables_map vm_;
};

}  // namespace CryptoGuard
