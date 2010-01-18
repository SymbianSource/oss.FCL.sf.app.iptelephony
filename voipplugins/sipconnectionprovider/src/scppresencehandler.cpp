/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Presence handler.
*
*/
#include <ximperrors.hrh>
#include <ximpcontext.h>
#include <presenceinfo.h> //MximpPresenceInfo
#include <presenceinfofilter.h> //info filtter
#include <presenceinfofield.h> //MximpPresenceInfoField
#include <ximpobjectfactory.h>
#include <presenceobjectfactory.h>
#include <servicepresenceinfo.h> //MximpServicePresenceInfo
#include <presenceinfofieldvalueenum.h> //MximpPresenceInfoFieldValueEnum
#include <presenceinfofieldcollection.h> //MximpPresenceInfoFieldCollection
#include <presenceinfofieldvaluetext.h> //MximpPresenceInfoFieldValueText
#include <personpresenceinfo.h> // MximpPersonPresenceInfo
#include <presencepublishing.h>//MPresencePublishing
#include <ximpfeatureinfo.h>
#include <presencefeatures.h>
#include <ximpidentity.h> //for MXIMPIdentity
#include <presentitygroups.h>

#include <ximprequestcompleteevent.h>
#include <ximpcontextstateevent.h>
#include <ownpresenceevent.h>
#include <ximpclient.h>
#include <ximpcontext.h>
#include <ximpstatus.h>
#include <pressettingsapi.h> //presence settings
#include <XdmSettingsApi.h>
#include <cvimpstsettingsstore.h>

#include "scppresencehandler.h"
#include "scpsubservice.h"
#include "scpservicestorage.h"
#include "scplogger.h"
#include "scputility.h"
#include "scpservice.h"
#include "scpprofilehandler.h"
#include "scpsettinghandler.h"
#include "scppresencehandlerrequest.h"

const TInt KXdmDmMaxIntLength      = 10;   // max length of 32bit integer
const TInt KMaxPresenceEnableCount = 5;
const TInt KCustomMessageMaxLength = 75;
const TInt KBufSize255 = 255;

// -----------------------------------------------------------------------------
// CScpPresenceHandler::NewL
// -----------------------------------------------------------------------------
//
CScpPresenceHandler* CScpPresenceHandler::NewL( CScpSubService& aSubService )
    {
    SCPLOGSTRING( "CScpPresenceHandler::NewL" );
    CScpPresenceHandler* self = new ( ELeave ) CScpPresenceHandler( aSubService );
    CleanupStack::PushL( self );    
    self->ConstructL();    
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CScpPresenceHandler::CCScpPresenceHandler
// -----------------------------------------------------------------------------
//
CScpPresenceHandler::CScpPresenceHandler( CScpSubService& aSubService ) :
    CScpServiceHandlerBase( aSubService ),
    iPresenceState( ENoBind ),
    iDisableAfterXimpRequestsCompleted( EFalse )
    {
    SCPLOGSTRING2( "CScpPresenceHandler[0x%x]::CScpPresenceHandler", this );
    }

// -----------------------------------------------------------------------------
// CScpPresenceHandler::ConstructL
// -----------------------------------------------------------------------------
//
void CScpPresenceHandler::ConstructL()
    {
    SCPLOGSTRING2( "CScpPresenceHandler[0x%x]::ConstructL", this );

    BaseConstructL();
    GetPresencePropertyIdL( ESubPropertyPresenceSettingsId, iPresenceSettingsId );
    
    iPresClient = MXIMPClient::NewClientL();
    SCPLOGSTRING2( "CScpPresenceHandler::ConstructL, iPresClient [0x%x]", iPresClient );

    if ( iPresClient )
        {
        // Set presence handler as observer of Voip subservice
        CScpSubService* voipSubService = GetVoipSubService();
        if ( voipSubService )
            {
            voipSubService->SetSubServiceObserver( this );
            }
        
        MXIMPContext* tmp = iPresClient->NewPresenceContextLC();        
        iFeature = MPresenceFeatures::NewL( tmp );
        
        RArray<TInt32> eventFilter;   
        CleanupClosePushL( eventFilter );

        eventFilter.Append( XIMP_IF_ID_REQUEST_COMPLETE_EVENT );     
        eventFilter.Append( XIMP_IF_ID_CONTEXT_STATE_EVENT );
        
        TArray<TInt32> eventFilterArray = eventFilter.Array();
        tmp->RegisterObserverL( *this, &eventFilterArray );
        
        CleanupStack::PopAndDestroy( &eventFilter );

        CleanupStack::Pop(); // tmp
        iPresenceCtx = tmp;
        tmp = NULL; 
        }
    else
        {
        User::Leave( KErrGeneral );
        }
    }

// -----------------------------------------------------------------------------
// CScpPresenceHandler::~CScpPresenceHandler
// -----------------------------------------------------------------------------
//
CScpPresenceHandler::~CScpPresenceHandler()
    {
    SCPLOGSTRING2( "CScpPresenceHandler[0x%x]::~CScpPresenceHandler", this );

    CancelDisableTimer();
    
    iReqIdArray.Close();
    
    delete iFeature;
    delete iPresenceCtx;
    
    // Set observer of Voip subservice to NULL
    CScpSubService* voipSubService = GetVoipSubService();
    if ( voipSubService )
        {
        voipSubService->SetSubServiceObserver( NULL );
        }
    
    delete iPresClient;
    }


// ====================== From CScpServiceHandlerBase ==========================

// -----------------------------------------------------------------------------
// CScpPresenceHandler::EnableSubServiceL
// -----------------------------------------------------------------------------
//
void CScpPresenceHandler::EnableSubServiceL()
    {
    SCPLOGSTRING4( "CScpPresenceHandler[0x%x]::EnableSubServiceL: 0x%x type: %i", 
                   this, &iSubService, iSubService.SubServiceType() );
       
    __ASSERT_DEBUG( iSubService.SubServiceType() == ECCHPresenceSub,
                    User::Panic( KNullDesC, KErrGeneral ) );     

    CScpServiceHandlerBase::RegisterProfileL();
    }

// -----------------------------------------------------------------------------
// CScpPresenceHandler::DisableSubService
// -----------------------------------------------------------------------------
//
TInt CScpPresenceHandler::DisableSubService()
    {
    SCPLOGSTRING2( "CScpPresenceHandler[0x%x]::DisableSubService", this );
    __ASSERT_DEBUG( iSubService.SubServiceType() == ECCHPresenceSub,
                    User::Panic( KNullDesC, KErrGeneral ) );  

    TInt result = KErrNone;
    // Publish offline and unbind presence context
    TRAP( result, HandleDeregistrationL( ETrue ) );
    
    if ( KErrNone == result  )
        {
        // Deregister if still connecting
        if ( iSubService.State() == ECCHConnecting )
            {
            DeregisterNow();            
            }
        else
            {
            StartForcedDisableTimer( CScpPresenceHandler::ForcePresenceServiceDisable );
            }
        }
    else
        {
        
        // Wait for XIMP request to be completed and handle 
        // de-registration after that or after force disable timer expires
        if ( iReqIdArray.Count() )
            {
            iDisableAfterXimpRequestsCompleted = ETrue;
            }
        else
            {
            DeregisterNow();
            }
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CScpPresenceHandler::SubServiceType
// -----------------------------------------------------------------------------
//
TCCHSubserviceType CScpPresenceHandler::SubServiceType() const
    {
    SCPLOGSTRING2( "CScpPresenceHandler[0x%x]::SubServiceType", this );
    return ECCHPresenceSub;
    }

// -----------------------------------------------------------------------------
// CScpPresenceHandler::HandleSipConnectionEvent
// -----------------------------------------------------------------------------
//
void CScpPresenceHandler::HandleSipConnectionEvent( const TUint32 aProfileId,
                                                    TScpConnectionEvent aEvent )
    {
    SCPLOGSTRING4( "CScpPresenceHandler[0x%x]::HandleSipConnectionEvent id: %d event: %d",
                   this, aProfileId, aEvent );
    
    if ( iSubService.SipProfileId() == aProfileId &&
        iSubService.EnableRequestedState() != CScpSubService::EScpNoRequest )
        {
        //if network lost, unbind context
        if ( EScpNetworkLost == aEvent )
            {
            SCPLOGSTRING( "CScpPresenceHandler - EScpNetworkLost -> unbind" );
            TRAP_IGNORE( HandleDeregistrationL( EFalse ) );
            }
        
        if ( EScpRoaming == aEvent )
            {
            SCPLOGSTRING( "CScpPresenceHandler - EScpRoaming -> unbind" );
            TRAP_IGNORE( ServerUnBindL() );
            }
        
        //if registered, time to bind context
        if ( EScpRegistered == aEvent &&
             CScpSubService::EScpEnabled == iSubService.EnableRequestedState() )
            {
            SCPLOGSTRING( "CScpPresenceHandler - EScpRegistered -> update iap and bind" );
            // update xmd settings 
            TRAPD( err, UpdateXdmSettingsL() );
            
            if ( KErrNone == err )
                {
                // Subscribe
                TRAP( err, ServerBindL() );
                iRebind = EFalse;
                
                if( KErrNone == err )
                    {
                    // Still connecting the service
                    aEvent = EScpNetworkFound;    
                    }
                else
                    {
                    SCPLOGSTRING2( "ServerBindL failed: %d", err );
                    aEvent = EScpRegistrationPending;
                    if ( KErrAlreadyExists == err )
                        {
                        // Ximp does not set request to queue, so we have to do rebind later
                        SCPLOGSTRING( "CScpPresenceHandler - rebind later" ); 
                        iRebind = ETrue;
                        }
                    }
                }
            else
                {
                SCPLOGSTRING2( "UpdateXdmSettingsL: %d", err );
                aEvent = EScpRegistrationFailed;
                }
            }
        else if ( EScpDeregistered == aEvent &&
                 iSubService.EnableRequestedState() == CScpSubService::EScpDisabled ||
                 iSubService.EnableRequestedState() == CScpSubService::EScpRefreshed )
            {
            if ( EScpRegistered != aEvent && ECCHDisconnecting != iSubService.State() )
                {
                CancelDisableTimer();
                }
            // When SIP is deregistered, change presence state to no bind
            iPresenceState = ENoBind;
            }
        iSubService.HandleConnectionEvent( aEvent );
        }
    SCPLOGSTRING( "CScpPresenceHandler::HandleSipConnectionEvent OUT" );
    }    


// ======================= From MXIMPContextObserver ===========================

// -----------------------------------------------------------------------------
// CScpPresenceHandler::HandlePresenceContextEvent
// -----------------------------------------------------------------------------
//   
void CScpPresenceHandler::HandlePresenceContextEvent(
    const MXIMPContext& /*aContext*/,
    const MXIMPBase& aEvent )
    {
    SCPLOGSTRING2( "CScpPresenceHandler[0x%x]::HandlePresenceContextEvent", this );

    const TInt32 eventType = aEvent.GetInterfaceId();
    
    switch ( eventType )
        {
        case MXIMPContextStateEvent::KInterfaceId:
            {
            HandleContextStateEvent( aEvent );
            break;
            }

        case MXIMPRequestCompleteEvent::KInterfaceId:
            {
            HandleRequestCompleteEvent( aEvent );
            break;
            }
              
        default:
            {
            break;
            }
        }
    SCPLOGSTRING( "CScpPresenceHandler::HandlePresenceContextEvent end" ); 
    }



// ===================== From MScpSubServiceObserver ===========================

// -----------------------------------------------------------------------------
// CScpPresenceHandler::HandleSubServiceChange
// -----------------------------------------------------------------------------
//
void CScpPresenceHandler::HandleSubServiceChange( TCCHSubserviceState aState, TInt aError )
    {
    SCPLOGSTRING4( "CScpPresenceHandler::HandleSubServiceChange presence subservice state: %d, aState: %d, aError: %d", 
                    iSubService.State(), aState, aError );
                    
    if ( iSubService.State() == ECCHEnabled && ( aError == KCCHErrorBandwidthInsufficient || aError == KErrNone ) )
        {
        if( aState == ECCHEnabled && aError == KErrNone )
            {
            if ( EPresenceOffline == iPresenceState )
                {
                SCPLOGSTRING( "CScpPresenceHandler::HandleSubServiceChange PublishOnline" );
                TRAP_IGNORE( PublishPresenceL( ETrue ) );
                }
            }
        else
            {
            if ( EPresenceOnline == iPresenceState && aState != ECCHConnecting )
                {
                SCPLOGSTRING( "CScpPresenceHandler::HandleSubServiceChange PublishOffline" );
                // Unsubscribe from the list first
                TRAP_IGNORE( UnsubscribePresentityGroupL() );
                TRAP_IGNORE( PublishPresenceL( EFalse ) );
                }
            }
        }
    }
 
// ========================= Other member funcions =============================


// -----------------------------------------------------------------------------
// CScpPresenceHandler::GetVoipSubService
// -----------------------------------------------------------------------------
//
CScpSubService* CScpPresenceHandler::GetVoipSubService()
    {
    CScpSubService* voipSubService = NULL;
    CScpServiceStorage& serviceStorage = iSubService.ServiceStorage();
    CScpService* service = serviceStorage.GetServiceByServiceId( iSubService.SubServiceId() );

    if ( service )
        {
        voipSubService = service->GetSubServiceByType( ECCHVoIPSub );
        }
    
    return voipSubService;
    }

// -----------------------------------------------------------------------------
// CScpPresenceHandler::DeregisterNow
// -----------------------------------------------------------------------------
//
void CScpPresenceHandler::DeregisterNow()
    {
    SCPLOGSTRING2( "CScpPresenceHandler[0x%x]::DeregisterNow",
                   this );
    
    CancelDisableTimer();
  
    iPresenceState = ENoBind;

    // Check if disable was not requested
    if ( iSubService.EnableRequestedState() == CScpSubService::EScpEnabled )
        {
        // Check the last ximp error
        if ( KErrNoMemory == iLastXimpError && 
            KMaxPresenceEnableCount < iSubService.EnableCounter() )
            {
            // No memory means 401 Unauthorized, we are trying to enable  
            // KMaxPresenceEnableCount times -> no more, this is truly
            // autentication failed.
            iSubService.HandleConnectionEvent( EScpAuthenticationFailed );
            }
        else
            {
            iSubService.HandleConnectionEvent( EScpDeregistered );
            }
        }
    else
        {
        DeregisterProfile();
        }
    }

// -----------------------------------------------------------------------------
// CScpPresenceHandler::ForcePresenceServiceDisable
// -----------------------------------------------------------------------------
//
TInt CScpPresenceHandler::ForcePresenceServiceDisable( TAny* aSelf )
    {
    SCPLOGSTRING( "CScpPresenceHandler::ForcePresenceServiceDisable" );

    CScpPresenceHandler* self = static_cast<CScpPresenceHandler*>( aSelf );

    self->DeregisterNow();

    return 1;
    }

// -----------------------------------------------------------------------------
// CScpPresenceHandler::UpdateXdmSettingsL
// -----------------------------------------------------------------------------
//
void CScpPresenceHandler::UpdateXdmSettingsL()
    {
    SCPLOGSTRING2( "CScpPresenceHandler[0x%x]::UpdateXdmSettingsL", this );
    
    // Get IAP id of sip profile
    CScpProfileHandler& profileHandler = iSubService.ProfileHandler();
    CScpSipConnection* sipConnection = profileHandler.GetSipConnection( iSubService.SipProfileId() );
    if ( !sipConnection )
        {
        User::Leave( KErrNotFound );
        }
    
    TUint32 apId = 0;
    User::LeaveIfError( sipConnection->GetIap( apId ) );
    
    SCPLOGSTRING2( "CScpPresenceHandler::UpdateXdmSettingsL apId is %d", apId );
    
    TPresSettingsSet mySet;
    User::LeaveIfError( PresSettingsApi::SettingsSetL( iPresenceSettingsId, mySet ));
        
    // set iap id to xdm settings
    HBufC* idBuf = HBufC::NewLC( KXdmDmMaxIntLength );
    TPtr ptrBuf = idBuf->Des();
    ptrBuf.AppendNum( apId );
        
    TXdmSettingsApi::UpdatePropertyL( mySet.iXDMSetting, *idBuf, EXdmPropToNapId );
    CleanupStack::PopAndDestroy( idBuf );

    SCPLOGSTRING( "CScpPresenceHandler::UpdateXdmSettingsL status online end" );
    }

// -----------------------------------------------------------------------------
// CScpPresenceHandler::GetPresencePropertyIdL
// -----------------------------------------------------------------------------
//
void CScpPresenceHandler::GetPresencePropertyIdL( TServicePropertyName aProperty,
                                                    TInt& aValue ) const
    {
    SCPLOGSTRING2( "CScpPresenceHandler[0x%x]::GetPresenceSettingsId", this );
    
    CScpServiceStorage& serviceStorage = iSubService.ServiceStorage();
    CScpSettingHandler& settingHandler = serviceStorage.SettingsHandler();
    
    settingHandler.GetSPSettingsIntPropertyL( iSubService.SubServiceId(),
        aProperty, 
        aValue );
    }

// -----------------------------------------------------------------------------
// CScpPresenceHandler::PublishPresenceL
// Publishes presence according to given parameter.
// -----------------------------------------------------------------------------
//
void CScpPresenceHandler::PublishPresenceL( TBool aPublishOnline )
    {
    SCPLOGSTRING3( "CScpPresenceHandler[0x%x]::PublishPresenceL, aPublishOnline = %d",
        this, aPublishOnline );

    // Get the management interface
    MPresencePublishing* presPub = &( iFeature->PresencePublishing() );
    // publish own presence in here
    MPresenceInfo* info = CreateInfoLC( aPublishOnline );
    TScpReqId reqId;
    
    if ( aPublishOnline )
        {
        reqId.SetType( EPublishOnlineReq );
        iPresenceState = EPublishing;
        }
    else
        {
        reqId.SetType( EPublishOfflineReq );
        iPresenceState = EPresenceOffline;
        }
        
    reqId.SetId( presPub->PublishOwnPresenceL( *info ) ); 
    iReqIdArray.Append( reqId );
    
    CleanupStack::PopAndDestroy( 1 );
    
    SCPLOGSTRING( "CScpPresenceHandler::PublishPresenceL end" ); 
    }


// -----------------------------------------------------------------------------
// CScpPresenceHandler::ServerBindL
// Binds the presence context
// -----------------------------------------------------------------------------
//
void CScpPresenceHandler::ServerBindL()
    {
    SCPLOGSTRING2( "CScpPresenceHandler[0x%x]::ServerBindL", this );
    SCPLOGSTRING2( "CScpPresenceHandler::ServerBindL -> bind service: %d", 
        iSubService.SubServiceId() );
    
    //Bind context to desired presence service
    TInt propertyId = 0;
    GetPresencePropertyIdL( EPropertyPCSPluginId, propertyId );
    TUid protocolUid = TUid::Uid( propertyId );
    
    TScpReqId reqId;
    reqId.SetType( EBindReq );
    reqId.SetId( iPresenceCtx->BindToL( protocolUid, iSubService.SubServiceId() ) );
    iReqIdArray.Append( reqId );
    iPresenceState = EBinding;
    SCPLOGSTRING( "CScpPresenceHandler::ServerBindL end" );
    }

// -----------------------------------------------------------------------------
// CScpPresenceHandler::ServerUnBindL
// Unbinds the presence context
// -----------------------------------------------------------------------------
//
void  CScpPresenceHandler::ServerUnBindL()
    {
    SCPLOGSTRING2( "CScpPresenceHandler[0x%x]::ServerUnBindL", this );
    TScpReqId reqId;
    reqId.SetType( EUnBindReq );
    reqId.SetId( iPresenceCtx->UnbindL() );
    iReqIdArray.Append( reqId ); 
    iPresenceState = EUnBinding;
    SCPLOGSTRING( "CScpPresenceHandler::ServerUnBindL end" );
    }
    
// -----------------------------------------------------------------------------
// CScpPresenceHandler::HandleDeregistrationL
// Handles the deregistration of presence.
// -----------------------------------------------------------------------------
//    
void CScpPresenceHandler::HandleDeregistrationL( TBool aDoStopPublish )
    {
     SCPLOGSTRING4( "CScpPresenceHandler[0x%x]::HandleDeregistrationL, aDoStopPublish: %d, iPresenceState: %d",
             this, aDoStopPublish, iPresenceState );

    if ( EPresenceOnline == iPresenceState 
        && aDoStopPublish && 
        iSubService.LastReportedError() == KErrNone )
        {
        // Get the management interface
        MPresencePublishing* presPub = &( iFeature->PresencePublishing() );
        // publish own presence in here
        MPresenceInfo* info = CreateInfoLC( EFalse );
        TScpReqId reqId;
        // After publishing offline, we need to unbind because we are
        // deregistering, so offline request id needs to be saved and handled
        reqId.SetType( EPublishOfflineReq );
        reqId.SetId( presPub->PublishOwnPresenceL( *info ) );
        iPresenceState = EPresenceOffline;
        iReqIdArray.Append( reqId );
        CleanupStack::PopAndDestroy( 1 );  // info
        }
    else
        {
        //unbind if there are no other requests going on. 
        //in case of ongoing unprocessed requests, 
        //reset array (further events ignored) and
        //leave here to deregister properly
        if ( iReqIdArray.Count() > 0 )
            {
            User::Leave( KErrNotReady );
            }
        else
            {
            ServerUnBindL();
            }
        }

    SCPLOGSTRING( "CScpPresenceHandler::HandleDeregistrationL end" );
    } 

// -----------------------------------------------------------------------------
// CScpPresenceHandler::CreateInfoLC
// Creates presence info item
// -----------------------------------------------------------------------------
//  
MPresenceInfo* CScpPresenceHandler::CreateInfoLC( TBool aState )
    {
    SCPLOGSTRING2( "CScpPresenceHandler[0x%x]::CreateInfoLC", this );
    
    MPresenceInfo* info = 
        iFeature->PresenceObjectFactory().NewPresenceInfoLC();
    // fill service info
    MServicePresenceInfo* srvInfo = 
        iFeature->PresenceObjectFactory().NewServicePresenceInfoLC();
    srvInfo->SetServiceTypeL( 
        NPresenceInfo::NServiceType::KVoip );  // voip

    MPresenceInfoField* infoField = 
        iFeature->PresenceObjectFactory().NewInfoFieldLC();
    
    MPresenceInfoFieldValueEnum* enumField = 
       iFeature->PresenceObjectFactory().NewEnumInfoFieldLC();
       
    // Set the textfield's value according to aState    
    if ( aState )
        {  
        enumField->SetValueL( NPresenceInfo::EAvailable );
        }
     else
        {
        enumField->SetValueL( NPresenceInfo::ENotAvailable );
        }
    
    infoField->SetFieldTypeL( 
        NPresenceInfo::NFieldType::KAvailabilityEnum ); // "availability"
    infoField->SetFieldValue( enumField );
    CleanupStack::Pop(); // enumField
    
    srvInfo->Fields().AddOrReplaceFieldL( infoField );
    CleanupStack::Pop(); // infoField 

    info->AddServicePresenceL( srvInfo );
    CleanupStack::Pop(); // srvInfo 

    // fill person info
    MPersonPresenceInfo* persInfo = 
        iFeature->PresenceObjectFactory().NewPersonPresenceInfoLC();
    
    MPresenceInfoField* infoField2 = 
        iFeature->PresenceObjectFactory().NewInfoFieldLC();
    
    MPresenceInfoFieldValueEnum* enumField2 = 
        iFeature->PresenceObjectFactory().NewEnumInfoFieldLC();
    
    TInt availabilityEnum(0);
    RBuf customMessage;
    CleanupClosePushL( customMessage );
    customMessage.Create( KCustomMessageMaxLength );
    GetStoredPresenceValuesL( availabilityEnum, customMessage );
    
    switch( availabilityEnum )
        {
        case NPresenceInfo::EAvailable:
            {
            enumField2->SetValueL( NPresenceInfo::EAvailable );
            break;
            }

        case NPresenceInfo::ENotAvailable:
            {
            enumField2->SetValueL( NPresenceInfo::ENotAvailable );
            break;
            }
            
        case NPresenceInfo::EBusy:
            {
            enumField2->SetValueL( NPresenceInfo::EBusy );
            break;
            }
            
        case NPresenceInfo::EDoNotDisturb:
            {
            enumField2->SetValueL( NPresenceInfo::EDoNotDisturb );
            break;
            }
                
        case NPresenceInfo::EAway:
            {
            enumField2->SetValueL( NPresenceInfo::EAway );
            break;
            }
            
        // if presence availability is not stored     
        default:
            {
            if ( aState )
                {  
                enumField2->SetValueL( NPresenceInfo::EAvailable );
                }
            else
                {
                enumField2->SetValueL( NPresenceInfo::ENotAvailable );
                }
            break;
            }
        }
    
    // set custom message if available
    if( customMessage.Length() ) 
        {
        MPresenceInfoField* customMessageinfoField = 
            iFeature->PresenceObjectFactory().NewInfoFieldLC();
                
        MPresenceInfoFieldValueText* textField = 
            iFeature->PresenceObjectFactory().NewTextInfoFieldLC();
        textField->SetTextValueL( customMessage );
        customMessageinfoField->SetFieldTypeL( 
        NPresenceInfo::NFieldType::KStatusMessage );
        customMessageinfoField->SetFieldValue( textField );
        CleanupStack::Pop(); //textField
        persInfo->Fields().AddOrReplaceFieldL( customMessageinfoField );
        CleanupStack::Pop(); //customMessageinfoField
        }
    
    CleanupStack::PopAndDestroy( &customMessage );
   
    infoField2->SetFieldTypeL( 
        NPresenceInfo::NFieldType::KAvailabilityEnum );
    infoField2->SetFieldValue( enumField2 );
    CleanupStack::Pop(); // enumField2

    persInfo->Fields().AddOrReplaceFieldL( infoField2 );
    CleanupStack::Pop(); // infoField2

    info->SetPersonPresenceL( persInfo );
    CleanupStack::Pop(); // persInfo

    return info;
    } 


// -----------------------------------------------------------------------------
// CScpPresenceHandler::HandleContextStateEvent
// Handles the XIMP context state events
// -----------------------------------------------------------------------------
//
void CScpPresenceHandler::HandleContextStateEvent( const MXIMPBase& aEvent )
    {
    const MXIMPContextStateEvent* event =
    TXIMPGetInterface< const MXIMPContextStateEvent >::From( aEvent, MXIMPBase::EPanicIfUnknown );

    MXIMPContextState::TState ctxState = event->ContextState().ContextState();
    SCPLOGSTRING2( "CScpPresenceHandler: MXIMPContextStateEvent: %d", (TInt)ctxState );
    
    const MXIMPStatus* status = event->StateChangeReason();
    if ( status )
        {
        iLastXimpError = status->ResultCode();
        }
    SCPLOGSTRING2( "CScpPresenceHandler -> context ximp error: %d", iLastXimpError );
        
    // Go through the reqid array to see, if unbind has been requested
    TBool unBindRequested( EFalse );
    for ( TInt i = 0; i < iReqIdArray.Count(); i++ )
        {
        if ( iReqIdArray[i].Type() == EUnBindReq )
            {
            unBindRequested = ETrue;
            }
        }
    
    if ( ctxState == MXIMPContextState::EInactive && 
        iReqIdArray.Count() > 0 &&
        !unBindRequested )
        {
        //Unbind was not requested, but EInactive state event received => try forced unbind
        TRAPD( binderr, HandleDeregistrationL( EFalse ) );
        SCPLOGSTRING2( "CScpPresenceHandler::HandleContextStateEvent: Unbind err:%d", binderr );
        if ( KErrNone != binderr )
            {
            DeregisterNow();
            }
        }
    }


// -----------------------------------------------------------------------------
// CScpPresenceHandler::HandleRequestCompleteEvent
// Handles the XIMP request complete events
// -----------------------------------------------------------------------------
//
void CScpPresenceHandler::HandleRequestCompleteEvent( const MXIMPBase& aEvent )
    {
    SCPLOGSTRING( "CScpPresenceHandler::HandleRequestCompleteEvent event MximpRequestCompleteEvent" );
    const MXIMPRequestCompleteEvent* event =
        TXIMPGetInterface< const MXIMPRequestCompleteEvent >::From( aEvent, MXIMPBase::EPanicIfUnknown );

    const TXIMPRequestId& reqId = event->RequestId();
    const MXIMPStatus& status = event->CompletionResult();
    if ( &status )
        {
        iLastXimpError = status.ResultCode();
        }
    SCPLOGSTRING2( "CScpPresenceHandler -> request ximp error: %d", iLastXimpError );
       
    if ( iLastXimpError == KXIMPErrServicRequestTimeouted )
        {
        iSubService.HandleConnectionEvent( EScpRegistrationFailed );
        }
    
    // Find the reqId from the reqid array and store it's type
    TInt index( KErrNotFound );
    TScpReqType reqType( EUnknownReq );
    for ( TInt i = 0; i < iReqIdArray.Count(); i++ )
        {
        if ( iReqIdArray[i].ReqId() == reqId )
            {
            index = i;
            reqType = iReqIdArray[i].Type();
            }
        }
    
    if ( index != KErrNotFound )
        {
        iReqIdArray.Remove( index );             
        }
    
    SCPLOGSTRING2( "CScpPresenceHandler -> request type: %d", reqType );

    // Bind complete event
    if ( ( reqType == EBindReq ) &&
        ( EBinding == iPresenceState ) &&
        ( iLastXimpError == KErrNone ) )
        {
        HandleBindCompleteEvent();
        }
    // Published online request complete
    else if ( ( reqType == EPublishOnlineReq ) && 
        ( iLastXimpError == KErrNone ) )
        {
        TRAP_IGNORE( SubscribePresentityGroupL() );
        iPresenceState = ESubscribing;
        }
    
    else if ( ( reqType == ESubscribeReq ) &&
        ( iLastXimpError == KErrNone ) )
        {
        // Set subservice enabled after subscribe is successfully done
        iPresenceState = EPresenceOnline;
        iSubService.HandleConnectionEvent( EScpRegistered );
        }
    
    // Published offline request complete
    else if ( reqType == EPublishOfflineReq )
        {
        SCPLOGSTRING( "CScpPresenceHandler::HandleRequestCompleteEvent status offline Unbind now" );
        TRAPD( err, ServerUnBindL() );
        // Deregistration ongoing, so if unbind fails, deregisterNow is called
        if ( err )
            {
            DeregisterNow();
            SCPLOGSTRING2( "CScpPresenceHandler::HandleRequestCompleteEvent status offline Unbind now end, err %d", err );
            }
        }
    
    // Unbind request complete
    else if ( reqType == EUnBindReq && EUnBinding == iPresenceState )
        {
        SCPLOGSTRING( "CScpPresenceHandler::HandleRequestCompleteEvent status offline" );       
        DeregisterNow();
        SCPLOGSTRING( "CScpPresenceHandler::HandleRequestCompleteEvent status offline end" );
        }
    
    if ( iRebind )
        {
         TRAPD( err, ServerBindL() );
         iRebind = EFalse;
         SCPLOGSTRING2( "CScpPresenceHandler - ServerBindL failed: %d", err );
         
         if ( KErrAlreadyExists == err )
             {
             iRebind = ETrue;
             }
        }
    
    if ( iDisableAfterXimpRequestsCompleted && !iReqIdArray.Count() )
        {
        iDisableAfterXimpRequestsCompleted = EFalse;
        DisableSubService();
        }
    
    SCPLOGSTRING( "CScpPresenceHandler::HandleRequestCompleteEvent OUT" ); 
    }


// -----------------------------------------------------------------------------
// CScpPresenceHandler::HandleBindCompleteEvent
// Handles the bind complete event
// -----------------------------------------------------------------------------
//
void CScpPresenceHandler::HandleBindCompleteEvent()
    {
    SCPLOGSTRING( "CScpPresenceHandler::HandleBindCompleteEvent" );
    
    iPresenceState = EBindComplete;
    
    TInt err( KErrNone );
    CScpSubService* voipSubService = GetVoipSubService();
    if ( voipSubService )
        {
        if ( voipSubService->State() == ECCHEnabled &&
            voipSubService->LastReportedError() == KErrNone )
            {
            SCPLOGSTRING( "CScpPresenceHandler::HandleBindCompleteEvent PublishOnline");
            TRAP( err, PublishPresenceL( ETrue ) );
            }
        else
            {
            SCPLOGSTRING( "CScpPresenceHandler::HandleBindCompleteEvent PublishOffline");
            TRAP( err, PublishPresenceL( EFalse ) );
            }
        }
    else
        {
        SCPLOGSTRING( "CScpPresenceHandler::HandleBindCompleteEvent PublishOffline, no VoIP");
        TRAP( err, PublishPresenceL( EFalse ) );
        err = KErrNotFound;
        }
     
    if ( err )
        {
        SCPLOGSTRING2( "CScpPresenceHandler::HandleBindCompleteEvent err %d", err );
        }   
    }


// -----------------------------------------------------------------------------
// CScpPresenceHandler::SubscribePresentityGroupL
// 
// -----------------------------------------------------------------------------
//
void CScpPresenceHandler::SubscribePresentityGroupL()
    {
    SCPLOGSTRING( "CScpPresenceHandler::SubscribePresentityGroupL subsribe buddies" );
    MXIMPObjectFactory& objFactoryFromXIMP = iPresenceCtx->ObjectFactory();
    
    //First we have to make identity
    SCPLOGSTRING( "CScpPresenceHandler::SubscribePresentityGroupL create buddy list identity" );
    MXIMPIdentity* groupIdentity = objFactoryFromXIMP.NewIdentityLC();
    groupIdentity->SetIdentityL( _L("buddylist") ); // NEEDS TO BE DEFINED IN XIMP API
    
    SCPLOGSTRING( "CScpPresenceHandler::SubscribePresentityGroupL subsribe group content" );
    TScpReqId reqId;

    reqId.SetType( ESubscribeReq );
    reqId.SetId( iFeature->PresentityGroups().SubscribePresentityGroupContentL(
        *groupIdentity ) );
    SCPLOGSTRING( "CScpPresenceHandler::SubscribePresentityGroupL append req to array" );
    iReqIdArray.Append( reqId );

    SCPLOGSTRING( "CScpPresenceHandler::SubscribePresentityGroupL subsribe group content ok" );
    CleanupStack::PopAndDestroy( 1 ); // groupIdentity
    }


// -----------------------------------------------------------------------------
// CScpPresenceHandler::UnsubscribePresentityGroupL
// 
// -----------------------------------------------------------------------------
//
void CScpPresenceHandler::UnsubscribePresentityGroupL()
    {
    MXIMPObjectFactory& objFactoryFromXIMP = iPresenceCtx->ObjectFactory();
    
    //First we have to make identity
    MXIMPIdentity* groupIdentity = objFactoryFromXIMP.NewIdentityLC();
    groupIdentity->SetIdentityL( _L("buddylist") ); // NEEDS TO BE DEFINED IN XIMP API
    
    TScpReqId reqId;
    // Set type to Unknown, because we don't need to handle the
    // request complete event
    reqId.SetType( EUnknownReq );
    reqId.SetId( iFeature->PresentityGroups().UnsubscribePresentityGroupContentL(
        *groupIdentity ) );
    iReqIdArray.Append( reqId );
    
    CleanupStack::PopAndDestroy( 1 );
    }

// -----------------------------------------------------------------------------
// CScpPresenceHandler::GetStoredPresenceValuesL
// 
// -----------------------------------------------------------------------------
//
void CScpPresenceHandler::GetStoredPresenceValuesL( TInt& aAvailabilityEnum, RBuf& aCustomMessage )
    {
    MVIMPSTSettingsStore* settings = CVIMPSTSettingsStore::NewLC();
        
    TInt serviceId = iSubService.SubServiceId();
    RBuf8 documentId;
    CleanupClosePushL( documentId );
    documentId.CreateL( KBufSize255 );
    TInt error = settings->GetL( 
        serviceId, EServicePresenceSessionIdentifier, documentId );
    TInt i = documentId.Length();
    TInt err1(0);
    TInt err2(0);
    
    if( documentId.Length() )
        {
        err1 = settings->GetL( 
            serviceId, EServicePresenceAvailablilityValue, aAvailabilityEnum );
        err2 = settings->GetL( 
            serviceId, EServiceCustomStatusMessage, aCustomMessage );
        
        // if other GetL fails -> clear both
        if( err1 || err2 )
            {
            aAvailabilityEnum = KErrNotFound;
            aCustomMessage.Zero();
            }
        }   
    else 
        {
        aAvailabilityEnum = KErrNotFound;
        }

    CleanupStack::PopAndDestroy( &documentId );
    CleanupStack::PopAndDestroy(); //settings
    }

//  End of File
