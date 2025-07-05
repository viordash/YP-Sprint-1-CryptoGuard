#include "crypto_guard_ctx.h"
#include <array>
#include <iostream>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <vector>

namespace CryptoGuard {

class CryptoGuardCtx::Impl {
private:
    using evp_ctx_ptr =
        std::unique_ptr<EVP_CIPHER_CTX, decltype([](EVP_CIPHER_CTX *ctx) { EVP_CIPHER_CTX_free(ctx); })>;

    struct AesCipherParams {
        static const size_t KEY_SIZE = 32;             // AES-256 key size
        static const size_t IV_SIZE = 16;              // AES block size (IV length)
        const EVP_CIPHER *cipher = EVP_aes_256_cbc();  // Cipher algorithm

        int encrypt;                              // 1 for encryption, 0 for decryption
        std::array<unsigned char, KEY_SIZE> key;  // Encryption key
        std::array<unsigned char, IV_SIZE> iv;    // Initialization vector
    };

    AesCipherParams CreateChiperParamsFromPassword(std::string_view password) {
        AesCipherParams params;
        constexpr std::array<unsigned char, 8> salt = {'1', '2', '3', '4', '5', '6', '7', '8'};

        if (!EVP_BytesToKey(params.cipher, EVP_sha256(), salt.data(),
                            reinterpret_cast<const unsigned char *>(password.data()), password.size(), 1,
                            params.key.data(), params.iv.data())) {
            throw std::runtime_error{"Failed to create a key from password"};
        }

        return params;
    }

    void ValidateInputStream(std::iostream &stream) {
        if (!stream.good()) {
            throw std::runtime_error("input stream not good");
        }
    }

    void ValidateOuputStream(std::iostream &stream) {
        if (!stream.good()) {
            throw std::runtime_error("output stream not good");
        }
    }

public:
    Impl() { OpenSSL_add_all_algorithms(); }
    ~Impl() { EVP_cleanup(); }

    void EncryptFile(std::iostream &inStream, std::iostream &outStream, std::string_view password) {
        ValidateInputStream(inStream);
        ValidateOuputStream(outStream);

        evp_ctx_ptr ctx{EVP_CIPHER_CTX_new()};

        auto params = CreateChiperParamsFromPassword(password);
        params.encrypt = 1;

        // Инициализируем cipher
        EVP_CipherInit_ex(ctx.get(), params.cipher, nullptr, params.key.data(), params.iv.data(), params.encrypt);

        std::vector<unsigned char> inBuf(16);
        std::vector<unsigned char> outBuf(inBuf.size() + EVP_MAX_BLOCK_LENGTH);
        int outLen;

        // Обрабатываем пачками символов
        while (!inStream.eof()) {
            inStream.read(reinterpret_cast<char *>(inBuf.data()), inBuf.size());
            if (!EVP_CipherUpdate(ctx.get(), outBuf.data(), &outLen, inBuf.data(), inStream.gcount())) {
                throw std::runtime_error("EVP_CipherUpdate error:" + std::to_string(ERR_get_error()));
            }

            outStream.write(reinterpret_cast<char *>(outBuf.data()), outLen);
        }

        // Заканчиваем работу с cipher
        EVP_CipherFinal_ex(ctx.get(), outBuf.data(), &outLen);
        if (outLen > 0) {
            outStream.write(reinterpret_cast<char *>(outBuf.data()), outLen);
        }
    }

    void DecryptFile(std::iostream &inStream, std::iostream &outStream, std::string_view password) {}

    std::string CalculateChecksum(std::iostream &inStream) { return "NOT_IMPLEMENTED"; }
};

CryptoGuardCtx::CryptoGuardCtx() : impl_(std::make_unique<Impl>()) {}
CryptoGuardCtx::~CryptoGuardCtx() = default;

void CryptoGuardCtx::EncryptFile(std::iostream &inStream, std::iostream &outStream, std::string_view password) {
    impl_->EncryptFile(inStream, outStream, password);
}

void CryptoGuardCtx::DecryptFile(std::iostream &inStream, std::iostream &outStream, std::string_view password) {
    impl_->DecryptFile(inStream, outStream, password);
}

std::string CryptoGuardCtx::CalculateChecksum(std::iostream &inStream) { return impl_->CalculateChecksum(inStream); }

}  // namespace CryptoGuard
