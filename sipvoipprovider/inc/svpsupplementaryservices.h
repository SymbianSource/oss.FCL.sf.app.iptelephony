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
* Description:  Class for implementing Call Restrictions
*
*/


#ifndef SVPSUPPLEMENTARYSERVICES_H
#define SVPSUPPLEMENTARYSERVICES_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <centralrepository.h> 

#include "svpconsts.h"
#include "svpsslogcall.h"
#include "svputdefs.h"

/**
 *  SVP supplementary services
 *
 *  Handles Do Not Disturb and anonymous call barring cases.
 *
 *  @lib svp.dll
 *  @since S60 3.2 
 */
// DATA TYPES
enum TCheckingState
    {
    ESVPIdle,
    ESVPAllowingDail,
    ESVPMakingCallLog,
    ESVPCheckingAnonymousToDial,
    ESVPCheckingAnonymousToLog,
    ESVPRejectingCall,
    ESVPError
    };
class CSVPSupplementaryServices : public CBase
    {

    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CSVPSupplementaryServices* NewL();
        static CSVPSupplementaryServices* NewLC();

        /**
        * For sending response to incoming INVITE.
        * @since S60 3.2
        * @param aHeaders SIP headers.  
        * @return type of restriction.
        */
        TSupplementaryServicesEvent CheckRestrictionsL( 
        		CDesC8Array& aHeaders);
        
        /**
        * Destructor.
        */
        virtual ~CSVPSupplementaryServices();

    private:


        /**
        * C++ default constructor.
        */
        CSVPSupplementaryServices();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
    
        /**
        * Supplementary Services event
        */
        TSupplementaryServicesEvent iSSEvent;
        
        /**
        * Instance of CSSLogCall for call logging (DND)
        */
        CSVPSSLogCall* iLogCall;
        
        /**
        * Instance of Central Repository
        */
        CRepository* iRichCallRepository;

	private:
		
		// For testing
        SVP_UT_DEFS
        
    };

#endif      // SVPSUPPLEMENTARYSERVICES_H
            
