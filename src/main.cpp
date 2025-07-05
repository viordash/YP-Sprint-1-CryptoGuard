#include "cmd_options.h"
#include "crypto_guard_ctx.h"
#include "handled_exception.h"
#include <fstream>
#include <iostream>
#include <openssl/evp.h>
#include <print>
#include <stdexcept>

int main(int argc, char *argv[]) {
    try {
        CryptoGuard::ProgramOptions options;
        CryptoGuard::CryptoGuardCtx cryptoCtx;

        options.Parse(argc, argv);

        using COMMAND_TYPE = CryptoGuard::ProgramOptions::COMMAND_TYPE;

        std::fstream inputFile(options.GetInputFile(), std::ios::in | std::ios::binary);
        std::fstream outputFile(options.GetOutputFile(), std::ios::out | std::ios::binary);

        switch (options.GetCommand()) {
        case COMMAND_TYPE::ENCRYPT:
            cryptoCtx.EncryptFile(inputFile, outputFile, options.GetPassword());
            std::print("File encoded successfully\n");
            break;

        case COMMAND_TYPE::DECRYPT:
            std::print("File decoded successfully\n");
            break;

        case COMMAND_TYPE::CHECKSUM:
            std::print("Checksum: {}\n", "CHECKSUM_NOT_IMPLEMENTED");
            break;

        default:
            throw std::runtime_error{"Unsupported command"};
        }

    } catch (const HandledException &e) {
        std::cout << e.what() << std::endl;
        return 0;
    } catch (const std::exception &e) {
        std::print(std::cerr, "Error: {}\n", e.what());
        return 1;
    }

    return 0;
}