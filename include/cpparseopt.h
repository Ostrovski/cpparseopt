#ifndef CPPARSEOPT_CPPARSEOPT_H
#define CPPARSEOPT_CPPARSEOPT_H

#include <string>
#include <vector>

namespace cpparseopt {
    //    class ParseResult {
    //    public:
    //        std::string getArg   (const size_t pos);
    //        int         getArgInt(const size_t pos);
    //        bool        getFlag  (const std::string &flag);
    //        std::string getOpt   (const std::string &opt);
    //        int         getOptInt(const std::string &opt);
    //    };

    // Arg - pos, name, default val, description.
    //       Default is used only if argument is not present.
    // flag - name, [alias, [alias, ...]], description.
    //        Default is always false, i.e. flag is not present.
    // Opt - name, [alias, [alias, ...]], description.
    //       Default is used only if the option is present.

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


    class PatternBuilder;

    class Pattern {
        friend class PatternBuilder;

        Arguments arguments_;
        Flags flags_;
        Options options_;
    public:
        str_t usage();

    private:
        Argument &addArg(const str_t &name);
        Flag     &addFlag(const str_t &name);
        Option   &addOpt(const str_t &name);
    };


    class ArgBuilder;
    class FlagBuilder;
    class OptBuilder;

    class PatternBuilder {
    protected:
        Pattern &pattern_;
    public:
        PatternBuilder(Pattern &pattern);
        ArgBuilder  arg(const str_t &name = "");
        FlagBuilder flag(const str_t &name);
        OptBuilder  opt(const str_t &name);
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
        AliasBuilder<T> alias(const str_t &name);
    };


    class FlagBuilder: public PatternBuilder {
        Flag &flag_;
    public:
        FlagBuilder(Flag &flag, Pattern &pattern);
        FlagBuilder alias(const str_t &name);
        AliasBuilder<Flag> descr(const str_t &descr);
    };


    class OptDescrBuilder;
    class OptValueBuilder;

    class OptBuilder: public PatternBuilder {
        Option &option_;
    public:
        OptBuilder(Option &option, Pattern &pattern);
        OptBuilder alias(const str_t &name);
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
}

#endif //CPPARSEOPT_CPPARSEOPT_H
