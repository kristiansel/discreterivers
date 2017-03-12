#ifndef OPTIONAL_H
#define OPTIONAL_H

#include "macro/macrodebugassert.h"

template<typename type>
class optional
{
public:
    optional() {mExists=false;}
    explicit optional(const type &object) {mVal = object; mExists = true;}

    static optional none() {return optional();}

    bool exists() const {return mExists;}
    type get() const { DEBUG_ASSERT(mExists); return mVal; }
private:
    bool mExists;

    type mVal;
};

template<class type>
optional<type> make_optional(type object) {return optional<type>(object);}
#endif // OPTIONAL_H
