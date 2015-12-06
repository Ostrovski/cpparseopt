#include "../include/cpparseopt.h"
#include <algorithm>
#include <cassert>

#ifdef _DEBUG
#define _THROW(ET, msg) throw ET((msg), __FILE__, __LINE__)
#else
#define _THROW(ET, msg) throw ET((msg))
#endif

using namespace cpparseopt;


ParamGeneric::ParamGeneric() {
}

ParamGeneric::ParamGeneric(const str_t &name)
        : names_(1, ensureName(name)) {
}

bool ParamGeneric::hasName(const str_t &name) const {
    return !name.empty()
           && std::find(names_.begin(), names_.end(), name) != names_.end();
}

const str_t &ParamGeneric::getDescr() const {
    return descr_;
}

void ParamGeneric::setDescr(const str_t &descr) {
    descr_ = descr;
}

const str_t &ParamGeneric::ensureName(const str_t &name) const {
    if (name.empty()) {
        _THROW(BadNameException, "Empty param name");
    }
    return name;
}


ParamAliased::ParamAliased(const str_t &name)
        : ParamGeneric(ensureName(name)) {

}

void ParamAliased::addAlias(const str_t &alias) {
    // TODO: check collision with other aliases
    names_.push_back(ensureName(alias));
}

const str_t &ParamAliased::getCanonicalName() const {
    return names_.at(0);
}

const str_t &ParamAliased::ensureName(const str_t &name) const {
    if (!(name.size() == 2 || name.size() >= 4)) {
        _THROW(BadNameException, "Bad flag/opt name [" + name + "]");
    }
    if ('-' != name[0]) {
        _THROW(BadNameException, "Bad param name [" + name + "]. Flag/opt name "
                                 "must start with one or two '-' symbols");
    }
    // Here and below name.size() >= 2
    if (name.size() == 2) {
        if ('-' != name[1]) {
            return name;
        }
    } else {
        if ('-' == name[1] && '-' != name[2]) {
            return name;
        }
    }
    _THROW(BadNameException, "Bad param name [" + name + "]");
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


Argument::Argument(size_t pos)
        : pos_(pos) {
}

Argument::Argument(size_t pos, const str_t &name)
        : ParamGeneric(ensureName(name)), pos_(pos) {
}

size_t Argument::getPos() const {
    return pos_;
}

const str_t &Argument::ensureName(const str_t &name) const {
    if (name.empty() || str_t::npos != name.find(' ') || name.find('-') == 0) {
        _THROW(BadNameException, "Bad argument name [" + name + "]");
    }
    return name;
}


Flag::Flag(const str_t &name)
        : ParamAliased(name) {
}


Option::Option(const str_t &name)
        : ParamAliased(name) {
}


CmdLineParams Pattern::match(int argc, char **argv) const {
    CmdLineParams result(*this);
    match(argc, argv, result);
    return result;
}

void Pattern::match(int argc, char **argv, CmdLineParams &dst) const {
    if (&dst.getPattern() != this) {
        _THROW(Exception, "Different patterns");
    }
    CmdLineParamsParser parser;
    parser.parse(argc, argv, dst);
}

const Argument &Pattern::getArg(size_t pos) const {
    if (pos >= arguments_.size()) {
        _THROW(UnknownParamException, "No argument at position "
                                      "[" + std::to_string(pos) + "]");
    }
    // TODO: try ... catch
    return arguments_.at(pos);
}

bool Pattern::hasArg(size_t pos) const {
    return pos < arguments_.size();
}

template<typename Container>
class NameMatcher
{
    const str_t &name_;
public:
    static typename Container::const_iterator find(const Container &params,
                                                   const str_t &name) {
        return std::find_if(params.begin(),
                            params.end(),
                            NameMatcher(name));
    }

    NameMatcher(const str_t &name) : name_(name) {}
    bool operator()(const typename Container::value_type &param) {
        return param.hasName(name_);
    }
};

const Argument &Pattern::getArg(const str_t &name) const {
    Arguments::const_iterator it = NameMatcher<Arguments>::find(arguments_, name);
    if (it == arguments_.end()) {
        _THROW(UnknownParamException, "No arguments with name [" + name + "]");
    }
    return *it;
}

bool Pattern::hasArg(const str_t &name) const {
    return NameMatcher<Arguments>::find(arguments_, name) != arguments_.end();
}

const Option &Pattern::getOpt(const str_t &name) const {
    Options::const_iterator it = NameMatcher<Options>::find(options_, name);
    if (it == options_.end()) {
        _THROW(UnknownParamException, "No options with name [" + name + "]");
    }
    return *it;
}


bool Pattern::hasOpt(const str_t &name) const {
    return NameMatcher<Options>::find(options_, name) != options_.end();
}

const Flag &Pattern::getFlag(const str_t &name) const {
    Flags::const_iterator it = NameMatcher<Flags>::find(flags_, name);
    if (it == flags_.end()) {
        _THROW(UnknownParamException, "No flags with name [" + name + "]");
    }
    return *it;
}

bool Pattern::hasFlag(const str_t &name) const {
    return NameMatcher<Flags>::find(flags_, name) != flags_.end();
}

str_t Pattern::usage() const {
    return "Usage here";
}

Argument &Pattern::addArg() {
    arguments_.push_back(Argument(arguments_.size()));
    return arguments_.back();
}

Argument &Pattern::addArg(const str_t &name) {
    // TODO: name collision check
    arguments_.push_back(Argument(arguments_.size(), name));
    return arguments_.back();
}

Flag &Pattern::addFlag(const str_t &name) {
    // TODO: name collision check
    flags_.push_back(Flag(name));
    return flags_.back();
}

Option &Pattern::addOpt(const str_t &name) {
    // TODO: name collision check
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

ArgBuilder PatternBuilder::arg() {
    return ArgBuilder(pattern_.addArg(), pattern_);
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

bool CmdLineParams::ArgCmp::operator()(const Argument &lhs,
                                       const Argument &rhs) const {
    return lhs.getPos() < rhs.getPos();
}

bool CmdLineParams::FlagCmp::operator()(const Flag &lhs,
                                        const Flag &rhs) const {
    return lhs.getCanonicalName() < rhs.getCanonicalName();
}

const ParsedParam &CmdLineParams::getArg(const str_t &name) const {
    const Argument &arg = getPattern().getArg(name);
    return arguments_.find(arg)->second;
}

const ParsedParam &CmdLineParams::getArg(size_t pos) const {
    return arguments_.find(getPattern().getArg(pos))->second;
}

bool CmdLineParams::hasFlag(const str_t &name) const {
    return flags_.find(getPattern().getFlag(name)) != flags_.end();
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
            parseFlag(param);
            continue;
        }

        if (isOptParam(param)) {
            parseOpt(param);
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
    return params_->getPattern().hasFlag(param);
}

bool CmdLineParamsParser::isOptParam(const char *param) {
    return params_->getPattern().hasOpt(param);
}

void CmdLineParamsParser::parseArg(const char *param) {
    size_t currentPos = params_->arguments_.size();
    const Argument &arg = params_->getPattern().getArg(currentPos);
    params_->arguments_.insert(
            CmdLineParams::ArgParamsItem(arg, ParsedArgParam(arg, param))
    );
}

void CmdLineParamsParser::parseFlag(const char *param) {
    const Flag &flag = params_->getPattern().getFlag(param);
    params_->flags_.insert(CmdLineParams::FlagParamsItem(flag, true));
}

void CmdLineParamsParser::parseOpt(const char *param) {

}

void CmdLineParamsParser::reset(int argc, char **argv, CmdLineParams &dst) {
    argc_ = argc;
    argv_ = argv;
    params_ = &dst;
    paramCounter_ = 0;
}


Exception::Exception(const std::string &msg)
        : runtime_error(msg) {
}

Exception::Exception(const std::string &msg, const char *file, size_t line)
        : runtime_error(makeMsg(msg, file, line)) {
}

std::string Exception::makeMsg(const std::string &msg, const char *file,
                           size_t line) {
    return msg + "\n    " + file + ":" + std::to_string(line);
}


BadNameException::BadNameException(const std::string &msg)
        : Exception(msg) {
}

BadNameException::BadNameException(const std::string &msg, const char *file,
                                   size_t line)
        : Exception(msg, file, line) {
}


UnknownParamException::UnknownParamException(const std::string &msg)
        : Exception(msg) {
}

UnknownParamException::UnknownParamException(const std::string &msg,
                                             const char *file, size_t line)
        : Exception(msg, file, line) {
}
