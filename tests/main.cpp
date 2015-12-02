#include "../include/cpparseopt.h"
#include <iostream>
#include <cassert>

using namespace cpparseopt;


// == begin: Utils
void assertEquals(const std::string &expected, const std::string &actual) {
    assert(expected == actual);
}

template<typename T, size_t N>
inline
size_t sizeOfArray(const T(&)[N]) {
    return N;
}
// == end of: Utils

void Test__PatternBuilder_1() {
    Pattern pattern;
    PatternBuilder(pattern).arg("arg");
}

void TestSuite__PatternBuilder() {
    Test__PatternBuilder_1();
}


void Test__Parser__SimpleArg() {
    Pattern pattern;
    PatternBuilder(pattern).arg("arg");

    const char *argv[] = {"/path/to/bin", "param0"};
    CmdLineParams params = pattern.match(static_cast<int>(sizeOfArray(argv)),
                                         const_cast<char **>(argv));

    assertEquals("arg", params.getArg(0));
    assertEquals("arg", params.getArg("arg"));
}

void TestSuite__Parser() {
    Test__Parser__SimpleArg();
}


int main(int argc, char *argv[]) {
    TestSuite__PatternBuilder();
    TestSuite__Parser();
}