/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <apeal/cmdline.hpp>
#include <apeal/image.hpp>

apl::String optInput;

void dumpImageInfo(const apl::Image& img, int curr) {
    using namespace apl;

    printlns(curr, "image_size: %d", img.size);
    printlns(curr, "color_model: %s", img.colorModelTag());
    //printlns(curr, "pixel_format: %s", fi.formatSignature());
    printlns(curr, "bits_per_sample: %d", img.depth);
    printlns(curr, "bytes_per_row: %d", img.bytesPerRow);
    printlns(curr, "bytes_per_sample: %d", img.bytesPerSample);

    if (!img.dict.empty()) {
        printlns(curr, "dict:");

        curr++;
        for (auto& entry: img.dict) {
            printlns(curr, "- %s: %s", entry.first, entry.second.substr(0, 90));
        }
        curr--;
    }

    println();
}

void dumpImageInfo(const apl::ImageContainer& ii) {
    using namespace apl;

    console_.setIndent(2);

    println("file_type: %s", ImageFile::fileFormatExt(ii.fileFormat));

    if (ii.images.size() == 1) {
        dumpImageInfo(ii.images.front(), 0);
    } else {
        println("images: %d", ii.images.size());

        int idx{};

        for (auto& hit: ii.byHeight) {
            auto& img = ii.images[hit.second];

            int curr = 1;
            printlns(curr, "- frame: %d", idx);

            ++curr;
            dumpImageInfo(img, curr);

            ++idx;
        }
    }
}



void process()
{
    auto imageInfo = apl::ImageFile::loadInfo(optInput);

    dumpImageInfo(imageInfo);
}


constexpr static auto helpHeader = std::string_view{R"(
Image information utility
Copyright (C) 2020-2024 Andrei Mirzoyan.
Do whatever you want with this code.
Usage: test_image_convert <options> [args...]

Options:
)"}.substr(1);


void cmdline(int argc, char *argv[])
{
    apl::CommandLine cmdline;

    cmdline.bindRequiredArgument(&optInput);

    cmdline.addFlag({.name="help", .action=[&] { cmdline.printout(helpHeader).stop(); }, .desc="Print this message"});
    cmdline.dup("h", "help");

    cmdline.process(argc, argv);
}

void body()
{
    cmdline(cmdlineArgC_, cmdlineArgV_);
    process();

    //for (auto&& filename: files)
	//	process(filename);
}
