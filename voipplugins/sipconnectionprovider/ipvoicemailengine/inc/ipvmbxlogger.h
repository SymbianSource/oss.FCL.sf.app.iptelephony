/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Logging macros
*
*/


#ifndef IPVMBXLOGGER_H
#define IPVMBXLOGGER_H


#ifdef _DEBUG

    #include <e32debug.h>
    #define DPRINT_ON

#endif // _DEBUG


#ifdef DPRINT_ON // RDebug

    #define IPVME( A )                 L##A
    #define IPVMESTUPID( A )           IPVME( A )
    #define IPVMESTR( A )              IPVMESTUPID( "[IPVME]" ) L##A
    #define IPVMESTRCAST( A )          TPtrC((const TText *) IPVMESTR( A ))
    
    #define IPVMEPRINT( D )            { RDebug::Print( IPVMESTRCAST( D ) ); }
    #define IPVMEPRINT2( A, B )        { RDebug::Print( IPVMESTRCAST( A ), B ); }
    #define IPVMEPRINT3( A, B, C )     { RDebug::Print( IPVMESTRCAST( A ), B, C ); }
    #define IPVMEPRINT4( A, B, C, D )  { RDebug::Print( IPVMESTRCAST( A ), B, C, D ); }

#else // No debug prints

    #define IPVMEPRINT( A )
    #define IPVMEPRINT2( A, B )
    #define IPVMEPRINT3( A, B, C )
    #define IPVMEPRINT4( A, B, C, D )

#endif


#endif // IPVMBXLOGGER_H
