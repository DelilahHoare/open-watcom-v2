/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2022 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  Global CPU breakpoint related definitions.
*
****************************************************************************/


#ifndef _BRKPTCPU_H_INCLUDED
#define _BRKPTCPU_H_INCLUDED

#if defined( _M_IX86 )

#define TRACE_BIT       0x100
#define BRKPOINT        0xCC
#define FAKE_BRKPOINT   0xF1

typedef unsigned char   opcode_type;

extern void BreakPoint( void );
#pragma aux BreakPoint = BRKPOINT;

extern void BreakPointParm( unsigned long );
#if defined( _M_I86 )
#pragma aux BreakPointParm = BRKPOINT __parm [__dx __ax] __aborts
#else
#pragma aux BreakPointParm = BRKPOINT __parm [__eax] __aborts
#endif

#elif defined( _M_X64 )

#define TRACE_BIT       0x100
#define BRKPOINT        0xCC

typedef unsigned char   opcode_type;

extern void BreakPoint( void );

#elif defined( __PPC__ )

#define TRACE_BIT       (1U << MSR_L_se)
#define BRKPOINT        0x7FE00008

typedef unsigned        opcode_type;

#elif defined( __MIPS__ )

//#define TRACE_BIT      0
#define BRKPOINT        0x0000000D

typedef unsigned        opcode_type;

#elif defined( __AXP__ )

//#define TRACE_BIT      0
#define BRKPOINT        0x00000080

typedef unsigned        opcode_type;

#else

    #error brkptcpu.h not configured for machine

#endif

#endif
