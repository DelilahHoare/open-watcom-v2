/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2022 The Open Watcom Contributors. All Rights Reserved.
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
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
* Description:  Windows 3.x breakpoint support.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <ctype.h>
#include <dos.h>
#include "stdwin.h"
#include "dbg386.h"
#include "wdebug.h"
#include "di386cli.h"
#include "initfini.h"
#include "descript.h"


/*
 * We keep a breakpoint list to support the "debug debugee only" and
 * to allow the movement of segments (if a segment moves, then we have
 * to replace the breakpoint). The switch is undocumented (';cge')
 * and is only to allow debugging of the debugger.
 *
 * Under Windows 3.1, it seems bloody unlikely that segments move, so
 * maintaining a breakpoint list is unnecessary.
 */

#define GMEM_FLAGS      (GMEM_SHARE + GMEM_MOVEABLE + GMEM_ZEROINIT)
#define BREAK_INCREMENT 64

#define MAX_WATCHES     8

typedef struct {
    addr48_ptr  loc;
    DWORD       value;
    DWORD       linear;
    word        dregs;
    word        len;
} watch_point;

opcode_type             BreakOpcode;

static break_point      __huge *brkList;
static WORD             numBreaks;
static HGLOBAL          brkHandle;
static watch_point      WatchPoints[MAX_WATCHES];
static WORD             WatchCount;

opcode_type place_breakpoint( addr48_ptr *addr )
{
    opcode_type old_opcode;

    ReadMemory( addr, &old_opcode, sizeof( old_opcode ) );
    WriteMemory( addr, &BreakOpcode, sizeof( BreakOpcode ) );
    return( old_opcode );
}

int remove_breakpoint( addr48_ptr *addr, opcode_type old_opcode )
{
    return( WriteMemory( addr, &old_opcode, sizeof( old_opcode ) ) != sizeof( old_opcode ) );
}

/*
 * IsOurBreakpoint - check if a segment/offset is a break point we set
 */
BOOL IsOurBreakpoint( WORD sel, DWORD off )
{
    int i;

    for( i = 0; i < numBreaks; i++ ) {
        if( brkList[i].in_use ) {
            if( brkList[i].loc.segment == sel && brkList[i].loc.offset == off ) {
                return( TRUE );
            }
        }
    }
    return( FALSE );

} /* IsOurBreakpoint */

/*
 * ResetBreakpoints - reset breakpoints in a specified segment
 */
void ResetBreakpoints( WORD sel )
{
    int             i;

    for( i = 0; i < numBreaks; i++ ) {
        if( brkList[i].in_use ) {
            if( brkList[i].loc.segment == sel ) {
                place_breakpoint( &brkList[i].loc );
            }
        }
    }

} /* ResetBreakpoints */

static break_point __far * findBrkEntry( void )
{
    int         i;
    int         old_num;

    for( i = 0; i < numBreaks; i++ ) {
        if( !brkList[i].in_use ) {
            return( &brkList[i] );
        }
    }
    old_num = numBreaks;
    numBreaks += BREAK_INCREMENT;
    if( old_num == 0 ) {
        brkHandle = GlobalAlloc( GMEM_FLAGS, numBreaks * sizeof( brkList[0] ) );
    } else {
        GlobalUnlock( brkHandle );
        brkHandle = GlobalReAlloc( brkHandle, numBreaks * sizeof( brkList[0] ), GMEM_FLAGS );
    }
    brkList = (break_point __huge *)GlobalLock( brkHandle );
    return( &brkList[old_num] );
}

trap_retval TRAP_CORE( Set_break )( void )
{
    set_break_req       *acc;
    set_break_ret       *ret;
    break_point         *brk;

    acc = GetInPtr( 0 );

    Out((OUT_BREAK,"AccSetBreak %4.4x:%8.8x", acc->break_addr.segment, acc->break_addr.offset ));
    Out((OUT_BREAK,"task=%4.4x", DebugeeTask ));

    brk = findBrkEntry();
    brk->old_opcode = place_breakpoint( &acc->break_addr );
    brk->loc = acc->break_addr;
    brk->in_use = TRUE;
    brk->hard_mode = TRUE;

    ret = GetOutPtr( 0 );
    ret->old = brk->old_opcode;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Clear_break )( void )
{
    int                 i;
    clear_break_req     *acc;

    acc = GetInPtr( 0 );
    Out((OUT_BREAK,"AccRestoreBreak %4.4x:%8.8x", acc->break_addr.segment, acc->break_addr.offset ));
    Out((OUT_BREAK,"task=%4.4x", DebugeeTask ));

    remove_breakpoint( &acc->break_addr, acc->old );

    for( i = 0; i < numBreaks; i++ ) {
        if( brkList[i].loc.segment == acc->break_addr.segment &&
                        brkList[i].loc.offset == acc->break_addr.offset ) {
            brkList[i].in_use = FALSE;
            break;
        }
    }
    return( 0 );
}

/*
 * Code to manipulate debug registers is here.  The Set/GetDebugRegister
 * calls are pragmas that talk to WDEBUG.386.  The pragmas themselves are
 * in bld/watcom/h/wdebug.h
 */
static void setDR6( DWORD tmp )
{
    SetDebugRegister( 6, &tmp );
} /* setDR6 */

static void setDR7( DWORD tmp )
{
    SetDebugRegister( 7, &tmp );
} /* setDR7 */

DWORD GetDR6( void )
{
    DWORD       tmp;

    GetDebugRegister( 6, &tmp );
    return( tmp );
} /* GetDR6 */

#if 0
static DWORD GetDR7( void )
{
    DWORD       tmp;

    GetDebugRegister( 7, &tmp );
    return( tmp );
} /* GetDR7 */
#endif

static DWORD SetDRn( int i, DWORD linear, long type )
{
    SetDebugRegister( i,&linear );
    return( ( type << DR7_RWLSHIFT(i) )
          | ( DR7_GEMASK << DR7_GLSHIFT(i) ) | DR7_GE
          | ( DR7_LEMASK << DR7_GLSHIFT(i) ) | DR7_LE );
} /* SetDRn */

void ClearDebugRegs( void )
{
    int i;

    if( WDebug386 ) {
        for( i = 0; i < 4; i++ )
            SetDRn( i, 0L, 0L );
        setDR6( 0 );
        setDR7( 0 );
    }
} /* ClearDebugRegs */

bool IsWatch( void )
{
    return( WatchCount != 0 );
}

static int DRegsCount( void )
{
    int     needed;
    int     i;

    needed = 0;
    for( i = 0; i < WatchCount; i++ ) {
        needed += WatchPoints[i].dregs;
    }
    return( needed );
}

BOOL SetDebugRegs( void )
{
    int         i,dr;
    DWORD       dr7;
    watch_point *wp;

    if( !WDebug386 )
        return( FALSE );

    if( DRegsCount() > 4 )
        return( FALSE );

    dr  = 0;
    dr7 = 0;
    for( wp = WatchPoints, i = 0; i < WatchCount; wp++, i++ ) {
        dr7 |= SetDRn( dr, wp->linear, DRLen( wp->len ) | DR7_BWR );
        dr++;
        if( wp->dregs == 2 ) {
            dr7 |= SetDRn( dr, wp->linear + wp->len, DRLen( wp->len ) | DR7_BWR );
            dr++;
        }
    }
    setDR7( dr7 );
    return( TRUE );
} /* SetDebugRegs */

/*
 * CheckWatchPoints - check if a watchpoint was hit
 */
BOOL CheckWatchPoints( void )
{
    DWORD       value;
    int         i;

    for( i = 0; i < WatchCount; i++ ) {
        value = 0;
        ReadMemory( &WatchPoints[i].loc, &value, WatchPoints[i].len );
        if( value != WatchPoints[i].value ) {
            return( TRUE );
        }
    }
    return( FALSE );
} /* CheckWatchPoints */

trap_retval TRAP_CORE( Set_watch )( void )
{
    set_watch_req       *acc;
    set_watch_ret       *ret;
    DWORD               value;
    watch_point         *curr;
    descriptor          desc;
    DWORD               linear;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->multiplier = 20000;
    ret->err = 1;       // fail
    if( WatchCount < MAX_WATCHES ) {
        ret->err = 0;   // OK
        value = 0;
        ReadMemory( &acc->watch_addr, &value, acc->size );
        GetDescriptor( acc->watch_addr.segment, &desc );
        linear = GET_DESC_BASE( desc ) + acc->watch_addr.offset;
        curr = WatchPoints + WatchCount;
        curr->loc.segment = acc->watch_addr.segment;
        curr->loc.offset = acc->watch_addr.offset;
        curr->len = acc->size;
        curr->value = value;
        curr->linear = linear & ~( curr->len - 1 );
        curr->dregs = ( linear & ( curr->len - 1 ) ) ? 2 : 1;
        WatchCount++;
        if( WDebug386 ) {
            if( DRegsCount() <= 4 ) {
                ret->multiplier |= USING_DEBUG_REG;
            }
        }
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Clear_watch )( void )
{
    clear_watch_req     *acc;
    watch_point         *dst;
    watch_point         *src;
    int         i;

    acc = GetInPtr( 0 );
    dst = src = WatchPoints;
    for( i = 0; i < WatchCount; i++ ) {
        if( src->loc.segment != acc->watch_addr.segment
         || src->loc.offset != acc->watch_addr.offset ) {
            dst->loc.offset = src->loc.offset;
            dst->loc.segment = src->loc.segment;
            dst->value = src->value;
            dst++;
        }
        src++;
    }
    WatchCount--;
    return( 0 );
}
