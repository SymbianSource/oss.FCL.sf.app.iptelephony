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
* Description:  Class for implementing Call Restrictions Supplementary services
*
*/



// INCLUDE FILES
#include    "svpsupplementaryservices.h"
#include    "svplogger.h"
#include    "svpconsts.h"
#include     <settingsinternalcrkeys.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSSCallRestrictionsApp::CSSCallRestrictionsApp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSVPSupplementaryServices::CSVPSupplementaryServices()
    {
    }

// -----------------------------------------------------------------------------
// CSVPSupplementaryServices::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSVPSupplementaryServices::ConstructL()
    {
    SVPDEBUG1("CSVPSupplementaryServices::ConstructL, Enter");

    iRichCallRepository = CRepository::NewL( KCRUidRichCallSettings );
    iLogCall = CSVPSSLogCall::NewL();
    
    SVPDEBUG1("CSVPSupplementaryServices::ConstructL, Exit");
    }

// -----------------------------------------------------------------------------
// CSVPSupplementaryServices::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSVPSupplementaryServices* CSVPSupplementaryServices::NewL()
    {
    CSVPSupplementaryServices* self = CSVPSupplementaryServices::NewLC();
    CleanupStack::Pop( self );
    
    SVPDEBUG1("CSVPSupplementaryServices::NewL, Exit");
    return self;
    }

// -----------------------------------------------------------------------------
// CSVPSupplementaryServices::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSVPSupplementaryServices* CSVPSupplementaryServices::NewLC()
    {
    CSVPSupplementaryServices* self = 
        new( ELeave ) CSVPSupplementaryServices;
    
    CleanupStack::PushL( self );
    self->ConstructL();

    SVPDEBUG1("CSVPSupplementaryServices::NewLC, Exit");
    return self;
    }

// -----------------------------------------------------------------------------
// CSVPSupplementaryServices::~CSVPSupplementaryServices
// Destructor
// -----------------------------------------------------------------------------
//
CSVPSupplementaryServices::~CSVPSupplementaryServices()
    {
	SVPDEBUG1("CSVPSupplementaryServices::~CSVPSupplementaryServices, Enter");
    delete iLogCall;
    delete iRichCallRepository;
    SVPDEBUG1("CSVPSupplementaryServices::~CSVPSupplementaryServices, Exit");
    }

// -----------------------------------------------------------------------------
// CSVPSupplementaryServices::CheckRestrictionsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TSupplementaryServicesEvent	CSVPSupplementaryServices::CheckRestrictionsL( 
									CDesC8Array& aHeaders )
    {
    SVPDEBUG1("CSVPSupplementaryServices:CheckRestrictionsL, Enter");

	TInt currentAnonymous( KSVPAnonymousBarringOff ); 
	TInt currentDoNotDisturb( KSVPDoNotDisturbOff ); 

	TInt isAnonymousAddress (0);
	TInt isAnonymousSecAddress ( 0 );
//  Should URI be checked separately too in case there is no display name?
	TInt isAnonymousAddressWithoutName ( 0 );
	TInt isAnonymousSecAddressWithoutName ( 0 );
	TInt isAnonymousName8 ( 0 );

	TPtrC8 originator = aHeaders.MdcaPoint( KSVPMceArrayIndexOriginator );
	
	TInt cycle ( 0 );
	TCheckingState state ( ESVPIdle );
	
	while ( cycle == 0 ) 
		{
	    switch( state )
	        {
	        
    	    case ESVPIdle:
				//	Check from CRepository if anonymous barring is on. Else DnD is on.
			    iRichCallRepository->Get( 
        			KRCSEAnonymousCallBlockRule, currentAnonymous );
				SVPDEBUG2(" CSVPSupplementaryServices:CheckRestrictionsL AnonymousBarring: %d",
				            currentAnonymous );                                                  
    			iRichCallRepository->Get( KRCSEDoNotDisturb, currentDoNotDisturb );
				SVPDEBUG2(" CSVPSupplementaryServices:CheckRestrictionsL DoNotDisturb: %d",
				            currentDoNotDisturb );
				
				if ( KSVPAnonymousBarringOff == currentAnonymous &&
                     KSVPDoNotDisturbOff == currentDoNotDisturb )
					{
					state = ESVPAllowingDail;
					}
				else if ( KSVPAnonymousBarringOff == currentAnonymous &&
                          KSVPDoNotDisturbOn == currentDoNotDisturb )
					{
					state = ESVPMakingCallLog;
					}
				else if ( KSVPAnonymousBarringOn == currentAnonymous )
					{
					isAnonymousAddress = originator.Find( KSVPMyAnonymousAddress );
					isAnonymousSecAddress = originator.Find( KSVPMyAnonymousSecAddress );
					isAnonymousAddressWithoutName = originator.Find( KSVPAnonymousAddressWithoutName );
					isAnonymousSecAddressWithoutName = originator.Find( KSVPAnonymousSecAddressWithoutName );
					isAnonymousName8 = originator.Find( KSVPAnonymousName8 );
					
					if ( KSVPDoNotDisturbOff == currentDoNotDisturb )
						{
						state = ESVPCheckingAnonymousToDial;
						}
					else if ( KSVPDoNotDisturbOn == currentDoNotDisturb )
						{
						state = ESVPCheckingAnonymousToLog;
						}
					else
						{
						state = ESVPError;
						}	
					}
				else
					{
					state = ESVPError;
					}	
				break;

        	case ESVPAllowingDail:
        		iSSEvent = ESVPSSDefault;	
        		state = ESVPIdle;
        		cycle = 1;
            	break;

        	case ESVPMakingCallLog:
    			iLogCall->HandleCallLoggingL( originator );
    			
        		iSSEvent = ESVPSSDoNotDisturb;	
        		state = ESVPRejectingCall;
            	break;

        	case ESVPCheckingAnonymousToDial:
        		if( KErrNotFound != isAnonymousAddress || 
          			KErrNotFound != isAnonymousSecAddress || 
          			KErrNotFound != isAnonymousAddressWithoutName || 
          			KErrNotFound != isAnonymousSecAddressWithoutName || 
          			KErrNotFound != isAnonymousName8 )
 	        		{
	        		iSSEvent = ESVPSSAnonymousBarring;
	        		state = ESVPRejectingCall;	
	        		}
	        	else 
		        	{
        			state = ESVPAllowingDail;	
		        	};
		        break;
		        
        	case ESVPCheckingAnonymousToLog:
        		if( KErrNotFound != isAnonymousAddress || 
          			KErrNotFound != isAnonymousSecAddress || 
          			KErrNotFound != isAnonymousAddressWithoutName || 
          			KErrNotFound != isAnonymousSecAddressWithoutName || 
          			KErrNotFound != isAnonymousName8 ) 
	        		{
					iSSEvent = ESVPSSAnonymousBarring;	
	        		state = ESVPRejectingCall;
	        		}
	        	else 
		        	{
        			state = ESVPMakingCallLog;
		        	};
		        break;
        		
        	case ESVPRejectingCall:
        		state = ESVPIdle;
        		cycle = 1;
            	break;

        	case ESVPError:
        		// Collect error situation. Should not come there ever
        		state = ESVPIdle;
        		cycle = 1;
            	break;
			}
		}

	SVPDEBUG2("CSVPSupplementaryServices:CheckRestrictionsL Exit, event: %d", iSSEvent);
	return iSSEvent; 

    }

//End of File    

