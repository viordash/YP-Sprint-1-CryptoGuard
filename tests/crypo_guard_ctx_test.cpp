#include "crypto_guard_ctx.h"
#include "handled_exception.h"
#include "gmock/gmock.h"
#include <gtest/gtest.h>

using ::CryptoGuard::CryptoGuardCtx;

TEST(CryptoGuardCtx, EncryptFile_throws_runtime_error_when_inputstream_eof) {
    CryptoGuardCtx testable;
    std::stringstream inStream;
    std::stringstream outStream;

    inStream.setstate(std::ios_base::eofbit);
    try {
        testable.EncryptFile(inStream, outStream, "12341234");
        FAIL();
    } catch (const std::exception &e) {
        ASSERT_STREQ(e.what(), "input stream not good");
    }
}

TEST(CryptoGuardCtx, EncryptFile_throws_runtime_error_when_inputstream_not_good) {
    CryptoGuardCtx testable;
    std::stringstream inStream;
    std::stringstream outStream;

    inStream.setstate(std::ios_base::failbit);
    try {
        testable.EncryptFile(inStream, outStream, "12341234");
        FAIL();
    } catch (const std::exception &e) {
        ASSERT_STREQ(e.what(), "input stream not good");
    }
}

TEST(CryptoGuardCtx, EncryptFile_throws_runtime_error_when_outputstream_eof) {
    CryptoGuardCtx testable;
    std::stringstream inStream;
    std::stringstream outStream;

    outStream.setstate(std::ios_base::eofbit);
    try {
        testable.EncryptFile(inStream, outStream, "12341234");
        FAIL();
    } catch (const std::exception &e) {
        ASSERT_STREQ(e.what(), "output stream not good");
    }
}

TEST(CryptoGuardCtx, EncryptFile_throws_runtime_error_when_outputstream_not_good) {
    CryptoGuardCtx testable;
    std::stringstream inStream;
    std::stringstream outStream;

    outStream.setstate(std::ios_base::failbit);
    try {
        testable.EncryptFile(inStream, outStream, "12341234");
        FAIL();
    } catch (const std::exception &e) {
        ASSERT_STREQ(e.what(), "output stream not good");
    }
}

TEST(CryptoGuardCtx, EncryptFile_write_to_outputstream) {
    CryptoGuardCtx testable;
    std::stringstream inStream("01234567890123456789");
    std::stringstream outStream;

    testable.EncryptFile(inStream, outStream, "12341234");
    ASSERT_EQ(outStream.str().size(), 32);
}