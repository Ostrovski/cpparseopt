#include "../include/cpparseopt.h"
#include <iostream>

using namespace cpparseopt;


int main(int argc, char **argv) {
    Pattern pattern;
    PatternBuilder(pattern)
            .arg("arg1")
            .arg("arg2").defaultVal("default2")
            .arg("arg3").descr("descr3")
            .arg("arg4").defaultVal("default4").descr("descr4")
            .arg("arg5").descr("descr5").defaultVal("default5")
            .arg()
            .flag("-f").descr("descr1").alias("--foo").alias("--foobar")
            .flag("-F").alias("--foo").descr("descr2").alias("--FOO")
            .opt("-b").alias("--baz").alias("--bazar").descr("And here we go!");

    std::cout << pattern.usage() << std::endl;

    CmdLineParams parsed = pattern.match(argc, argv);

    return 0;
}