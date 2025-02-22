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


#ifndef WRGLBL_INCLUDED
#define WRGLBL_INCLUDED

#include "commonui.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "watcom.h"
#include "wresid.h"
#include "wrdll.h"
#include "wrmemi.h"


/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define WRD_VERSION     0x0100

#define UINT32STRLEN    10
#define UINT16STRLEN    5
#define UINT8STRLEN     3

#define CMPFEXT(e,c)    (e[0] == '.' && stricmp(e + 1, c) == 0)

#define VALU8(p)        *(uint_8 *)(p)
#define VALU16(p)       *(uint_16 *)(p)
#define VALU32(p)       *(uint_32 *)(p)

#define INCU8(p)        (p) += sizeof( uint_8 )
#define INCU16(p)       (p) += sizeof( uint_16 )
#define INCU32(p)       (p) += sizeof( uint_32 )

#define SIZEU8          sizeof( uint_8 )
#define SIZEU16         sizeof( uint_16 )
#define SIZEU32         sizeof( uint_32 )

#endif
