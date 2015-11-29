#include "../include/cpparseopt.h"
#include <iostream>

using namespace cpparseopt;

void Test1() {
    Pattern pattern;
    PatternBuilder(pattern).arg("arg1");
}

void TestSuite__PatternBuilder() {
    Test1();
}

int main() {
    TestSuite__PatternBuilder();
}