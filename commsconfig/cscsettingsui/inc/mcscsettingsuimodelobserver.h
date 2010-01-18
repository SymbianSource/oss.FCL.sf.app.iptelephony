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
* Description:  Observer interface for notifying CSCSettingsUi for 
*              : completed service events from model.
*
*/


#ifndef M_CSCSETTINGSUIMODELOBSERVER_H
#define M_CSCSETTINGSUIMODELOBSERVER_H

#include "cscengcchhandler.h"

/**
 *  MCSCSettingsUiModelObserver class
 *  
 *  Declarition of MCSCSettingsUiModelObserver.
 *
 *  @lib CSCSettingsUi.lib
 *  @since S60 v3.2
 */
class MCSCSettingsUiModelObserver
    {
    public:

        /**
         * Called when softkeys need to be changed.
         *
         * @since S60 v3.2
         */
        virtual void UpdateSoftkeys() const = 0;

    };
    
#endif // M_CSCSETTINGSUIMODELOBSERVER_H    
