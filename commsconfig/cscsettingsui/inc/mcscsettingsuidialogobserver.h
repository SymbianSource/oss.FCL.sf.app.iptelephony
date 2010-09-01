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
*              : completed dialog events.
*
*/


#ifndef M_CSCSETTINGSUIDIALOGOBSERVER_H
#define M_CSCSETTINGSUIDIALOGOBSERVER_H

/**
 *  MCSCSettingsUiDialogObserver class
 *  
 *  Declarition of MCSCSettingsUiDialogObserver.
 *
 *  @lib CSCSettingsUi.lib
 *  @since S60 v3.2
 */
class MCSCSettingsUiDialogObserver
    {
    public:
        
        /**
         * Called when dialog is dismissed from screen.
         *
         * @since S60 v3.2
         * @param aError for error id
         */
        virtual void DialogDismissed( TInt aError ) = 0;
         
    };
    
#endif // M_CSCSETTINGSUIDIALOGOBSERVER_H    
