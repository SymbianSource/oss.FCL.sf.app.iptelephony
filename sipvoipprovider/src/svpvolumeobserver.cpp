/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVP telephony volume observer.
*
*/


#include <centralrepository.h>
#include <TelephonyAudioRouting.h>
#include <telephonydomaincrkeys.h>

#include "svpvolumeobserver.h"
#include "svplogger.h"
#include "svpvolumeupdateobserver.h"

// Default volume for initialization
const TInt KDefaultVolume = 4;


// ---------------------------------------------------------------------------
// CSVPVolumeObserver::CSVPVolumeObserver
// ---------------------------------------------------------------------------
//
CSVPVolumeObserver::CSVPVolumeObserver( MSVPVolumeUpdateObserver& aObserver ) 
    : iCurrentIhfVolume( KDefaultVolume ), iCurrentEarVolume( KDefaultVolume ),
    iMode( CTelephonyAudioRouting::EHandset ), iRepository( NULL ),
    iVolumeUpdate( aObserver )
    {
    SVPDEBUG1("CSVPVolumeObserver::CSVPVolumeObserver()");
    }

// ---------------------------------------------------------------------------
// CSVPVolumeObserver::ConstructL
// ---------------------------------------------------------------------------
//
void CSVPVolumeObserver::ConstructL()
    {
    iRepository = CRepository::NewL( KCRUidInCallVolume );
    iNotifier = CCenRepNotifyHandler::NewL( *this, *iRepository );
    
    // Get the current EAR volume from repository
    User::LeaveIfError( 
        iRepository->Get( KTelIncallEarVolume, iCurrentEarVolume ) );
        
    // Get the current IHF volume from repository
    User::LeaveIfError( iRepository->Get( KTelIncallLoudspeakerVolume, 
        iCurrentIhfVolume ) );
    
    // create instance of telephony audio routing for tracking mode changes
    iTelephonyAudioRouting = CTelephonyAudioRouting::NewL( *this );
    
    SVPDEBUG2("CSVPVolumeObserver::ConstructL iCurrentIhfVolume: %d",
        iCurrentIhfVolume );
    SVPDEBUG2("CSVPVolumeObserver::ConstructL iCurrentEarVolume: %d",
        iCurrentEarVolume );
    
    iNotifier->StartListeningL();
    }

// ---------------------------------------------------------------------------
// CSVPVolumeObserver::NewL
// ---------------------------------------------------------------------------
//
CSVPVolumeObserver* CSVPVolumeObserver::NewL( 
        MSVPVolumeUpdateObserver& aObserver )
    {
    CSVPVolumeObserver* self = new( ELeave ) CSVPVolumeObserver( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// CSVPVolumeObserver::~CSVPVolumeObserver
// ---------------------------------------------------------------------------
//
CSVPVolumeObserver::~CSVPVolumeObserver()
    {
    if ( iNotifier )
        {
        iNotifier->StopListening();
        }
    
    delete iNotifier;
    delete iRepository;
    delete iTelephonyAudioRouting;
    }
    
// ---------------------------------------------------------------------------
// CSVPVolumeObserver::HandleNotifyGeneric
// ---------------------------------------------------------------------------
//
void CSVPVolumeObserver::HandleNotifyGeneric( TUint32 aId )
    {
    SVPDEBUG1("CSVPVolumeObserver::HandleNotifyGeneric() In");
    TInt error = KErrArgument;
    
    if ( KTelIncallEarVolume == aId )
        {
        error = iRepository->Get( KTelIncallEarVolume, iCurrentEarVolume );
        
        SVPDEBUG2("CSVPVolumeObserver::HandleNotifyGeneric EAR VOL: %d",
            iCurrentEarVolume );
        }
    else if ( KTelIncallLoudspeakerVolume == aId )
        {
        error = iRepository->Get( KTelIncallLoudspeakerVolume,
            iCurrentIhfVolume );
        
        SVPDEBUG2("CSVPVolumeObserver::HandleNotifyGeneric IHF VOL: %d",
            iCurrentIhfVolume );
        }
    // else we are not interested in this notification. Note that we set error
    // to be KErrArgument and thus we do not set volume if aId is not what
    // we accept it to be.
    
    if( KErrNone == error )
        {
        // Handle volume so that everything else is like earpiece but
        // loudspeaker.
        TInt volume =  KDefaultVolume;
        if ( CTelephonyAudioRouting::ELoudspeaker == iMode )
            {
            volume = iCurrentIhfVolume;
            }
        else
            {
            volume = iCurrentEarVolume;
            }
            
        // volume update
        iVolumeUpdate.VolumeChanged( volume );
        }
        
    SVPDEBUG2("CSVPVolumeObserver::HandleNotifyGeneric ERROR: %d", error );
    }
    
// ---------------------------------------------------------------------------
// CSVPVolumeObserver::OutputChanged
// ---------------------------------------------------------------------------
//
void CSVPVolumeObserver::OutputChanged( 
    CTelephonyAudioRouting& aTelephonyAudioRouting )
    {
    SVPDEBUG1("CSVPVolumeObserver::OutputChanged In");
    
    // Only unique audio route is the loudspeaker.
    if ( CTelephonyAudioRouting::ELoudspeaker ==
        aTelephonyAudioRouting.Output() )
        {
        SVPDEBUG1("CSVPVolumeObserver::OutputChanged to LOUDSPEAKER");
        iMode = CTelephonyAudioRouting::ELoudspeaker;
        iVolumeUpdate.VolumeChanged( iCurrentIhfVolume );
        }
    else
        {
        // in other cases apply earpiece volume level.
        SVPDEBUG2("CSVPVolumeObserver::OutputChanged to %d",
            aTelephonyAudioRouting.Output() );
        
        iMode = CTelephonyAudioRouting::EHandset;
        iVolumeUpdate.VolumeChanged( iCurrentEarVolume );
        }
         
    SVPDEBUG1("CSVPVolumeObserver::OutputChanged Out");
    }

// ---------------------------------------------------------------------------
// CSVPVolumeObserver::SetOutputComplete
// ---------------------------------------------------------------------------
//
void CSVPVolumeObserver::SetOutputComplete( 
    CTelephonyAudioRouting& /*aTelephonyAudioRouting*/, TInt /*aError*/ )
    {
    // no operations
    }

// ---------------------------------------------------------------------------
// CSVPVolumeObserver::AvailableOutputsChanged
// ---------------------------------------------------------------------------
//
void CSVPVolumeObserver::AvailableOutputsChanged( 
    CTelephonyAudioRouting& /*aTelephonyAudioRouting*/ )
    {
    // no operations
    }











