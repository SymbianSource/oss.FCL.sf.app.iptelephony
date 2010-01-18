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
* Description:  Active object for watching P&S changes
*
*/



#ifndef CSVPPROPERTYWATCH_H
#define CSVPPROPERTYWATCH_H

#include <e32base.h>
#include <e32property.h>

#include "svppropertywatchobserver.h"
#include "svputdefs.h"

 /**
 *  Attaches itself to the specified P&S property and notifies clients when 
 *  property was changed.
 *
 *
 *  @lib svp.dll
 *  @since S60 3.2
 */
class CSVPPropertyWatch : public CActive
    {

public:
       
    /**
    * Two-phased constructor.
    */
    static CSVPPropertyWatch* NewL( MSVPPropertyWatchObserver& aClient,
                                    const TUid& aCategory,
                                    TInt aKey );
    
    /**
    * Destructor.
    */
    virtual ~CSVPPropertyWatch();

protected:  // Functions from base classes 
        
    /**
    * From CActive. Handles an active object's request completion event.
    */
    void RunL();

    /**
    * From CActive. Implements cancellation of an outstanding request.
    */
    void DoCancel();
    
    /**
    * From CActive. Handles a leave occurring in the request completion
    * event handler RunL().
    */
    TInt RunError( TInt aError );
        
private:

    /**
    * C++ default constructor.
    */
    CSVPPropertyWatch( MSVPPropertyWatchObserver& aClient, TInt aKey );

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL( const TUid& aCategory );
 
     
private:  // Data
        
    /**
    * The client interested being notified about property changes
    */
    MSVPPropertyWatchObserver& iClient;

    /**
    * The key defining property where watcher attaches to
    */
    TInt iKey;

    /**
    * The property to attach to
    */
    RProperty iProperty;

private:

    // For testing
    SVP_UT_DEFS
    
    };

#endif //  C_SVPPROPERTYWATCH_H
