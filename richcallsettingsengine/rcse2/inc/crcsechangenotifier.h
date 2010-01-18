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
* Description:  Provides notification service for changes in settings.
*
*/



#ifndef CRCSECHANGENOTIFIER_H
#define CRCSECHANGENOTIFIER_H

//  INCLUDES
#include <centralrepository.h>
#include <e32base.h>
#include <rcsedefaultdbnames.h>
#include <mrcsebackupobserver.h>

// FORWARD DECLARATIONS
class CRCSEBackupObserver;

// CLASS DECLARATION

/**
*  Provides notification service for any changes in storage.
*
*  @lib RCSE.lib
*  @since S60 v3.0
*/
class CRCSEChangeNotifier : public CBase, public MRCSEBackupObserver
    {
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    *
    * @param aName defines used notifier. aNames are defined 
    *        in RCSEDefaultDBNames.h.
    */
    IMPORT_C static CRCSEChangeNotifier* NewL( const TDesC& aName );
    
    /**
    * Destructor.
    */
    IMPORT_C virtual ~CRCSEChangeNotifier();

public: // From base classes

    /**
    * Handle backup observer event
    *
    * @since S60 v3.0
    */
    void HandleBackupOperationEventL();

public: // New functions
    
    /**
    * Makes an asyncronous request for notification of changes in
    * VoIP profiles. The change can be anything from adding, deleting or
    * updating a profile. The request status > KErrNone in case of commit.
    *
    * @since S60 v3.0
    * @param aStatus The request status object, 
    */
    IMPORT_C void NotifyChange( TRequestStatus& aStatus );

    /**
    * Cancels outstanding notify request.
    *
    * @since S60 v3.0
    */
    IMPORT_C void CancelNotify();
    
private:

    /**
    * C++ default constructor.
    */
    CRCSEChangeNotifier();

    /**
    * By default Symbian 2nd phase constructor is private.
    *
    * @param aName used database identifier.
    */
    void ConstructL( const TDesC& aName );

    /**
    * Setup notifier to requested data source
    *
    * @since S60 v3.1
    */
    void SetNotifierL( TRequestStatus& aStatus );


private:    // Data

    /**
     * Central Repository object.
     * Own.
     */
    CRepository* iRepository;

    /**
     * Request status if notify request is made during backup
     * Own.
     */
    TBuf<100> iName;

    /**
     * Request status if notify request is made during backup
     * No Own.
     */
    TRequestStatus* iReqStatus;

    /**
     * Backup / Restore observer
     * Own.
     */
	CRCSEBackupObserver* iBackupObserver;

    /**
     * Defines range of keys where notifications are requested.
     */
    TUint32 iRange;

    /**
     * Indicates whether notifications requested or not.
     */
    TBool iNotifyRequested;

    // For testing
    #ifdef TEST_EUNIT
        friend class UT_CRCSEChangeNotifier;
    #endif                                  


    };

#endif      // CRCSECHANGENOTIFIER_H   
            
// End of File
