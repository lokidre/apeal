/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#if __OS == __OS_OS

#if __CC == __CC_BC3
// only tasm can handle it
#pragma inline
#endif

extern "C" dword _xaos_api_hand ;
extern "C" dword _xaos_api_call ;
extern "C" dword _xaos_api_code ;
extern "C" dword _xaos_api_parm ;


// everything needs to be rewritten

extern "C"
void _xaos_api_dispatcher ( void ) ;

extern "C"
bool xaos_api ( XaosParam *parm, dword apiclass, dword apifunc )
{
  parm->handle   = _xaos_api_hand ;
  parm->version  = 0 ;  // needed
  parm->apiclass = apiclass ;
  parm->apifunc  = apifunc ;


  // obsolete preparation
  _xaos_api_code = 0 ;  // regular call
  _xaos_api_parm = (dword)parm ;


  // local variables for inline assembly code
  dword code = _xaos_api_code ;
  dword param_adr = (dword)parm ;
  dword handle = _xaos_api_hand ;


  _asm          mov     eax, code
  _asm          mov     edx, param_adr
  _asm          mov     ebx, handle
  _xaos_api_dispatcher() ;


  /*
  #if __MM != __MM_FLAT
  // prepare to call operating system
  _asm {
                mov     ax, cs:DGROUP
                mov     es, ax

                // prepare arguments in stack
                mov     eax, param_adr
                push    eax

                mov     eax, code
                push    eax

                mov     eax, handle
                push    eax

                call    dword ptr es:[offset _xaos_api_call]

                add     sp, 12
  }
  #endif
  */


  // check the result
  return parm->result ? true : false ;
}
#endif
