#include "../include/cpparseopt.h"
#include <assert.h>

using namespace cpparseopt;


Param::Param(const str_t &name)
        : name_(name), hasDefault_(false) {

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


Nexus Pattern::chain(Pattern &pattern) {
    return Nexus(pattern);
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


Nexus::Nexus(Pattern &pattern)
        : pattern_(pattern) {

}

ArgNexus Nexus::arg(const str_t &name) {
    return ArgNexus(pattern_.addArg(name), pattern_);
}

FlagNexus Nexus::flag(const str_t &name) {
    return FlagNexus(pattern_.addFlag(name), pattern_);
}

OptNexus Nexus::opt(const str_t &name) {
    return OptNexus(pattern_.addOpt(name), pattern_);
}


ArgNexus::ArgNexus(Argument &arg, Pattern &pattern)
        : Nexus(pattern), arg_(arg) {

}

ArgDescrNexus ArgNexus::defaultVal(const str_t &val) {
    arg_.setDefault(val);
    return ArgDescrNexus(arg_, pattern_);
}

ArgValueNexus ArgNexus::descr(const str_t &descr) {
    arg_.setDescr(descr);
    return ArgValueNexus(arg_, pattern_);
}


ArgDescrNexus::ArgDescrNexus(Argument &arg, Pattern &pattern)
        : Nexus(pattern), arg_(arg) {

}

Nexus ArgDescrNexus::descr(const str_t &descr) {
    arg_.setDescr(descr);
    return Nexus(pattern_);
}


ArgValueNexus::ArgValueNexus(Argument &arg, Pattern &pattern)
        : Nexus(pattern), arg_(arg) {

}

Nexus ArgValueNexus::defaultVal(const str_t &val) {
    arg_.setDefault(val);
    return Nexus(pattern_);
}


FlagNexus::FlagNexus(Flag &flag, Pattern &pattern)
        : Nexus(pattern), flag_(flag) {

}


OptNexus::OptNexus(Option &option, Pattern &pattern)
        : Nexus(pattern), option_(option) {

}
