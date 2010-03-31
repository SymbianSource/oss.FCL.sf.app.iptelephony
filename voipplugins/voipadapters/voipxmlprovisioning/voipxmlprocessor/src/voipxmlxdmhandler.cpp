/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  XDM handler for VoIP XML processor
*
*/


#include <e32cmn.h>
#include <coecntrl.h>
#include <XdmSettingsApi.h>
#include <XdmSettingsCollection.h>
#include <sysutil.h>
#include <pathinfo.h>
#include <authority16.h>
#include <StringLoader.h>
#include <escapeutils.h>

#include "voipxmlutils.h"
#include "voipxmlxdmhandler.h"
#include "voipxmlprocessorlogger.h"
#include "voipxmlprocessordefaults.h"

// ---------------------------------------------------------------------------
// CVoipXmlXdmHandler::CVoipXmlXdmHandler
// ---------------------------------------------------------------------------
//
CVoipXmlXdmHandler::CVoipXmlXdmHandler()
    {
    }

// ---------------------------------------------------------------------------
// CVoipXmlXdmHandler::NewL
// ---------------------------------------------------------------------------
//
CVoipXmlXdmHandler* CVoipXmlXdmHandler::NewL()
    {
    CVoipXmlXdmHandler* self = new ( ELeave ) CVoipXmlXdmHandler;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CVoipXmlXdmHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CVoipXmlXdmHandler::ConstructL()
    {
    DBG_PRINT( "CVoipXmlXdmHandler::ConstructL begin" );
    iProfile = new (ELeave) CXdmSettingsCollection();
    iProfile->AppendL( KDefaultXdmAuthType, EXdmPropAuthType );
    iProfile->AppendL( KDefaultXdmUri, EXdmPropUri );
    DBG_PRINT( "CVoipXmlXdmHandler::ConstructL end" );
    }

// ---------------------------------------------------------------------------
// CVoipXmlXdmHandler::~CVoipXmlXdmHandler
// ---------------------------------------------------------------------------
//
CVoipXmlXdmHandler::~CVoipXmlXdmHandler()
    {
    delete iProfile;
    }

// ---------------------------------------------------------------------------
// Sets XDM setting.
// ---------------------------------------------------------------------------
//
void CVoipXmlXdmHandler::SetSetting( TInt aParam, const TDesC& aValue )
    {
    // Ignore too long descriptors.
    if ( KMaxNodeValueLength < aValue.Length() )
        {
        return;
        }

    TRAP_IGNORE( SetSettingL( aParam, aValue ) );
    }

// ---------------------------------------------------------------------------
// Stores settings through XDM settings API.
// ---------------------------------------------------------------------------
//
TInt CVoipXmlXdmHandler::StoreSettings()
    {
    if ( !iSettingsSet )
        {
        // No settings to be stored => method not supported.
        return KErrNotSupported;
        }
    TRAPD( err, iProfileId = TXdmSettingsApi::CreateCollectionL( *iProfile ));
    if ( KErrNone != err )
        {
        err = KErrCompletion;
        }
    return err;
    }

// ---------------------------------------------------------------------------
// Returns the profile ID if the profile saved in StoreSettings.
// ---------------------------------------------------------------------------
//
TUint32 CVoipXmlXdmHandler::SettingsId()
    {
    return iProfileId;
    }

// ---------------------------------------------------------------------------
// Sets XDM setting.
// ---------------------------------------------------------------------------
//
void CVoipXmlXdmHandler::SetSettingL( TInt aParam, const TDesC& aValue )
    {
    switch ( aParam )
        {
        case EName:
            {
            TBuf<KMaxNodeValueLength> name( KNullDesC );
            name.Copy( aValue );
            CreateProviderNameL( name );
            iProfile->AppendL( name, EXdmPropName );
            iSettingsSet = ETrue;
            break;
            }
        case EType:
            {
            // First remove default value.
            iProfile->RemoveL( EXdmPropAuthType );
            iProfile->AppendL( aValue, EXdmPropAuthType );
            iSettingsSet = ETrue;
            break;
            }
        case EUri:
            {
            // First remove default value.
            iProfile->RemoveL( EXdmPropUri );
            iProfile->AppendL( aValue, EXdmPropUri );
            iSettingsSet = ETrue;
            break;
            }
        case EUsername:
            {
            iProfile->AppendL( aValue, EXdmPropAuthName );
            iSettingsSet = ETrue;
            break;
            }
        case EPassword:
            {
            iProfile->AppendL( aValue, EXdmPropAuthSecret );
            iSettingsSet = ETrue;
            break;
            }
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// Checks for duplicate named XDM sets. Renames if same.
// ---------------------------------------------------------------------------
//
void CVoipXmlXdmHandler::CreateProviderNameL( TDes& aName )
    {
    DBG_PRINT( "CVoipXmlXdmHandler::CreateProviderNameL begin" );

    RArray<TInt> settingIds;
    CleanupClosePushL( settingIds ); // CS:1
    // CS:2
    CDesCArray* names = TXdmSettingsApi::CollectionNamesLC( settingIds );

    HBufC* newName = HBufC::NewLC( KMaxNodeNameLength ); // CS:3
    newName->Des().Copy( aName );
    const TInt count( names->MdcaCount() );
    TUint i( 1 ); // Add number to the name if name already in use.

    // Go through each profile and see if the name of the new profile    
    // matches one of the existing names. If it does change it and
    // check the new name again.
    for ( TInt counter = 0; counter < count; counter++ )
        {
        TBuf<KMaxNodeValueLength> loadedName;
        loadedName.Copy( names->MdcaPoint( counter ));
        if ( 0 == newName->Des().Compare( loadedName ) )
            {
            // If the name is changed we need to begin the comparison
            // again from the first profile.
            newName->Des().Copy( aName );
            newName->Des().Append( KOpenParenthesis() );
            newName->Des().AppendNum( i );
            newName->Des().Append( KClosedParenthesis() );  
            counter = 0;
            i++;
            if ( KMaxProfileNames < i )
                {
                User::Leave( KErrBadName );
                }
            }
        }
    aName.Copy( newName->Des() );

    // newName, names, &settingIds
    CleanupStack::PopAndDestroy( 3, &settingIds );
    DBG_PRINT( "CVoipXmlXdmHandler::CreateProviderNameL end" );
    }

// End of file.
