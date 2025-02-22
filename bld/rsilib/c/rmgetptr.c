/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2011-2022 The Open Watcom Contributors. All Rights Reserved.
*
* =========================================================================
*
* Description:  rsi get real-mode far pointer.
*
****************************************************************************/


#include "rsi1632.h"

void FarPtr rsi_get_rm_ptr( void FarPtr p )
{
    descriptor  g;
    unsigned    seg;

    if( rsi_get_descriptor( _FP_SEG( p ), &g ) ) {
        seg = makelong( g.base_23_16, g.base_15_0 ) >> 4;
        if( g.base_15_0 & 0x0F ) {
            if( _FP_OFF( p ) > ( g.base_15_0 & 0x0F ) + _FP_OFF( p ) ) {
                return( makeptr( seg + 1, _FP_OFF( p ) + ( g.base_15_0 & 0x0F ) - 0x10 ) );
            }
            return( makeptr( seg, _FP_OFF( p ) + ( g.base_15_0 & 0x0F ) ) );
        }
        return( makeptr( seg, _FP_OFF( p ) ) );
    }
    return( NULL_PTR );
}
