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
        str_t val_;
        str_t default_;
        bool hasVal_;
        bool hasDefault_;

    public:
        ParamValued();

        void setVal(const str_t &val);
        bool hasVal() const;

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

    typedef std::vector<Argument> Arguments;


    class Flag: public ParamGeneric, public ParamAliased {
    public:
        Flag(const str_t &name);
    };

    typedef std::vector<Flag> Flags;


    class Option: public ParamGeneric, public ParamAliased, public ParamValued {
    public:
        Option(const str_t &name);
    };

    typedef std::vector<Option> Options;


    class CmdLineParser;
    class PatternBuilder;

    class Pattern {
        friend class CmdLineParser;
        friend class PatternBuilder;

        Arguments arguments_;
        Flags     flags_;
        Options   options_;

        bool parsed_;
    public:
        Pattern();

        void parse(int argc, char **argv);

        // Next methods raise an exception in case of parsed_ == false;
        // Also raise an exception in case of unknown param name/pos.
        const Argument &getArg(size_t pos) const;
        const Argument &getArg(const str_t &name) const;
        const Option &getOpt(const str_t &name) const;
        bool hasFlag() const;

        str_t usage() const;

    private:
        Argument &addArg(const str_t &name);
        Flag     &addFlag(const str_t &name);
        Option   &addOpt(const str_t &name);
        void     registerAlias(Flag &flag, const str_t &alias);
        void     registerAlias(Option &option, const str_t &alias);

        // Next methods raise an exception only in case of unknown name/pos.
        // Argument &getArgRaw(size_t pos);
        // Option &getOptRaw(const str_t &name);
        // Flag &getFlagRaw(const str_t &name);
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


    class CmdLineParser {
        // Arg - pos, name, default val, description.
        //       Default is used only if argument is not present.
        // flag - name, [alias, [alias, ...]], description.
        //        Default is always false, i.e. flag is not present.
        // Opt - name, [alias, [alias, ...]], description.
        //       Default is used only if the option is present.

        int argc_;
        char **argv_;
        int paramCounter_;
        Pattern &pattern_;
    public:
        CmdLineParser(int argc, char **argv, Pattern &pattern);

        void parse();
        const char *getCurrentParam();
        bool        hasNextParam();
        void        nextParam();
        bool isFlagParam();
        bool isOptParam();
    };
}

#endif //CPPARSEOPT_CPPARSEOPT_H
