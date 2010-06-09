/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Transfer state context class for state machine
*
*/

#include <mcetransactiondatacontainer.h> // TMceTransactionDataContainer 
#include <mceoutrefer.h>                 // CMceRefer, CMceOutRefer
#include <mceoutevent.h>                 // CMceEvent
#include <escapeutils.h>
#include <mcesession.h>
#include <crcseprofileentry.h>
#include <crcseprofileregistry.h>

#include "svpsessionbase.h"
#include "svptransferstatecontext.h"
#include "svptransferstatebase.h"
#include "svptransferidlestate.h"
#include "svptransferpendingstate.h"
#include "svptransferacceptedstate.h"
#include "svptransferterminatingstate.h"
#include "svptransferterminatedstate.h" 
#include "svptransferobserver.h"
#include "svplogger.h"          
#include "svpuriparser.h"
#include "svpconsts.h"
#include "svpholdcontext.h"
#include "svpholdcontroller.h"
#include "svpcleanupresetanddestroy.h"


// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CSVPTransferStateContext::CSVPTransferStateContext( 
                                CMceSession* aMceSession,                                
                                CSVPSessionBase* aSVPSession,                                
                                TMceTransactionDataContainer& aContainer, 
                                MSVPTransferObserver& aObserver ) :
        iMceSession( aMceSession ),
        iSVPSession( aSVPSession ),        
        iTargetSession( NULL ),
        iContainer( aContainer ),
        iTransferObserver( aObserver ),
        iStates( NULL ),
        iCurrentState( NULL ),
        iMceRefer( NULL ),        
        iMceEvent( NULL ),
        iAttended( EFalse ),
        iIncomingReferTo ( NULL ),
        iIncomingReferredBy ( NULL ),
        iIncomingReplaces (NULL)
    {
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::ConstructL
// ---------------------------------------------------------------------------
//
void CSVPTransferStateContext::ConstructL() 
    {
    SVPDEBUG1( "CSVPTransferStateContext::ConstructL In" )
    
    // Create the transfer states in the state array.
    InitializeStateArrayL();  
           
    // Initialize idle state to current state. Note, that it is not
    // applied yet.
    SetCurrentStateL( KSVPTransferIdleStateIndex );
    
    SVPDEBUG1( "CSVPTransferStateContext::ConstructL Out" )
    }

// -----------------------------------------------------------------------------
// CSVPTransferStateContext::NewL
// -----------------------------------------------------------------------------
CSVPTransferStateContext* CSVPTransferStateContext::NewL( 
                                CMceSession* aMceSession,
                                CSVPSessionBase* aSVPSession,
                                TMceTransactionDataContainer& aContainer, 
                                MSVPTransferObserver& aObserver )
    {    
    CSVPTransferStateContext* self = new ( ELeave ) CSVPTransferStateContext(
                                                        aMceSession,
                                                        aSVPSession,
                                                        aContainer, 
                                                        aObserver );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// ---------------------------------------------------------------------------
// CSVPTransferStateContext::~CSVPTransferStateContext
// ---------------------------------------------------------------------------
//
CSVPTransferStateContext::~CSVPTransferStateContext()
    {
    SVPDEBUG1( "CSVPTransferStateContext::~CSVPTransferStateContext In" )
    
    if ( iStates )
        {
        iStates->ResetAndDestroy();
        iStates->Close();
        delete iStates;
        }
    
    delete iMceEvent;
    delete iMceRefer;
    delete iIncomingReferTo;
    delete iIncomingReferredBy;
    delete iIncomingReplaces;
    
    SVPDEBUG1( "CSVPTransferStateContext::~CSVPTransferStateContext Out" )        
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::SetCurrentStateL
// ---------------------------------------------------------------------------
//
void CSVPTransferStateContext::SetCurrentStateL(
        TSVPTransferStateIndex aStateIndex )
    {
    // Check, if the transition is valid
    if ( !IsStateTransitionAccepted( aStateIndex ) )
        {
        SVPDEBUG2( "CSVPTransferStateContext::SetCurrentStateL: STATE ERROR stateindex: %i", aStateIndex )
        User::Leave( KSVPErrTransferStateError );
        }
    else
        {
        iCurrentState = ( *iStates )[ aStateIndex ];
        iCurrentState->Enter( *this );
        }
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::CurrentState
// ---------------------------------------------------------------------------
//
TSVPTransferStateIndex CSVPTransferStateContext::CurrentState() const
    {
    return iStates->Find( iCurrentState );    
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::ApplyCurrentStateL
// ---------------------------------------------------------------------------
//
void CSVPTransferStateContext::ApplyCurrentStateL()
   {
   iCurrentState->ApplyL( *this );
   }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::TransferObserver
// ---------------------------------------------------------------------------
//
MSVPTransferObserver& CSVPTransferStateContext::TransferObserver()
    {
    return iTransferObserver;    
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::SetMceSessionObject
// ---------------------------------------------------------------------------
//
void CSVPTransferStateContext::SetMceSessionObject( CMceSession* aSession )
    {
    iMceSession = aSession;
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::MceSessionObject
// ---------------------------------------------------------------------------
//
CMceSession* CSVPTransferStateContext::MceSessionObject()
    {
    return iMceSession;
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::SetMceRefer
// ---------------------------------------------------------------------------
//
void CSVPTransferStateContext::SetMceRefer( CMceRefer* aRefer )
    {
    if ( iMceRefer != aRefer )
        {
        delete iMceRefer;
        iMceRefer = aRefer;
        }
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::MceRefer
// ---------------------------------------------------------------------------
//
CMceRefer* CSVPTransferStateContext::MceRefer()
    {
    return iMceRefer;
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::SetAttended
// ---------------------------------------------------------------------------
//
void CSVPTransferStateContext::SetAttended( const TBool aAttended )
    {
    iAttended = aAttended;
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::IsAttended
// ---------------------------------------------------------------------------
//
TBool CSVPTransferStateContext::IsAttended()
    {
    return iAttended;
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::CheckIsSessionRemoteHold
// ---------------------------------------------------------------------------
//
TBool CSVPTransferStateContext::CheckIsSessionRemoteHold()
    {
    SVPDEBUG1( "CSVPTransferStateContext::CheckIsSessionRemoteHold In" )
    
    TBool ret = EFalse;
    
    if ( iSVPSession->HasHoldController() &&
         MCCPCallObserver::ECCPStateDisconnecting != iSVPSession->State() )
        {
        SVPDEBUG1("CSVPTransferStateContext::CheckIsSessionRemoteHold, Check hold state" )
        
        if ( ESVPOnHold == iSVPSession->HoldController().HoldState() &&
             ESVPRemoteHold == iSVPSession->HoldController().HoldRequest() )
            {
            // Session is remote holded -> Snom/EyeBeam as unattended transferer case.
            SVPDEBUG1( "CSVPTransferStateContext:CheckIsSessionRemoteHold, transferer is Snom, EyeBeam or similar" )
            ret = ETrue;
            }
        }
    
    SVPDEBUG2("CSVPTransferStateContext::CheckIsSessionRemoteHold Out return: %d",ret )
    return ret;
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::SetTransferDataL
// ---------------------------------------------------------------------------
//
void CSVPTransferStateContext::SetTransferDataL(
        CDesC8Array* aUserAgentHeaders, TInt aSecureStatus )
    {
    SVPDEBUG1( "CSVPTransferStateContext::SetTransferDataL In" )
    
    if ( aUserAgentHeaders )
        {
        if ( iIncomingReplaces && IsAttended() )
            {
            SVPDEBUG1( "CSVPTransferStateContext::SetTransferDataL: Add replaces header" )
            // fetch "replaces:" string
            HBufC* replacesStringHeap16 = IncomingReplaces().AllocLC(); // CS: 1
            
            // Copy incoming replaces to 8-bit buffer
            HBufC8* replacesStringHeap8 =
                    HBufC8::NewLC( replacesStringHeap16->Length() +
                                   KSVPReplacesColonTxt().Length() ); // CS: 2
            
            replacesStringHeap8->Des().Copy( *replacesStringHeap16 );
            CleanupStack::Pop( 1 );
            CleanupStack::PushL( replacesStringHeap8 ); // ReAlloc possible
            
            // add replaces header
            replacesStringHeap8->Des().Insert( 0, KSVPReplacesColonTxt );
            CleanupStack::Pop( 1 );
            CleanupStack::PushL( replacesStringHeap8 ); // ReAlloc possible
            SVPDEBUG2( "CSVPTransferStateContext::SetTransferDataL - length: %d", replacesStringHeap8->Length() )
            
            // Finally add collected Replaces string to header
            aUserAgentHeaders->AppendL( *replacesStringHeap8 );
            CleanupStack::PopAndDestroy( replacesStringHeap8 );      // CS: 1
            CleanupStack::PopAndDestroy( replacesStringHeap16 );     // CS: 0
            }
        
        if ( iIncomingReferredBy )
            {
            SVPDEBUG1( "CSVPTransferStateContext::SetTransferDataL: Add referredBy header" )
            TBuf8<KSVPTempStringlength> referredByString;
            referredByString.Append( KSVPReferredBy );
            // add IncomingReferredBy
            referredByString.Append( IncomingReferredBy() );
            // Finally add collected Referred-By string to header
            aUserAgentHeaders->AppendL( referredByString );
            }
        }
    
    // Update transfer target address according preferred securesetting.
    UpdateTransferTargetL( aSecureStatus );
    
    SVPDEBUG1( "CSVPTransferStateContext::SetTransferDataL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::IsIncoming
// ---------------------------------------------------------------------------
//
TBool CSVPTransferStateContext::IsIncoming()
    {
    return ( NULL != iIncomingReferTo );
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::SetIncomingReferToL
// ---------------------------------------------------------------------------
//
void CSVPTransferStateContext::SetIncomingReferToL( const TDesC8& aReferTo )
    {
    SVPDEBUG1("CSVPTransferStateContext::SetIncomingReferToL In" )
    
    ResetIncomingReferTo();
    
    // modify aReferTo
    HBufC8* referTo = CompleteReferToL( aReferTo );
    CleanupStack::PushL( referTo );

    // at this point referto must contains legal sip uri
    RemoveExtraParameters( referTo );
    
    // Check length - moSession construct limitation
    // sip: or sips: prefix will be added later, so 95 is maximum
    if ( referTo->Length() < ( KSVPMaxUriLength - KSVPSipPrefixLength ) )
        {
        // Copy and convert the "refer to" -parameter.
        iIncomingReferTo = HBufC::NewL( referTo->Length() );
        TPtr temp = iIncomingReferTo->Des();
        temp.Copy( *referTo );
        }
    else
        {
        SVPDEBUG1( "CSVPTransferStateContext::SetIncomingReferToL: referTo too long, Leave" )
        User::Leave( KErrArgument );
        }
    
    CleanupStack::PopAndDestroy( referTo );
    
   SVPDEBUG2( "CSVPTransferStateContext::SetIncomingReferToL lenght: %d",
            iIncomingReferTo->Length())
    SVPDEBUG1( "CSVPTransferStateContext::SetIncomingReferToL Out" )
    }

// -----------------------------------------------------------------------------
// CSVPSessionBase::SetIncomingReferredByL
// -----------------------------------------------------------------------------
//
void CSVPTransferStateContext::SetIncomingReferredByL( const TDesC8& aReferredBy )
    {
    SVPDEBUG1( "CSVPTransferStateContext::SetIncomingReferredByL In" )
    
    ResetIncomingReferredBy();
    
    // modify aReferredBy
    HBufC8* referredBy = CompleteReferredByL( aReferredBy );
    CleanupStack::PushL( referredBy );
    
    // Copy and convert the "Referred By" -parameter.    
    iIncomingReferredBy = HBufC::NewL( referredBy->Length() );
    TPtr temp = iIncomingReferredBy->Des();
    temp.Copy( *referredBy );
    CleanupStack::PopAndDestroy( referredBy );
    
    SVPDEBUG2( "CSVPTransferStateContext::SetIncomingReferredByL lenght: %d",
            iIncomingReferredBy->Length() )
    SVPDEBUG1( "CSVPTransferStateContext::SetIncomingReferredByL Out" )
    }    

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::SetIncomingReplacesL
// ---------------------------------------------------------------------------
//
void CSVPTransferStateContext::SetIncomingReplacesL( const TDesC8& aString )
    { 
    SVPDEBUG1("CSVPTransferStateContext::SetIncomingReplacesL In" )
    
    ResetIncomingReplaces();
    
    // modify Replaces
    HBufC8* string = CompleteReplacesL( aString );
    CleanupStack::PushL( string );
    
    // Copy and convert the "Replaces" -parameter.    
    iIncomingReplaces = HBufC::NewL( string->Length() );
    TPtr temp = iIncomingReplaces->Des();
    temp.Copy( *string );
    CleanupStack::PopAndDestroy( string );
    
    SVPDEBUG2( "CSVPTransferStateContext::SetIncomingReplacesL lenght: %d",
            iIncomingReplaces->Length() )
    SVPDEBUG1( "CSVPTransferStateContext::SetIncomingReplacesL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::CompleteReplacesL
// ---------------------------------------------------------------------------
//
HBufC8* CSVPTransferStateContext::CompleteReplacesL( const TDesC8& aString )
    {
    SVPDEBUG1( "CSVPTransferStateContext::CompleteReplacesL In" )
    
    // Copy the parameter to a new buffer.
    HBufC8* string = aString.AllocLC();
    // If "?Replaces=" found attended transfer case
    TInt position = CheckReplacesTxt( *string );
    SVPDEBUG2( "    CheckReplacesTxt returns = %d", position );
    
    if ( KErrNotFound != position )
        {
        TakeReplacesTxt( string, position );
        CleanupStack::Pop( 1 ); // string, ReAlloc possible
        CleanupStack::PushL( string );
        }
    
    // Check ">" and remove text after it if exists
    position = CheckRightBracket( *string );
    SVPDEBUG2("    CheckRightBracket returns = %d" , position )
    
    if ( KErrNotFound != position )
        {
        // ">" found
        CutStringFromPosition( string, position );
        CleanupStack::Pop( 1 ); // string, ReAlloc possible
        CleanupStack::PushL( string );
        }
    
    HBufC8* temp = EscapeUtils::EscapeDecodeL( *string );
    CleanupStack::PopAndDestroy( string );
    
    SVPDEBUG1( "CSVPTransferStateContext::CompleteReplacesL Out" )
    return temp;
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::CompleteReferToL
// ---------------------------------------------------------------------------
//
HBufC8* CSVPTransferStateContext::CompleteReferToL( const TDesC8& aUri )
    {
    SVPDEBUG1( "CSVPTransferStateContext::CompleteReferToL In" )
    
    // Copy the parameter to a new buffer.
    HBufC8* uri = aUri.AllocLC();
    
    // Check "<" and remove it and text before it if exists
    TInt position = CheckLeftBracket( *uri );
    
    if ( KErrNotFound != position )
        {
        RemoveLeftBracket( uri, position );
        CleanupStack::Pop( 1 ); // uri, ReAlloc possible
        CleanupStack::PushL( uri );
        }
    
    // Check ";user=phone" and remove if exists
    position = CheckUserEqualsPhone( *uri );
    
    if ( KErrNotFound != position )
        {
        // ";user=phone" found
        RemoveUserEqualsPhone( uri, position );
        CleanupStack::Pop( 1 ); // uri, ReAlloc possible
        CleanupStack::PushL( uri );
        }
    
    // Check ">" and remove text after it if exists
    position = CheckRightBracket( *uri );
    
    if ( KErrNotFound != position )
        {
        // ">" found
        CutStringFromPosition( uri, position );
        CleanupStack::Pop( 1 ); // uri, ReAlloc possible
        CleanupStack::PushL( uri );
        }
    
    // Check "KSVPQuesReplacesTxt" and remove text after it if exists
    // If it is found -> attended transfer case
    position = CheckReplacesTxt( *uri );
    
    if ( KErrNotFound != position )
        {
        // Found -> attended case.
        SVPDEBUG1( "CSVPTransferStateContext::CompleteReferToL ?Replaces= found -> attended transfer" )
        SetAttended( ETrue );
        SetIncomingReplacesL( aUri );
        CutStringFromPosition( uri, position );
        }
    else
        {
        // Not found -> unattended case.
        SVPDEBUG1( "CSVPTransferStateContext::CompleteReferToL ?Replaces= not found -> unattended transfer" )
        SetAttended( EFalse );
        }

    // Check "?X-Sipx-Authidentity=" and remove text after it if exists
    position = CheckAuthidentity( *uri );

    if ( KErrNotFound != position )
        {
        // "?X-Sipx-Authidentity=" found
        CutStringFromPosition( uri, position );
        CleanupStack::Pop( 1 ); // uri, ReAlloc possible
        CleanupStack::PushL( uri );
        }

    // Check "sip:" and remove it if exists
    if ( KErrNone == uri->Find( KSVPSipPrefix ) )
        {
        // sip: is in the beginning of the string
        SVPDEBUG1( "CSVPTransferStateContext::CompleteReferToL remove sip:" )
        uri->Des().Delete( 0, KSVPSipPrefixLength );
        }

    // Check "sips:" and remove it if exists
    if ( KErrNone == uri->Find( KSVPSipsPrefix ) )
        {
        // sips: is in the beginning of the string
        SVPDEBUG1( "CSVPTransferStateContext::CompleteReferToL remove sips:" )
        uri->Des().Delete( 0, KSVPSipsPrefixLength );
        }

    // Check ":" and remove text after it if exists,
    // some server might add this after transfer target address
    position = uri->Find( KSVPCln );

    if ( KErrNotFound != position )
        {
        // ":" found
        SVPDEBUG2( "CSVPTransferStateContext::CompleteReferToL remove text after %d ", position )
        uri->Des().Delete( position, uri->Length() - position );
        }

    CleanupStack::Pop( 1 ); // uri, ReAlloc possible
    
    SVPDEBUG1( "CSVPTransferStateContext::CompleteReferToL Out" )
    return uri;
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::CompleteReferredByL
// ---------------------------------------------------------------------------
//
HBufC8* CSVPTransferStateContext::CompleteReferredByL(
        const TDesC8& aReferredBy )
    {
    SVPDEBUG1( "CSVPTransferStateContext::CompleteReferredByL In" )
    
    // Copy the parameter to a new buffer.
    HBufC8* referredBy = aReferredBy.AllocLC();
    
    // Check "Referred-By:" and remove it
    TInt position = CheckReferredByTxt( *referredBy );
    
    if ( KErrNotFound != position )
        {
        // "Referred-By:" found.
        RemoveReferredByTxt( referredBy, position );
        }
    
    CleanupStack::Pop( 1 ); // referredBy, ReAlloc possible
    
    SVPDEBUG1( "CSVPTransferStateContext::CompleteReferredByL Out" )
    return referredBy;
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::CheckUserEqualsPhone
// ---------------------------------------------------------------------------
//
TInt CSVPTransferStateContext::CheckUserEqualsPhone( const TDesC8& aUri ) const
    {
    SVPDEBUG1( "CSVPTransferStateContext::CheckUserEqualsPhone" )
    
    return ( aUri.FindF( KSVPUserEqualsPhone ) );
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::RemoveUserEqualsPhone
// ---------------------------------------------------------------------------
//
void CSVPTransferStateContext::RemoveUserEqualsPhone(
        HBufC8*& aUri, TInt aPosition ) const
    {
    SVPDEBUG2( "CSVPTransferStateContext::RemoveUserEqualsPhone posit = %d", aPosition )
    
    aUri->Des().Delete( aPosition, KSVPUserEqualsPhoneLenght );
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::RemoveExtraParameters
// ---------------------------------------------------------------------------
//
void CSVPTransferStateContext::RemoveExtraParameters( HBufC8*& aUri ) const 
    {
    SVPDEBUG1( "CSVPTransferStateContext::RemoveExtraParameters In" )
    
    // remove all extra parameters from given address
    TInt index = aUri->Locate( KSVPSemiColonMark );
    TInt bracketLocation = KErrNotFound;
    
    SVPDEBUG3( "CSVPTransferStateContext::RemoveExtraParameters index = %d, length = %d",
        index, aUri->Length() )
    
    if ( KErrNotFound != index )
        {
        // Delete ";" and text after it
        aUri->Des().Delete( index, aUri->Length() );
        }
    
    // Check "<" and remove it and text before it if exists
    bracketLocation = CheckLeftBracket( *aUri );
    
    if ( KErrNotFound != bracketLocation )
        {
        RemoveLeftBracket( aUri, bracketLocation );
        }
    
    // Check ">" and remove text after it if exists
    bracketLocation = CheckRightBracket( *aUri );
    
    if ( KErrNotFound != bracketLocation )
        {
        CutStringFromPosition( aUri, bracketLocation );
        }
    
    SVPDEBUG1( "CSVPTransferStateContext::RemoveExtraParameters Out" )
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::CheckReplacesTxt
// ---------------------------------------------------------------------------
//
TInt CSVPTransferStateContext::CheckReplacesTxt( const TDesC8& aUri ) const
    {
    SVPDEBUG1( "CSVPTransferStateContext::CheckReplacesTxt" )
    
    return ( aUri.FindF( KSVPQuesReplacesTxt ) );
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::CutStringFromPosition
// ---------------------------------------------------------------------------
//
void CSVPTransferStateContext::CutStringFromPosition(
        HBufC8*& aUri, TInt aPosition ) const
    {
    SVPDEBUG2( "CSVPTransferStateContext::CutStringFromPosition posit = %d", aPosition )
    aUri->Des().Delete( aPosition, aUri->Length() - aPosition );
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::CheckReferredByTxt
// ---------------------------------------------------------------------------
//
TInt CSVPTransferStateContext::CheckReferredByTxt(
        const TDesC8& aReferredBy ) const
    {
    SVPDEBUG1( "CSVPTransferStateContext::CheckReferredByTxt" )
    
    return ( aReferredBy.FindF( KSVPReferredBy ) );
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::RemoveReferredByTxt
// ---------------------------------------------------------------------------
//
void CSVPTransferStateContext::RemoveReferredByTxt(
        HBufC8*& aReferredBy, TInt aPosition ) const
    {
    SVPDEBUG2( "CSVPTransferStateContext::RemoveReferredByTxt posit = %d", aPosition )
    
    aReferredBy->Des().Delete( aPosition, KSVPReferredByLength + 1 );
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::TakeReplacesTxt
// ---------------------------------------------------------------------------
//
void CSVPTransferStateContext::TakeReplacesTxt(
        HBufC8*& aString, TInt aPosition ) const
    {
    SVPDEBUG2( "CSVPTransferStateContext::TakeReplacesTxt posit = %d", aPosition )
    
    aString->Des().Delete( 0, aPosition + KSVPQuesReplacesTxtLength );
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::CheckLeftBracket
// ---------------------------------------------------------------------------
//
TInt CSVPTransferStateContext::CheckLeftBracket( const TDesC8& aUri ) const
    {
    SVPDEBUG1( "CSVPTransferStateContext::CheckLeftBracket" )
    
    // Return position of "<" or KErrNotFound
    return ( aUri.Find( KSVPLeftBracketMark ) );
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::RemoveLeftBracket
// ---------------------------------------------------------------------------
//
void CSVPTransferStateContext::RemoveLeftBracket(
        HBufC8*& aUri, TInt aPosition ) const
    {
    SVPDEBUG2( "CSVPTransferStateContext::RemoveLeftBracket posit = %d", aPosition )
    
    aUri->Des().Delete( 0, aPosition + KSVPSingleBracketLength );
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::CheckRightBracket
// ---------------------------------------------------------------------------
//
TInt CSVPTransferStateContext::CheckRightBracket( const TDesC8& aUri ) const
    {
    SVPDEBUG1( "CSVPTransferStateContext::CheckRightBracket" )
    
    return ( aUri.Find( KSVPRightBracketMark ) );
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::CheckAuthidentity  
// ---------------------------------------------------------------------------
//
TInt CSVPTransferStateContext::CheckAuthidentity( const TDesC8& aUri ) const
    {
    SVPDEBUG1( "CSVPTransferStateContext::CheckAuthidentity" )
    TInt returnValue = aUri.Find( KSVPAuthidentity );
    SVPDEBUG2( "CSVPTransferStateContext::CheckAuthidentity return: %d", returnValue )
    if ( returnValue == KErrNotFound )
        {
        returnValue = aUri.Find( KSVPAuthidentity2 );
        SVPDEBUG2( "CSVPTransferStateContext::CheckAuthidentity second return: %d", returnValue )
        }
    return returnValue;
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::IncomingReferTo
// ---------------------------------------------------------------------------
//
const TDesC& CSVPTransferStateContext::IncomingReferTo()
    {
    SVPDEBUG1( "CSVPTransferStateContext::IncomingReferTo" )
    
    return *iIncomingReferTo;
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::UpdateTransferTargetL
// ---------------------------------------------------------------------------
//
void CSVPTransferStateContext::UpdateTransferTargetL( TInt aSecureStatus )
    {
    SVPDEBUG1( "CSVPTransferStateContext::UpdateTransferTargetL In" )
    
    if ( iIncomingReferTo )
        {
        if ( KSVPStatusNonSecure == aSecureStatus )
            {
            SVPDEBUG1( "CSVPTransferStateContext::UpdateTransferTargetL add SIP uri" )
            
            // Check "sips:" and remove it if exists
            if ( KErrNone == iIncomingReferTo->Des().FindF( KSVPSipsPrefix2 ) )
                {
                // sips: is in the beginning of the string, position 0
                SVPDEBUG1( "CSVPTransferStateContext::UpdateTransferTargetL remove sips:" )
                iIncomingReferTo->Des().Delete( 0, KSVPSipsPrefixLength );
                }
            
            // Add "sip:" prefix in the beginning of the string, if it's missing.
            if ( KErrNotFound == iIncomingReferTo->Des().FindF( KSVPSipPrefix2 ) )
                {
                SVPDEBUG1( "CSVPTransferStateContext::UpdateTransferTargetL add sip:" )
                iIncomingReferTo = iIncomingReferTo->ReAllocL(
                        iIncomingReferTo->Length() + KSVPSipPrefixLength );
                iIncomingReferTo->Des().Insert( 0, KSVPSipPrefix2 );
                }
            }
        else
            {
            SVPDEBUG1( "CSVPTransferStateContext::UpdateTransferTargetL add SIPS" )
            
            // Check "sip:" and remove it if exists
            if ( KErrNone == iIncomingReferTo->Des().FindF( KSVPSipPrefix2 ) )
                {
                // sip: is in the beginning of the string
                SVPDEBUG1( "CSVPTransferStateContext::UpdateTransferTargetL remove sip:" )
                iIncomingReferTo->Des().Delete( 0, KSVPSipPrefixLength );
                }
            
            // Add "sips:" prefix in the beginning of the string, if it's missing.
            if ( KErrNotFound == iIncomingReferTo->Des().FindF( KSVPSipsPrefix2 ) )
                {
                SVPDEBUG1( "CSVPTransferStateContext::UpdateTransferTargetL add sips:" )
                iIncomingReferTo = iIncomingReferTo->ReAllocL(
                        iIncomingReferTo->Length() + KSVPSipsPrefixLength );
                iIncomingReferTo->Des().Insert( 0, KSVPSipsPrefix2 );
                }
            }
        }
    else
        {
        SVPDEBUG1( "CSVPTransferStateContext::UpdateTransferTargetL IncomingReferTo Not OK" )
        }
    
    SVPDEBUG1( "CSVPTransferStateContext::UpdateTransferTargetL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::ResetIncomingReferTo
// ---------------------------------------------------------------------------
//
void CSVPTransferStateContext::ResetIncomingReferTo()
    {
    SVPDEBUG1( "CSVPTransferStateContext::ResetIncomingReferTo" )    
    
    if ( iIncomingReferTo )
        {
        delete iIncomingReferTo;
        iIncomingReferTo = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::ResetIncomingReferredBy
// ---------------------------------------------------------------------------
//
void CSVPTransferStateContext::ResetIncomingReferredBy()
    {
    SVPDEBUG1( "CSVPTransferStateContext::ResetIncomingReferredBy" )
    
    if ( iIncomingReferredBy )
        {
        delete iIncomingReferredBy;
        iIncomingReferredBy = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::IncomingReferredBy
// ---------------------------------------------------------------------------
//
const TDesC& CSVPTransferStateContext::IncomingReferredBy()
    {
    SVPDEBUG1( "CSVPTransferStateContext::IncomingReferredBy" )
    
    return *iIncomingReferredBy;
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::IncomingReplaces
// ---------------------------------------------------------------------------
//
const TDesC& CSVPTransferStateContext::IncomingReplaces()
    {
    SVPDEBUG1( "CSVPTransferStateContext::IncomingReplaces" )
    
    return *iIncomingReplaces;
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::ResetIncomingReplaces
// ---------------------------------------------------------------------------
//
void CSVPTransferStateContext::ResetIncomingReplaces()
    {
    SVPDEBUG1( "CSVPTransferStateContext::ResetIncomingReplaces" )
    
    if ( iIncomingReplaces )
        {
        delete iIncomingReplaces;
        iIncomingReplaces = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::SetTransferParmsL
// ---------------------------------------------------------------------------
//
void CSVPTransferStateContext::SetTransferParmsL(
        CSVPSessionBase* aTargetSession, const TDesC& aTarget,
        const TBool aAttendedTransfer )
    {
    SVPDEBUG1( "CSVPTransferStateContext::SetTransferParmsL In" )
    
    // No target session as default. 
    iTargetSession = NULL;
    
    // Delete possible old refer 
    delete iMceRefer;
    iMceRefer = NULL;
    
    // Set attended/unattended transfer
    SetAttended( aAttendedTransfer );
    
    // Create mce out refer using given target parameter or call
    if ( aTarget.Length() > 0 )
        {
        CreateMceOutReferL( aTarget );
        }
    else
        {
        CreateMceOutReferL( aTargetSession );
        }
    
    SVPDEBUG1( "CSVPTransferStateContext::SetTransferParmsL Out" )
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::CreateMceOutReferL
// ---------------------------------------------------------------------------
//
void CSVPTransferStateContext::CreateMceOutReferL( const TDesC& aTarget )
    {
    // Create mce out refer using target string (unattended).   
    SVPDEBUG1( "CSVPTransferStateContext::CreateMceOutReferL(aTarget) In" )
    
    // Copy 'refer to' argument
    HBufC8* target = HBufC8::NewLC( aTarget.Length() );
    target->Des().Copy( aTarget );
    
    // Parse target with uri parser, use session recipient as domain source 
    CSVPUriParser* uriParser = CSVPUriParser::NewLC();
    HBufC8* referto = NULL;
    RPointerArray< CRCSEProfileEntry > entryArray;
    CleanupResetAndDestroy< RPointerArray<CRCSEProfileEntry> >::PushL( entryArray );
    CRCSEProfileRegistry* reg = CRCSEProfileRegistry::NewLC();
    
    // Get VoIP profile by service id
    reg->FindByServiceIdL( iSVPSession->Parameters().ServiceId(), entryArray );
    __ASSERT_ALWAYS( entryArray.Count(), User::Leave( KErrArgument ) );
    CRCSEProfileEntry* entry = entryArray[0];
    uriParser->SetUserEqualsPhoneRequiredL(
            CRCSEProfileEntry::EOn == entry->iUserPhoneUriParameter );
    
    CleanupStack::PopAndDestroy( reg );
    CleanupStack::PopAndDestroy( &entryArray );

    HBufC8* recipient = NULL;
    const CMceSession& session = iSVPSession->Session();
    __ASSERT_ALWAYS( &session, User::Leave( KErrArgument ) );
    
    TBool isCLIROn = iSVPSession->IsCLIROnL();
    
    if ( ( iSVPSession->IsMobileOriginated() && !isCLIROn ) 
            || ( !iSVPSession->IsMobileOriginated() && isCLIROn ) )
        {
        SVPDEBUG1( "CSVPTransferStateContext::CreateMceOutReferL(aTarget), orig" )
        const TDesC8& orig = session.Originator(); 
        __ASSERT_ALWAYS( &orig, User::Leave( KErrArgument ) );
        recipient = HBufC8::NewLC( orig.Length() );
        recipient->Des().Copy( orig );
        }
    else
        {
        SVPDEBUG1( "CSVPTransferStateContext::CreateMceOutReferL(aTarget), recip" )
        const TDesC8& recip = session.Recipient(); 
        __ASSERT_ALWAYS( &recip, User::Leave( KErrArgument ) );
        recipient = HBufC8::NewLC( recip.Length() );
        recipient->Des().Copy( recip );    
        }

    // remove all extra parameters from recipient address
    RemoveExtraParameters( recipient );
    
    SVPDEBUG2( "CSVPTransferStateContext::CreateMceOutReferL(aTarget) recipient->Length() = %d",
               recipient->Length() )
    
    if ( iSVPSession->SecureMandatory() || iSVPSession->SecurePreferred() )
        {
        SVPDEBUG1( "CSVPTransferStateContext::CreateMceOutReferL(aTarget) SIPS URI..." )
        referto = uriParser->CompleteSecureSipUriL( *target, *recipient );
        }
    else
        {
        SVPDEBUG1( "CSVPTransferStateContext::CreateMceOutReferL(aTarget) SIP URI..." )
        referto = uriParser->CompleteSipUriL( *target, *recipient );
        }
    
    CleanupStack::PushL( referto );
    
    SVPDEBUG2( "CSVPTransferStateContext::CreateMceOutReferL(aTarget) 1 referto->Length() = %d",
               referto->Length() )
    
    // remove all extra parameters from referto
    RemoveExtraParameters( referto );

    SVPDEBUG2( "CSVPTransferStateContext::CreateMceOutReferL(aTarget) 2 referto->Length() = %d",
               referto->Length() )
    
    // Create the refer
    delete iMceRefer;
    iMceRefer = NULL;
    iMceRefer = CMceOutRefer::NewL( *iMceSession, *referto, CMceRefer::ENoSuppression );
    CleanupStack::PopAndDestroy( referto );
    
    CleanupStack::PopAndDestroy( recipient );
    CleanupStack::PopAndDestroy( uriParser );
    CleanupStack::PopAndDestroy( target );
    
    SVPDEBUG1( "CSVPTransferStateContext::CreateMceOutReferL(aTarget) Out" )
	}

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::CreateMceOutReferL
// ---------------------------------------------------------------------------
//
void CSVPTransferStateContext::CreateMceOutReferL( CSVPSessionBase* aTargetSession )
    {
    // Create mce out refer using session (attended).   
    SVPDEBUG1( "CSVPTransferStateContext::CreateMceOutReferL(aTargetSession) In" )    
    
    // Store target session 
    iTargetSession = aTargetSession;
    
    // Construct the refer-to string
    HBufC8* referto = HBufC8::NewLC( KSVPTempStringlength );
    TPtr8 refptr = referto->Des();
    
    if ( iTargetSession )
        {
        // Set refer-to for the attended transfer using the target session,
        // that can not be the same as the owner session of the 
        // transfer controller.
        if ( iSVPSession != iTargetSession )
            {
            SVPDEBUG2( "CSVPTransferStateContext::CreateMceOutReferL(aTargetSession) TargetSession = 0x%x", &iTargetSession )
            
            HBufC8* recipient = NULL;
            const CMceSession& session = iTargetSession->Session();
            __ASSERT_ALWAYS( &session, User::Leave( KErrArgument ) );
            
            if ( iTargetSession->IsMobileOriginated() ) 
                {
                const TDesC8& recip = session.Recipient();
                __ASSERT_ALWAYS( &recip, User::Leave( KErrArgument ) );
                recipient = HBufC8::NewLC( recip.Length() );
                recipient->Des().Copy( recip );
                }
            else
                {
                const TDesC8& orig = session.Originator();
                __ASSERT_ALWAYS( &orig, User::Leave( KErrArgument ) );
                recipient = HBufC8::NewLC( orig.Length() );
                recipient->Des().Copy( orig );    
                }
            
            // remove all extra parameters from recipient address
            RemoveExtraParameters( recipient );
            
            SVPDEBUG2( "CSVPTransferStateContext::CreateMceOutReferL(aTargetSession) recipient->Length() = %d",
               recipient->Length() )
            
            // Set the refer-to address (recipient)
            refptr.Append( recipient->Des() );
            CleanupStack::PopAndDestroy( recipient );
            
            // Gather up the rest of the refer-to data (replaces etc.)  
            // from the target session (sessionbase) member variables.
            TBuf8<KSVPTempStringlength> tmp( KNullDesC8 );
            tmp.Append( KSVPQuesReplacesTxt ); // Set CallId
            
            if ( iTargetSession->CallId() )
                {
                // Set "replaces" callid string starting from next char 
                // the of KSVPCallId_replaces -text
                TDesC8* callid = iTargetSession->CallId();
                TInt index = callid->Find( KSVPCallId_replaces );
                
                if ( KErrNotFound != index )
                    {
                    // Check and encode the possible @ char of the callid 
                    // to "%40" format. This kind of callid e.g. in Cisco 7960.
                    TInt index2 = callid->Find( KSVPAt );
                    
                    if ( KErrNotFound != index2 )
                	    {
                	    HBufC8* tmpCallId = HBufC8::NewLC( callid->Length() );
                	    tmpCallId->Des().Append( callid->Mid( index +
                	            KSVPCallId_replaces().Length() ) );
                	    CSVPUriParser::EscapeEncodeSipUriL( tmpCallId,
                                EscapeUtils::EEscapeUrlEncoded );
                        tmp.Append( *tmpCallId );
                        CleanupStack::Pop( 1 ); // tmpCallId, ReAlloc possible
                        delete tmpCallId;
                        }
                    else
                        {
                        // Normal copy from the correct position
                        tmp.Append( callid->Mid( index +
                                KSVPCallId_replaces().Length() ) );
                        }
                    }
                }
            else
                {
                SVPDEBUG2( "CSVPTransferStateContext::CreateMceOutReferL(aTargetSession) error - missing CallId %i", KSVPErrTransferReferTo )
                User::Leave( KSVPErrTransferReferTo );
                }
            
            // To-header                    
            if ( iTargetSession->ToHeader() )
                {
                tmp.Append( KSVPTo_tag );
                
                // Set "to" tag string starting from next char 
                // of the KSVPTo_tag_replaces -text
                TDesC8* toHdr = iTargetSession->ToHeader();
                TInt index = toHdr->Find( KSVP_tag );
                SVPDEBUG2( "CSVPTransferStateContext::CreateMceOutReferL(aTargetSession) toHdr index = %d", index )
                
                if ( KErrNotFound != index )
                    {
                    tmp.Append( toHdr->Mid( index + KSVPTagLength ) );
                    }
                }
            else
                {
                SVPDEBUG2( "CSVPTransferStateContext::CreateMceOutReferL(aTargetSession) error - missing ToHeader %i", KSVPErrTransferReferTo )        
                User::Leave( KSVPErrTransferReferTo );
                }
            
            // From-header
            if ( iTargetSession->FromHeader() )
                {                    
                tmp.Append( KSVPFrom_tag );                    
                
                // Set "from" tag string starting from next char of 
                // the KSVPTo_tag_replaces -text
                TDesC8* fromHdr = iTargetSession->FromHeader();
                TInt index = fromHdr->Find( KSVP_tag );
                SVPDEBUG2( "CSVPTransferStateContext::CreateMceOutReferL(aTargetSession) fromHdr index = %d", index )
                
                if ( KErrNotFound != index )
                    {
                    tmp.Append( fromHdr->Mid( index + KSVPTagLength ) );
                    }
                }
            else
                {
                SVPDEBUG2( "CSVPTransferStateContext::CreateMceOutReferL(aTargetSession) error - missing FromHeader %i", KSVPErrTransferReferTo )
                User::Leave( KSVPErrTransferReferTo );
                }
            
            refptr.Append( tmp );
            } // if ( iSVPSession != iTargetSession )
        else
            {
            SVPDEBUG2( "CSVPTransferStateContext::CreateMceOutReferL(aTargetSession) error - the same target session %i", KSVPErrTransferReferTo )
            User::Leave( KSVPErrTransferReferTo );
            }
        
        // Create the refer
        iMceRefer = CMceOutRefer::NewL( *iMceSession,
                *referto, CMceRefer::ENoSuppression );
        } // if (iTargetSession)
    else
        {
        SVPDEBUG2( "CSVPTransferStateContext::CreateMceOutReferL(aTargetSession) error - no target session %i", KSVPErrTransferReferTo )        
        User::Leave( KSVPErrTransferReferTo );
        }
    
    CleanupStack::PopAndDestroy( referto );    	
    
    SVPDEBUG1( "CSVPTransferStateContext::CreateMceOutReferL(aTargetSession) Out" )        
	}

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::ExecuteReferL
// ---------------------------------------------------------------------------
//
void CSVPTransferStateContext::ExecuteReferL()
    {
    SVPDEBUG1( "CSVPTransferStateContext::ExecuteReferL In" )
    
    delete iMceEvent;
    iMceEvent = NULL;
    
    // Create headers for refer call.
    CDesC8Array* headers = SetupHeadersL();
    CleanupStack::PushL( headers );
    
    // Refer with headers, passes ownership.
    iMceEvent = static_cast< CMceOutRefer* >(MceRefer())->ReferL( headers );          
    CleanupStack::Pop( headers );
    
    // Execution of the refer. Pass ownership of the arguments. Generates 
    // event to the MMceReferObserver. New MceOutEvent is created and events 
    // of that then through MMceEventObserver interface.
    iSVPSession->StartTimerL( KSVPReferExpirationTime,
            KSVPReferTimerExpired  );// Start refer timer
    
    SVPDEBUG1( "CSVPTransferStateContext::ExecuteReferL Out" )
	}

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::SetMceEvent
// ---------------------------------------------------------------------------
//
void CSVPTransferStateContext::SetMceEvent( CMceEvent* aEvent )
    {
    SVPDEBUG1( "CSVPTransferStateContext::SetMceEvent In" )
    
    if ( iMceEvent != aEvent )
        {
        delete iMceEvent;
        iMceEvent = aEvent;
        SVPDEBUG1( "CSVPTransferStateContext::SetMceEvent Updated" )
        }
    
    SVPDEBUG1( "CSVPTransferStateContext::SetMceEvent Out" )
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::MceEvent
// ---------------------------------------------------------------------------
//
CMceEvent* CSVPTransferStateContext::MceEvent()
    {
    return iMceEvent;
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::StopReferTimer
// ---------------------------------------------------------------------------
//
void CSVPTransferStateContext::StopReferTimer()
    {
    SVPDEBUG1( "CSVPTransferStateContext::StopReferTimer" )
    
    iSVPSession->StopTimer( KSVPReferTimerExpired );
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::SetupHeadersL
// ---------------------------------------------------------------------------
//
CDesC8Array* CSVPTransferStateContext::SetupHeadersL()
    {
    SVPDEBUG1( "CSVPTransferStateContext::SetupHeadersL In" )
    
    CDesC8ArrayFlat* headers = new( ELeave ) CDesC8ArrayFlat(
            KSVPHeaderArrayGranularity );
    CleanupStack::PushL( headers );
    
    // Set "referred by" value - that is this end of the session.
    TBuf8<KSVPTempStringlength> tempRefBy;
    tempRefBy.Append( KSVPReferredBy );
    
    if ( iSVPSession->IsMobileOriginated() )
        {
        TDesC8* fromHdr = iSVPSession->FromHeader();
        if ( fromHdr )
            {
            TInt indexLeft = fromHdr->Find( KSVPLeftBracketMark );
            TInt indexRight = fromHdr->Find( KSVPRightBracketMark );
            SVPDEBUG3( "CSVPTransferStateContext::SetupHeadersL: indexLeft = %d, indexRight =  %d", indexLeft, indexRight );

            tempRefBy.Append( fromHdr->Mid( indexLeft,
                    indexRight - indexLeft + KSVPSingleBracketLength ) );
            }
        }
    else
        {
        TDesC8* toHdr = iSVPSession->ToHeader();
        if ( toHdr )
            {
            TInt indexLeft = toHdr->Find( KSVPLeftBracketMark );
            TInt indexRight = toHdr->Find( KSVPRightBracketMark );
            SVPDEBUG3( "CSVPTransferStateContext::SetupHeadersL: indexLeft = %d, indexRight =  %d", indexLeft, indexRight )

            tempRefBy.Append( toHdr->Mid( indexLeft,
                    indexRight - indexLeft + KSVPSingleBracketLength ) );
            }
        }
    
    headers->AppendL( tempRefBy );
    CleanupStack::Pop( headers );
    
    SVPDEBUG1( "CSVPTransferStateContext::SetupHeadersL Out" )
    return headers;
    }

// ---------------------------------------------------------------------------
// CSVPTransferStateContext::InitializeStateArrayL
// ---------------------------------------------------------------------------
//
void CSVPTransferStateContext::InitializeStateArrayL()
    {
    // Create the array of the transfer states.
    iStates = new ( ELeave ) RPointerArray<CSVPTransferStateBase>
            ( KSVPTransferStateArraySize );
    
    // Transfer state classes are created here:
    // Idle state
    CSVPTransferStateBase* state = CSVPTransferIdleState::NewLC();
    User::LeaveIfError( iStates->Insert(
            state, KSVPTransferIdleStateIndex ) );
    CleanupStack::Pop( state );
    
    // Pending state
    state = CSVPTransferPendingState::NewLC();
    User::LeaveIfError( iStates->Insert(
            state, KSVPTransferPendingStateIndex ) );
    CleanupStack::Pop( state );
    
    // Accepted state
    state = CSVPTransferAcceptedState::NewLC();
    User::LeaveIfError( iStates->Insert(
            state, KSVPTransferAcceptedStateIndex ) );
    CleanupStack::Pop( state );
    
    // Terminating state
    state = CSVPTransferTerminatingState::NewLC();
    User::LeaveIfError( iStates->Insert(
            state, KSVPTransferTerminatingStateIndex ) );
    CleanupStack::Pop( state );
    
    // Terminated state
    state = CSVPTransferTerminatedState::NewLC();
    User::LeaveIfError( iStates->Insert(
            state, KSVPTransferTerminatedStateIndex ) );
    CleanupStack::Pop( state );
    }

// -----------------------------------------------------------------------------
// CSVPTransferStateContext::IsStateTransitionAccepted
// -----------------------------------------------------------------------------
//
TBool CSVPTransferStateContext::IsStateTransitionAccepted(
        const TSVPTransferStateIndex aNewState )
	{
	TSVPTransferStateIndex current = CurrentState();
	
    SVPDEBUG2( "CSVPTransferStateContext::IsStateTransitionAccepted: current  = %d", current )
    SVPDEBUG2( "CSVPTransferStateContext::IsStateTransitionAccepted: newstate = %d", aNewState )
    
    switch( current )
        {
        case KErrNotFound:
            {
            // Only idle state can be the first one.
            if ( KSVPTransferIdleStateIndex  == aNewState )
                {
                return ETrue;
                }
            else
                {
                return EFalse;
                }
            }
        
        case KSVPTransferIdleStateIndex:
            {
            if ( KSVPTransferPendingStateIndex == aNewState ||
                 KSVPTransferTerminatingStateIndex == aNewState )
                {
                return ETrue;
                }
            else
                {
                return EFalse;
                }
            }
        
        case KSVPTransferPendingStateIndex:
            {
            if ( KSVPTransferAcceptedStateIndex == aNewState ||
                 KSVPTransferTerminatingStateIndex == aNewState )
                {
                return ETrue;
                }
            else
                {
                return EFalse;
                }
            }
        
        case KSVPTransferAcceptedStateIndex:
            {
            // If in accepted state, new transition is valid (return true).
            // No transition actually occurs because
            // state "changes" from accepted to accepted.
            // This ignores sequent icoming NOTIFY's after REFER has been sent.
            if ( KSVPTransferTerminatingStateIndex == aNewState ||
                    KSVPTransferAcceptedStateIndex == aNewState)
                {
                return ETrue;
                }
            else
                {
                return EFalse;
                }
            }
        
        case KSVPTransferTerminatingStateIndex:
            {
            // From terminating state, transition to terminated is accepted.
            if ( KSVPTransferTerminatingStateIndex == aNewState ||
                 KSVPTransferTerminatedStateIndex == aNewState )
                {
                return ETrue;
                }
            else
                {
                return EFalse;
                }
            }
        
        case KSVPTransferTerminatedStateIndex:
            {
            // From terminated state, transition to idle is accepted.
            if ( KSVPTransferIdleStateIndex == aNewState )
                {
                return ETrue;
                }
            else
                {
                return EFalse;
                }
            }
        
        default:
            {
            // Should not come here, since all the states are handled
            SVPDEBUG1( "CSVPTransferStateContext::IsStateTransitionAccepted - Error" )
            return EFalse;
            }
        // No breaks in switch case due returns.  
        }
	}

