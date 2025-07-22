/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

/*
    Taken from:

    http://www.gnu.org/software/tar/manual/html_node/Standard.html
*/

#include "buffer.hpp"
#include "file.hpp"

namespace apl {


#pragma pack ( push, 1 )
struct TarPosixHeader {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
};

struct TarSparse {
    char offset[12];
    char numbytes[12];
};

enum {
    SPARSES_IN_EXTRA_HEADER  = 16,
    SPARSES_IN_OLDGNU_HEADER = 4,
    SPARSES_IN_SPARSE_HEADER = 21,
};


struct TarSparseHeader {
    TarSparse sp[SPARSES_IN_SPARSE_HEADER];
    char isextended;
};

struct OldGNUHeader {
    char unused_pad1[345];
    char atime[12];
    char ctime[12];
    char offset[12];
    char longnames[4];
    char unused_pad2;
    TarSparse sp[SPARSES_IN_OLDGNU_HEADER];
    char isextended;
    char realsize[12];
};


struct StarHeader {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[131];
    char atime[12];
    char ctime[12];
};

constexpr int SPARSES_IN_STAR_HEADER  = 4;
constexpr int SPARSES_IN_STAR_EXT_HEADER = 21;

//#define SPARSES_IN_STAR_HEADER      4
//#define SPARSES_IN_STAR_EXT_HEADER  21

struct star_in_header {
    char fill[345];       /*   0  Everything that is before t_prefix */
    char prefix[1];       /* 345  t_name prefix */
    char fill2;           /* 346  */
    char fill3[8];        /* 347  */
    char isextended;      /* 355  */
    TarSparse sp[SPARSES_IN_STAR_HEADER]; /* 356  */
    char realsize[12];    /* 452  Actual size of the file */
    char offset[12];      /* 464  Offset of multivolume contents */
    char atime[12];       /* 476  */
    char ctime[12];       /* 488  */
    char mfill[8];        /* 500  */
    char xmagic[4];       /* 508  "tar" */
};

struct star_ext_header {
    TarSparse sp[SPARSES_IN_STAR_EXT_HEADER];
    char isextended;
};

#pragma pack(pop)

class Tar {
    //typedef policy_T policy_type ;
    //typedef typename typemap::find<KStringPolicy,policy_T>::X::Value::string_type string_type ;
    using string_type = String;


    static constexpr const char* TMAGIC = "ustar";       /* ustar and a null */;
    static constexpr int TMAGLEN = 6;
    static constexpr const char* TVERSION = "00";           /* 00 and no null */
    static constexpr int TVERSLEN = 2;

    /* Values used in typeflag field.  */
//#define REGTYPE  '0'            /* regular file */
//#define AREGTYPE '\0'           /* regular file */
//#define LNKTYPE  '1'            /* link */
//#define SYMTYPE  '2'            /* reserved */
//#define CHRTYPE  '3'            /* character special */
//#define BLKTYPE  '4'            /* block special */
//#define DIRTYPE  '5'            /* directory */
//#define FIFOTYPE '6'            /* FIFO special */
//#define CONTTYPE '7'            /* reserved */
//
//#define XHDTYPE  'x'            /* Extended header referring to the
//                                   next file in the archive */
//#define XGLTYPE  'g'            /* Global extended header */

                                   // Bits used in the mode field, values in octal.
    enum Mode {
        TSUID = 04000,          /* set UID on execution */
        TSGID = 02000,          /* set GID on execution */
        TSVTX = 01000,          /* reserved */
                                /* file permissions */
                                TUREAD   =00400,          /* read by owner */
                                TUWRITE  =00200,          /* write by owner */
                                TUEXEC   =00100,          /* execute/search by owner */
                                TGREAD   =00040,          /* read by group */
                                TGWRITE  =00020,          /* write by group */
                                TGEXEC   =00010,          /* execute/search by group */
                                TOREAD   =00004,          /* read by other */
                                TOWRITE  =00002,          /* write by other */
                                TOEXEC   =00001,          /* execute/search by other */
    };


    /* OLDGNU_MAGIC uses both magic and version fields, which are contiguous.
       Found in an archive, it indicates an old GNU header format, which will be
       hopefully become obsolescent.  With OLDGNU_MAGIC, uname and gname are
       valid, though the header is not truly POSIX conforming.  */
//#define OLDGNU_MAGIC "ustar  "  /* 7 chars and a null */

       /* The standards committee allows only capital A through capital Z for
          user-defined expansion.  Other letters in use include:

          'A' Solaris Access Control List
          'E' Solaris Extended Attribute File
          'I' Inode only, as in 'star'
          'N' Obsolete GNU tar, for file names that do not fit into the main header.
          'X' POSIX 1003.1-2001 eXtended (VU version)  */

          /* This is a dir entry that contains the names of files that were in the
             dir at the time the dump was made.  */
//#define GNUTYPE_DUMPDIR 'D'

             /* Identifies the *next* file on the tape as having a long linkname.  */
//#define GNUTYPE_LONGLINK 'K'

/* Identifies the *next* file on the tape as having a long name.  */
//#define GNUTYPE_LONGNAME 'L'

/* This is the continuation of a file that began on another volume.  */
//#define GNUTYPE_MULTIVOL 'M'

/* This is for sparse files.  */
//#define GNUTYPE_SPARSE 'S'

/* This file is a tape/volume header.  Ignore it on extraction.  */
//#define GNUTYPE_VOLHDR 'V'

/* Solaris extended header */
//#define SOLARIS_XHDTYPE 'X'	



public:

    enum Options {
        NOPATH = 0x00001,
    };

    void setOptions(uint32_t opts) { options_ = 0; }

    std::vector<string_type> files;

    void extract(const string_type& filename, const string_type& destination)
    {
        AFileSystem fs;

        files.clear();

        File tarFile;
        tarFile.open(filename, File::AccessMode::Read);

        Buffer<uint8_t> headerBuffer, dataBuffer;

        headerBuffer.resize(0x200);

        auto header = (TarPosixHeader*)headerBuffer.get();

        for (;;) {
            auto count = tarFile.read(headerBuffer.get(), 0x200);
            if (count <= 0)
                break;

            if (header->typeflag != '0' && header->typeflag != 0)
                continue;

            if (header->name[0] == 0)
                continue;

            int size = octalToInt(header->size, sizeof(header->size)-1);

            dataBuffer.resize(size);
            tarFile.read(dataBuffer.get(), (int)size);

            File destFile;
            auto destFilename = makeFilePath(destination, header->name);
            destFile.save(destFilename, dataBuffer.get(), size);

            files.push_back(header->name);

            if ((size & 0x1FF) != 0) {
                size_t pad = 0x200 - (size&0x1FF);
                tarFile.seek(pad, File::Origin::Current);
            }
        }
    }




private:
    uint32_t options_ = 0;

    int octalToInt(const char* str, int size) {
        int v = 0;
        while (size > 0) {
            v = v * 8 + (*str - '0');
            ++str;
            --size;
        }
        return v;
    }
};



}  // namespaca apl
