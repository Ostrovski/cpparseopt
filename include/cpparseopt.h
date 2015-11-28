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


    class Builder;

    class Pattern {
        friend class Builder;

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

    class Builder {
    protected:
        Pattern &pattern_;
    public:
        Builder(Pattern &pattern);
        ArgBuilder  arg(const str_t &name = "");
        FlagBuilder flag(const str_t &name);
        OptBuilder  opt(const str_t &name);
    };


    class ArgDescrBuilder;
    class ArgValueBuilder;

    class ArgBuilder: public Builder {
        Argument &arg_;
    public:
        ArgBuilder(Argument &arg, Pattern &pattern);
        ArgDescrBuilder defaultVal(const str_t &val);
        ArgValueBuilder descr(const str_t &descr);
    };

    class ArgDescrBuilder: public Builder {
        Argument &arg_;
    public:
        ArgDescrBuilder(Argument &arg, Pattern &pattern);
        Builder descr(const str_t &descr);
    };

    class ArgValueBuilder: public Builder {
        Argument &arg_;
    public:
        ArgValueBuilder(Argument &arg, Pattern &pattern);
        Builder defaultVal(const str_t &val);
    };


    class FlagAliasBuilder;

    class FlagBuilder: public Builder {
        Flag &flag_;
    public:
        FlagBuilder(Flag &flag, Pattern &pattern);
        FlagBuilder alias(const str_t &name);
        FlagAliasBuilder descr(const str_t &descr);
    };

    class FlagAliasBuilder: public Builder {
        Flag &flag_;
    public:
        FlagAliasBuilder(Flag &flag, Pattern &pattern);
        FlagAliasBuilder alias(const str_t &name);
    };


    class OptBuilder: public Builder {
        Option &option_;
    public:
        OptBuilder(Option &option, Pattern &pattern);
    };
}

#endif //CPPARSEOPT_CPPARSEOPT_H
