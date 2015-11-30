#ifndef CPPARSEOPT_CPPARSEOPT_H
#define CPPARSEOPT_CPPARSEOPT_H

#include <string>
#include <vector>

namespace cpparseopt {
    typedef std::string str_t;

    class ParamGeneric {
        str_t name_;
        str_t descr_;

    public:
        ParamGeneric(const str_t &name = "");

        const str_t &getDescr() const;
        void setDescr(const str_t &descr);
    };


    class ParamAliased {
        std::vector<str_t> aliases_;
    public:
        void addAlias(const str_t &name);
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


    class Argument: public ParamGeneric, public ParamValued {
        size_t pos_;
    public:
        Argument(size_t pos, const str_t &name = "");
        size_t getPos() const;
    };


    class Flag: public ParamGeneric, public ParamAliased {
    public:
        Flag(const str_t &name);
    };


    class Option: public ParamGeneric, public ParamAliased, public ParamValued {
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
        const Argument &getArg(const str_t &name) const;
        const Option   &getOpt(const str_t &name) const;
        const Flag     &getFlag(const str_t &name) const;

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

    class ArgBuilder: public PatternBuilder {
        Argument &arg_;
    public:
        ArgBuilder(Argument &arg, Pattern &pattern);
        ArgDescrBuilder defaultVal(const str_t &val);
        ArgValueBuilder descr(const str_t &descr);
    };

    class ArgDescrBuilder: public PatternBuilder {
        Argument &arg_;
    public:
        ArgDescrBuilder(Argument &arg, Pattern &pattern);
        PatternBuilder descr(const str_t &descr);
    };

    class ArgValueBuilder: public PatternBuilder {
        Argument &arg_;
    public:
        ArgValueBuilder(Argument &arg, Pattern &pattern);
        PatternBuilder defaultVal(const str_t &val);
    };


    template<typename T>
    class AliasBuilder: public PatternBuilder {
        T &param_;
    public:
        AliasBuilder(T &param, Pattern &pattern);
        AliasBuilder<T> alias(const str_t &alias);
    };


    class FlagBuilder: public PatternBuilder {
        Flag &flag_;
    public:
        FlagBuilder(Flag &flag, Pattern &pattern);
        FlagBuilder alias(const str_t &alias);
        AliasBuilder<Flag> descr(const str_t &descr);
    };


    class OptDescrBuilder;
    class OptValueBuilder;

    class OptBuilder: public PatternBuilder {
        Option &option_;
    public:
        OptBuilder(Option &option, Pattern &pattern);
        OptBuilder alias(const str_t &alias);
        OptDescrBuilder defaultVal(const str_t &val);
        OptValueBuilder descr(const str_t &descr);
    };

    class OptDescrBuilder: public PatternBuilder {
        Option &option_;
    public:
        OptDescrBuilder(Option &option, Pattern &pattern);
        AliasBuilder<Option> descr(const str_t &descr);
    };

    class OptValueBuilder: public PatternBuilder {
        Option &option_;
    public:
        OptValueBuilder(Option &option, Pattern &pattern);
        AliasBuilder<Option> defaultVal(const str_t &val);
    };


    class ValuedParamProxy {
        // Value-object pattern.
        const ParamValued &param_;
        const str_t &val_;
        const bool hasVal_;
    public:
        ValuedParamProxy(const ParamValued &param, const str_t &val);
        const str_t &asString() const;
        // TODO:
        // int          asInt() const;
        // double       asDouble() const;
        // asTime(), etc...
    };


    class CmdLineParamsParser;

    class CmdLineParams {
        friend class CmdLineParamsParser;

        const Pattern &pattern_;
        // ValuedParamProxy by name
        // ValuedParamProxy by pos
        // Flags by name/alias
    public:
        CmdLineParams(const Pattern &pattern);
// На этом этапе нужно проверять только валидность имен/позиций.
// Значение (явно переданное или дефолтное) уже точно установлено во время
// парсинга.
//        const ValuedParamProxy &getArg(const str_t &name) const;
//        const ValuedParamProxy &getArg(size_t pos) const;
//        const ValuedParamProxy &getOpt(const str_t &name) const;
//        bool                    hasFlag(const str_t &name) const;
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
        CmdLineParams &dst_;
    public:
        CmdLineParamsParser(int argc, char **argv, CmdLineParams &dst);

        void parse(CmdLineParams &dst);
        const char *getCurrentParam();
        bool        hasNextParam();
        void        nextParam();
        bool isFlagParam();
        bool isOptParam();
    };
}

#endif //CPPARSEOPT_CPPARSEOPT_H
