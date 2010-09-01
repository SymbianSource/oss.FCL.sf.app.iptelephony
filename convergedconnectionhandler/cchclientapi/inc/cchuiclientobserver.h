/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Encapsulates singleton client and provided observer
*
*/



#ifndef C_CCHUICLIENTOBSERVER_H
#define C_CCHUICLIENTOBSERVER_H

#include <e32base.h>
#include <e32const.h>

class MCchUiObserver;

/**
 * This class stores client provider reference and api instance used to create
 * it. Those can be mapped when api instance is deleted to remove observer. 
 * This way singleton instance will not call observers that are already deleted.
 * This class implements the use of Service provider settings.
 *
 * @code
 * @endcode
 *
 * @lib cchui.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( CCchUiClientObserver ): public CBase
    {
public:
    
    /**
     * Two-phased constructor.
     */
    static CCchUiClientObserver* NewL( MCchUiObserver& aObserver);
    
    /**
     * Two-phased constructor.
     */
    static CCchUiClientObserver* NewLC( MCchUiObserver& aObserver );

    /**
     * Destructor.
     */
    virtual ~CCchUiClientObserver();
      
    /**
     * Return observer.
     *
     * @since S60 5.0
     * @return observer reference
     */
    MCchUiObserver& Observer();

private:

    CCchUiClientObserver( MCchUiObserver& aObserver );

private: // data

    /**
     * Observer reference.
     */
    MCchUiObserver& iObserver;
    };

#endif // C_CCHUICLIENTOBSERVER_H
