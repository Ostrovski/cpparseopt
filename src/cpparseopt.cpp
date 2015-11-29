#include "../include/cpparseopt.h"
#include <assert.h>

using namespace cpparseopt;


ParamGeneric::ParamGeneric(const str_t &name)
        : name_(name) {

}

const str_t &ParamGeneric::getDescr() const {
    return descr_;
}

void ParamGeneric::setDescr(const str_t &descr) {
    descr_ = descr;
}


void ParamAliased::addAlias(const str_t &name) {
    aliases_.push_back(name);
}


ParamValued::ParamValued()
        : hasDefault_(false), hasVal_(false) {

}

void ParamValued::setVal(const str_t &val) {
    val_ = val;
    hasVal_ = true;
}

bool ParamValued::hasVal() const {
    return hasVal_;
}

const str_t &ParamValued::getDefault() const {
    assert(hasDefault());
    return default_;
}

bool ParamValued::hasDefault() const {
    return hasDefault_;
}

void ParamValued::setDefault(const str_t &val) {
    default_ = val;
    hasDefault_ = true;
}


Argument::Argument(size_t pos, const str_t &name)
        : ParamGeneric(name), pos_(pos) {

}

size_t Argument::getPos() const {
    return pos_;
}


Flag::Flag(const str_t &name)
        : ParamGeneric(name) {

}


Option::Option(const str_t &name)
        : ParamGeneric(name) {

}


str_t Pattern::usage() {
    return "Usage here";
}

Argument &Pattern::addArg(const str_t &name) {
    arguments_.push_back(Argument(arguments_.size(), name));
    return arguments_.back();
}

Flag &Pattern::addFlag(const str_t &name) {
    flags_.push_back(Flag(name));
    return flags_.back();
}

Option &Pattern::addOpt(const str_t &name) {
    options_.push_back(Option(name));
    return options_.back();
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


template <typename T>
AliasBuilder<T>::AliasBuilder(T &param, Pattern &pattern)
        : PatternBuilder(pattern), param_(param) {

}

template <typename T>
AliasBuilder<T> AliasBuilder<T>::alias(const str_t &name) {
    param_.addAlias(name);
    return AliasBuilder(param_, pattern_);
}

template class AliasBuilder<Flag>;
template class AliasBuilder<Option>;


FlagBuilder::FlagBuilder(Flag &flag, Pattern &pattern)
        : PatternBuilder(pattern), flag_(flag) {

}

FlagBuilder FlagBuilder::alias(const str_t &name) {
    flag_.addAlias(name);
    return FlagBuilder(flag_, pattern_);
}


AliasBuilder<Flag> FlagBuilder::descr(const str_t &descr) {
    flag_.setDescr(descr);
    return AliasBuilder<Flag>(flag_, pattern_);
}


OptBuilder::OptBuilder(Option &option, Pattern &pattern)
        : PatternBuilder(pattern), option_(option) {

}

OptBuilder OptBuilder::alias(const str_t &name) {
    option_.addAlias(name);
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
