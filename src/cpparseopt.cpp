#include "../include/cpparseopt.h"
#include <algorithm>
#include <cassert>

using namespace cpparseopt;


ParamGeneric::ParamGeneric(const str_t &name)
        : names_(1, name) {
}

bool ParamGeneric::hasName(const str_t &name) const {
    return std::find(names_.begin(), names_.end(), name) != names_.end();
}

const str_t &ParamGeneric::getDescr() const {
    return descr_;
}

void ParamGeneric::setDescr(const str_t &descr) {
    descr_ = descr;
}


ParamAliased::ParamAliased(const str_t &name)
        : ParamGeneric(name) {

}

void ParamAliased::addAlias(const str_t &alias) {
    // TODO: check collision with other aliases
    names_.push_back(alias);
}


ParamValued::ParamValued()
        : hasDefault_(false) {
}

const str_t &ParamValued::getDefault() const {
    assert(hasDefault());
    return default_;
}

bool ParamValued::hasDefault() const {
    return hasDefault_;
}

void ParamValued::setDefault(const str_t &val) {
    assert(!hasDefault());
    default_ = val;
    hasDefault_ = true;
}


Argument::Argument(size_t pos, const str_t &name)
        : ParamGeneric(name), pos_(pos) {
    // TODO: check name format
}

size_t Argument::getPos() const {
    return pos_;
}


Flag::Flag(const str_t &name)
        : ParamAliased(name) {
    // TODO: check name format
}


Option::Option(const str_t &name)
        : ParamAliased(name) {
    // TODO: check name format
}


CmdLineParams Pattern::match(int argc, char **argv) const {
    CmdLineParams result(*this);
    match(argc, argv, result);
    return result;
}

void Pattern::match(int argc, char **argv, CmdLineParams &dst) const {
    if (&dst.getPattern() != this) {
        throw 1; // TODO: ...
    }
    CmdLineParamsParser parser;
    parser.parse(argc, argv, dst);
}

const Argument &Pattern::getArg(size_t pos) const {
    // TODO: try ... catch
    return arguments_.at(pos);
}

class ArgPosMatcher
{
    size_t pos_;
public:
    ArgPosMatcher(size_t pos) : pos_(pos) {}
    bool operator()(const Argument &arg) { return pos_ == arg.getPos(); }
};

bool Pattern::hasArg(size_t pos) const {
    return std::find_if(arguments_.begin(),
                        arguments_.end(),
                        ArgPosMatcher(pos)) != arguments_.end();
}

const Argument &Pattern::getArg(const str_t &name) const {
    // TODO: implement me!
    return arguments_.back();
}

bool Pattern::hasArg(const str_t &name) const {
    return false;
}

const Option &Pattern::getOpt(const str_t &name) const {
    // TODO: implement me!
    return options_.back();
}


bool Pattern::hasOpt(const str_t &name) const {
    return false;
}

const Flag &Pattern::getFlag(const str_t &name) const {
    // TODO: implement me!
    return flags_.back();
}

bool Pattern::hasFlag(const str_t &name) const {
    return false;
}

str_t Pattern::usage() const {
    return "Usage here";
}

Argument &Pattern::addArg(const str_t &name) {
    // TODO: add name collision check
    arguments_.push_back(Argument(arguments_.size(), name));
    return arguments_.back();
}

Flag &Pattern::addFlag(const str_t &name) {
    // TODO: add name collision check
    flags_.push_back(Flag(name));
    return flags_.back();
}

Option &Pattern::addOpt(const str_t &name) {
    // TODO: add name collision check
    options_.push_back(Option(name));
    return options_.back();
}

void Pattern::registerAlias(Flag &flag, const str_t &alias) {
    // TODO: add collision check with other flags & options.
    flag.addAlias(alias);
}

void Pattern::registerAlias(Option &option, const str_t &alias) {
    // TODO: add collision check with other options & flags.
    option.addAlias(alias);
}


PatternBuilder::PatternBuilder(Pattern &pattern)
        : pattern_(pattern) {
}

ArgBuilder PatternBuilder::arg(const str_t &name) {
    return ArgBuilder(pattern_.addArg(name), pattern_);
}

FlagBuilder PatternBuilder::flag(const str_t &name) {
    return FlagBuilder(pattern_.addFlag(name), pattern_);
}

OptBuilder PatternBuilder::opt(const str_t &name) {
    return OptBuilder(pattern_.addOpt(name), pattern_);
}

void PatternBuilder::registerAlias(Flag &flag, const str_t &alias) {
    pattern_.registerAlias(flag, alias);
}

void PatternBuilder::registerAlias(Option &option, const str_t &alias) {
    pattern_.registerAlias(option, alias);
}


ArgBuilder::ArgBuilder(Argument &arg, Pattern &pattern)
        : PatternBuilder(pattern), arg_(arg) {
}

ArgDescrBuilder ArgBuilder::defaultVal(const str_t &val) {
    arg_.setDefault(val);
    return ArgDescrBuilder(arg_, pattern_);
}

ArgValueBuilder ArgBuilder::descr(const str_t &descr) {
    arg_.setDescr(descr);
    return ArgValueBuilder(arg_, pattern_);
}


ArgDescrBuilder::ArgDescrBuilder(Argument &arg, Pattern &pattern)
        : PatternBuilder(pattern), arg_(arg) {
}

PatternBuilder ArgDescrBuilder::descr(const str_t &descr) {
    arg_.setDescr(descr);
    return PatternBuilder(pattern_);
}


ArgValueBuilder::ArgValueBuilder(Argument &arg, Pattern &pattern)
        : PatternBuilder(pattern), arg_(arg) {
}

PatternBuilder ArgValueBuilder::defaultVal(const str_t &val) {
    arg_.setDefault(val);
    return PatternBuilder(pattern_);
}


template<typename T>
AliasBuilder<T>::AliasBuilder(T &param, Pattern &pattern)
        : PatternBuilder(pattern), param_(param) {
}

template<typename T>
AliasBuilder<T> AliasBuilder<T>::alias(const str_t &alias) {
    registerAlias(param_, alias);
    return AliasBuilder(param_, pattern_);
}

template class AliasBuilder<Flag>;
template class AliasBuilder<Option>;


FlagBuilder::FlagBuilder(Flag &flag, Pattern &pattern)
        : PatternBuilder(pattern), flag_(flag) {
}

FlagBuilder FlagBuilder::alias(const str_t &alias) {
    registerAlias(flag_, alias);
    return FlagBuilder(flag_, pattern_);
}


AliasBuilder<Flag> FlagBuilder::descr(const str_t &descr) {
    flag_.setDescr(descr);
    return AliasBuilder<Flag>(flag_, pattern_);
}


OptBuilder::OptBuilder(Option &option, Pattern &pattern)
        : PatternBuilder(pattern), option_(option) {
}

OptBuilder OptBuilder::alias(const str_t &alias) {
    registerAlias(option_, alias);
    return OptBuilder(option_, pattern_);
}


OptDescrBuilder OptBuilder::defaultVal(const str_t &val) {
    option_.setDefault(val);
    return OptDescrBuilder(option_, pattern_);
}

OptValueBuilder OptBuilder::descr(const str_t &descr) {
    option_.setDescr(descr);
    return OptValueBuilder(option_, pattern_);
}


OptDescrBuilder::OptDescrBuilder(Option &option, Pattern &pattern)
        : PatternBuilder(pattern), option_(option) {
}

AliasBuilder<Option> OptDescrBuilder::descr(const str_t &descr) {
    option_.setDescr(descr);
    return AliasBuilder<Option>(option_, pattern_);
}


OptValueBuilder::OptValueBuilder(Option &option, Pattern &pattern)
        : PatternBuilder(pattern), option_(option) {
}

AliasBuilder<Option> OptValueBuilder::defaultVal(const str_t &val) {
    option_.setDefault(val);
    return AliasBuilder<Option>(option_, pattern_);
}


ParsedParam::ParsedParam(const str_t &val)
        : val_(val) {
}

ParsedParam::operator std::string() const {
    return str_t(asString());
}

const str_t &ParsedParam::asString() const {
    return val_;
}


ParsedArgParam::ParsedArgParam(const Argument &argument, const str_t &val)
        : ParsedParam(val), argument_(argument) {
}

const Argument &ParsedArgParam::getArg() const {
    return argument_;
}


CmdLineParams::CmdLineParams(const Pattern &pattern)
        : pattern_(pattern) {
}

const ParsedParam &CmdLineParams::getArg(const str_t &name) const {
    for (ArgParamsIter it = arguments_.begin(); it != arguments_.end(); ++it) {
        if ((*it).getArg().hasName(name)) {
            return *it;
        }
    }

    throw 1;  // TODO: ...
}

const ParsedParam &CmdLineParams::getArg(size_t pos) const {
    for (ArgParamsIter it = arguments_.begin(); it != arguments_.end(); ++it) {
        if ((*it).getArg().getPos() == pos) {
            return *it;
        }
    }

    throw 1;  // TODO: ...
}

const Pattern &CmdLineParams::getPattern() const {
    return pattern_;
}


CmdLineParamsParser::CmdLineParamsParser()
        : paramCounter_(0), argc_(0), argv_(0), params_(0) {
}

void CmdLineParamsParser::parse(int argc, char **argv, CmdLineParams &dst) {
    reset(argc, argv, dst);

    // На этом этапе нужно отловить все неожидаемые параметры и
    // все недопереданные параметры (т.е. те opts и args, для которых не заданы
    // default val в pattern).
    while (hasNextParam()) {
        const char *param = nextParam();
        if (isFlagParam(param)) {
            parseFlag();
            continue;
        }

        if (isOptParam(param)) {
            parseOpt();
            continue;
        }

        parseArg(param);
    }
}

const char *CmdLineParamsParser::currentParam() {
    if (paramCounter_ < argc_) {
        return argv_[paramCounter_];
    }
    throw 1;  // TODO: ...
}

bool CmdLineParamsParser::hasNextParam() {
    return (paramCounter_ + 1 < argc_);
}

const char *CmdLineParamsParser::nextParam() {
    if (hasNextParam()) {
        paramCounter_++;
        return currentParam();
    }
    throw 1;  // TODO: ...
}

bool CmdLineParamsParser::isFlagParam(const char *param) {
    return false; // TODO: params_->getPattern().hasFlag(currentParam());
}

bool CmdLineParamsParser::isOptParam(const char *param) {
    return false; // TODO: params_->getPattern().hasOpt(currentParam());
}

void CmdLineParamsParser::parseArg(const char *param) {
    if (!params_->getPattern().hasArg(params_->arguments_.size())) {
        throw 1;  // TODO: ...
    }
    // TODO: beautify!
    params_->arguments_.push_back(ParsedArgParam(params_->getPattern().getArg(params_->arguments_.size()),
                                                 param));
}

void CmdLineParamsParser::parseFlag() {
    // TODO: this.flags.set(currentParam)
}

void CmdLineParamsParser::parseOpt() {

}

void CmdLineParamsParser::reset(int argc, char **argv, CmdLineParams &dst) {
    argc_ = argc;
    argv_ = argv;
    params_ = &dst;
    paramCounter_ = 0;
}
