#include "../include/cpparseopt.h"
#include <assert.h>

using namespace cpparseopt;


Param::Param(const str_t &name)
        : name_(name), hasVal_(false), hasDefault_(false) {

}

void Param::setVal(const str_t &val) {
    val_ = val;
    hasVal_ = true;
}

bool Param::hasVal() const {
    return hasVal_;
}

const str_t &Param::getDefault() const {
    return default_;
}

bool Param::hasDefault() const {
    return hasDefault_;
}

void Param::setDefault(const str_t &val) {
    default_ = val;
    hasDefault_ = true;
}

const str_t &Param::getDescr() const {
    return descr_;
}

void Param::setDescr(const str_t &descr) {
    descr_ = descr;
}


Argument::Argument(size_t pos, const str_t &name)
        : Param(name), pos_(pos) {

}

const str_t &Argument::getDefault() const {
    assert(hasDefault());
    return default_;
}

size_t Argument::getPos() const {
    return pos_;
}


AliasedParam::AliasedParam(const str_t &name)
        : Param(name) {
}

void AliasedParam::addAlias(const str_t &name) {
    aliases_.push_back(name);
}


Flag::Flag(const str_t &name)
        : AliasedParam(name) {

}


Option::Option(const str_t &name)
        : AliasedParam(name) {

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


Builder::Builder(Pattern &pattern)
        : pattern_(pattern) {

}

ArgBuilder Builder::arg(const str_t &name) {
    return ArgBuilder(pattern_.addArg(name), pattern_);
}

FlagBuilder Builder::flag(const str_t &name) {
    return FlagBuilder(pattern_.addFlag(name), pattern_);
}

OptBuilder Builder::opt(const str_t &name) {
    return OptBuilder(pattern_.addOpt(name), pattern_);
}


ArgBuilder::ArgBuilder(Argument &arg, Pattern &pattern)
        : Builder(pattern), arg_(arg) {

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
        : Builder(pattern), arg_(arg) {

}

Builder ArgDescrBuilder::descr(const str_t &descr) {
    arg_.setDescr(descr);
    return Builder(pattern_);
}


ArgValueBuilder::ArgValueBuilder(Argument &arg, Pattern &pattern)
        : Builder(pattern), arg_(arg) {

}

Builder ArgValueBuilder::defaultVal(const str_t &val) {
    arg_.setDefault(val);
    return Builder(pattern_);
}


FlagBuilder::FlagBuilder(Flag &flag, Pattern &pattern)
        : Builder(pattern), flag_(flag) {

}


OptBuilder::OptBuilder(Option &option, Pattern &pattern)
        : Builder(pattern), option_(option) {

}
