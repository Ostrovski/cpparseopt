#include "../include/cpparseopt.h"
#include <iostream>

using namespace cpparseopt;


int main() {
    Pattern pattern;
    Builder(pattern)
            .arg("arg1")
            .arg("arg2").defaultVal("default2")
            .arg("arg3").descr("descr3")
            .arg("arg4").defaultVal("default4").descr("descr4")
            .arg("arg5").descr("descr5").defaultVal("default5")
            .arg()
            .opt("-b")/*.alias("--baz").alias("--bazar").descr("And here we go!")*/;

    std::cout << pattern.usage() << std::endl;
    return 0;
}