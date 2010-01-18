/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#ifndef M_MSCPPROPERTYOBSERVER_H
#define M_MSCPPROPERTYOBSERVER_H

/**
* MScpPropertyObserver
*/
class MScpPropertyObserver 
    {
public:
 
    /**
     * Reports about changed property
     * @param aPropertyValue Value of the property 
     */
    virtual void HandlePropertyChanged( TInt aPropertyValue ) = 0; 

    };

#endif      // C_MSCPPROPERTYOBSERVER_H   
            
// End of File
