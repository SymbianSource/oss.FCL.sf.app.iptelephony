/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Creates SMS message if Event in MailBox has occurred.
*
*/



// Include hierarchy change can't be done. Smuthdr.h uses
// other files from messaging/sms (smsstd.h and smutset.h). It doesn't seem
// to find them without old way to declare include folder using SYSTEMINCLUDE.
//
#include <smuthdr.h>/*messaging/\sms/\*/
#include <msvuids.h>
#include <txtrich.h>
#include <spsettings.h>
#include <spproperty.h>
#include <utf.h>
#include <mcemanager.h>
#include <ipvoicemailengine.rsg>

#include "ipvmbxengine.h"
#include "ipvmbxeventmonitor.h"
#include "ipvmbxbase.h"
#include "ipvmbxlogger.h"
#include "ipvmbxconstants.h"
#include "ipvmbxpanic.h"


const TUid KAppUID = { 0x1020596F };
const TInt KMaxMessageDigits = 10;
const TInt KAccountTextLength = 21;
const TInt KOneChar = 1;
const TInt KTwoChars = 2;
const TInt KSmsLength = 160;
const TInt KTotalLength = 2;
const TInt KTotalOldLength = 3;
const TInt KMinIpVoiceMailBoxUriLength = 3;
const TInt KSpecialNameCharsCount = 10;
const TInt KMaxMsgDescriptions = 99;

_LIT( KEndLine, "\n" );
_LIT( KTotal, "%N" );
_LIT( KTotalNew, "%0N" );
_LIT( KIpVmbxAppEngineResourceFileDirectory, "\\resource\\" );
_LIT( KIpVmbxAppEngineResourceFileName, "ipvoicemailengine.rsc" );
_LIT( KSipString, "sip:");
_LIT( KOptionalSeparator, "\n\n" );
_LIT8( KTotalOld, "%1N" );
_LIT8( KOneMessage, "1" );
_LIT8( KNoMessages, "0" );
_LIT8( KMessagesWaiting8, "messages-waiting" );
_LIT8( KNewMessages8, "yes" );
_LIT8( KMessageAccount8, "message-account" );
_LIT8( KVoiceMessage8, "voice-message" );
_LIT8( KSlash8, "/" );
_LIT8( KLParen8, "(" );
_LIT8( KRParen8, ")" );
_LIT8( KColon8, ":" );
_LIT8( KCrlf8, "\r\n" );
_LIT8( KEndLine8, "\n" );
_LIT8( KHTab8, "\t");
_LIT8( KSpace8, " ");

const TText KNameDigitLowest = '\x30';
const TText KNameDigitHighest = '\x39';
const TText KNameCharUpLowest = '\x41';
const TText KNameCharUpHighest = '\x5a';
const TText KNameCharLowLowest = '\x61';
const TText KNameCharLowHighest = '\x7a';
// Legal characters in name header
// 2d, 2e, 21, 25, 2a, 5f, 2b, 60, 27, 7e
const TText8 KSpecialNameChars[KSpecialNameCharsCount] =
    {
    '-',
    '.',
    '!',
    '%',
    '*',
    '_',
    '+',
    '`',
    '\'',
    '~'
    };
const TText KSpace = '\x20';
const TText KValueLowestChar = KSpace;
const TText KValueHighestChar = '\xfd';
const TText KCr = '\x0d';
const TText KLf = '\x0a';
const TText KHTab = '\x09';


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CIpVmbxEngine::CIpVmbxEngine( CIpVmbxInterface& aInterface ):
    iEventData(),
    iInterface( aInterface )
    {
    }


// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CIpVmbxEngine::ConstructL()
    {
    // One manager = one event observer (restricted)
    iIpVmbxEventMonitor = new ( ELeave ) TIpVmbxEventMonitor( *this );
    }


// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
EXPORT_C CIpVmbxEngine* CIpVmbxEngine::NewL( CIpVmbxInterface& aInterface )
    {
    IPVMEPRINT( "CIpVmbxEngine::NewL" );

    CIpVmbxEngine* self = new( ELeave ) CIpVmbxEngine( aInterface );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// ----------------------------------------------------------------------------
// Destructor.
// ----------------------------------------------------------------------------
//
CIpVmbxEngine::~CIpVmbxEngine()
    {
    IPVMEPRINT( "CIpVmbxEngine::~CIpVmbxEngine - IN" );

    iVmbxBaseArray.ResetAndDestroy();
    iVmbxBaseArray.Close();

    delete iMceManager;
    delete iServiceSettings;
    delete iIpVmbxEventMonitor;

    IPVMEPRINT( "CIpVmbxEngine::~CIpVmbxEngine - OUT" );
    }


// ----------------------------------------------------------------------------
// Starts subscription to VoiceMailBox -server
// ----------------------------------------------------------------------------
//
void CIpVmbxEngine::SubscribeProfileL(
    TUint32 aServiceProviderId,
    CSIPProfile& aSipProfile )
    {
    IPVMEPRINT( "CIpVmbxEngine::SubscribeProfileL - IN" );

    if ( !iBasicServicesRunning )
        {
        iServiceSettings = CSPSettings::NewL();
        // one manager recommended for one component
        iMceManager = CMceManager::NewL( KAppUID, &iEventData );
        iMceManager->SetEventObserver( iIpVmbxEventMonitor );
        iBasicServicesRunning = ETrue;
        }

    HBufC16* voiceMailUri16 = HBufC16::NewLC( KMaxIpVoiceMailBoxUriLength );
    TPtr16 ptrvoiceMailUri16( voiceMailUri16->Des() );
    TInt reSubscribe = 0;

    // Fetch MWI address
    CSPProperty* mwiAddress = CSPProperty::NewLC();
    User::LeaveIfError( iServiceSettings->FindPropertyL(
        aServiceProviderId,
        ESubPropertyVMBXMWIAddress,
        *mwiAddress ) );
    User::LeaveIfError( mwiAddress->GetValue( ptrvoiceMailUri16 ) );
    if ( ptrvoiceMailUri16.Length() < KMinIpVoiceMailBoxUriLength )
        {
        User::Leave( KErrNotFound );
        }
    IPVMEPRINT( "CIpVmbxEngine::MWI found" );
    if ( KErrNotFound == ptrvoiceMailUri16.Find( KSipString ) )
        {
        ptrvoiceMailUri16.Insert( 0, KSipString );
        }
    CleanupStack::PopAndDestroy( mwiAddress );

    // Fetch also subscribe interval
    CSPProperty* mwiInterval = CSPProperty::NewLC();
    User::LeaveIfError( iServiceSettings->FindPropertyL(
        aServiceProviderId,
        ESubPropertyVMBXMWISubscribeInterval,
        *mwiInterval) );
    User::LeaveIfError( mwiInterval->GetValue( reSubscribe ) );
    CleanupStack::PopAndDestroy( mwiInterval );

    HBufC8* voiceMailUri8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(
        ptrvoiceMailUri16 );
    CleanupStack::PopAndDestroy( voiceMailUri16 );
    CleanupStack::PushL( voiceMailUri8 );

    TInt index = iVmbxBaseArray.Count();
    TBool newProfile = ETrue;
    if ( index )
        {
        // Might be second subscription or pending operation.
        // Though IPVME and SCP supports currently only one VMBX connection
        for ( TInt i = 0; i < index; i++ )
            {
            CIpVmbxBase* const subscription( iVmbxBaseArray[i] );
            if ( *voiceMailUri8 == subscription->VmbxUrl() )
                {
                // Subscription already exists
                if ( CIpVmbxBase::EDisabled == subscription->State() )
                    {
                    // Subscription was disabled, enable
                    newProfile = EFalse;
                    subscription->Initialize(
                        aServiceProviderId,
                        aSipProfile );

                    subscription->SubscribeL( reSubscribe );
                    }
                else
                    {
                    User::Leave( KErrAlreadyExists );
                    }
                }
            }
        }
    if ( newProfile )
        {
        // Subscription to new VMBX account
        IPVMEPRINT( "CIpVmbxEngine::New AppBase" );
        CIpVmbxBase* app = CIpVmbxBase::NewL(
            *this,
            *voiceMailUri8,
            *iMceManager );
        CleanupStack::PushL( app );
        iVmbxBaseArray.AppendL( app );
        CleanupStack::Pop( app );

        app->Initialize( aServiceProviderId, aSipProfile );
        app->SubscribeL( reSubscribe );
        }

    CleanupStack::PopAndDestroy( voiceMailUri8 );

    IPVMEPRINT( "CIpVmbxEngine::SubscribeProfileL - OUT" );
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CIpVmbxEngine::ProfileSubscribed(
    TUint32 aServiceProviderId,
    TBool& aProfileSubscribed )
    {
    CIpVmbxBase* subscription( SubscriptionByProvider( aServiceProviderId ) );
    TInt err = KErrNotFound;

    if ( subscription )
        {
        err = KErrNone;
        if ( CIpVmbxBase::ERegistered == subscription->State() )
            {
            aProfileSubscribed = ETrue;
            }
        else
            {
            aProfileSubscribed = EFalse;
            }
        }

    return err;
    }


// ---------------------------------------------------------------------------
// Resolve base class matching to recipient
// ---------------------------------------------------------------------------
//
CIpVmbxBase* CIpVmbxEngine::SubscriptionByRecipient(
    const TDesC8& aRecipient8 )
    {
    CIpVmbxBase* base( NULL );

    for ( TInt i = 0; i < iVmbxBaseArray.Count(); i++ )
        {
        if ( iVmbxBaseArray[i]->VmbxUrl() == aRecipient8 )
            {
            base = iVmbxBaseArray[i];
            }
        }

    return base;
    }


// ----------------------------------------------------------------------------
// EventMonitor has received event and the event will be gained and checked
// ----------------------------------------------------------------------------
//
void CIpVmbxEngine::EventReceivedL( const TDesC8& aRecipient8 )
    {
    IPVMEPRINT( "CIpVmbxEngine::EventReceivedL - IN" );

    CIpVmbxBase* subscription( SubscriptionByRecipient( aRecipient8 ) );
    if ( !subscription )
        {
        User::Leave( KErrNotFound );
        }

    HBufC8* content8( iEventData.GetContent() );
    CleanupStack::PushL( content8 );

    HBufC8* totalMessages8 = HBufC8::NewLC( KMaxMessageDigits );
    HBufC8* newMessages8 = HBufC8::NewLC( KMaxMessageDigits );
    HBufC8* from8 = HBufC8::NewLC(
        KMaxIpVoiceMailBoxUriLength + KAccountTextLength );
    TPtr8 ptrTotalMessages8( totalMessages8->Des() );
    TPtr8 ptrNewMessages8( newMessages8->Des() );
    TPtr8 ptrFrom8( from8->Des() );
    TPtr8 ptrContent8( content8->Des() );

    TBool createSMS = EFalse;
    // no need to handle errors, returned parameters can still
    // be interpreted correctly
    //
    TRAP_IGNORE(
        ParseNotifyContentL(
            createSMS,
            ptrContent8,
            ptrTotalMessages8,
            ptrNewMessages8,
            ptrFrom8 ) );

    if ( ptrNewMessages8 == KNoMessages )
        {
        // protocol test fix, new message should not be created
        // if new message count is 0
        createSMS = EFalse;
        }
    if ( 0 == ptrFrom8.Length() )
        {
        ptrFrom8 = subscription->VmbxUrl();
        }

    TInt totalMsgs = 0;
    TInt newMsgs = 0;
    TLex8 msgsConvert;
    msgsConvert.Assign( *totalMessages8 );
    if ( KErrNone == msgsConvert.Val( totalMsgs ) )
        {
        msgsConvert.Assign( *newMessages8 );
        if ( KErrNone != msgsConvert.Val( newMsgs ) )
            {
            totalMsgs = 0;
            }
        }

    TInt curTotal = 0;
    TInt curNew = 0;
    subscription->AccountMessageCount( curTotal, curNew );

    TBool statusChanged = EFalse;
    if ( totalMsgs != curTotal || newMsgs != curNew )
        {
        statusChanged = ETrue;
        subscription->SetAccountMessageCount( totalMsgs, newMsgs );
        }

    if ( createSMS && statusChanged )
        {
        TBuf8< KSmsLength > messageBody8;
        CreateMessageBodyL(
            *content8,
            ptrTotalMessages8,
            ptrNewMessages8,
            ptrFrom8,
            messageBody8 );
        CreateSMSMessageL( ptrFrom8, messageBody8 );
        }

    CleanupStack::PopAndDestroy( from8 );
    CleanupStack::PopAndDestroy( newMessages8 );
    CleanupStack::PopAndDestroy( totalMessages8 );
    CleanupStack::PopAndDestroy( content8 );

    IPVMEPRINT( "CIpVmbxEngine::EventReceivedL - OUT" );
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CIpVmbxEngine::UnsubscribeProfileL( TUint32 aServiceProviderId )
    {
    IPVMEPRINT2( "CIpVmbxEngine::UnsubscribeProfileL: %d - IN", 
	    aServiceProviderId );
    
    CIpVmbxBase* subscription( SubscriptionByProvider( aServiceProviderId ) );
    if ( !subscription )
        {
        User::Leave( KErrNotFound );
        }
    
    IPVMEPRINT2( "CIpVmbxEngine::UnsubscribeProfileL: state=%d",
	    subscription->State() );

    switch( subscription->State() )
        {
        case CIpVmbxBase::ERegistered:
            {
            subscription->TerminateEventL();
            break;
            }
        case CIpVmbxBase::EDisabled:
            {
            CleanVmbxBase();
            break;
            }
        case CIpVmbxBase::ESubscribing:
            {
            subscription->Cancel();
            CleanVmbxBase();
            break;
            }
        case CIpVmbxBase::ETerminating:
            {
            User::Leave( KErrCancel );
            break;
            }
        default:
            {
            IPVMEPRINT( "No implementation" );
            }
        }

    }


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CIpVmbxEngine::CleanVmbxBase()
    {
    for ( TInt i = 0; i < iVmbxBaseArray.Count(); i++ )
        {
        if ( CIpVmbxBase::ETerminating == iVmbxBaseArray[i]->State() ||
            CIpVmbxBase::EDisabled == iVmbxBaseArray[i]->State() )
            {
            delete iVmbxBaseArray[i];
            iVmbxBaseArray.Remove( i );
            }
        }
    }


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CIpVmbxEngine::HandleMessage(
    TUint32 aServiceProviderId,
    CIpVmbxEngine::TIpVmbxMessages aMessage )
    {
    IPVMEPRINT2( "CIpVmbxEngine::HandleMessage: %d - IN", aMessage );

    CIpVmbxBase* subscription( SubscriptionByProvider( aServiceProviderId ) );
    // Save current state because some functions modify states
    CIpVmbxBase::TIpVmbxBaseStates baseState = subscription->State();
    MIpVmbxObserver::TVmbxMessage message =
        MIpVmbxObserver::EIncorrectSettings;
    TBool send = ETrue;

    switch ( aMessage )
        {
        case CIpVmbxEngine::EEngineSubscribed:
            message = MIpVmbxObserver::ESubscribed;
            break;
        case CIpVmbxEngine::EEngineTerminated:
            {
            if ( CIpVmbxBase::EDisabled != baseState )
                {
                CleanVmbxBase();
                message = MIpVmbxObserver::EUnsubscribed;
                }
            else
                {
                send = EFalse;
                }
            break;
            }
        case CIpVmbxEngine::EEngineUndefined:
            {
            send = EFalse;
            break;
            }
        case CIpVmbxEngine::EEngineSubscribeRejected:
        case CIpVmbxEngine::EEngineIncorrectAccount:
            {
            subscription->Cancel();
            CleanVmbxBase();
            message = MIpVmbxObserver::EIncorrectSettings;
            break;
            }
        case CIpVmbxEngine::EEngineSmsError:
            {
            subscription->Cancel();
            message = MIpVmbxObserver::ESmsError;
            break;
            }
        case CIpVmbxEngine::EEngineSmsOom:
            message = MIpVmbxObserver::ENoMemory;
            break;
        case CIpVmbxEngine::EEngineNetworkLost:
            {
            subscription->Cancel();
            subscription->DeleteEvent();
            send = EFalse;
            break;
            }
        case CIpVmbxEngine::EEngineNetworkError:
            {
            subscription->Cancel();
            subscription->DeleteEvent();
            message = MIpVmbxObserver::ENetworkError;
            break;
            }
        default:
            IPVMEPRINT( "Unhandled message!" );
        }

    if ( send )
        {
        iInterface.SendMessage( aServiceProviderId, message );
        }

    IPVMEPRINT( "CIpVmbxEngine::HandleMessage - OUT" );
    }


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
CIpVmbxBase* CIpVmbxEngine::SubscriptionByProvider(
    TUint32 aServiceProviderId )
    {
    CIpVmbxBase* base( NULL );
    for ( TInt i = 0; i < iVmbxBaseArray.Count(); i++ )
        {
        if ( iVmbxBaseArray[i]->ServiceProviderId() == aServiceProviderId )
            {
            base = iVmbxBaseArray[i];
            }
        }
    return base;
    }


// ----------------------------------------------------------------------------
// From class MMsvSessionObserver.
// Indicates an event has occurred from a Message Server session.
// ----------------------------------------------------------------------------
//
void CIpVmbxEngine::HandleSessionEventL(
    TMsvSessionEvent /*aEvent*/,
    TAny* /*aArg1*/,
    TAny* /*aArg2*/,
    TAny* /*aArg3*/ )
    {
    IPVMEPRINT( "CIpVmbxEngine::HandleSessionEventL - Dummy implementation" );
    }


// ----------------------------------------------------------------------------
// Parses critical and important optional parts of content. Already
// processed data is cut from content.
// ----------------------------------------------------------------------------
//
void CIpVmbxEngine::ParseNotifyContentL(
    TBool& aCreateSms,
    TDes8& aContent8,
    TDes8& aTotalMessages8,
    TDes8& aNewMessages8,
    TDes8& aFrom8 ) const
    {
    IPVMEPRINT( "CIpVmbxEngine::ParseNotifyContentL - IN" );
    aCreateSms = EFalse;

    // check required content
    TLex8 analyzer8;
    TPtrC8 posPtr8;
    TPtrC8 dataPtr8;
    TPtrC8 messagePtr8;

    TInt atPosWaiting = aContent8.FindF( KMessagesWaiting8 );
    // check important OPTIONAL fields
    TInt atPosAccount = aContent8.FindF( KMessageAccount8 );
    TInt atPosMessage = aContent8.FindF( KVoiceMessage8 );

    if ( 0 == atPosWaiting )
        {
        // data field found from correct place
        messagePtr8.Set( FetchMessagePartL( aContent8 ) );
        analyzer8.Assign(
            messagePtr8.Mid( messagePtr8.Find( KColon8 ) + KOneChar ) );
        dataPtr8.Set( analyzer8.NextToken() );
        if ( 0 == dataPtr8.CompareF( KNewMessages8 ) )
            {
            // message(s) waiting
            aCreateSms = ETrue;
            }
        // cut processed data
        posPtr8.Set( aContent8.Mid( messagePtr8.Length() ) );
        analyzer8.Assign( posPtr8 );
        }
    else
        {
        // malformed critical part of message
        User::Leave( KErrCorrupt );
        }

    if ( KErrNotFound != atPosAccount && KErrNotFound != atPosMessage )
        {
        // both optionals found
        if ( !( atPosAccount < atPosMessage ) )
            {
            // incorrect format
            User::Leave( KErrCorrupt );
            }
        }

    if ( KErrNotFound != atPosAccount && aCreateSms )
        {
        // get account
        messagePtr8.Set( FetchMessagePartL( posPtr8 ) );
        analyzer8.Assign(
            messagePtr8.Mid( messagePtr8.Find( KColon8 ) + KOneChar ) );
        dataPtr8.Set( analyzer8.NextToken() );
        posPtr8.Set( posPtr8.Mid( messagePtr8.Length() ) );
        analyzer8.Assign( posPtr8 );


        aFrom8.Copy( dataPtr8.Left( aFrom8.MaxLength() ) );
        }

    if ( KErrNotFound != atPosMessage && aCreateSms )
        {
        messagePtr8.Set( FetchMessagePartL( posPtr8 ) );
        analyzer8.Assign(
            messagePtr8.Mid( messagePtr8.Find( KColon8 ) + KOneChar ) );

        // can hold value required by specification
        TUint oldMessageCount = 0;
        TUint newMessageCount = 0;
        TLex8 value;

        analyzer8.SkipSpace();
        User::LeaveIfError( analyzer8.Val( newMessageCount ) );
        analyzer8.SkipSpace();
        User::LeaveIfError( KSlash8().Locate( analyzer8.Get() ) );

        analyzer8.SkipSpace();
        User::LeaveIfError( analyzer8.Val( oldMessageCount ) );
        // save result here because of leaving parts above
        aNewMessages8.Num( newMessageCount );
        // result saved here in case of leave from optional parsing of optional part below
        aTotalMessages8.Num( ( TInt64 ) oldMessageCount +  ( TInt64 ) newMessageCount );

        analyzer8.SkipSpace();
        if ( KErrNotFound != KLParen8().Locate( analyzer8.Get() ) )
            {
            // urgent messages found
            TUint urgentNew = 0;
            TUint urgentOld = 0;
            User::LeaveIfError( analyzer8.Val( urgentNew ) );
            analyzer8.SkipSpace();
            User::LeaveIfError( KSlash8().Locate( analyzer8.Get() ) );

            User::LeaveIfError( analyzer8.Val( urgentOld ) );
            newMessageCount += urgentNew;
            oldMessageCount += urgentOld;
            analyzer8.SkipSpace();
            User::LeaveIfError( KRParen8().Locate( analyzer8.Get() ) );
            }
        // save status again, might have updated
        aNewMessages8.Num( newMessageCount );
        aTotalMessages8.Num( ( TInt64 ) oldMessageCount + ( TInt64 ) newMessageCount );

        posPtr8.Set( posPtr8.Mid( messagePtr8.Length() ) );
        }

    // Clean string off processed data
    aContent8.Delete( 0, aContent8.Length() - posPtr8.Length() );

    IPVMEPRINT( "CIpVmbxEngine::ParseNotifyContentL - OUT" );
    }


// ----------------------------------------------------------------------------
// Creates SMS message and sends it
// ----------------------------------------------------------------------------
//
void CIpVmbxEngine::CreateSMSMessageL(
    const TDesC8& aFrom8,
    const TDesC8& aMessageBody8 )
    {
    IPVMEPRINT( "CIpVmbxEngine::CreateSMSMessageL - IN" );

    // Create the SMS header object...
    CParaFormatLayer* paraFormatLayer = CParaFormatLayer::NewL();
    CleanupStack::PushL( paraFormatLayer );
    CCharFormatLayer* charFormatLayer = CCharFormatLayer::NewL();
    CleanupStack::PushL( charFormatLayer );

    CRichText* bodyText = CRichText::NewL( paraFormatLayer, charFormatLayer );
    CleanupStack::PushL( bodyText );
    TInt position = 0;
    TBuf< KSmsLength > messageBody;
    messageBody.Copy( aMessageBody8.Left( messageBody.MaxLength() ) );
    bodyText->InsertL( position, messageBody );

    CSmsHeader* header = CSmsHeader::NewL( CSmsPDU::ESmsDeliver, *bodyText );
    CleanupStack::PushL( header );
    TBuf< KMaxIpVoiceMailBoxUriLength > from;
    from.Copy( aFrom8 );
    header->SetFromAddressL( from );

    TMsvEntry entry;
    entry.SetVisible(ETrue );
    entry.SetUnread( ETrue );
    entry.SetNew( ETrue );
    entry.iServiceId = KMsvRootIndexEntryId;
    entry.iType = KUidMsvMessageEntry;
    entry.iMtm = KUidMsgTypeSMS;
    entry.iDate.UniversalTime();
    entry.iSize = 0;
    entry.iDescription.Set( KNullDesC );
    entry.iDetails.Set( KNullDesC );

    header->Deliver().SetServiceCenterTimeStamp( entry.iDate );

    CSmsSettings* smsSettings = CSmsSettings::NewL();
    CleanupStack::PushL( smsSettings );
    smsSettings->SetDelivery( ESmsDeliveryImmediately );
    smsSettings->SetValidityPeriod( ESmsVPWeek );
    smsSettings->SetValidityPeriodFormat( TSmsFirstOctet::ESmsVPFInteger );
    smsSettings->SetReplyQuoted( EFalse );
    smsSettings->SetRejectDuplicate( ETrue );
    smsSettings->SetDelivery( ESmsDeliveryImmediately );
    smsSettings->SetDeliveryReport( ETrue );
    smsSettings->SetReplyPath( EFalse );
    smsSettings->SetMessageConversion( ESmsConvPIDNone );
    smsSettings->SetCanConcatenate( ETrue );
    smsSettings->SetUseServiceCenterTimeStampForDate( ETrue );

    header->SetSmsSettingsL(*smsSettings );

    CSmsNumber* rcpt = CSmsNumber::NewL();
    CleanupStack::PushL( rcpt );
    rcpt->SetAddressL( from.Left( KSmcmSmsNumberMaxNumberLength ) );
    header->Recipients().AppendL( rcpt );
    CleanupStack::Pop( rcpt );

    // Update entry description and details...
    CArrayPtrFlat< CSmsNumber >& recipient = header->Recipients();
    entry.iDetails.Set( recipient[0]->Address() );
    entry.iDescription.Set( bodyText->Read(
        0, smsSettings->DescriptionLength() ));
    entry.SetInPreparation( EFalse );

    // Create the entry - set context to the global outbox.
    TMsvSelectionOrdering ordering = TMsvSelectionOrdering(
        KMsvNoGrouping,
        EMsvSortByDescription,
        ETrue );
        CMsvSession* session = CMsvSession::OpenSyncL( *this );
        CleanupStack::PushL( session );
    CMsvEntry* centry = CMsvEntry::NewL(
        *session, KMsvRootIndexEntryId, ordering );
    CleanupStack::PushL( centry );
    centry->SetEntryL( KMsvGlobalInBoxIndexEntryId );
    centry->CreateL( entry );

    // Create new store and save header information
    centry->SetEntryL( entry.Id() );
    CMsvStore* store = centry->EditStoreL();
    CleanupStack::PushL(store);
    header->StoreL( *store );
    store->StoreBodyTextL( *bodyText );
    store->CommitL();

    CleanupStack::PopAndDestroy( store );
    CleanupStack::PopAndDestroy( centry );
    CleanupStack::PopAndDestroy( session );
    CleanupStack::PopAndDestroy( smsSettings );
    CleanupStack::PopAndDestroy( header );
    CleanupStack::PopAndDestroy( bodyText );
    CleanupStack::PopAndDestroy( charFormatLayer );
    CleanupStack::PopAndDestroy( paraFormatLayer );

    IPVMEPRINT( "CIpVmbxEngine::CreateSMSMessageL - OUT" );
    }


// ----------------------------------------------------------------------------
// Parse optional headers. Parser must take account various optional
// characters in headers. String lengths are monitored every round to prevent
// panics when setting data.
// ----------------------------------------------------------------------------
//
void CIpVmbxEngine::ParseOptionalHeadersL(
    const TDesC8& aContent8,
    TDes8& aMessageBody8 ) const
    {
    IPVMEPRINT( "CIpVmbxEngine::ParseOptionalHeadersL - IN" );

    if ( aContent8.Left( KCrlf8().Length() ).Compare( KCrlf8  ) ||
        aContent8.Length() == KTwoChars )
        {
        // Checking if optional message does not exist or its too short.
        // Will also fix parsing of non standard basic Pingtel message.
        //
        User::Leave( KErrNotFound );
        }

    TFileName dll;
    Dll::FileName( dll );
    TFileName fileName( TParsePtrC( dll ).Drive() );
    fileName += KIpVmbxAppEngineResourceFileDirectory;
    fileName += KIpVmbxAppEngineResourceFileName;
    CStringResourceReader* resourceReader =
        CStringResourceReader::NewL( fileName );
    CleanupStack::PushL( resourceReader );

    if ( aMessageBody8.MaxLength() == aMessageBody8.Length() )
        {
        User::Leave( KErrOverflow );
        }
    else
        {
        // Add another separator before optional messages
        aMessageBody8.Append( KEndLine8 );
        }

    TPtrC8 messagePtr8;
    TPtrC8 tagPtr8;
    TPtrC8 dataPtr8;
    TPtrC8 posPtr8( aContent8 );
    TPtrC8 colon8;
    TInt messages = 0;
    HBufC* locTemp( HBufC::NewLC( KSmsLength ) );
    TPtr locPtr( locTemp->Des() );
    TPtrC resourcePtr( resourceReader->ReadResourceString( R_VOIP_VM_HEADER_COUNT ) );
    HBufC* variant( NULL );
    TLex8 analyzer8;
    HBufC8* partTemp( HBufC8::NewLC( KSmsLength ) );
    TPtr8 partPtr( partTemp->Des() );
    TBool messageEmpty = EFalse; // prevents creation of empty optional message part
    TBool appendMsgChange = EFalse;
    // Start sorting through message
    do
        {
        if ( ( KErrNotFound !=
            posPtr8.Left( KCrlf8().Length() ).FindF( KCrlf8 ) ||
            0 == messages ) && !messageEmpty )
            {
            // beginning of optional messages or another optional message
            if ( messages <= KMaxMsgDescriptions )
                {
                // add message count for first/next optional message
                // count is limited to two numbers = 99 (prevents also overflow when
                // KTotal is replaced)
                TBuf< KTwoChars > appendNum;
                appendNum.AppendNum( ++messages );
                locPtr.Zero();
                locPtr.Append( KOptionalSeparator );
                locPtr.Append( resourcePtr );
                locPtr.Replace( locPtr.Find( KTotal ), KTotal().Length(), appendNum );
                locPtr.Append( KEndLine );
                posPtr8.Set( posPtr8.Mid( KCrlf8().Length() ) );
                messageEmpty = ETrue;
                appendMsgChange = ETrue;
                }
            else
                {
                User::Leave( KErrOverflow );
                }
            }
        messagePtr8.Set( FetchMessagePartL( posPtr8 ) );
        analyzer8.Assign(
            messagePtr8.Left( messagePtr8.Find( KColon8 ) + KOneChar ) );
        tagPtr8.Set( analyzer8.NextToken() );
        if ( KErrNotFound == tagPtr8.Find( KColon8 ) )
            {
            // colon required
            colon8.Set( analyzer8.NextToken() );
            }
        analyzer8.Assign(
            messagePtr8.Mid( messagePtr8.Find( KColon8 ) + KOneChar ) );
        analyzer8.SkipSpace(); // skipping ws's since one newline is added later
        if ( analyzer8.Eos() )
            {
            // data contained only ws's we must decrease counter by length of CRLF
            analyzer8.UnGet();
            analyzer8.UnGet();
            }
        dataPtr8.Set(
            messagePtr8.Mid(
                messagePtr8.Find( KColon8 ) + KOneChar + analyzer8.Offset(),
                analyzer8.Remainder().Length() - KCrlf8().Length() ) );
        posPtr8.Set( posPtr8.Mid( messagePtr8.Length() ) );
        analyzer8.Assign( posPtr8 );

        variant = TranslateTagL( tagPtr8, *resourceReader );

        if ( variant &&
            variant->Length() + dataPtr8.Length() + KOneChar <
            partPtr.MaxLength() - partPtr.Length() )
            {
            // Resource translated to users phone variant language.
            // Optional message headers other than these aren't supported
            // because language cannot be verified and might differ from phone
            // variant language.
            partPtr.Append( *variant );
            partPtr.Append(  dataPtr8 );
            partPtr.Append( KEndLine8 );
            messageEmpty = EFalse;
            }
        delete variant;
        variant = NULL;

        if ( !appendMsgChange && !messageEmpty && aMessageBody8.MaxLength() - aMessageBody8.Length() >= partPtr.Length() )
            {
            aMessageBody8.Append( partPtr );
            partPtr.Zero();
            }
        else
            if ( !messageEmpty && aMessageBody8.MaxLength() - aMessageBody8.Length() >= partPtr.Length() + locPtr.Length() )
                {
                // Content OK, append to actual actual message
                aMessageBody8.Append( locPtr );
                aMessageBody8.Append( partPtr );
                partPtr.Zero();
                messageEmpty = EFalse;
                appendMsgChange = EFalse;
                }
            else
                {
                // partial ptr was too long for message, reset and try next part from content
                partPtr.Zero();
                messageEmpty = ETrue;
                }

        }while( !analyzer8.Eos() );

    CleanupStack::PopAndDestroy( partTemp );
    CleanupStack::PopAndDestroy( locTemp );
    CleanupStack::PopAndDestroy( resourceReader );

    IPVMEPRINT( "CIpVmbxEngine::ParseOptionalHeadersL - OUT" );
    }


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CIpVmbxEngine::ReadResourcesL(
    const TDesC8& aTotalMessages8,
    const TDesC8& aNewMessages8,
    TDes8& aMessagesBody8 )
    {
    IPVMEPRINT( "CIpVmbxEngine::ReadResourcesL - IN" );
    __ASSERT_DEBUG( KSmsLength == aMessagesBody8.MaxLength(), Panic( KErrArgument ) );

    TFileName dll;
    Dll::FileName( dll );
    TFileName fileName( TParsePtrC( dll ).Drive() );
    fileName += KIpVmbxAppEngineResourceFileDirectory;
    fileName += KIpVmbxAppEngineResourceFileName;
    CStringResourceReader* resourceReader =
        CStringResourceReader::NewL( fileName );
    CleanupStack::PushL( resourceReader );
    TPtrC messageText;

    if( !aTotalMessages8.Length() )
        {
        messageText.Set(
            resourceReader->ReadResourceString( R_VOIP_VM_MSG_SUM_UNKNOWN ) );
        aMessagesBody8.Copy( messageText );
        }
    else if( !aNewMessages8.Compare( KOneMessage ) &&
        !aTotalMessages8.Compare( KOneMessage ) )
        {
        messageText.Set(
            resourceReader->ReadResourceString( R_VOIP_NEW_VOICE_MESSAGE ) );
        aMessagesBody8.Copy( messageText );
        }
    else if( !aNewMessages8.Compare( KOneMessage ) &&
        aTotalMessages8.Compare( KOneMessage ) )
        {
        TBuf< KSmsLength > tempText;
        messageText.Set(
            resourceReader->ReadResourceString(
                R_VOIP_NEW_AND_OLD_VOICE_MSG ) );
        tempText.Copy( messageText );
        TInt atPosFirst = tempText.Find( KTotal );
        TInt messageLength = tempText.Length();
        TPtrC messagePart1 = tempText.Mid( 0 , atPosFirst );
        aMessagesBody8.Copy( messagePart1 );
        aMessagesBody8.Append( aTotalMessages8.Left( KMaxMessageDigits ) );
        TPtrC messagePart2 = tempText.Mid(
            atPosFirst + KTotalLength,
            messageLength - atPosFirst - KTotalLength );
        aMessagesBody8.Append( messagePart2 );
        }
    else if( aNewMessages8.Compare( KOneMessage ) &&
        aTotalMessages8.Compare( KOneMessage ) )
        {
        messageText.Set(
            resourceReader->ReadResourceString( R_VOIP_NEW_VOICE_MESSAGES ) );

        // Search new messages location and replace with real value
        TBuf8< KSmsLength > messagePart1;
        TInt pos = messageText.Find( KTotalNew );
        messagePart1.Copy( messageText.Left( pos ) );
        messagePart1.Append( aNewMessages8.Left( KMaxMessageDigits ) );
        messagePart1.Append( messageText.Mid( pos + KTotalOldLength ) );

        // Search total messages location and replace with real value
        TBuf8< KSmsLength > messagePart2;
        pos = messagePart1.Find( KTotalOld );
        messagePart2.Copy( messagePart1.Left( pos ) );
        messagePart2.Append( aTotalMessages8.Left( KMaxMessageDigits ) );
        messagePart2.Append( messagePart1.Mid( pos + KTotalOldLength ) );

        aMessagesBody8.Append( messagePart2 );
        }

    aMessagesBody8.Append( KEndLine );
    aMessagesBody8.Append( KEndLine );
    aMessagesBody8.Append(
        resourceReader->ReadResourceString( R_VOIP_VM_MSG_ACCOUNT ) );

    CleanupStack::PopAndDestroy( resourceReader );

    IPVMEPRINT( "CIpVmbxEngine::ReadResourcesL - OUT" );
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CIpVmbxEngine::CreateMessageBodyL(
    const TDesC8& aContent8,
    const TDesC8& aTotal8,
    const TDesC8& aNew8,
    TDes8& aFrom8,
    TDes8& aMessageBody8 )
    {
    aFrom8.TrimRight();
    if ( 0 == aFrom8.Length() )
        {
        User::Leave( KErrArgument );
        }

    ReadResourcesL(
        aTotal8,
        aNew8,
        aMessageBody8 );

    if( aFrom8.Length() &&
        aMessageBody8.Length() + aFrom8.Length() < KSmsLength )
        {
        aMessageBody8.Append( aFrom8 );
        }

    TRAP_IGNORE( ParseOptionalHeadersL( aContent8, aMessageBody8 ) );
    }

// ----------------------------------------------------------------------------
// Creates translated tag from resources
// ----------------------------------------------------------------------------
//
HBufC* CIpVmbxEngine::TranslateTagL(
    const TDesC8& aTagPtr8,
    CStringResourceReader& aResourceReader ) const
    {
    HBufC* locTemp( NULL );
    TLex8 analyzer8( aTagPtr8 );
    TPtrC8 tag( analyzer8.NextToken() );
    if ( KErrNotFound != tag.Find( KColon8 ) )
        {
        // remove possible colon char to make tag matching easier
        tag.Set( tag.Left( tag.Length() - KOneChar ) );
        }
    TIpVmbxParseType::TParseTypes i = TIpVmbxParseType::EDetailTo;
    TIpVmbxParseType parseType;
    for ( ;i <= TIpVmbxParseType::EDetailId; i++ )
        {
        parseType.Set( i );
        if ( 0 == tag.MatchF( parseType.Tag() ) )
            {
            i = TIpVmbxParseType::EDetailId;
            locTemp =
                aResourceReader.ReadResourceString(
                    parseType.ResourceId() ).AllocL();
            }
        };

    return locTemp;
    }

// ----------------------------------------------------------------------------
// Gets and validates part of message.
// ----------------------------------------------------------------------------
//
TPtrC8 CIpVmbxEngine::FetchMessagePartL( const TDesC8& aContent8 ) const
    {
    IPVMEPRINT( "CIpVmbxEngine::FetchMessagePartL - IN" );

    TInt crlf = User::LeaveIfError( aContent8.FindF( KCrlf8 ) ) + KCrlf8().Length();
    // check for optional CRLF
    while( crlf < aContent8.Length() &&
        ( KErrNotFound != KHTab8().Locate( aContent8[crlf] ) ||
        KErrNotFound != KSpace8().Locate( aContent8[crlf] ) ) )
        {
        // This was optional CRLF, try next one
        crlf += User::LeaveIfError( aContent8.Mid( crlf ).FindF( KCrlf8 ) ) + KTwoChars;
        }
    // now we should have correct partial data
    TPtrC8 part8( aContent8.Left( crlf ) );
    TLex8 analyzer8( part8 );

    TPtrC8 tagPtr8(
        part8.Left( User::LeaveIfError( part8.Find( KColon8 ) ) ) );
    if ( !tagPtr8.Length() )
        {
        // At least one character required
        User::Leave( KErrCorrupt );
        }
    TestNamePartL( tagPtr8 );

    TPtrC8 dataPtr8( part8.Mid( tagPtr8.Length() ) );
    TestValuePartL( dataPtr8 );

    IPVMEPRINT( "CIpVmbxEngine::FetchMessagePartL - OUT" );
    return aContent8.Left( part8.Length() );
    }

// ----------------------------------------------------------------------------
// Validates name part of message.
// ----------------------------------------------------------------------------
//
void CIpVmbxEngine::TestNamePartL( const TDesC8& aNameHeader8 ) const
    {
    // Check legal chars %x41-5A / %x61-7A %x30-39 ( 2d, 2e, 21, 25, 2a, 5f, 2b, 60, 27, 7e)
    // from name header
    const TFixedArray< TText8, KSpecialNameCharsCount > legalCharsArray(
        &KSpecialNameChars[0], KSpecialNameCharsCount );
    TChar curChar;
    TLex8 analyzer8( aNameHeader8 );
    curChar = analyzer8.Get();
    TBool valid = ETrue;
    while ( !curChar.Eos() && valid )
        {
        valid = EFalse;
        if ( KNameDigitLowest <= curChar && KNameDigitHighest >= curChar ||
            KNameCharUpLowest <= curChar && KNameCharUpHighest >= curChar ||
            KNameCharLowLowest <= curChar && KNameCharLowHighest >= curChar )
            {
            valid = ETrue;
            }
        for ( TInt i = 0; legalCharsArray.Count() > i && !valid; i++ )
            {
            if ( curChar == legalCharsArray.At( i ) )
                {
                valid = ETrue;
                }
            }
        if ( valid )
            {
            curChar = analyzer8.Get();
            }
        }
    if ( !valid )
        {
        valid = ETrue;
        while ( !curChar.Eos() && valid )
            {
            valid = EFalse;
            if ( KSpace == curChar || KHTab == curChar )
                {
                valid = ETrue;
                }
            curChar = analyzer8.Get();
            }
        }
    if ( !valid )
        {
        User::Leave( KErrCorrupt );
        }
    }

// ----------------------------------------------------------------------------
// Validates value part of message.
// ----------------------------------------------------------------------------
//
void CIpVmbxEngine::TestValuePartL( const TDesC8& aValueHeader8 ) const
    {
    TBool valid = ETrue;
    TChar curChar;
    TLex8 analyzer8( aValueHeader8 );
    curChar = analyzer8.Get();

    while ( !curChar.Eos() && valid  )
        {
        valid = EFalse;
        if ( KValueLowestChar <= curChar && KValueHighestChar >= curChar )
            {
            valid = ETrue;
            }
        else
            if ( KCr == curChar || KLf == curChar || KHTab == curChar )
                {
                valid = ETrue;
                }
        curChar = analyzer8.Get();
        }
    if ( !valid )
        {
        User::Leave( KErrCorrupt );
        }
    }

