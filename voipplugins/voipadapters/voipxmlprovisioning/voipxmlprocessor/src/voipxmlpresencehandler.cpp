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
* Description:  Presence handler for VoIP XML processor
*
*/


#include <e32cmn.h>
#include <coecntrl.h>
#include <pressettingsapi.h>
#include <pressettingsset.h>
#include <sysutil.h>
#include <pathinfo.h>
#include <authority16.h>
#include <StringLoader.h>
#include <escapeutils.h>

#include "voipxmlutils.h"
#include "voipxmlpresencehandler.h"
#include "voipxmlprocessorlogger.h"
#include "voipxmlprocessordefaults.h"

// ---------------------------------------------------------------------------
// Default constructor.
// ---------------------------------------------------------------------------
//
CVoipXmlPresenceHandler::CVoipXmlPresenceHandler()
    {
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CVoipXmlPresenceHandler* CVoipXmlPresenceHandler::NewL()
    {
    CVoipXmlPresenceHandler* self = new ( ELeave ) CVoipXmlPresenceHandler;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
void CVoipXmlPresenceHandler::ConstructL()
    {
    iProfile.iObjectSize        = KMaxObjectSize;
    iProfile.iPublicationInt    = KPublishInterval;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CVoipXmlPresenceHandler::~CVoipXmlPresenceHandler()
    {
    }

// ---------------------------------------------------------------------------
// Sets Presence setting.
// ---------------------------------------------------------------------------
//
void CVoipXmlPresenceHandler::SetSetting( TInt aParam, const TDesC& aValue )
    {
    // Ignore too long descriptors.
    if ( KMaxNodeValueLength < aValue.Length() )
        {
        return;
        }

    TInt intVal( KErrNotFound );
    switch ( aParam )
        {
        case EName:
            {
            TBuf<KMaxNodeValueLength> name( KNullDesC );
            name.Copy( aValue );
            TRAPD( err, CreateProviderNameL( name ) );
            if ( KErrNone == err )
                {
                iProfile.iSetName.Copy( name );
                iSettingsSet = ETrue;
                }
            break;
            }
        case EMaxObjectSize:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intVal ))
                {
                iProfile.iObjectSize = intVal;
                iSettingsSet = ETrue;
                }
            break;
            }
        case EPublishInterval:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intVal ))
                {
                iProfile.iPublicationInt = intVal;
                iSettingsSet = ETrue;
                }
            break;
            }
        case EMaxSubscriptions:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intVal ))
                {
                iProfile.iMaxSubscriptions = intVal;
                iSettingsSet = ETrue;
                }
            break;
            }
        case EMaxContacts:
            {
            if ( KErrNone == VoipXmlUtils::DesToInt( aValue, intVal ))
                {
                iProfile.iMaxContactsInList = intVal;
                iSettingsSet = ETrue;
                }
            break;
            }
        case EDomainSyntax:
            {
            iProfile.iDomainSyntax.Copy( aValue );
            iSettingsSet = ETrue;
            break;
            }
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// Stores settings to 'Presence registry', i.e. creates a new Presence set.
// ---------------------------------------------------------------------------
//
TInt CVoipXmlPresenceHandler::StoreSettings()
    {
    if ( !iSettingsSet )
        {
        // No settings to be stored => method not supported.
        return KErrNotSupported;
        }
    iProfile.iStatusOfProps = EPresSettingOpened;
    TRAPD( err, iProfileId = PresSettingsApi::CreateSetL( iProfile ));
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
TUint32 CVoipXmlPresenceHandler::SettingsId()
    {
    return iProfileId;
    }

// ---------------------------------------------------------------------------
// Links other settings to presence profile.
// ---------------------------------------------------------------------------
//
void CVoipXmlPresenceHandler::LinkSettings( TInt aType, TUint32 aSettingsId )
    {
    switch ( aType )
        {
        case ESip:
            iProfile.iSipProfile = aSettingsId;
            break;
        case EXdm:
            iProfile.iXDMSetting = aSettingsId;
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// Finalizes profile, i.e. stores the profile with linkage information.
// ---------------------------------------------------------------------------
//
TInt CVoipXmlPresenceHandler::FinalizeSettings()
    {
    TInt err1( KErrNone );
    TRAPD( err2, err1 = PresSettingsApi::UpdateSetL( iProfile, iProfileId ));
    if ( KErrNone != err1 || KErrNone != err2 )
        {
        err1 = KErrGeneral;
        }
    return err1;
    }

// ---------------------------------------------------------------------------
// Checks for duplicate named Presence profiles. Renames if same.
// ---------------------------------------------------------------------------
//
void CVoipXmlPresenceHandler::CreateProviderNameL( TDes& aName )
    {
    DBG_PRINT( "CVoipXmlPresenceHandler::CreateProviderNameL begin" );
    
    const TInt maxModifyLength = 
        KMaxNodeNameLength - KMaxProfileNameAppendLength;
    RArray<TInt> settingIds;
    CleanupClosePushL( settingIds ); // CS:1
    // CS:2
    CDesCArray* names = PresSettingsApi::GetAllSetsNamesLC( settingIds );

    HBufC* newName = HBufC::NewLC( KMaxNodeNameLength ); // CS:3
    newName->Des().Copy( aName.Left( maxModifyLength ) );
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
            newName->Des().Copy( aName.Left( maxModifyLength ) );
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
    DBG_PRINT( "CVoipXmlPresenceHandler::CreateProviderNameL end" );
    }

// End of file.
