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
* Description:  
*
*/


#ifndef C_CSCPPROPERTYNOTIFIER_H
#define C_CSCPPROPERTYNOTIFIER_H

#include <e32base.h>
#include <e32property.h>

class MScpPropertyObserver;

/**
*  Property notifier
*
*/
class CScpPropertyNotifier : public CActive
    {
public:

    /**
     * Two-phased constructor.
     */
    static CScpPropertyNotifier* NewL( TUid aUid, 
                                       TInt aType,
                                       MScpPropertyObserver& aObserver  );
    
    /**
     * Destructor.
     */
    virtual ~CScpPropertyNotifier();

    /**
     * Returns the property's value
     * @param aValue Value
     * @return Symbian error codes
     */
    TInt GetValue( TInt& aValue ) const;
       
// From CActive         
private:

    /**
     * RunL
     */
    void RunL();
    
    /**
     * Catches errors if RunL leaves
     * @param aError error code
     * @return error code
     */
    TInt RunError( TInt aError );
    
    /**
     * Cancels the monitor
     */
    void DoCancel();  

private:

    /**
     * C++ default constructor.
     */
    CScpPropertyNotifier( TUid aUid, 
                          TInt aType, 
                          MScpPropertyObserver& aObserver );
    
    /**
     * ConstructL
     */
    void ConstructL();

    /**
     * Subscribes to property's notifications
     */
    void Subscribe();

private:

    /**
     * UID of the property
     */
    TUid iUid;
    
    /**
     * Property type
     */
    TInt iType; 

    /**
     * Property observer
     */
    MScpPropertyObserver& iObserver;

    /**
     * Property
     */
    RProperty iProperty;

#ifdef _DEBUG

friend class T_CScpPropertyNotifier;

#endif

    };


#endif      // C_CSCPPROPERTYNOTIFIER_H   
            
// End of File
