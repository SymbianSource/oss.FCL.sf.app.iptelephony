/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for MAcpDialogObserver
*
*/


#ifndef MACPDIALOGOBSERVER_H
#define MACPDIALOGOBSERVER_H

/**
 *  MAcpDialogObserver class
 *  Declarition of MAcpDialogObserver.
 *
 *  @lib accountcreationplugin.lib
 *  @since S60 v3.2
 */
class MAcpDialogObserver
    {
    public:
        
        /**
         * Called when dialog is dismissed from screen.
         *
         * @since S60 v3.2
         * @param aError for error id
         */
        virtual void DialogDismissedL( TInt aError ) = 0;
         
    };
    
#endif // MACPDIALOGOBSERVER_H

// End of file.
