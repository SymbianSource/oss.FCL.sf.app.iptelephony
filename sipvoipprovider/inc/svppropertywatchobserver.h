/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Observers changes in proverty values
*
*/


#ifndef M_SVPPROPERTYWATCHOBSERVER_H
#define M_SVPPROPERTYWATCHOBSERVER_H

/**
 *  Observer interface for clients being interested in getting notifications
 *  when Publish&Subscribe property under watching changes.
 *
 *  @lib svp.dll
 *  @since S60 3.2
 */
class MSVPPropertyWatchObserver
    {

public:

    /**
     * MSVPPropertyWatch calls this when some property under watching was
     * changed. 
     *
     * @since S60 3.2
     * @param aKey Property which was changed
     * @param aValue New value of the property
     */
     virtual void ValueChangedL( TInt aKey, TInt aValue ) = 0;
    
    /**
     * MccPropertyWatch calls this when property was deleted.
     *
     * @since S60 3.2
     * @param aKey Property which was deleted
     */
     virtual void PropertyDeleted( TInt aKey ) = 0;

    };

#endif // M_SVPPROPERTYWATCHOBSERVER_H
