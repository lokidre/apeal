/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <file.h>

#include <console.h>
#include <err.h>
#include <keybrd.h>


tMedia TestMedia ;
tVolume TestVolume ;
AFile<> TestFile ;

dword Free, Total ;
tVolumeInfo VolumeInfo ;

tConsole Con(CONSOLE_STD_OUTPUT) ;


int main ( void )
{
  // File functions
  Con.Out ( "File functions tests...\n" ) ;

  Con.Out ( "Drives:\n" ) ;

  /*
  char *sDriveType, *CurDrive ;
  char Volumes[256] ;
  TestMedia.GetVolumes ( Volumes, 256 ) ;

  for ( CurDrive = Volumes ; StrLen(CurDrive) ; CurDrive += StrLen(CurDrive) + 1 ) {
		switch ( TestVolume.GetType(CurDrive) ) {
			case DRIVE_UNKNOWN :
				sDriveType = "Unknown" ;
				break ;
			case DRIVE_NO_ROOT_DIR :
				sDriveType = "No root directory" ;
				break ;
			case DRIVE_REMOVABLE :
				sDriveType = "Removable" ;
				break ;
			case DRIVE_FIXED :
				sDriveType = "Fixed" ;
				break ;
			case DRIVE_REMOTE :
				sDriveType = "Network" ;
				break ;
			case DRIVE_CDROM :
				sDriveType = "CD-ROM" ;
				break ;
			case DRIVE_RAMDISK :
				sDriveType = "RAM disk" ;
				break ;
			default :
				InternalError() ;
				break ;
		}

		Con.OutF ( "    %s - %s\r\n", CurDrive, sDriveType ) ;
	}


	Con.Out ( "\r\n" ) ;

	TestMedia.GetCurrentDirectory ( TestFile.Name ) ;
	Con.OutF ( "Current directory: %s\r\n", TestFile.Name ) ;

	if ( TestVolume.GetInfo ( NULL, &VolumeInfo ) ) {
		Con.OutF ( "Volume label: %s\r\n", VolumeInfo.Label ) ;
		Con.OutF ( "Serial number: %X\r\n", VolumeInfo.SerNum ) ;
		Con.OutF ( "Max file name length: %u\r\n", VolumeInfo.MaxFileName ) ;
		Con.OutF ( "Max path length: %u\r\n", VolumeInfo.MaxPath ) ;
		Con.OutF ( "File system Id: %s\r\n\n", VolumeInfo.FsName ) ;
	}

  if ( TestVolume.GetSpace ( TestFile.Name, &Free, &Total ) )
    Con.OutF ( "Total bytes: %u\r\nFree bytes: %u\r\n", Total, Free ) ;
  */

  Con.OutF ( "\n\r     --- Press Any Key to Exit --- \n\r" ) ;
  waitkey() ;

  return 0 ;
}

