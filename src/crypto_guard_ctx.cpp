#include "crypto_guard_ctx.h"
#include <array>
#include <iomanip>
#include <iostream>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <sstream>
#include <vector>

namespace CryptoGuard {

class CryptoGuardCtx::Impl {
private:
    using evp_ctx_ptr =
        std::unique_ptr<EVP_CIPHER_CTX, decltype([](EVP_CIPHER_CTX *ctx) { EVP_CIPHER_CTX_free(ctx); })>;

    using evp_md_ctx_ptr = std::unique_ptr<EVP_MD_CTX, decltype([](EVP_MD_CTX *ctx) { EVP_MD_CTX_free(ctx); })>;

    struct AesCipherParams {
        static const size_t KEY_SIZE = 32;             // AES-256 key size
        static const size_t IV_SIZE = 16;              // AES block size (IV length)
        const EVP_CIPHER *cipher = EVP_aes_256_cbc();  // Cipher algorithm

        int encrypt;                              // 1 for encryption, 0 for decryption
        std::array<unsigned char, KEY_SIZE> key;  // Encryption key
        std::array<unsigned char, IV_SIZE> iv;    // Initialization vector
    };

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

    void ThrowOpenSSLError(std::string prefix) {
        char err_buf[512];
        ERR_error_string_n(ERR_get_error(), err_buf, sizeof(err_buf));
        throw std::runtime_error(prefix + ": " + std::string(err_buf));
    }

public:
    Impl() { OpenSSL_add_all_algorithms(); }
    ~Impl() { EVP_cleanup(); }

    AesCipherParams CreateChiperParamsFromPassword(std::string_view password) {
        AesCipherParams params;
        constexpr std::array<unsigned char, 8> salt = {'1', '2', '3', '4', '5', '6', '7', '8'};

        if (!EVP_BytesToKey(params.cipher, EVP_sha256(), salt.data(),
                            reinterpret_cast<const unsigned char *>(password.data()), password.size(), 1,
                            params.key.data(), params.iv.data())) {
            ThrowOpenSSLError("EVP_BytesToKey");
        }

        return params;
    }

    void CipherFile(std::iostream &inStream, std::iostream &outStream, const AesCipherParams &params) {
        ValidateInputStream(inStream);
        ValidateOuputStream(outStream);

        evp_ctx_ptr ctx{EVP_CIPHER_CTX_new()};

        if (!EVP_CipherInit_ex(ctx.get(), params.cipher, nullptr, params.key.data(), params.iv.data(),
                               params.encrypt)) {
            ThrowOpenSSLError("EVP_CipherInit_ex");
        }

        std::vector<unsigned char> inBuf(4096);
        std::vector<unsigned char> outBuf(inBuf.size() + EVP_MAX_BLOCK_LENGTH);
        int outLen;

        while (inStream.read(reinterpret_cast<char *>(inBuf.data()), inBuf.size()) || inStream.gcount() > 0) {
            if (!EVP_CipherUpdate(ctx.get(), outBuf.data(), &outLen, inBuf.data(), inStream.gcount())) {
                ThrowOpenSSLError("EVP_CipherUpdate");
            }

            outStream.write(reinterpret_cast<char *>(outBuf.data()), outLen);
        }

        if (!EVP_CipherFinal_ex(ctx.get(), outBuf.data(), &outLen)) {
            ThrowOpenSSLError("EVP_CipherFinal_ex");
        }
        if (outLen > 0) {
            outStream.write(reinterpret_cast<char *>(outBuf.data()), outLen);
        }
    }

    std::string CalculateChecksum(std::iostream &inStream) {
        ValidateInputStream(inStream);

        evp_md_ctx_ptr mdctx{EVP_MD_CTX_new()};

        const EVP_MD *md = EVP_sha256();
        if (md == nullptr) {
            ThrowOpenSSLError("EVP_sha256");
        }

        if (!EVP_DigestInit_ex(mdctx.get(), md, nullptr)) {
            ThrowOpenSSLError("EVP_DigestInit_ex");
        }

        std::array<unsigned char, 4096> buffer;
        unsigned int md_len = 0;
        std::array<unsigned char, EVP_MAX_MD_SIZE> md_value;

        while (inStream.read(reinterpret_cast<char *>(buffer.data()), buffer.size()) || inStream.gcount() > 0) {
            if (!EVP_DigestUpdate(mdctx.get(), buffer.data(), inStream.gcount())) {
                ThrowOpenSSLError("EVP_DigestUpdate");
            }
        }

        if (!EVP_DigestFinal_ex(mdctx.get(), md_value.data(), &md_len)) {
            ThrowOpenSSLError("EVP_DigestFinal_ex");
        }

        std::ostringstream hex_stream;
        hex_stream << std::hex << std::setfill('0');

        for (unsigned int i = 0; i < md_len; i++) {
            hex_stream << std::setw(2) << static_cast<unsigned>(md_value[i]);
        }

        return hex_stream.str();
    }
};

CryptoGuardCtx::CryptoGuardCtx() : impl_(std::make_unique<Impl>()) {}
CryptoGuardCtx::~CryptoGuardCtx() = default;

void CryptoGuardCtx::EncryptFile(std::iostream &inStream, std::iostream &outStream, std::string_view password) {
    auto params = impl_->CreateChiperParamsFromPassword(password);
    params.encrypt = 1;
    impl_->CipherFile(inStream, outStream, params);
}

void CryptoGuardCtx::DecryptFile(std::iostream &inStream, std::iostream &outStream, std::string_view password) {
    auto params = impl_->CreateChiperParamsFromPassword(password);
    params.encrypt = 0;
    impl_->CipherFile(inStream, outStream, params);
}

std::string CryptoGuardCtx::CalculateChecksum(std::iostream &inStream) { return impl_->CalculateChecksum(inStream); }

}  // namespace CryptoGuard
