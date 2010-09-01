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
* Description:  Monitors Events from VoiceMailBox -server
*
*/



#ifndef IPVMBXEVENTMONITOR_H
#define IPVMBXEVENTMONITOR_H


#include <mceeventobserver.h>


class CIpVmbxEngine;


/**
 *  Handles and processes events from VoiceMailBox -server
 *
 *  @lib IPVoiceMailAppEngine.lib
 */
NONSHARABLE_CLASS( TIpVmbxEventMonitor ) : public MMceEventObserver
    {
    
public:  // Constructors and destructor
    
    /**
     * C++ default constructor.
     * 
     * @param Instance to engine for message sending.
     */
    TIpVmbxEventMonitor( CIpVmbxEngine& aEngine );
    
    
public:

// From base class MMceEventObserver

    /**
     * From base class MMceEventObserver
     * The state of the event has changed
     * 
     * @param aEvent, the event that has changed
     * @param aContainer, if present, holds details of
     *        transaction causing state change
     */
    void EventStateChanged(
        CMceEvent& aEvent,
        TMceTransactionDataContainer* aContainer );
            
    /**
     * From base class MMceEventObserver
     * The state of the event has changed
     * 
     * @param aEvent, event received notification
     * @param aContainer, if present, holds details of
     *        transaction causing state change
     */
    void NotifyReceived(
        CMceEvent& aEvent,
        TMceTransactionDataContainer* aContainer );
                                
    /**
     * From base class MMceEventObserver
     * The state of the event used by the refer has changed
     * 
     * @param aEvent, the event that has changed
     * @param aActive, ETrue if connection active, EFalse if connection 
     *        inactive
     */
    void EventConnectionStateChanged( CMceEvent& aEvent, TBool aActive );
                    
    /**
     * From base class MMceEventObserver
     * An error has occurred concerning a specific SIP event
     * Note, that each error causes the event state to be ETerminated
     * 
     * @param aEvent, The event raising the error
     * @param aError, Error code
     */
    void Failed( CMceEvent& aEvent, TInt aError );
        
    
private:    // Data

    /**
     * Instance to engine
     */
    CIpVmbxEngine& iEngine;
    
    };

#endif      // IPVMBXEVENTMONITOR_H
