/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Prototypes for command parser functions
*
****************************************************************************/


extern bool             ProcImport( void );
extern bool             ProcExport( void );
extern bool             ProcSegment( void );
extern bool             ProcAlignment( void );
extern bool             ProcHeapSize( void );
extern bool             ProcOffset( void );
extern bool             ProcNoRelocs( void );

extern void             InitCmdFile( void );
extern void             SetSegMask(void);
extern char             *GetNextLink( void );
extern void             DoCmdFile( const char * );
extern void             Syntax( void );
extern void             FreePaths( void );
extern void             Burn( void );
extern void             Ignite( void );
extern void             SetFormat( void );
extern void             AddFmtLibPaths( void );
extern bool             HintFormat( exe_format );
extern void             DecideFormat( void );
extern void             FreeFormatStuff( void );
extern void             AddCommentLib( const char *, size_t, lib_priority );
extern void             ExecSystem( const char * );
extern void             PruneSystemList( void );
extern void             BurnSystemList( void );
extern void             AddLibPaths( const char *, size_t, bool );
extern void             AddLibPathsToEnd( const char * );
extern void             AddLibPathsToEndList( const char *list );
extern file_list        *AddObjLib( const char *, lib_priority );
extern int              DoBuffCmdParse( const char *cmd );
