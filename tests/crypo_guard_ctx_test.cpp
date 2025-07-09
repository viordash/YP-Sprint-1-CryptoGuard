#include "crypto_guard_ctx.h"
#include "handled_exception.h"
#include "gmock/gmock.h"
#include <gtest/gtest.h>

using ::CryptoGuard::CryptoGuardCtx;
using ::testing::HasSubstr;

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

TEST(CryptoGuardCtx, DecryptFile_throws_runtime_error_when_inputstream_eof) {
    CryptoGuardCtx testable;
    std::stringstream inStream;
    std::stringstream outStream;

    inStream.setstate(std::ios_base::eofbit);
    try {
        testable.DecryptFile(inStream, outStream, "12341234");
        FAIL();
    } catch (const std::exception &e) {
        ASSERT_STREQ(e.what(), "input stream not good");
    }
}

TEST(CryptoGuardCtx, DecryptFile_throws_runtime_error_when_inputstream_not_good) {
    CryptoGuardCtx testable;
    std::stringstream inStream;
    std::stringstream outStream;

    inStream.setstate(std::ios_base::failbit);
    try {
        testable.DecryptFile(inStream, outStream, "12341234");
        FAIL();
    } catch (const std::exception &e) {
        ASSERT_STREQ(e.what(), "input stream not good");
    }
}

TEST(CryptoGuardCtx, DecryptFile_throws_runtime_error_when_outputstream_eof) {
    CryptoGuardCtx testable;
    std::stringstream inStream;
    std::stringstream outStream;

    outStream.setstate(std::ios_base::eofbit);
    try {
        testable.DecryptFile(inStream, outStream, "12341234");
        FAIL();
    } catch (const std::exception &e) {
        ASSERT_STREQ(e.what(), "output stream not good");
    }
}

TEST(CryptoGuardCtx, DecryptFile_throws_runtime_error_when_outputstream_not_good) {
    CryptoGuardCtx testable;
    std::stringstream inStream;
    std::stringstream outStream;

    outStream.setstate(std::ios_base::failbit);
    try {
        testable.DecryptFile(inStream, outStream, "12341234");
        FAIL();
    } catch (const std::exception &e) {
        ASSERT_STREQ(e.what(), "output stream not good");
    }
}

TEST(CryptoGuardCtx, DecryptFile_with_invalid_input_throws_runtime_error) {
    CryptoGuardCtx testable;
    std::stringstream inStream("01234567890123456789012345678901234567890123456789012345");
    std::stringstream outStream;

    try {
        testable.DecryptFile(inStream, outStream, "12341234");
        FAIL();
    } catch (const std::exception &e) {
        ASSERT_THAT(e.what(), HasSubstr("wrong final block length"));
    }
}

TEST(CryptoGuardCtx, Different_passwords_throws_runtime_error) {
    CryptoGuardCtx testable;
    std::stringstream inEncryptStream("01234567890123456789");
    std::stringstream outEncryptStream;
    std::stringstream outDecryptStream;

    try {
        testable.EncryptFile(inEncryptStream, outEncryptStream, "12341234");
        testable.DecryptFile(outEncryptStream, outDecryptStream, "12341234+1");
        FAIL();
    } catch (const std::exception &e) {
        ASSERT_THAT(e.what(), HasSubstr("routines::bad decrypt"));
    }

    ASSERT_EQ(outEncryptStream.str().size(), 32);
    ASSERT_EQ(outDecryptStream.str().size(), 16);
    ASSERT_NE("01234567890123456789", outDecryptStream.str());
}

static std::string CreateFixedString(size_t size) {
    static const std::string base = "0123456789"
                                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                    "abcdefghijklmnopqrstuvwxyz"
                                    "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";

    if (size == 0)
        return "";

    std::string input;
    input.reserve(size);

    for (size_t copied = 0; copied < size;) {
        size_t to_copy = std::min(base.size(), size - copied);
        input.append(base, 0, to_copy);
        copied += to_copy;
    }

    return input;
}

TEST(CryptoGuardCtx, EncryptFile_and_DecryptFile_are_compatible_fixed_data) {
    const std::vector<size_t> inputSizes = {0,   1,   15,  16,  17,  31,  32,  33,  63,   64,   65,   127, 128,
                                            129, 255, 256, 257, 511, 512, 513, 999, 1000, 4095, 4096, 4097};

    const std::vector<size_t> passwordSizes = {0, 1, 7, 8, 9, 15, 16, 17, 31, 32, 33, 63, 64, 65, 99, 100};

    CryptoGuardCtx testable;
    for (size_t inputSize : inputSizes) {
        const std::string input_data = CreateFixedString(inputSize);
        std::stringstream inEncryptStream(input_data);

        for (size_t passwordSize : passwordSizes) {
            std::string password = CreateFixedString(passwordSize);
            std::stringstream outEncryptStream;
            std::stringstream outDecryptStream;

            inEncryptStream.clear();
            inEncryptStream.seekg(0);
            testable.EncryptFile(inEncryptStream, outEncryptStream, password);

            outEncryptStream.seekg(0);
            testable.DecryptFile(outEncryptStream, outDecryptStream, password);

            ASSERT_EQ(input_data, outDecryptStream.str());
        }
    }
}

TEST(CryptoGuardCtx, ChecksumCalculation) {
    CryptoGuardCtx testable;
    std::stringstream inStream("01234567890123456789");

    auto checksum = testable.CalculateChecksum(inStream);
    EXPECT_EQ(checksum, "4e76ad8354461437c04ef9b9b242540b6406d782ff2c3fb28afdab5b423f88fe");
}

TEST(CryptoGuardCtx, ChecksumCalculation_for_long_string) {
    CryptoGuardCtx testable;
    std::string input_data = CreateFixedString(1000);
    std::stringstream inStream(std::move(input_data));

    auto checksum = testable.CalculateChecksum(inStream);
    EXPECT_EQ(checksum, "856fc5d1201b902510c46d5cc5f3005a117d7d3b856ac3c4dbbc3370b1b99937");
}

TEST(CryptoGuardCtx, ChecksumCalculation_for_empty_data) {
    CryptoGuardCtx testable;
    std::stringstream inStream;

    auto checksum = testable.CalculateChecksum(inStream);
    EXPECT_EQ(checksum, "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
}

TEST(CryptoGuardCtx, ChecksumCalculation_is_independent_of_previous_call_EncryptFile_or_DecryptFile) {
    CryptoGuardCtx testable;

    std::stringstream inEncryptStream("01234567890123456789");
    std::stringstream outEncryptStream;
    std::stringstream outDecryptStream;
    testable.EncryptFile(inEncryptStream, outEncryptStream, "12341234");
    testable.DecryptFile(outEncryptStream, outDecryptStream, "12341234");

    auto checksum = testable.CalculateChecksum(outDecryptStream);
    EXPECT_EQ(checksum, "4e76ad8354461437c04ef9b9b242540b6406d782ff2c3fb28afdab5b423f88fe");
}