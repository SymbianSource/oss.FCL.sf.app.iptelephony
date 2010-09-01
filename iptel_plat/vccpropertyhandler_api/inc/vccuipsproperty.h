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
* Description:   Vcc P&S property manager
*
*/




#ifndef C_VCCPSPROPERTY_H
#define C_VCCPSPROPERTY_H

#include <e32base.h>
#include <e32std.h>
#include <e32property.h>

#include <vccsubscribekeys.h>

/**
 *  CVccUiPsProperty manages the VCC P&S keys on the UI side:
 *
 *  This class defines the VCC property keys used by the UI.
 *  It provides access to read/write values. Keys can be deleted.
 *
 *  Key KVccPropKeyHoRequest:
 *      EVccNoRequestOngoing                idle state
 *      EVccManualStartCsToPsHoRequest      manual HO PS->CS
 *      EVccManualCancelCsToPsHoRequest     manual HO CS->PS
 *      EVccUndefinedRequest                P&S key not available
 *
 *  @code
 *      CVccUiPsProperty* iVccPsp = CVccUiPsProperty::NewL();
 *      ...
 *      iVccPsp.NotifySubscriberL(EVccManualStartCsToPsHoRequest);
 *      ...
 *      iVccPsp.NotifySubscriberL(EVccManualCancelCsToPsHoRequest);
 *      ...
 *      delete iVccPsp;
 *  @endcode
 *
 *
 *  @lib vccuipropertyhandler.lib
 *  @since S60 v3.2
 */
class CVccUiPsProperty : public CBase
{
public:

    /**
     * Two-phased constructor.
     *
     * Defines UI side VCC property keys if they are not available.
     */
    IMPORT_C static CVccUiPsProperty* NewL();

    /**
     * Destructor.
     * Note! Deletes Vcc UI side property keys
     */
    IMPORT_C virtual ~CVccUiPsProperty();

    /**
     * Update VCC property key KVccPropKeyHoRequest with the given value.
     *
     *  @since S60 v3.2
     * @param aValue The new value
     */
    IMPORT_C void NotifySubscriberL( TVccHoRequest aValue );

    /**
     * Gets the current value of the Vcc property key KVccPropKeyHoRequest.
     *
    *  @since S60 v3.2
     * @param aValue The value of the key
     * @return KErrNone / KErrPermissionDenied/ KErrNotFound/KErrArgument
     */
    IMPORT_C TInt GetCurrentRequest( TVccHoRequest& aValue );

    /**
     * Deletes keys used by UI
     */
    IMPORT_C TInt DeleteKeys();

private:
    /**
     * C++ Constructor
     */
    CVccUiPsProperty();

    /**
     * Symbian 2nd phase Constructor
     */
    void ConstructL();

    /**
     * Defines keys used by UI
     */
    void DefineKeysL();

    /**
     * creates handles to property keys used by UI
     */
    void CreateHandlesL();

private: //data

    /**
     * Handle to property which is updated by the UI
     * Own
     */
    RProperty iHoRequestKey;
};

#endif /*C_VCCPSPROPERTY_H*/
