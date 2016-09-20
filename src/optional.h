#ifndef OPTIONAL_H
#define OPTIONAL_H

template<typename type>
class optional
{
public:
    optional() {mExists=false;}
    explicit optional(const type &object) {mVal = object; mExists = true;}

    static optional none() {return optional();}

    bool exists() {return mExists;}
    type get() {assert(mExists); return mVal;}
private:
    bool mExists;

    type mVal;
};

template<class type>
optional<type> make_optional(type object) {return optional<type>(object);}
#endif // OPTIONAL_H
