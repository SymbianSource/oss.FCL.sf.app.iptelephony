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
* Description:  CCchPhoneStartupMonitor.
*
*/

#ifndef C_CCHPHONESTARTUPMONITOR_H
#define C_CCHPHONESTARTUPMONITOR_H

#include <e32std.h>
#include <e32base.h>
#include <e32property.h>

class CCchUIHandler;

/**
 *  CCchPhoneStartupMonitor declaration
 *  Monitors phone startup.
 *  @lib cchserver.exe
 *  @since S60 5.0
 */
class CCchPhoneStartupMonitor : public CActive
    {

public: // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CCchPhoneStartupMonitor* NewL( CCchUIHandler& aCchUIHandler );

    /**
     * Two-phased constructor.
     */
    static CCchPhoneStartupMonitor* NewLC( CCchUIHandler& aCchUIHandler );

    /**
     * Destructor.
     */
    virtual ~CCchPhoneStartupMonitor();

    /**
    Implements cancellation of an outstanding request.
    @see CActive::Cancel
    */
    virtual void DoCancel();

    /**
    Handles an active object's request completion event.
    */
    virtual void RunL();
    
    /**
    Handles leaving of RunL.
    */
    virtual TInt RunError(TInt aError);
    
private:

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * C++ default constructor.
     */
    CCchPhoneStartupMonitor( CCchUIHandler& aCchUIHandler );
    
    /**
    Monitor phone startup phases.
    */
    void MonitorPhoneStartupPhaseL();
    
private: // data

    /**
     * Handle to cch ui handler.
     */
    CCchUIHandler& iCchUIHandler;
    
    /**
     * Handle to RProperty.
     */
    RProperty iStartupUiPhaseProperty;
    
    /**
     * Should I commit suicide
     */
    TBool iDie;
    };

#endif // C_CCHPHONESTARTUPMONITOR_H

// End of file
