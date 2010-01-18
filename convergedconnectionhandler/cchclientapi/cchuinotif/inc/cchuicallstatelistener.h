/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CCH UI Call State Listener
*
*/

#ifndef C_CCHUICALLSTATELISTENER_H
#define C_CCHUICALLSTATELISTENER_H


#include <e32property.h>
#include <e32base.h>

class MCchUiCallStateObserver;


/**
*  Class for listening call state changes.
 *
 *  @code
 *    CCchUiCallStateListener listener =
 *        CCchUiCallStateListener::NewL( *this );
 *  @endcode
 *
 *  @lib cchuinotif.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CCchUiCallStateListener ) : public CActive
    {

public:

    /**
     * Two-phased constructor.
     * @param aObserver callback observer
     */
    static CCchUiCallStateListener* NewL( MCchUiCallStateObserver& aObserver );

    /**
     * Destructor.
     */
    virtual ~CCchUiCallStateListener();

protected:

// from base class CActive

    /**
     * From CActive
     * See base class.
     */
    void RunL();

    /**
     * From CActive
     * See base class.
     */
    void DoCancel();

private:

    CCchUiCallStateListener( MCchUiCallStateObserver& aObserver );

    /**
     * Handle state change.
     *
     * @since S60 v5.0
     */
    void HandleChange();

    /**
     * Start listening call state changes.
     *
     * @since S60 v5.0
     */
    void IssueRequest();

private: // data

    /*
     * Property handle used to get property value
     */
    RProperty iProperty;

    /**
     * Call state
     */
    TInt iCallState;

    /**
     * CCH UI call state observer for callbacks
     */
    MCchUiCallStateObserver& iObserver;
    
    // For unit testing
    friend class t_cchuicallstatelistener;
    
    };

#endif // C_CCHUICALLSTATELISTENER_H
