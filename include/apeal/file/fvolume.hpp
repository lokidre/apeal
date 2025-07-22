/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once


#if 0  // TODO: redo
#if !defined APEAL_OS_WINDOWS
#define DRIVE_UNKNOWN      3
#define DRIVE_NO_ROOT_DIR  4
#define DRIVE_REMOVABLE    5
#define DRIVE_FIXED        6
#define DRIVE_REMOTE       7
#define DRIVE_CDROM        8
#define DRIVE_RAMDISK      9
#else /* Windows NT names
#define DRIVE_UNKNOWN 
#define DRIVE_NO_ROOT_DIR 
#define DRIVE_REMOVABLE 
#define DRIVE_FIXED 
#define DRIVE_REMOTE 
#define DRIVE_CDROM 
#define DRIVE_RAMDISK  */
#endif
#endif


namespace apl {


// Volume information
#pragma pack(push,1)
struct tVolumeInfo {
  char  Label[256+1] ;
  uint32_t SerNum ;  // Serial number of volume
  uint32_t MaxFileName ;  // Maximum length of file name
  uint32_t MaxPath ;  // Maximal length of path
  char  FsName[128] ;  // Name of the file system (FAT, NTFS etc.)
  uint32_t Flags ;
} ;
#pragma pack(pop)

// Logical volume
class tVolume {
  public :
    uint32_t Error ;
    uint32_t Options ;

  public :  // user interface
    tVolume ( uint32_t Opt = 0 ) ;

    // Retrieve volume info. If VolumeName is zero, function returns 
    // current volume information.
    bool GetInfo ( char *VolumeName, tVolumeInfo *pVolumeInfo ) ;

    // Set volume label
    bool SetLabel ( char *VolumeName, char *LabelName ) ;

    // Get free and total space on volume
    bool GetSpace ( char *VolumeName, uint32_t *FreeSpace, uint32_t *TotalSpace ) ;

    // Get disk type - fixed, cdrom etc
    uint32_t GetType ( char *VolumeName ) ;

} ;


}

