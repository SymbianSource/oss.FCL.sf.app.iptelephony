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
* Description:  Provides access to service provider settings
*
*/


#ifndef C_CCHUISPSHANDLER_H
#define C_CCHUISPSHANDLER_H

#include <e32base.h>
#include <e32const.h>

class CSPSettings;

/**
 * Service provider settings handler.
 * This class implements the use of Service provider settings.
 *
 * @code
 *  CCchUiSpsHandler* spsHandler = CCchUiSpsHandler::NewLC();
 *  spsHandler->SetTemporaryIapIdL( aServiceId, aIapId );
 *  CleanupStack::PopAndDestroy( spsHandler );
 * @endcode
 *
 * @lib cchui.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( CCchUiSpsHandler ): public CBase
    {
public:
    
    /**
     * Two-phased constructor.
     */
    static CCchUiSpsHandler* NewL();
    
    /**
     * Two-phased constructor.
     */
    static CCchUiSpsHandler* NewLC();

    /**
     * Destructor.
     */
    virtual ~CCchUiSpsHandler();
      
    /**
     * Sets temporary iap id for the sevice.
     *
     * @since S60 5.0
     * @param aServiceId Service id.
     * @param aIapId Iap id to be set.
     */
    void SetTemporaryIapIdL( 
        TUint32 aServiceId, 
        TUint32 aIapId );
    
    /**
     * Sets SNAP id for specific service.
     *
     * @since S60 5.0
     * @param aServiceId Service id.
     * @param aSnapId Snap id to be set.
     */
    void SetSnapIdL( 
        TUint32 aServiceId,
        TUint32 aSnapId );    

    /**
     * Resolves service name.
     *
     * @since S60 5.0
     * @param aServiceId Service id.
     * @param aServiceName, will store service name
     */
    void ServiceNameL( 
        TUint32 aServiceId, 
        TDes& aServiceName );

private:

    CCchUiSpsHandler();
    
    void ConstructL();

private: // data

    /**
     * Service provider settings.
     * Own.
     */
    CSPSettings* iSettings;
    };

#endif // C_CCHUISPSHANDLER_H
