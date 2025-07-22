/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "../string.hpp"
#include "../strview.hpp"



#ifdef QT_CORE_LIB

// Disable annoying warnings from Qt
#ifdef _MSC_VER
#define APEAL_QT_HEADERS_BEGIN\
    __pragma (warning(push, 0))

//    _Pragma("warning(push, 0)")
//    __pragma (warning(disable:4251))\

#define APEAL_QT_HEADERS_END\
    __pragma(warning(pop))

//    _Pragma("warning(pop)")

#else
#define APEAL_QT_HEADERS_BEGIN\
    _Pragma("clang diagnostic push")\
    _Pragma("clang diagnostic ignored \"-Wshorten-64-to-32\"")

#define APEAL_QT_HEADERS_END \
    _Pragma("clang diagnostic pop")
#endif

APEAL_QT_HEADERS_BEGIN
#include <QString>
APEAL_QT_HEADERS_END


namespace apl {

inline QString toQt(StrView str) {
    return QString::fromUtf8(str.data(), (int)str.size());
}

inline String fromQt(QStringView str) {
    auto data = str.toUtf8();
    return String{data.data(), (std::size_t)data.size()};
}

inline QByteArray toQByteArray(StrView str) {
    return QByteArray::fromBase64(QByteArray{str.data(), (int)str.size()});
}

inline String fromQByteArray(const QByteArray data) {
    return data.toBase64().toStdString();
}

}  // namespace 


#endif  // If Qt enabled
