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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include "digtypes.h"
#include "digcli.h"
#include "digld.h"
#include "ntopath.h"
#include "servio.h"


FILE *DIGLoader( Open )( const char *name, size_t name_len, const char *defext, char *result, size_t max_result )
{
    bool                has_ext;
    bool                has_path;
    const char          *src;
    char                *dst;
    char                c;
    char                trpfile[256];
    FILE                *fp;

    result = result; max_result = max_result;
    has_ext = FALSE;
    has_path = FALSE;
    src = name;
    dst = trpfile;
    while( name_len-- > 0 ) {
        c = *src++;
        *dst++ = c;
        switch( c ) {
        case '.':
            has_ext = TRUE;
            break;
        case '/':
            has_ext = FALSE;
            has_path = TRUE;
            break;
        }
    }
    if( !has_ext ) {
        *dst++ = '.';
        dst = StrCopyDst( defext, dst );
    }
    *dst = '\0';
    fp = NULL;
    if( has_path ) {
        fp = fopen( trpfile, "rb" );
    } else if( FindFilePath( DIG_FILETYPE_DBG, trpfile, RWBuff ) ) {
        fp = fopen( RWBuff, "rb" );
    }
    return( fp );
}

int DIGLoader( Read )( FILE *fp, void *buff, size_t len )
{
    return( fread( buff, 1, len, fp ) != len );
}

int DIGLoader( Seek )( FILE *fp, unsigned long offs, dig_seek where )
{
    return( fseek( fp, offs, where ) );
}

int DIGLoader( Close )( FILE *fp )
{
    return( fclose( fp ) );
}

void *DIGCLIENTRY( Alloc )( size_t amount )
{
    return( malloc( amount ) );
}

void DIGCLIENTRY( Free )( void *p )
{
    free( p );
}

