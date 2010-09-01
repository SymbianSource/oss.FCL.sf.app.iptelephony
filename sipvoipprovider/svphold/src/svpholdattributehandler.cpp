/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handles SDP direction attribute related issues.
*
*/


#include    <badesca.h>
#include    "svpholdattributehandler.h"
#include    "svpholdcontext.h"
#include    "svplogger.h"


// ---------------------------------------------------------------------------
// CSVPHoldAttributeHandler::CSVPHoldAttributeHandler
// ---------------------------------------------------------------------------
//
CSVPHoldAttributeHandler::CSVPHoldAttributeHandler() :
    iAttributeArray( NULL )
    {
    
    }

// ---------------------------------------------------------------------------
// CSVPHoldAttributeHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CSVPHoldAttributeHandler::ConstructL()
    {
    // Create attribute array
    iAttributeArray =
        new ( ELeave ) CDesC8ArrayFlat( KSVPHoldDirectionAttributeCount );
        
    iAttributeArray->InsertL( KSVPHoldSendrecvIndex,
                              KSVPHoldAttributeSendrecv );
    iAttributeArray->InsertL( KSVPHoldSendonlyIndex,
                              KSVPHoldAttributeSendonly );
    iAttributeArray->InsertL( KSVPHoldRecvonlyIndex,
                              KSVPHoldAttributeRecvonly );
    iAttributeArray->InsertL( KSVPHoldInactiveIndex,
                              KSVPHoldAttributeInactive );
    }

// ---------------------------------------------------------------------------
// CSVPHoldAttributeHandler::NewL
// ---------------------------------------------------------------------------
//
CSVPHoldAttributeHandler* CSVPHoldAttributeHandler::NewL()
    {
    CSVPHoldAttributeHandler* self = new ( ELeave ) CSVPHoldAttributeHandler;
    CleanupStack::PushL( self );
    
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// CSVPHoldAttributeHandler::~CSVPHoldAttributeHandler
// ---------------------------------------------------------------------------
//
CSVPHoldAttributeHandler::~CSVPHoldAttributeHandler()
    {
    if ( iAttributeArray )
        {
        iAttributeArray->Reset();
        }
        
    delete iAttributeArray;
    }

// ---------------------------------------------------------------------------
// CSVPHoldAttributeHandler::FindDirectionAttribute
// ---------------------------------------------------------------------------
//
TInt CSVPHoldAttributeHandler::FindDirectionAttribute(
    MDesC8Array* aAttributeLines )
    {
    if ( !aAttributeLines )
        {
        SVPDEBUG1( "CSVPHoldAttributeHandler::FindDirectionAttribute - NULL" )
        
        return KErrNotFound;
        }
        
    SVPDEBUG2( "CSVPHoldAttributeHandler::FindDirectionAttribute - Attribute count is %i",
        aAttributeLines->MdcaCount() )
    
    for ( TInt j = 0; j < aAttributeLines->MdcaCount(); j++ )
        {
        SVPDEBUG1( "CSVPHoldAttributeHandler::FindDirectionAttribute - Searching" )
        
        const TPtrC8 attr = aAttributeLines->MdcaPoint( j );
        TInt pos = 0;
        TInt res = iAttributeArray->Find(
            attr.Left( KSVPMaxDirectionAttributeLength ), pos );
        
        SVPDEBUG2( "CSVPHoldAttributeHandler::FindDirectionAttribute res: %d",
            res )
        SVPDEBUG2( "CSVPHoldAttributeHandler::FindDirectionAttribute pos: %d",
            pos )
                
        if( 0 == res )
            {
            // Found right direction attribute
            return pos;
            }
        }
    
    SVPDEBUG1( "CSVPHoldAttributeHandler::FindDirectionAttribute - Not found" )
    
    return KErrNotFound;
    }


