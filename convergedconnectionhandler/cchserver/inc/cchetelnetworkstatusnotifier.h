/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCchEtelNetworkStatusNotifier handles cch's CTelephony 
*              : related observing functionality.
*
*/


#ifndef CCHETELNETWORKSTATUSNOTIFIER_H
#define CCHETELNETWORKSTATUSNOTIFIER_H

// INCLUDES
#include <e32base.h>
#include <etelmm.h>

// FORWARD DECLARATIONS

class MCchEtelNetworkStatusObserver;

// CLASS DECLARATION
/**
*  CCchEtelNetworkStatusNotifier
*  
*  @lib   cchserver
*  @since 3.2
*/

enum TCCHEtelConnectionState
    {
    ECCHEtelConnect = 0,
    ECCHEtelLoadModule,
    ECCHEtelOpenPhone,
    ECCHEtelConnected
    };
    
class CCchEtelNetworkStatusNotifier : 
              public CActive
{
public:

    /**
    * Two-phased constructor.
    */         
    static CCchEtelNetworkStatusNotifier* NewL( 
        MCchEtelNetworkStatusObserver& aEtelNetworkStatusObserver );

    /**
    * Destructor.
    */
    ~CCchEtelNetworkStatusNotifier();

public: // Other functions
    
    /**
    * Returns whether network status is no-service
    *
    * @since S60 3.1
    * @param None
    * @return TBool ETrue	if network status is no-service
    *				EFalse 	if network status is something else 
    *                           than no-service
    */
    TBool IsNetworkStatusNoService() const; 

    /**
    * Connects to etel
    *
    * @since S60 3.2
    * @param None
    * @return None
    */
    void Connect(); 


protected:

    // From CActive
    void RunL();
    void DoCancel();

private:

    /**
    * Constructors.
    */         
    CCchEtelNetworkStatusNotifier( MCchEtelNetworkStatusObserver& aEtelNetworkStatusObserver );
    void ConstructL();

    /*
    * Order notification
    */
    void OrderNotification();

private:
    /**
     * ETel server
     */
    RTelServer                                      iEtelServer;
    /**
     * phone
     */
    RMobilePhone                                    iPhone;
    /*
    * observer, not own
    */
    MCchEtelNetworkStatusObserver&                  iEtelNetworkStatusObserver;  
    
    /*
    * current cs network registration status
    */    
    RMobilePhone::TMobilePhoneRegistrationStatus    iRegistrationStatus;
 
    /*
     * Last cs network registration status
     */    
    RMobilePhone::TMobilePhoneRegistrationStatus    iLastRegistrationStatus;
    
    /*
     * Is notification already ordered?
     */    
    TBool                                            iNotified;
  
    /*
    * delay before next try to connect to etel
    */    
    TInt                                            iDelay;
    
    /*
    * status of etel connection
    */    
    TCCHEtelConnectionState                         iState;
    
    /*
    * connection timer
    */    
    RTimer                                          iTimer;
 
};

#endif // CCHETELNETWORKSTATUSNOTIFIER_H

// End of File
