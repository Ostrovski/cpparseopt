## CPParseOpt - simple but powerful way to parse command line arguments
- It's simple!
- It's object oriented approach.
- Reading docs is not required, just look at your IDE autocomplete.

### Example
    #include "cpparseopt.h"
    #include <iostream>

    using namespace cpparseopt;

    int main(int argc, char *argv[]) {
        Pattern pattern;
        Pattern::chain(pattern)
                .arg("arg1")
                .arg("arg2").defaultVal("default2")
                .arg("arg3").descr("descr3")
                .arg("arg4").defaultVal("default4").descr("descr4")
                .arg("arg5").descr("descr5").defaultVal("default5")
                .arg()
                .opt("-b").alias("--baz").alias("--bazar").descr("And here we go!")
                .flag("-f").alias("--foo").desc("So simple!");
    
        std::cout << pattern.usage() << std::endl;
        
        ParseResult arguments = parse(argc, argv, pattern);
        std::cout << arguments.getArg("arg1").asString() << std::endl;
        std::cout << arguments.getArg(5).asDouble() << std::endl;
        std::cout << arguments.getOpt("-b").asInt() << std::endl;
        std::cout << arguments.getFlag("--foo") << std::endl;
    }
    
### Version 0.0.1 (under construction)    