#include "../include/cpparseopt.h"
#include <iostream>

#define STOP_ON_ERR 0

#define ASSERT_EQ(e, a) assertEquals((e), (a), STOP_ON_ERR, __FILE__, __LINE__)
#define ASSERT(v) ASSERT_EQ(true, (v))
#define _THROWS(expression, ET, throws) do {                                   \
                                            try {                              \
                                                (expression); ASSERT(!throws); \
                                            } catch(const ET &e) {             \
                                                std::cout << e.what()          \
                                                    << std::endl;              \
                                                ASSERT(throws);                \
                                            }                                  \
                                        } while(false)
#define ASSERT_THROWS(expression, ET) _THROWS(expression, ET, true)
#define ASSERT_NOTHROW(expression, ET) _THROWS(expression, ET, false)

using namespace cpparseopt;


// == begin: Utils
template<typename T>
void assertEquals(const T &expected, const T &actual, bool stopOnFailure,
                  const char *file, size_t line) {
    bool ok = (expected == actual);
    if (!ok) {
        std::cout << "Assertion failed: ['" <<
                expected << "' != '" << actual << "']" << std::endl <<
                "    on " << file << ":" << line << std::endl;
        if (stopOnFailure) {
            exit(1);
        }
    } else {
        std::cout << ".";
    }
}

template<typename T, size_t N>
inline
size_t sizeOfArray(const T(&)[N]) {
    return N;
}
// == end of: Utils


void Test__PatternBuilder__SimpleArg() {
    Pattern pattern;
    PatternBuilder(pattern).arg("arg");

    size_t pos = 0;
    const Argument &argByPos = pattern.getArg(pos);
    const Argument &argByName = pattern.getArg("arg");

    ASSERT(&argByPos == &argByName);
    ASSERT_EQ(pos, argByPos.getPos());
    ASSERT(argByPos.hasName("arg"));
    ASSERT(!argByPos.hasDefault());
}

void Test__PatternBuilder__AnonymousArg() {
    Pattern pattern;
    PatternBuilder(pattern).arg();
    ASSERT(pattern.hasArg(0));
    ASSERT(!pattern.hasArg(""));
}

void Test__PatternBuilder__NameFormats() {
    Pattern pattern;
    ASSERT_THROWS(PatternBuilder(pattern).arg(""), Exception);
    ASSERT_THROWS(PatternBuilder(pattern).arg("arg name"), Exception);
    ASSERT_THROWS(PatternBuilder(pattern).arg("-arg"), Exception);
    ASSERT_THROWS(PatternBuilder(pattern).arg("--arg"), Exception);
    ASSERT_NOTHROW(PatternBuilder(pattern).arg("arg"), Exception);

    const char* badNames[] = {"", "p", "param", "param name",
                              "-param", "--p", "---p", "---param"};
    for (int i = 0; i < sizeOfArray(badNames); i++) {
        ASSERT_THROWS(PatternBuilder(pattern).flag(badNames[i]), Exception);
        ASSERT_THROWS(PatternBuilder(pattern).opt(badNames[i]), Exception);
    }

    const char* goodNames[] = {"-1", "-p", "-P", "--param",
                               "--Param", "--PARAM"};
    for (int i = 0; i < sizeOfArray(goodNames); i++) {
        ASSERT_NOTHROW(PatternBuilder(pattern).flag(badNames[i]), Exception);
        ASSERT_NOTHROW(PatternBuilder(pattern).opt(badNames[i]), Exception);
    }
}

void TestSuite__PatternBuilder() {
    std::cout << "Test Suite: PatternBuilder" << std::endl;

    Test__PatternBuilder__SimpleArg();
    Test__PatternBuilder__AnonymousArg();
    Test__PatternBuilder__NameFormats();

    std::cout << std::endl;
}


void Test__Parser__SimpleArgs() {
    Pattern pattern;
    PatternBuilder(pattern).arg("arg0").arg().arg("arg2");

    const char *argv[] = {"/path/to/bin", "param0", "param1", "param2"};
    CmdLineParams params = pattern.match(static_cast<int>(sizeOfArray(argv)),
                                         const_cast<char **>(argv));

    ASSERT_EQ(str_t("param0"), str_t(params.getArg(0)));
    ASSERT_EQ(str_t("param0"), str_t(params.getArg("arg0")));

    ASSERT_EQ(str_t("param1"), str_t(params.getArg(1)));
    ASSERT_THROWS(params.getArg("arg1"), Exception);

    ASSERT_EQ(str_t("param2"), str_t(params.getArg(2)));
    ASSERT_EQ(str_t("param2"), str_t(params.getArg("arg2")));

    ASSERT_THROWS(params.getArg(3), Exception);
}

void Test__Parser__SimpleFlags() {

}

void TestSuite__Parser() {
    std::cout << "Test Suite: Parser" << std::endl;

    Test__Parser__SimpleArgs();
    Test__Parser__SimpleFlags();

    std::cout << std::endl;
}


int main(int argc, char *argv[]) {
    TestSuite__PatternBuilder();

    std::cout << std::endl;

    TestSuite__Parser();
}