#include "../include/cpparseopt.h"
#include <iostream>

using namespace cpparseopt;

void Test__PatternBuilder_1() {
    Pattern pattern;
    PatternBuilder(pattern).arg("arg1");
}

void TestSuite__PatternBuilder() {
    Test__PatternBuilder_1();
}


void Test__Parser_1() {
//    Pattern pattern;
//    PatternBuilder(pattern).arg("arg1");
//
//    const char *argv[] = {"param0"};
//    CmdLineParams params = pattern.match(1, argv);
}

void TestSuite__Parser() {
    Test__Parser_1();
}


int main(int argc, char *argv[]) {
    Pattern pattern;
    PatternBuilder(pattern).arg("arg1");

    //const char *argv[] = {"param0"};
    CmdLineParams params = pattern.match(1, argv);

    TestSuite__PatternBuilder();
    TestSuite__Parser();
}