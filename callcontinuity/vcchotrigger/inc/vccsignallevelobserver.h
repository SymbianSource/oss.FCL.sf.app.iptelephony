/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Definition of a signal level observer API
*
*/



#ifndef VCCSIGNALLEVELOBSERVER_H
#define VCCSIGNALLEVELOBSERVER_H

#include <e32base.h>
#include <wlanmgmtcommon.h>

class CVccSignalLevelHandler;

/**
 *  Defines the API for the signal level observer
 *
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib vcchotrigger.dll
 *  @since S60 v3.2
 */
class MVccSignalLevelObserver
    {
public:

    /** Signal RSS class */
    enum TSignalStrengthClass
        {
        /** Received signal level is 'normal'. */
        ESignalClassNormal,
        /** Received signal level is 'weak'. */
        ESignalClassWeak,
        /** Signal strength is not known yet, no handover. */
        ESignalClassUndefined
        };

    /**
     * WLAN signal level has been changed.
     *
     * @since S60 3.2
     * @param aRSS level in absolute dBm values.
     * @param aRssClass specifies the current class of the received signal.
     * @return ?description
     */
    virtual void WlanSignalChanged(
            TInt32 aSignalStrength,
            TSignalStrengthClass aClass ) =  0;

    /**
     * GSM signal level has been changed.
     *
     * @since S60 3.2
     * @param aRSS level in absolute dBm values.
     * @param aRssClass specifies the current class of the received signal.
     */
    virtual void GsmSignalChanged(
            TInt32 aSignalStrength,
            TSignalStrengthClass aClass ) =  0;

    };

#endif // VCCSIGNALLEVELOBSERVER_H
