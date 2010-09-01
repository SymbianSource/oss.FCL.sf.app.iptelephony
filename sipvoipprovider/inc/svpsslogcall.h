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
* Description:  Logs call in case of Do Not Disturb 
*
*/



#ifndef SVPSSLOGCALL_H
#define SVPSSLOGCALL_H

//  INCLUDES
// logs
#include <logwrap.h>
#include <logcli.h>
#include <logeng.h>
#include <f32file.h>
#include <e32base.h>
#include <cntdef.h>
#include <centralrepository.h>
#include "svputdefs.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <logwraplimits.h>
#include <logengdurations.h>
#endif

/**
 *  Logs call in case of Do Not Disturb.
 *
 *  @lib svp.dll
 *  @since S60 3.2
 */
class CSVPSSLogCall : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CSVPSSLogCall* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CSVPSSLogCall();

    public: // New functions
        
        /**
        * Logs call
        * @since Series 60 3.0
        * @param aFrom Caller address
        * @return None
        */
        void HandleCallLoggingL( const TDesC8& aFrom );
        
        /**
        * Resets LogEvent
        * @since Series 60 3.0
        */
        void Reset();
                
    public: // Functions from CActive
        
        /**
        * From CActive Called when an aychronic request cancelled.
        * @since Series 60 3.0
        */
        void DoCancel();
        
        /**
        * From CActive Called when an aychronic request is completed.
        * @since Series 60 3.0
        */
        void RunL();
               
    
    private:

        /**
        * C++ default constructor.
        */
        CSVPSSLogCall();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Finds contact title from phonebook if one exists
        * @since Series 60 3.0
        * @param aSipUri Caller address
        * @return contact title if match found, null otherwise
        */
        HBufC* FindContactTitleL( const TDesC& aSipUri );        
        
        /* 
        * Removes some unused characters from SIP address. Firstly,
        * display name, '<' and '>' are removed. Secondly, the "sip:"
        * prefix is removed if needed.
        * Since Series60 3.0
        * @param aAddress for SIP URI descriptor.
        */
        void RemoveUnusedCharsFromSipAddress( TDes& aAddress );
        
        /*
        * Compares plain SIP URIs. 
        * Username part is case sensitive but uri path isn't.
        * Since Series60 3.0
        * @param aTelNumber contains number to compare.
        * @param aPbookNumber contains phonebook number to compare.
        * @return ETrue if match found, EFalse otherwise.
        */            
        TBool CompareSipUri( const TDesC& aTelNumber, 
                                const TDesC& aPbookNumber );
        
        /*
        * Parses and compares SIP URI username parts.
        * Since Series60 3.0
        * @param aTelNumber contains number to compare.
        * @param aPbookNumber contains phonebook number to compare.
        * @return ETrue if match found, EFalse otherwise.
        */            
        TBool CompareUsernamePart( const TDesC& aTelNumber, 
                                        const TDesC& aPbookNumber );


    private:    // Data
    
        // A handle to a file server session
        RFs iFs;
        
        // Instance of Log Event
        CLogEvent* iLogEvent;
        
        // Instance of Log Client
        CLogClient* iLogClient;
        
        // A CR session & notifier for logs
        CRepository* iLogsSession;
    
	private:
		
		// For testing
		SVP_UT_DEFS
    
    };

#endif      // SVPSSLOGCALL_H   
            
// End of File
