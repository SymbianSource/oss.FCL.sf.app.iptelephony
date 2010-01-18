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
* Description:  Header file of psy progress notifier observer
*
*/


#ifndef DHCPSYPROGRESSREQUESTERNOTIFYOBSERVER_H_
#define DHCPSYPROGRESSREQUESTERNOTIFYOBSERVER_H_

#include <e32base.h>

class MDhcpPsyRequesterProgressObserver
    {
    public:

        enum TRequestProgressPhases
            {
            EConnectionInitiated,
            ELocationQueryStarted,
            ELocationQueryFinished,
            ECancelled
            };

    public:

        /**
        * Notify progress phase of location query
        *
        * @param aPhase         Current location query phase.
        * @aPhaseValue          Additional information from current phase. (Status value)
        *
        * @since 3.2
        */
        virtual void  ProgressPsyRequestNotificationL (TRequestProgressPhases aPhase,
            TInt aPhaseValue)=0;
    };

#endif // DHCPSYPROGRESSREQUESTERNOTIFYOBSERVER_H_
