/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Logs Call details in case of Do Not Disturb
*
*/


// INCLUDE FILES
#include    "svpsslogcall.h"
#include    "svpconsts.h"
#include    "svplogger.h"

#include    <utf.h> 

#include	<cntitem.h>
#include    <CPbkContactEngine.h> 
#include    <CPbkContactIter.h>
#include    <CPbkContactItem.h>
#include    <CPbkFieldInfo.h>

#include    <AknNotifyStd.h>
#include    <AknSoftNotifier.h>
#include    <LogsDomainCRKeys.h>

#include    <RPbkViewResourceFile.h>
#include    <coemain.h>

// LOCAL CONSTANTS
_LIT( KLogSubject, "0" );

_LIT8( KLogTagVOIP, "VOIP" ); 
_LIT8( KLogTagURL, "URL" );  
_LIT8( KLogTagMA, "MA" );   
_LIT8( KLogFieldDelimiter, "\t" );
_LIT8( KLogValueDelimiter, "=" );

const TInt KLogEventTempBufSize = 300;
const TInt KMaxAddressLength = 256;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// SVPSSLogCall::SVPSSLogCall
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSVPSSLogCall::CSVPSSLogCall() : CActive(0)
    {
    }

// -----------------------------------------------------------------------------
// SVPSSLogCall::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSVPSSLogCall::ConstructL()
    {
    SVPDEBUG1("CSVPSSLogCall::ConstructL, Enter"); 
    User::LeaveIfError( iFs.Connect() );

    iLogEvent = CLogEvent::NewL();
    iLogClient = CLogClient::NewL( iFs );
	iLogsSession = CRepository::NewL( KCRUidLogs );

    CActiveScheduler::Add(this);
    SVPDEBUG1("CSVPSSLogCall::ConstructL, Exit"); 
    }

// -----------------------------------------------------------------------------
// SVPSSLogCall::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSVPSSLogCall* CSVPSSLogCall::NewL()
    {
    SVPDEBUG1("CSVPSSLogCall::NewL, Enter");
    
    CSVPSSLogCall* self = new( ELeave ) CSVPSSLogCall;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    SVPDEBUG1("CSVPSSLogCall::NewL, Exit");
    return self;
    }
    
// -----------------------------------------------------------------------------
// SVPSSLogCall::~CSVPSSLogCall
// Destructor
// -----------------------------------------------------------------------------
//
CSVPSSLogCall::~CSVPSSLogCall()
    {
    SVPDEBUG1("CSVPSSLogCall::~CSVPSSLogCall, Enter"); 

    Cancel();
    delete iLogsSession;
    delete iLogClient;
    delete iLogEvent;
	iFs.Close();

	SVPDEBUG1("CSVPSSLogCall::~CSVPSSLogCall, Exit"); 
    }


// -----------------------------------------------------------------------------
// SVPSSLogCall::Reset
// REsets iLogEvent
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSVPSSLogCall::Reset()
    {
    SVPDEBUG1("CSVPSSLogCall::Reset, Enter");
    
    iLogEvent->SetId( KLogNullId ); 
    iLogEvent->SetEventType( KNullUid );
    iLogEvent->SetRemoteParty( KNullDesC );
    iLogEvent->SetDirection( KNullDesC );
    iLogEvent->SetTime( Time::NullTTime() );
    iLogEvent->SetDurationType( KLogNullDurationType );
    iLogEvent->SetDuration( KLogNullDuration );
    iLogEvent->SetStatus( KNullDesC ); 
    iLogEvent->SetSubject( KNullDesC );
    iLogEvent->SetNumber( KNullDesC );
    iLogEvent->SetContact( KNullContactId ); 
    iLogEvent->SetLink( KLogNullLink ); 
    iLogEvent->SetDescription( KNullDesC ); 
    iLogEvent->SetFlags( KLogNullFlags );  
    TRAP_IGNORE( iLogEvent->SetDataL( KNullDesC8 ) );
    
    SVPDEBUG1("CSVPSSLogCall::Reset, Exit");
    }

// -----------------------------------------------------------------------------
// SVPSSLogCall::FindContactIdL
// Finds contact if from phonebook if one exists 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CSVPSSLogCall::FindContactTitleL( const TDesC& aSipUri )
    {
    SVPDEBUG1("CSVPSSLogCall::FindContactTitleL, Enter");    
    HBufC* title ( NULL );
    TBool manyCompleteMatches( EFalse );
	TContactItemId compId( NULL );    
       
    CPbkContactEngine* contacts = CPbkContactEngine::NewL();
    CleanupStack::PushL( contacts );
    
    CPbkContactIter* iter = contacts->CreateContactIteratorLC();
	TContactItemId contactId = iter->FirstL();

    while ( contactId != KNullContactId && !manyCompleteMatches )
        {
        //Get contact item
        CPbkContactItem* tempContactItem = iter->CurrentL();        
        
        TInt count = tempContactItem->CardFields().Count();
        SVPDEBUG2("  CSVPSSLogCall::FindContactTitleL count: %d", count );

        for ( TInt ind = 0; ind < count ; ind++ )
            {
            if ( EPbkFieldIdVOIP ==
                     tempContactItem->CardFields()[ind].FieldInfo().FieldId() ||
                 EPbkFieldIdXsp ==
                     tempContactItem->CardFields()[ind].FieldInfo().FieldId() ||
                 tempContactItem->CardFields()[ind].FieldInfo().IsVoipField() )
                {
                SVPDEBUG1("  CSVPSSLogCall::FindContactTitleL: Contact is FOUND" );
                
                //If contact have phone or Internet number fields, get address..  
                TBuf<KMaxAddressLength> tmpPbookNumber( 
                    tempContactItem->CardFields()[ind].Text() );
				
                RemoveUnusedCharsFromSipAddress( tmpPbookNumber );
                
                if ( EPbkFieldIdXsp == tempContactItem->CardFields(
                         )[ind].FieldInfo().FieldId() )
                    {
                    SVPDEBUG1("CSVPSSLogCall::FindContactTitleL, EPbkFieldIdXsp" );
                    // remove all characters before ":" only leftmost part
                    TInt pos = tmpPbookNumber.Find( KSVPCln2 );
                    if ( KErrNotFound != pos )
                        {
                        tmpPbookNumber.Delete( KErrNone, pos + 1 );
                        }
                    }
                
                SVPDEBUG3("  CSVPSSLogCall::FindContactTitleL FoundContactText%d = %S",
                             ind, &tmpPbookNumber );
                
                // Compare complete addresses and set Id if found.
                if ( CompareSipUri( aSipUri, tmpPbookNumber ) )
                    {
                    if ( compId )
                        {
                        SVPDEBUG1("CSVPSSLogCall::FindContactTitleL, manyCompleteMatches");
                        manyCompleteMatches = ETrue;
                        
                        // Set to NULL if multiple matches                     
                        if ( title )
                            {
                            delete title;
                            title = NULL;
                            }       
                        }
                    else
                        {
                        title = tempContactItem->GetContactTitleL();
                        compId = contactId;
                        ind = count;
                        }
                    }
                }
            }
        contactId = iter->NextL();
        }
    
    // Set contact if only one match found
    if ( compId && !manyCompleteMatches )
        {
        iLogEvent->SetContact( compId );
        } 

    CleanupStack::PopAndDestroy( iter );
    CleanupStack::PopAndDestroy( contacts );

    SVPDEBUG1("CSVPSSLogCall::FindContactTitleL, Exit");
    return title;
    }

// -----------------------------------------------------------------------------
// SVPSSLogCall::HandleCallLoggingL
// Logs Call information 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSVPSSLogCall::HandleCallLoggingL( const TDesC8& aFrom )
    {
    SVPDEBUG1("CSVPSSLogCall::HandleCallLoggingL, Enter");
    
    Reset();
    
    // For recognizing VoIP number and getting
    // correct "Options" and internet call icon to log.
	TBuf8<KLogEventTempBufSize> tempBufOne;
    TBuf8<KLogEventTempBufSize> tempBufTwo;
    
    tempBufOne.Append( KLogTagVOIP() );
    tempBufOne.Append( KLogFieldDelimiter() );
    tempBufOne.Append( KLogTagURL() );
    tempBufOne.Append( KLogValueDelimiter() );
    
    iLogEvent->SetEventType(KLogCallEventTypeUid);
    iLogEvent->SetSubject(KLogSubject);
    
    TLogString missed;
    iLogClient->GetString(missed, R_LOG_DIR_MISSED);
    iLogEvent->SetDirection(missed);

    iLogEvent->SetDurationType( KLogDurationNone );
    TLogDuration duration = 0;
    iLogEvent->SetDuration( duration );
    
    iLogEvent->SetNumber( KNullDesC );
    
    TLogString number;
    CnvUtfConverter::ConvertToUnicodeFromUtf8( number, aFrom );
    RemoveUnusedCharsFromSipAddress( number );
    SVPDEBUG2("  CSVPSSLogCall::HandleCallLoggingL, number = %S", &number );

    if ( KErrNotFound == aFrom.Find( KSVPAnonymousName8 ) )
    	{
        SVPDEBUG1("  CSVPSSLogCall::HandleCallLoggingL, URI address is seen" );
        HBufC* contactName = FindContactTitleL( number );
        if ( contactName )
            { 
            //  Phonebook contact id has been found
            tempBufTwo.Copy( number );
            iLogEvent->SetRemoteParty( contactName->Des() );
            }
        else
            { // No phonebook contact id has been found
           	tempBufTwo.Copy( number );
           	iLogEvent->SetRemoteParty( number );
            }
        
        if ( contactName )
            {
            delete contactName;
            contactName = NULL;
            }
        
        tempBufOne.Append( tempBufTwo );
        tempBufTwo.Zero();
        tempBufOne.Append( KLogFieldDelimiter() );
        tempBufOne.Append( KLogTagMA() );
        tempBufOne.Append( KLogValueDelimiter() );
        }
    else
        {
        SVPDEBUG1("  CSVPSSLogCall::HandleCallLoggingL, URI address is Anonymous" );
        iLogEvent->SetRemoteParty( KSVPPrivateNumber );
        }
    TRAP_IGNORE( iLogEvent->SetDataL( tempBufOne ) );
    TTime eventTime;
    eventTime.UniversalTime();
    iLogEvent->SetTime(eventTime);
    if( !IsActive() )
        {
        iLogClient->AddEvent(*iLogEvent, iStatus);
        SetActive();
        }
	SVPDEBUG1("CSVPSSLogCall::HandleCallLoggingL, Exit");
    }

// -----------------------------------------------------------------------------
// SVPSSLogCall::DoCancel
// Called when an aychronic request is cancelled
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSVPSSLogCall::DoCancel()
    {
    SVPDEBUG1("CSVPSSLogCall::DoCancel"); 
	iLogClient->Cancel();
    }
    
// -----------------------------------------------------------------------------
// SVPSSLogCall::RunL
// Called when an aychronic request is completed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSVPSSLogCall::RunL()
    {
    SVPDEBUG1("CSVPSSLogCall::RunL, Enter");
    // If LogClient AddEvent successful, then show missed call soft notifier
    if ( KErrNone == iStatus.Int() )
        {
        TInt missedCallCount(0);
        TInt err = iLogsSession->Get( KLogsNewMissedCalls, missedCallCount );
        
        if( KErrNotFound == err || KErrUnknown == err || missedCallCount < 0 )
            {       
            missedCallCount = 0;
            iLogsSession->Set( KLogsNewMissedCalls, missedCallCount );
            }
        else
            {
            missedCallCount++;
            iLogsSession->Set( KLogsNewMissedCalls, missedCallCount );    
            }
        SVPDEBUG2("  CSVPSSLogCall::RunL, missedCallCount: %d", missedCallCount );
        }
        
	SVPDEBUG1("CSVPSSLogCall::RunL, Exit");
    }
  
// -----------------------------------------------------------------------------
// SVPSSLogCall::RemoveUnusedCharsFromSipAddress
// Removes unused chars from sip address
// (other items were commented in a header). 
// -----------------------------------------------------------------------------
//
void CSVPSSLogCall::RemoveUnusedCharsFromSipAddress( TDes& aAddress )
    {
    SVPDEBUG1("CSVPSSLogCall::RemoveUnusedCharsFromSipAddress: Enter" );
    // Display name, '<' and '>' are not used when comparing addresses.
    TInt pos = aAddress.Find( KSVPLeftBracketMark2 );
    if ( KErrNotFound != pos )
        {
	    SVPDEBUG1("CSVPSSLogCall::RemoveUnusedCharsFromSipAddress: '<' from uri" );
        aAddress.Delete( 0, pos + 1 );
        }
    
    pos = aAddress.Find( KSVPRightBracketMark2 );
    if ( KErrNotFound != pos )
        {
	    SVPDEBUG3("CSVPSSLogCall::RemoveUnusedCharsFromSipAddress: \
    	'>' in uri pos = %d, length = %d",
        pos, aAddress.Length() );
        aAddress.Delete( pos, aAddress.Length() - pos );
        }
        
    // "sip:" is not used when comparing addresses. 
    if ( KSVPSipPrefixLength < aAddress.Length() )
        {
        TBuf<KSVPSipPrefixLength> sipPrefix;
        sipPrefix.Copy( aAddress.Left( KSVPSipPrefixLength ) );
        sipPrefix.LowerCase();
        if ( !sipPrefix.Compare( KSVPSipPrefix2 ) )
            {
            SVPDEBUG1("CSVPSSLogCall::RemoveUnusedCharsFromSipAddress: 'sip:' in uri" );
			aAddress.Delete( KErrNone, KSVPSipPrefixLength );
            }
        }

    // remove all extra parameters from given address
	pos = aAddress.Find( KSVPSemiColon );        
    if ( KErrNotFound != pos )
        {
	    SVPDEBUG3("CSVPSSLogCall::RemoveUnusedCharsFromSipAddress: \
    	There is ';' in uri pos = %d, length = %d",
        pos, aAddress.Length() );

        // Delete ";" and text after it
        aAddress.Delete( pos, aAddress.Length() - pos );
        }

    SVPDEBUG1("CSVPSSLogCall::RemoveUnusedCharsFromSipAddress: Exit" );
    }    

// -----------------------------------------------------------------------------
// SVPSSLogCall::::CompareSipUri
// Compares whole sip uri
// (other items were commented in a header).  
// -----------------------------------------------------------------------------
//
TBool CSVPSSLogCall::CompareSipUri( 
    const TDesC& aTelNumber, const TDesC& aPbookNumber )
    {
    // Complete SIP address must contain host part.
    if ( KErrNotFound < aTelNumber.Find( KSVPAt2 ) &&
         KErrNotFound < aPbookNumber.Find( KSVPAt2 ) )
        {
        TBuf<KMaxAddressLength> tmpTelNumber( aTelNumber );
        TBuf<KMaxAddressLength> tmpPbookNumber( aPbookNumber );
    
        // Compare username parts, which are case sensitive.
        if ( CompareUsernamePart( tmpTelNumber, tmpPbookNumber ) )
            {
            // Convert descriptors to lowercase.
            tmpTelNumber.LowerCase();
            tmpPbookNumber.LowerCase();

            // Compare host part, which is not case sensitive.
            if ( !tmpTelNumber.Compare( tmpPbookNumber ) )
                {
                SVPDEBUG1("CSVPSSLogCall::CompareSipUri: SipUri's EQUAL");
                return ETrue;
                }
            }
        }
    SVPDEBUG1("CSVPSSLogCall::CompareSipUri: SipUri's do NOT EQUAL");    
    return EFalse;    
    }
    
// -----------------------------------------------------------------------------
// SVPSSLogCall::::CompareUsernamePart
// Compares username of sip uri
// (other items were commented in a header).  
// -----------------------------------------------------------------------------
//
TBool CSVPSSLogCall::CompareUsernamePart( 
    const TDesC& aTelNumber, const TDesC& aPbookNumber )
    {
    TBuf<KMaxAddressLength> tmpTelNumber( aTelNumber );
    TBuf<KMaxAddressLength> tmpPbookNumber( aPbookNumber );
    
    // Remove uri part from tel. number.
    TInt pos = tmpTelNumber.Find( KSVPAt2 );
    if ( KErrNotFound < pos )
        {
        tmpTelNumber.Delete( pos, tmpTelNumber.Length() - pos );
        }
    
    // Remove uri part from phonebook number.
    pos = tmpPbookNumber.Find( KSVPAt2 );
    if ( KErrNotFound < pos )
        {
        tmpPbookNumber.Delete( pos, tmpPbookNumber.Length() - pos );
        }            

    // Compare parsed numbers.
    if ( !tmpTelNumber.Compare( tmpPbookNumber ) )
        {
        SVPDEBUG1("CSVPSSLogCall::CompareUsernamePart: Names EQUAL");
        return ETrue;
        }
	SVPDEBUG1("CSVPSSLogCall::CompareUsernamePart: Names do NOT EQUAL");        
    return EFalse;
    }

//  End of File  
