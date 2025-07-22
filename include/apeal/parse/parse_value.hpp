/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "../def.hpp"
#include "../error.hpp"
#include "../string.hpp"

//#include "./parse_booparse/bool.hpp"
//#include "parse/float.hpp"
//#include "parse/int.hpp"


namespace apl {


class AVariant {
    using Self = AVariant;

public:
    enum class Type {
        NONE,
        INT,
        BOOL,
        CHAR,
        DOUBLE,
        FLOAT,
        STRING,
    };


public:  // initialization
    AVariant() {}
    AVariant(char v) { setChar(v); }
    AVariant(int v) { setInt(v); }
    //AVariant ( const float v ) {  ctor(), setDouble(v) ;  }
    AVariant(double v) { setDouble(v); }
    AVariant(const String& v) { setString(v); }
    //AVariant ( const char *v ) {  setString(v) ;  }

    //~AVariant() {  clear() ;  }


public:  // information functions
    bool valid() const { return Type::NONE == type_ ? false : true; }
    bool empty() const { return Type::NONE == type_ ? true : false; }
    bool null() const { return Type::NONE == type_ ? true : false; }

    Type type() const { return type_; }

public:  // operations
    void clear()
    {
        if (Type::STRING == type_)
            string_value_.clear();

        type_ = Type::NONE;
    }


public:  // set value
    //template <class T>
    //void set_value ( const T &v ) ;

    void setChar(char v) { clear(), type_ = Type::CHAR, value_.char_value = v; }
    void setInt(int v) { clear(), type_ = Type::INT, value_.intValue = v; }
    void setBool(bool v) { clear(), type_ = Type::BOOL, value_.bool_value = v; }
    void setDouble(double v) { clear(), type_ = Type::DOUBLE, value_.double_value = v; }
    void setString(const String& v) { clear(), type_ = Type::STRING, string_value_ = v; }

    Self& operator=(char v)
    {
        setChar(v);
        return *this;
    }
    Self& operator=(int v)
    {
        setInt(v);
        return *this;
    }
    Self& operator=(double v)
    {
        setDouble(v);
        return *this;
    }
    Self& operator=(const String& v)
    {
        setString(v);
        return *this;
    }


public:  // read value
    bool boolValue() const
    {
        switch (type_) {
        case Type::NONE: return false;
        case Type::BOOL: return value_.bool_value;
        case Type::INT: return value_.intValue ? true : false;
        case Type::CHAR: return value_.char_value ? true : false;
        case Type::STRING:
            if (string_value_.empty())
                return false;
            {
                bool v = false;
                if (!parse_bool(string_value_, &v))
                    throw std::runtime_error(
                        "Variant:Cannot convert to bool:" + string_value_);
                return v;
            }
        default: throw std::runtime_error("Variant:boolValue:invalid conversion");
        }

        return false;
    }

    int intValue() const
    {
        switch (type_) {
        case Type::NONE: return 0;
        case Type::INT: return value_.intValue;
        case Type::BOOL: return value_.bool_value ? 1 : 0;
        case Type::CHAR: return value_.char_value;
        case Type::STRING:
            if (string_value_.empty())
                return 0;
            {
                int v = 0;
                if (!parse_int(string_value_, v))
                    throw std::runtime_error(
                        "Variant:Cannot convert to int:" + string_value_);
                return v;
            }
        default: throw std::runtime_error("Variant:int_value:invalid conversion");
        }

        return 0;
    }

    double doubleValue() const
    {
        switch (type_) {
        case Type::DOUBLE: return value_.double_value;
        case Type::NONE: return 0;
        case Type::INT: return (double)value_.intValue;

        case Type::BOOL: return value_.bool_value ? 1 : 0;

        case Type::STRING:
            if (string_value_.empty())
                return 0;
            return parseDouble(string_value_);

        default: throwError("Variant:doubleValue:invalid conversion");
        }

        return 0;
    }

    const String stringValue() const
    {
        switch (type_) {
        case Type::NONE: return "";
        case Type::INT: return sformat("%d", value_.intValue);
        case Type::DOUBLE: return sformat("%g", value_.double_value);

        case Type::BOOL: return value_.bool_value ? "1" : "0";
        case Type::STRING: return string_value_;

        default: throwError("Variant:stringValue:invalid conversion");
        }

        //return "" ;
    }

    const auto& stringAccess() const { return string_value_; }


    template <class T>
    const T value();


private:
    union Value {
        int intValue;
        bool bool_value;
        char char_value;
        double double_value;
        float float_value;
    } value_{};

    String string_value_;

    Type type_ = Type::NONE;

    //void ctor() {  clear() ;  }
};

template <class S, class F>
void str_format_type(S& str, F& fmt, F fend, const AVariant& v)
{
    ++fmt;  // skip the type character
    // TODO: more efficient and for each type
    str += v.stringValue();
    //str_format_append ( str, "%f,%f,%f", u.x, u.y, u.z ) ;
}


}  // namespace apl
