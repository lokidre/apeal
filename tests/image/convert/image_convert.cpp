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
bool optLastFrame{};
apl::String optOutput;

bool optVerbose{};
bool optDumpInfo{};
bool optYCbCr{}, optHSV{}, optHSL{}, optRGB{};

void dump_image_info(const ImageContainer& ii)
{
    using namespace apl;

    int indent = 2;

    println("file_type: %s", ImageFile::fileFormatExt(ii.fileFormat));
    println("images: %d", ii.images.size());

    for (Index idx{}; auto& hit: ii.byHeight) {
        auto& fi = ii.images[hit.second];

        int curr = 0; //indent;

        printlns(curr, "- frame: %d", idx);

        curr += indent;
        printlns(curr, "image_size: %d", fi.size);
        printlns(curr, "pixel_format: %s", fi.formatSignature());
        printlns(curr, "bits_per_sample: %d", fi.depth);
        printlns(curr, "bytes_per_row: %d", fi.bpr);
        printlns(curr, "bytes_per_pixel: %d", fi.bpp);
        //printlns(curr, "bytes_per_row: %d", fi.bytesPerRow);

        if (!fi.dict.empty()) {
            printlns(curr, "dict:");

            curr += indent;
            for (auto& entry: fi.dict) {
                printlns(curr, "- %s: %s", entry.first, entry.second.substr(0, 90));
            }
            curr -= indent;
        }

        println();

        ++idx;
    }
}

void process_channels_RGB(const Image& in)
{
    println("Processing RGB...");

    using SValT = uint8_t;
    using DValT = uint8_t;

    constexpr auto sMinVal = SValT(0);
    constexpr auto sMaxVal = SValT(255);

    constexpr auto sMinValF = float(sMinVal);
    constexpr auto sMaxValF = float(sMaxVal);

    constexpr auto dMinVal = DValT(0);
    constexpr auto dMaxVal = DValT(255);

    constexpr auto dMinValF = float(dMinVal);
    constexpr auto dMaxValF = float(dMaxVal);


    struct ProcInfo {
        StrView tag;

        Index schan;
        std::pair<Index, Index> scmp;

        Index dchan;
        std::pair<Index, Index> dcmp;
    };

    const auto [width, height] = in.size;

    auto [name, ext] = splitFileExt(optOutput);

    auto out = Image::make({.cm = ColorModel::RGB, .size = in.size});

    auto& schans = in.compsLayout;
    auto& dchans = out.compsLayout;

    ProcInfo procInfo[] = {
        {.tag="red", .schan=schans.r, .scmp={schans.g, schans.b}, .dchan=dchans.r, .dcmp={dchans.g, dchans.b}},
        {.tag="grn", .schan=schans.g, .scmp={schans.r, schans.b}, .dchan=dchans.g, .dcmp={dchans.r, dchans.b}},
        {.tag="blu", .schan=schans.b, .scmp={schans.r, schans.g}, .dchan=dchans.b, .dcmp={dchans.r, dchans.g}},
    };

    //using ChanT = uint8_t;
    //constexpr ChanT ChanMax = ChanT(-1);

    for (auto& tab: procInfo) {

        auto srow = in.data();
        auto sbpr = in.bpr;
        auto sbpp = in.bpp;

        auto drow = out.data();
        auto dbpr = out.bpr;
        auto dbpp = out.bpp;

        for (Index j{}; j < height; ++j, srow += sbpr, drow += dbpr) {
            auto scol = srow;
            auto dcol = drow;

            for (Index i{}; i < width; ++i, scol += sbpp, dcol += dbpp) {

                // Source pixel value
                auto spix = (const SValT*)scol;

                auto schan = spix[tab.schan];
                auto scomp = std::pair{spix[tab.scmp.first], spix[tab.scmp.second]};


                auto dpix = (DValT*)dcol;
                auto& dchan = dpix[tab.dchan];
                std::pair<DValT&, DValT&> dcomp{dpix[tab.dcmp.first], dpix[tab.dcmp.second]};


                // start with white
                dchan = dcomp.first = dcomp.second = dMaxVal;

                if (schan > scomp.first && schan > scomp.second) {
                    int max = std::max(scomp.first, scomp.second);
                    int min = std::min(scomp.first, scomp.second);
                    int diff = schan - max;

                    //diff = std::min(diff * 4, min);
                    auto pdiff = (DValT)diff;

                    dcomp.first -= pdiff;
                    dcomp.second -= pdiff;
                }


                //schan += sbpp;
                //scomp.first += sbpp;
                //scomp.second += sbpp;

                //dchan += dbpp;
                //dcomp.first += dbpp;
                //dcomp.second += dbpp;
            }
        }

        auto outfile = fmt("%s_%s.%s", name, tab.tag, ext);
        println("    writing %s...", outfile);
        ImageFile::save(outfile, out);
    }
}

void process_components_RGB_HSV(const Image& in)
{
    println("Processing HSV...");

    auto ridx = in.compsLayout.r;
    auto gidx = in.compsLayout.g;
    auto bidx = in.compsLayout.b;

    using SValType = uint8_t;
    using DValType = uint8_t;

    constexpr auto sMinVal = SValType(0);
    constexpr auto sMaxVal = SValType(255);

    constexpr auto sMinValF = float(sMinVal);
    constexpr auto sMaxValF = float(sMaxVal);

    constexpr auto dMinVal = DValType(0);
    constexpr auto dMaxVal = DValType(255);

    constexpr auto dMinValF = float(dMinVal);
    constexpr auto dMaxValF = float(dMaxVal);


    auto [width, height] = in.getSize();

    auto inData = in.data();
    auto srow = inData;
    auto sbpr = in.bpr;
    auto sbpp = in.bpp;


    // hue
    auto hueImage = Image::make({.cm=ColorModel::RGB, .dim=in.dim});
    auto hrow = hueImage.data();
    auto hbpr = hueImage.bpr;
    auto hbpp = hueImage.bpp;

    // saturation
    auto satImage = Image::make({.cm=ColorModel::Gray, .dim=in.dim});
    auto satrow = satImage.data();
    auto satbpr = satImage.bpr;
    auto satbpp = satImage.bpp;


    // value
    auto valImage = Image::make({.cm=ColorModel::Gray, .dim=in.dim});
    auto vrow = valImage.data();
    auto vbpr = valImage.bpr;
    auto vbpp = valImage.bpp;


    for (Index j{}; j < height; ++j, srow += sbpr, hrow += hbpr, satrow += satbpr, vrow += vbpr) {
        auto spix = srow;
        auto hpix = hrow;

        auto satpix = satrow;
        auto vpix = vrow;

        for (Index i{}; i < width; ++i, spix += sbpp, hpix += hbpp, satpix += satbpp, vpix += vbpp) {
            auto sptr = (const SValType*)spix;
            auto r = sptr[ridx], g = sptr[gidx], b = sptr[bidx];
            auto rf = float(r) / sMaxValF, gf = float(g) / sMaxValF, bf = float(b) / sMaxValF;



            auto hptr = (DValType*)hpix;

            auto hsv = Image::rgb_to_hsv(rf, gf, bf);
            if (std::get<0>(hsv) == 0.f) {
                hptr[0] = hptr[1] = hptr[2] = 0;
            } else {
                auto hrgb = Image::hue_to_rgb(std::get<0>(hsv));

                auto& hr = hptr[hueImage.compsLayout.r];
                auto& hg = hptr[hueImage.compsLayout.g];
                auto& hb = hptr[hueImage.compsLayout.b];

                hr = (SValType)std::clamp(std::get<0>(hrgb) * dMaxValF, dMinValF, dMaxValF);
                hg = (SValType)std::clamp(std::get<1>(hrgb) * dMaxValF, dMinValF, dMaxValF);
                hb = (SValType)std::clamp(std::get<2>(hrgb) * dMaxValF, dMinValF, dMaxValF);
            }


            auto& sat = *(DValType *)satpix;
            sat = (DValType)std::clamp(std::get<1>(hsv) * dMaxValF, dMinValF, dMaxValF);

            auto& val = *(DValType *)vpix;
            val = (DValType)std::clamp(std::get<2>(hsv) * dMaxValF, dMinValF, dMaxValF);
        }
    }


    auto [name, ext] = splitFileExt(optOutput);

    auto outfile = fmt("%s_hue.%s", name, ext);
    println("    writing %s...", outfile);
    ImageFile::save(outfile, hueImage);

    outfile = fmt("%s_sat.%s", name, ext);
    println("    writing %s...", outfile);
    ImageFile::save(outfile, satImage);

    outfile = fmt("%s_val.%s", name, ext);
    println("    writing %s...", outfile);
    ImageFile::save(outfile, valImage);

}



void process()
{
    //auto orig = loadImage(filename);
    auto imageInfo = ImageFile::loadInfo(optInput);

    if (optDumpInfo)
        dump_image_info(imageInfo);

    Index frameIndex = 0;
    if (optLastFrame)
        frameIndex = imageInfo.numImages() - 1;

    auto input = ImageFile::load(optInput, {.index=frameIndex});

    auto& image = input.front();

    if (optRGB) {
        switch (image.getColorModel()) {
        case ColorModel::RGB:
        case ColorModel::RGBA:
            process_channels_RGB(image);
            break;
        default:
            process_channels_RGB(image.convert(ColorModel::RGB));
            break;
        }
    }

    if (optHSV) {
        process_components_RGB_HSV(image);
    }
}


constexpr static auto helpHeader = std::string_view{R"(
Image conversion and channel extraction sample
Copyright (C) 2020-2024 Andrei Mirzoyan.
Do whatever you want with this code.
Usage: test_image_convert <options> [args...]

Options:
)"}.substr(1);


void cmdline(int argc, char *argv[])
{
    CommandLine cmdline;

    cmdline.bindRequiredArgument(&optInput);

    cmdline.addFlag({.name="help", .action=[&] { cmdline.printout(helpHeader).stop(); }, .desc="Print this message"});
    cmdline.dup("h", "help");


    cmdline.addFlag({.name="last", .bind=&optLastFrame, .desc="Choose the last (smallest) frame"});
    cmdline.addFlag({.name="verbose", .bind=&optVerbose, .desc="Print progress information"});

    cmdline.addFlag({.name="info", .bind=&optDumpInfo, .desc="Dump image information."});
    cmdline.dup("i", "info");

    cmdline.add({.name="output", .sbind=&optOutput, .desc="Output file name"});
    cmdline.dup("o", "output");

    cmdline.addFlag({.name="ycbcr", .bind=&optYCbCr, .desc="Process YCbCr"});
    cmdline.addFlag({.name="hsv", .bind=&optHSV, .desc="Process HSV"});
    cmdline.addFlag({.name="hsl", .bind=&optHSL, .desc="Process HSL"});
    cmdline.addFlag({.name="rgb", .bind=&optRGB, .desc="Process RGB"});

    cmdline.process(argc, argv);
}

void body()
{
    cmdline(cmdlineArgC_, cmdlineArgV_);
    process();

    //for (auto&& filename: files)
	//	process(filename);
}
