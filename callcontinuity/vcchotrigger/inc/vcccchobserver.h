/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Definition of a CCH observer API
*
*/



#ifndef VCCCCHOBSERVER_H
#define VCCCCHOBSERVER_H

#include <e32base.h>

/**
 *  Observer class for CVccCchMonitor
 *
 *  The class that defines observer interfaces.
 *  Used to get notifications when CCH states changes.
 *  @code
 *   class CMyClass : public CBase, public MVccCchObserver
 *       {
 *   public:
 *       ...
 *       void CchServiceStatusChanged( TServiceStatus aStatus );
 *       ...
 *       }
 *
 *   void MyClass::CchServiceStatusChanged( TServiceStatus aStatus )
 *       {
 *       if ( aStatus == MVccCchObserver::EServiceUnavailable )
 *           {
 *           ServiceNotWorking();
 *           }
 *       else
 *           {
 *           ServiceWorking();
 *           }
 *       }
 *
 *  @endcode
 *
 *  @lib vcchotrigger.lib
 *  @since S60 v3.2
 */
class MVccCchObserver
    {
public:

    /**  Service status */
    enum TServiceStatus
        {
        EServiceUnavailable = 0,
        EServiceAvailable
        };

    /**
     * Observer interface. Called when the CCH status has been changed.
     *
     * @since S60 v3.2
     * @param aStatus Status of the service availability.
     */

    virtual void CchServiceStatusChanged( TServiceStatus aStatus ) = 0;

    };

#endif // VCCCCHOBSERVER_H
