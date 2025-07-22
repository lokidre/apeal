/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <test_buffer.hpp>

#include <apeal/buffer.hpp>
#include <apeal/geo.hpp>


using MarginsF = apl::Margins<double>;

MarginsF margins_{};

template <class T>
void dumpMargins(const apl::Margins<T>& m) {

    con_("top: %d, right: %d, bottom: %d, left: %d", m.top, m.right, m.bottom, m.left);
}


void setMargins(MarginsF m) {
    margins_ = m;

    dumpMargins(margins_);
}


void body()
{

    setMargins(1);

    MarginsF m{{ .horz = 2 }};
    setMargins(m);

    setMargins({{.vert = 3}});



    //apl::Buffer<int> buffer;

    //buffer.push_back(2);

    //dumpBuffer(buffer);
}
