#include "cmd_options.h"
#include "handled_exception.h"
#include "gmock/gmock.h"
#include <gtest/gtest.h>

using ::CryptoGuard::ProgramOptions;
using ::testing::AllOf;
using ::testing::HasSubstr;
using ::testing::StartsWith;

TEST(ProgramOptions, help_throws_handled_exception_and_returns_list_of_allowed_options) {
    ProgramOptions testable;

    const char *argv[]{nullptr, "--help"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    try {
        testable.Parse(argc, const_cast<char **>(argv));
        FAIL();
    } catch (const HandledException &e) {
        ASSERT_THAT(e.what(), StartsWith("Allowed options:"));
    }
}

TEST(ProgramOptions, short_help_throws_handled_exception_and_returns_list_of_allowed_options) {
    ProgramOptions testable;

    const char *argv[]{nullptr, "-h"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    try {
        testable.Parse(argc, const_cast<char **>(argv));
        FAIL();
    } catch (const HandledException &e) {
        ASSERT_THAT(e.what(), StartsWith("Allowed options:"));
    }
}

TEST(ProgramOptions, invalid_command_throws_unhandled_exception_with_reason_in_message) {
    ProgramOptions testable;

    const char *argv[]{
        nullptr, "--command", "invalid_command", "-i", "input.txt", "-o", "output.txt", "-p", "42",
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    try {
        testable.Parse(argc, const_cast<char **>(argv));
        FAIL();
    } catch (const HandledException &e) {
        FAIL();
    } catch (const std::exception &e) {
        ASSERT_THAT(e.what(), AllOf(HasSubstr("'invalid_command'"), HasSubstr("--command")));
    }
}

TEST(ProgramOptions, invalid_short_command_throws_unhandled_exception_with_reason_in_message) {
    ProgramOptions testable;

    const char *argv[]{
        nullptr, "-c", "invalid_command", "-i", "input.txt", "-o", "output.txt", "-p", "42",
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    try {
        testable.Parse(argc, const_cast<char **>(argv));
        FAIL();
    } catch (const HandledException &e) {
        FAIL();
    } catch (const std::exception &e) {
        ASSERT_THAT(e.what(), AllOf(HasSubstr("'invalid_command'"), HasSubstr("--command")));
    }
}

TEST(ProgramOptions, command_option_is_case_sensitive) {
    ProgramOptions testable;

    const char *argv[]{
        nullptr, "-c", "ENCRYPT", "-i", "input.txt", "-o", "output.txt", "-p", "42",
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    try {
        testable.Parse(argc, const_cast<char **>(argv));
        FAIL();
    } catch (const HandledException &e) {
        FAIL();
    } catch (const std::exception &e) {
        ASSERT_THAT(e.what(), AllOf(HasSubstr("'ENCRYPT'"), HasSubstr("--command")));
    }
}

TEST(ProgramOptions, command_option_is_requried) {
    ProgramOptions testable;

    const char *argv[]{
        nullptr, "-i", "input.txt", "-o", "output.txt", "-p", "42",
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    try {
        testable.Parse(argc, const_cast<char **>(argv));
        FAIL();
    } catch (const HandledException &e) {
        FAIL();
    } catch (const std::exception &e) {
        ASSERT_THAT(e.what(), AllOf(HasSubstr("--command"), HasSubstr("required"), HasSubstr("missing")));
    }
}

TEST(ProgramOptions, parse_command_options_ENCRYPT) {
    ProgramOptions testable;

    const char *argv[]{
        nullptr, "--command", "encrypt", "-i", "input.txt", "-o", "output.txt", "-p", "42",
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    testable.Parse(argc, const_cast<char **>(argv));
    ASSERT_EQ(testable.GetCommand(), ProgramOptions::COMMAND_TYPE::ENCRYPT);
}

TEST(ProgramOptions, parse_command_options_DECRYPT) {
    ProgramOptions testable;

    const char *argv[]{
        nullptr, "--command", "decrypt", "-i", "input.txt", "-o", "output.txt", "-p", "42",
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    testable.Parse(argc, const_cast<char **>(argv));
    ASSERT_EQ(testable.GetCommand(), ProgramOptions::COMMAND_TYPE::DECRYPT);
}

TEST(ProgramOptions, parse_command_options_CHECKSUM) {
    ProgramOptions testable;

    const char *argv[]{
        nullptr, "-c", "checksum", "-i", "input.txt",
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    testable.Parse(argc, const_cast<char **>(argv));
    ASSERT_EQ(testable.GetCommand(), ProgramOptions::COMMAND_TYPE::CHECKSUM);
}

TEST(ProgramOptions, input_option_is_requried) {
    ProgramOptions testable;

    const char *argv[]{
        nullptr, "-c", "checksum", "-o", "output.txt", "-p", "42",
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    try {
        testable.Parse(argc, const_cast<char **>(argv));
        FAIL();
    } catch (const HandledException &e) {
        FAIL();
    } catch (const std::exception &e) {
        ASSERT_THAT(e.what(), AllOf(HasSubstr("--input"), HasSubstr("required"), HasSubstr("missing")));
    }
}

TEST(ProgramOptions, output_option_is_optional_when_command_is_checksum) {
    ProgramOptions testable;

    const char *argv[]{
        nullptr, "-c", "checksum", "-i", "input.txt", "-p", "42",
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    ASSERT_NO_THROW(testable.Parse(argc, const_cast<char **>(argv)));
    ASSERT_EQ(testable.GetOutputFile(), "");
}

TEST(ProgramOptions, output_option_is_requried_when_command_is_encrypt) {
    ProgramOptions testable;

    const char *argv[]{
        nullptr, "-c", "encrypt", "-i", "input.txt", "-p", "42",
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    try {
        testable.Parse(argc, const_cast<char **>(argv));
        FAIL();
    } catch (const HandledException &e) {
        FAIL();
    } catch (const std::exception &e) {
        ASSERT_THAT(e.what(), AllOf(HasSubstr("--output"), HasSubstr("required"), HasSubstr("missing")));
    }
}

TEST(ProgramOptions, output_option_is_requried_when_command_is_decrypt) {
    ProgramOptions testable;

    const char *argv[]{
        nullptr, "-c", "decrypt", "-i", "input.txt", "-p", "42",
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    try {
        testable.Parse(argc, const_cast<char **>(argv));
        FAIL();
    } catch (const HandledException &e) {
        FAIL();
    } catch (const std::exception &e) {
        ASSERT_THAT(e.what(), AllOf(HasSubstr("--output"), HasSubstr("required"), HasSubstr("missing")));
    }
}

TEST(ProgramOptions, password_option_is_optional_when_command_is_checksum) {
    ProgramOptions testable;

    const char *argv[]{
        nullptr, "--command", "checksum", "-i", "input.txt", "-o", "output.txt",
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    ASSERT_NO_THROW(testable.Parse(argc, const_cast<char **>(argv)));
    ASSERT_EQ(testable.GetPassword(), "");
}

TEST(ProgramOptions, password_option_is_requried_when_command_is_encrypt) {
    ProgramOptions testable;

    const char *argv[]{nullptr, "-c", "encrypt", "-i", "input.txt", "-o", "output.txt"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    try {
        testable.Parse(argc, const_cast<char **>(argv));
        FAIL();
    } catch (const HandledException &e) {
        FAIL();
    } catch (const std::exception &e) {
        ASSERT_THAT(e.what(), AllOf(HasSubstr("--password"), HasSubstr("required"), HasSubstr("missing")));
    }
}

TEST(ProgramOptions, password_option_is_requried_when_command_is_decrypt) {
    ProgramOptions testable;

    const char *argv[]{
        nullptr, "-c", "decrypt", "-i", "input.txt", "-o", "output.txt",
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    try {
        testable.Parse(argc, const_cast<char **>(argv));
        FAIL();
    } catch (const HandledException &e) {
        FAIL();
    } catch (const std::exception &e) {
        ASSERT_THAT(e.what(), AllOf(HasSubstr("--password"), HasSubstr("required"), HasSubstr("missing")));
    }
}

TEST(ProgramOptions, parse_options) {
    ProgramOptions testable;

    const char *argv[]{
        nullptr, "--command", "encrypt", "--input", "input.txt", "--output", "output.txt", "--password", "42",
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    testable.Parse(argc, const_cast<char **>(argv));
    ASSERT_EQ(testable.GetCommand(), ProgramOptions::COMMAND_TYPE::ENCRYPT);
    ASSERT_STREQ(testable.GetInputFile().c_str(), "input.txt");
    ASSERT_STREQ(testable.GetOutputFile().c_str(), "output.txt");
    ASSERT_STREQ(testable.GetPassword().c_str(), "42");
}

TEST(ProgramOptions, parse_unordered_options) {
    ProgramOptions testable;

    const char *argv[]{
        nullptr, "--output", "output.txt", "--input", "input.txt", "--command", "encrypt", "--password", "42",
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    testable.Parse(argc, const_cast<char **>(argv));
    ASSERT_EQ(testable.GetCommand(), ProgramOptions::COMMAND_TYPE::ENCRYPT);
    ASSERT_STREQ(testable.GetInputFile().c_str(), "input.txt");
    ASSERT_STREQ(testable.GetOutputFile().c_str(), "output.txt");
    ASSERT_STREQ(testable.GetPassword().c_str(), "42");
}

TEST(ProgramOptions, parse_short_options) {
    ProgramOptions testable;

    const char *argv[]{
        nullptr, "-c", "encrypt", "-i", "input.txt", "-o", "output.txt", "-p", "42",
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    testable.Parse(argc, const_cast<char **>(argv));
    ASSERT_EQ(testable.GetCommand(), ProgramOptions::COMMAND_TYPE::ENCRYPT);
    ASSERT_STREQ(testable.GetInputFile().c_str(), "input.txt");
    ASSERT_STREQ(testable.GetOutputFile().c_str(), "output.txt");
    ASSERT_STREQ(testable.GetPassword().c_str(), "42");
}