#ifndef CPPARSEOPT_CPPARSEOPT_H
#define CPPARSEOPT_CPPARSEOPT_H

#include <string>
#include <vector>

namespace cpparseopt {
    typedef std::string str_t;

    class ParamGeneric {
        str_t descr_;

    protected:
        std::vector<str_t> names_;

    public:
        ParamGeneric(const str_t &name = "");

        bool hasName(const str_t &name) const;

        const str_t &getDescr() const;
        void setDescr(const str_t &descr);
    };


    class ParamAliased : public ParamGeneric {
    public:
        ParamAliased(const str_t &name);
        void addAlias(const str_t &alias);
    };


    class ParamValued {
        str_t default_;
        bool hasDefault_;

    public:
        ParamValued();

        const str_t &getDefault() const;
        bool hasDefault() const;
        void setDefault(const str_t &val);
    };


    class Argument : public ParamGeneric, public ParamValued {
        // Just a positional argument. Can have human-readable name.
        size_t pos_;
    public:
        Argument(size_t pos, const str_t &name = "");
        size_t getPos() const;
    };


    class Flag : public ParamAliased {
        // Boolean flag. Exists or not. Without value.
        // Examples:
        //      -f / --foo / -F / --FOO
    public:
        Flag(const str_t &name);
    };


    class Option : public ParamAliased, public ParamValued {
        // Looks like a flag, but with value. Can have default value.
        // Examples:
        //      -f / --foo / -F / --FOO  (default value must be provided within the pattern!)
        //      --foo[=<fVal>]           (the way to override default value)
        //      -f <fVal> / --foo <fVal> (an opt without default value)
    public:
        Option(const str_t &name);
    };


    class CmdLineParams;
    class PatternBuilder;

    class Pattern {
        // Pattern is immutable. Can be constructed only through PatternBuilder.
        friend class PatternBuilder;

        typedef std::vector<Argument> Arguments;
        typedef std::vector<Flag> Flags;
        typedef std::vector<Option> Options;

        Arguments arguments_;
        Flags     flags_;
        Options   options_;
    public:
        CmdLineParams match(int argc, char **argv) const;
        void          match(int argc, char **argv, CmdLineParams &dst) const;

        // Next methods raise exceptions in case of unknown param name/pos.
        const Argument &getArg(size_t pos) const;
        bool            hasArg(size_t pos) const;
        const Argument &getArg(const str_t &name) const;
        bool            hasArg(const str_t &name) const;

        const Option   &getOpt(const str_t &name) const;
        bool            hasOpt(const str_t &name) const;

        const Flag     &getFlag(const str_t &name) const;
        bool            hasFlag(const str_t &name) const;

        str_t usage() const;

    private:
        Argument &addArg(const str_t &name);
        Flag     &addFlag(const str_t &name);
        Option   &addOpt(const str_t &name);
        void     registerAlias(Flag &flag, const str_t &alias);
        void     registerAlias(Option &option, const str_t &alias);
    };


    class ArgBuilder;
    class FlagBuilder;
    class OptBuilder;

    class PatternBuilder {
        // Important! All classes in the Builder's hierarchy must not
        // contain any fields, except Pattern& and Param&.
        // This fields must not be owned (i.e. must be references).
    protected:
        Pattern &pattern_;

    public:
        PatternBuilder(Pattern &pattern);
        ArgBuilder  arg(const str_t &name = "");
        FlagBuilder flag(const str_t &name);
        OptBuilder  opt(const str_t &name);

    protected:
        void registerAlias(Flag &flag, const str_t &alias);
        void registerAlias(Option &option, const str_t &alias);
    };


    class ArgDescrBuilder;
    class ArgValueBuilder;

    class ArgBuilder : public PatternBuilder {
        Argument &arg_;
    public:
        ArgBuilder(Argument &arg, Pattern &pattern);
        ArgDescrBuilder defaultVal(const str_t &val);
        ArgValueBuilder descr(const str_t &descr);
    };

    class ArgDescrBuilder : public PatternBuilder {
        Argument &arg_;
    public:
        ArgDescrBuilder(Argument &arg, Pattern &pattern);
        PatternBuilder descr(const str_t &descr);
    };

    class ArgValueBuilder : public PatternBuilder {
        Argument &arg_;
    public:
        ArgValueBuilder(Argument &arg, Pattern &pattern);
        PatternBuilder defaultVal(const str_t &val);
    };


    template<typename T>
    class AliasBuilder : public PatternBuilder {
        T &param_;
    public:
        AliasBuilder(T &param, Pattern &pattern);
        AliasBuilder<T> alias(const str_t &alias);
    };


    class FlagBuilder : public PatternBuilder {
        Flag &flag_;
    public:
        FlagBuilder(Flag &flag, Pattern &pattern);
        FlagBuilder alias(const str_t &alias);
        AliasBuilder<Flag> descr(const str_t &descr);
    };


    class OptDescrBuilder;
    class OptValueBuilder;

    class OptBuilder : public PatternBuilder {
        Option &option_;
    public:
        OptBuilder(Option &option, Pattern &pattern);
        OptBuilder alias(const str_t &alias);
        OptDescrBuilder defaultVal(const str_t &val);
        OptValueBuilder descr(const str_t &descr);
    };

    class OptDescrBuilder : public PatternBuilder {
        Option &option_;
    public:
        OptDescrBuilder(Option &option, Pattern &pattern);
        AliasBuilder<Option> descr(const str_t &descr);
    };

    class OptValueBuilder : public PatternBuilder {
        Option &option_;
    public:
        OptValueBuilder(Option &option, Pattern &pattern);
        AliasBuilder<Option> defaultVal(const str_t &val);
    };


    class ParsedParam {
        // Value-object pattern.
        const str_t val_;
    public:
        ParsedParam(const str_t &val = "");
        operator std::string() const;
        const str_t &asString() const;
        // TODO:
        // int          asInt() const;
        // double       asDouble() const;
        // asTime(), etc...
    };


    class ParsedArgParam : public ParsedParam {
        const Argument &argument_;
    public:
        ParsedArgParam(const Argument &argument, const str_t &val);
        const Argument &getArg() const;
    };

    class CmdLineParamsParser;

    class CmdLineParams {
        friend class CmdLineParamsParser;

        typedef std::vector<ParsedArgParam> ArgParams;
        typedef std::vector<ParsedArgParam>::const_iterator ArgParamsIter;

        const Pattern &pattern_;
        ArgParams arguments_;
        // FlagParams flags_
        // OptParams options_
    public:
        CmdLineParams(const Pattern &pattern);
// На этом этапе нужно проверять только валидность имен/позиций для get*()-методов.
// Значение (явно переданное или дефолтное) уже точно установлено во время
// парсинга.
        const ParsedParam &getArg(const str_t &name) const;
        const ParsedParam &getArg(size_t pos) const;
        //const ParsedParam &getOpt(const str_t &name) const;
        //bool                    hasFlag(const str_t &name) const;
        const Pattern &getPattern() const;
    };


    class CmdLineParamsParser {
        // Arg - pos, name, default val, description.
        //       Default is used only if argument is not present.
        // flag - name, [alias, [alias, ...]], description.
        //        Default is always false, i.e. flag is not present.
        // Opt - name, [alias, [alias, ...]], description.
        //       Default is used only if the option is present.

        int argc_;
        char **argv_;
        int paramCounter_;
        CmdLineParams *params_;
    public:
        CmdLineParamsParser();
        void parse(int argc, char **argv, CmdLineParams &dst);

    private:
        const char *currentParam();
        bool        hasNextParam();
        const char *nextParam();

        bool isFlagParam(const char *param);
        bool isOptParam(const char *param);

        void parseArg(const char *param);
        void parseFlag();
        void parseOpt();

        void reset(int argc, char **argv, CmdLineParams &dst);
    };


    class Exception : public std::runtime_error {
    public:
        Exception(const std::string &msg);
    };
}

#endif //CPPARSEOPT_CPPARSEOPT_H
