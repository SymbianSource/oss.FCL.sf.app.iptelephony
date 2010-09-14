/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Main class for svtmatching.
*
*/


#include <implementationproxy.h>
#include <data_caging_path_literals.hrh>

#include "svtmatching.h"
#include "svtsettingshandler.h"
#include "svturiparser.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CSvtMatching::ConstructL()
    {
    iSettingsHandler = CSvtSettingsHandler::NewL();
    iUriParser = CSvtUriParser::NewL();
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CSvtMatching::CSvtMatching()
    {
    }

  
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CSvtMatching* CSvtMatching::NewL()
    {
    CSvtMatching* self = new ( ELeave ) CSvtMatching;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CSvtMatching::~CSvtMatching()
    {
    delete iUriParser;
    delete iSettingsHandler;
    delete iOriginalAddress;
    }


// ---------------------------------------------------------------------------
// Initializes plugin
// ---------------------------------------------------------------------------
//
void CSvtMatching::InitializeL( 
    TUint aServiceId, 
    const TDesC& aOrigAddress )
    {
    if ( 0 == aOrigAddress.Length() ||
         0 == aServiceId )
        {
        User::Leave( KErrArgument );
        }
    
    iSettingsHandler->InitializeSettingsL( aServiceId );
        
    // Check original address for spaces. If found found begin or end
    // of string, remove them.
    RBuf checkedAddress;
    CleanupClosePushL( checkedAddress );  
    iUriParser->CheckForSpacesL( aOrigAddress, checkedAddress );
    
    // Realloc buffer
    delete iOriginalAddress;
    iOriginalAddress = NULL;
    iOriginalAddress = HBufC::NewL( checkedAddress.Length() );
    iOriginalAddress->Des().Copy( checkedAddress ); 
    CleanupStack::PopAndDestroy( &checkedAddress );
    
    iServiceId = aServiceId;
    }
    
    
// ---------------------------------------------------------------------------
// Gets address and meaningful digits for matching.
// ---------------------------------------------------------------------------
//
TInt CSvtMatching::GetAddressForMatching( 
    RBuf& aParsedAddress, 
    TInt& aMeaningfulDigits,
    TBool& aAllowUserNameMatch )
    {
    if( iServiceId == 0 || !iOriginalAddress )
        {
        return KErrNotReady;
        }
    
    aParsedAddress.Close();
    TInt ret = aParsedAddress.Create( iOriginalAddress->Length() );
    
    if ( KErrNone == ret )
        {
        TRAP( ret, iUriParser->ParseAddressL( 
                iSettingsHandler->IgnoreDomainPartValue(), 
                *iOriginalAddress, 
                aParsedAddress ) );
        
        if ( KErrNone == ret )
            {
            aMeaningfulDigits = iSettingsHandler->MeaningfulDigits();
            
            switch ( iSettingsHandler->IgnoreDomainPartValue() )
                {
                case 1:
                    {
                    aAllowUserNameMatch = EFalse;
                    if ( iUriParser->IsValidGsmNumber( aParsedAddress ) )
                        {
                        aAllowUserNameMatch = ETrue;
                        }
                    }
                    break;
                case 2:
                    {
                    aAllowUserNameMatch = ETrue;
                    }
                    break;
                case 0:
                default:
                    aAllowUserNameMatch = EFalse;
                    break;
                }
            }     
        }
    
    return ret;
    }


// ---------------------------------------------------------------------------
// Gets contact store uris
// ---------------------------------------------------------------------------
//
TInt CSvtMatching::GetContactStoreUris( CDesCArray& aStoreUris )
    {
    if( iServiceId == 0 )
        {
        return KErrNotReady;
        }
    
    return iSettingsHandler->GetContactStoreUris( aStoreUris );
    }
   
    
// ---------------------------------------------------------------------------
// Gets remote party name
// ---------------------------------------------------------------------------
//
TInt CSvtMatching::GetRemotePartyName( RBuf& aRemotePartyName )
    {
    if( iServiceId == 0 || !iOriginalAddress )
        {
        return KErrNotReady;
        }

    aRemotePartyName.Close();
    
    TInt ret = iUriParser->DisplayNameFromUri(
                    *iOriginalAddress, 
                    aRemotePartyName );
    
    if ( KErrNoMemory == ret )
        {
        return ret;
        }
 
    // If displayname is not found aRemotePartyName
    // will be empty and return value is still KErrNone.
    return KErrNone;
    }    
    


// ========================== OTHER EXPORTED FUNCTIONS =======================

//
// Rest of the file is for ECom initialization. 
//

// Map the interface UIDs
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( 
        0x2001E2A5, CSvtMatching::NewL )
    };


// ---------------------------------------------------------------------------
// ImplementationGroupProxy implements for ECom
// Exported proxy for instantiation method resolution
// Returns: ImplementationTable
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
    TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }

