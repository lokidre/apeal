/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <console.h>
#include <keybrd.h>
#include <os.h>

tConsole Con ( CONSOLE_STD_OUTPUT ) ;

tOsVersion OsVersion ;
tSystemInfo SysInfo ;

int main ( void )
{
  Con.Out ( "System information:\r\n" ) ;
  char *sProcessorArchitecture ;
  GetSystemInfo ( &SysInfo ) ;
  switch ( SysInfo.ProcessorArchitecture ) {
    case PROCESSOR_ARCHITECTURE_INTEL :
      sProcessorArchitecture = "x86" ;
      break ;
    default :
      sProcessorArchitecture = "Unknown" ;
      break ;
  }

  Con.Out ( "    Processor architecture: %s\r\n", sProcessorArchitecture ) ;
  Con.Out ( "    Active processors mask: %b\r\n", SysInfo.ActiveProcessorMask ) ;
  Con.Out ( "         Processors number: %u\r\n", SysInfo.ProcessorsNumber ) ;
  Con.Out ( "           Processor level: %u\r\n", SysInfo.ProcessorLevel ) ;
  Con.Out ( "        Processor features: MMX instructions - %s, FP emulation - %s\r\n", (SysInfo.ProcessorFeatures & PROCESSOR_FEATURE_MMX)? "yes" : "no", (SysInfo.ProcessorFeatures & PROCESSOR_FEATURE_FLOATING_POINT_EMU)? "yes" : "no" ) ;
  Con.Out ( "\r\n" ) ;

  Con.Out ( "OS information...\r\n" ) ;
  GetOsVersion ( &OsVersion ) ;
  char *Platform ;
  switch ( OsVersion.Platform ) {
    case PLATFORM_DOS :
      Platform = "DOS" ;
      break ;
    case PLATFORM_WIN_31 :
      Platform = "Windows 3.1" ;
      break ;
    case PLATFORM_WIN_95 :
      Platform = "Windows 95" ;
      break ;
    case PLATFORM_WIN_NT :
      Platform = "Windows NT" ;
      break ;
    default :
      Platform = "Unknown" ;
      break ;
  }

  Con.Out ( "    Platform: %s\r\n", Platform ) ;
  Con.Out ( "     Version: %d.%d\r\n", OsVersion.Major, OsVersion.Minor ) ;
  Con.Out ( "       Build: %d\r\n", OsVersion.Build ) ;
  Con.Out ( "         CSD: %s\r\n", OsVersion.SID ) ;
  Con.Out ( "\r\n" ) ;


  /*
  Wait() ;

  {
    Con->OutCon ( "Environment:\r\n" ) ;
    char *pEnv = GetEnv() ;
    for ( int iEnv = 0 ; pEnv[iEnv] ; ) {
      Con->OutCon ( "%s\r\n", (CON)&pEnv[iEnv] ) ;
      while ( pEnv[iEnv++] )
        ;
    }
  }

  Wait() ;
  */

  return 0 ;
}


