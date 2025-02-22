/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "stdnt.h"

/*
 * getRealBase - get real base address, based on segment/offset
 */
static ULONG_PTR getRealBase( addr48_ptr *addr, ULONG_PTR *plimit )
{
#if defined( MD_axp ) || defined( MD_ppc )

    *plimit = (ULONG_PTR)-1L;
    return( addr->offset );

#elif defined( MD_x86 ) || defined( MD_x64 )

    ULONG_PTR   realbase;
    LDT_ENTRY   ldt;
    ULONG_PTR   limit;
    ULONG_PTR   selbase;
    WORD        sel;

    sel = addr->segment;
    if( sel == FlatDS || sel == FlatCS ) {
        *plimit = (ULONG_PTR)-1L;
        return( addr->offset );
    }
    if( !GetSelectorLDTEntry( sel, &ldt ) ) {
        *plimit = 0;
        return( 0 );
    }
    limit = 1 + (ULONG_PTR)ldt.LimitLow +
        ( (ULONG_PTR)ldt.HighWord.Bits.LimitHi << 16L );
    if( ldt.HighWord.Bits.Granularity ) {
        limit *= 0x1000L;
    }
    if( !ldt.HighWord.Bits.Default_Big ) {
        realbase = (ULONG_PTR)(WORD)addr->offset;
    } else {
        realbase = addr->offset;
    }
    selbase = (ULONG_PTR)ldt.BaseLow +
        ( (ULONG_PTR)ldt.HighWord.Bytes.BaseMid << 16L ) +
        ( (ULONG_PTR)ldt.HighWord.Bytes.BaseHi << 24L );
    *plimit = limit + selbase;
    return( realbase + selbase );

#else

    #error getRealBase not configured

#endif
}

/*
 * ReadMemory - read some memory
 */
DWORD ReadMemory( addr48_ptr *addr, LPVOID buff, DWORD size )
{
    SIZE_T      bytes;
    ULONG_PTR   limit;
    ULONG_PTR   base;
#ifdef DEBUGGING_THIS_DAMN_WIN95_PROBLEM
    static bool first = true;
#endif

    if( DebugeePid == 0 ) {
        return( 0 );
    }
    base = getRealBase( addr, &limit );
    if( limit < base ) {
        limit = base;
    }
    if( base + size < base ) { // wants to wrap segment
        size = ( ~(ULONG_PTR)0 ) - base;
    }
    if( limit != (ULONG_PTR)-1L ) {
        if( base + size > limit ) {
            if( limit < base ) {
                size = 0;
            } else {
                size = limit - base;
            }
        }
    }
    if( size > 2048 )
        size = 2048;
#ifdef DEBUGGING_THIS_DAMN_WIN95_PROBLEM
    FILE        *io;
    io = fopen( "t.t", "a+" );
    fprintf( io, "%4.4x:%8.8x, base=%8.8x, limit=%8.8x, size=%d\n",
                addr->segment, addr->offset, base, limit, size );
    fclose( io );
#endif
    bytes = 0;
    ReadProcessMemory( ProcessInfo.process_handle, (LPVOID)base, buff, size, &bytes );
#ifdef DEBUGGING_THIS_DAMN_WIN95_PROBLEM
    if( first ) {
        remove( "t.t" );
        first = false;
    }
    if( bytes != size ) {
        io = fopen( "t.t", "a+" );
        fprintf( io, "got=%d\n", (int)bytes );
        fclose( io );
    }
#endif
    return( bytes );
}

/*
 * WriteMemory - write some memory
 */
DWORD WriteMemory( addr48_ptr *addr, LPVOID buff, DWORD size )
{
    SIZE_T      bytes;
    ULONG_PTR   limit;
    ULONG_PTR   base;

    if( DebugeePid == 0 ) {
        return( 0 );
    }
    base = getRealBase( addr, &limit );
    if( limit != (ULONG_PTR)-1L ) {
        if( base + size > limit ) {
            if( limit < base ) {
                size = 0;
            } else {
                size = limit - base;
            }
        }
    }
    bytes = 0;
    WriteProcessMemory( ProcessInfo.process_handle, (LPVOID)base, buff, size, &bytes );
    GetLastError();
    return( bytes );

}

trap_retval TRAP_CORE( Read_mem )( void )
{
    read_mem_req    *acc;

    acc = GetInPtr( 0 );
    if( DebugeePid != 0 )
        return( ReadMemory( &acc->mem_addr, GetOutPtr( 0 ), acc->len ) );
    return( 0 );
}

trap_retval TRAP_CORE( Write_mem )( void )
{
    write_mem_req   *acc;
    write_mem_ret   *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->len = 0;
    if( DebugeePid != 0 ) {
        ret->len = WriteMemory( &acc->mem_addr, GetInPtr( sizeof( *acc ) ),
                                GetTotalSizeIn() - sizeof( *acc ) );
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Checksum_mem )( void )
{
    addr48_ptr          addr;
    size_t              len;
    WORD                value;
    DWORD               sum;
    checksum_mem_req    *acc;
    checksum_mem_ret    *ret;

    sum = 0;
    if( DebugeePid ) {
        acc = GetInPtr( 0 );
        addr.offset = acc->in_addr.offset;
        addr.segment = acc->in_addr.segment;
        for( len = acc->len; len > 0; ) {
            ReadMemory( &addr, &value, sizeof( value ) );
            sum += value & 0xff;
            addr.offset++;
            len--;
            if( len > 0 ) {
                sum += value >> 8;
                addr.offset++;
                len--;
            }
        }
    }
    ret = GetOutPtr( 0 );
    ret->result = sum;
    return( sizeof( *ret ) );
}
