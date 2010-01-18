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
* Description:  Observers changes in volume levels, used by SVPVolumeObserver
*
*/


#ifndef M_SVPVOLUMEUPDATEOBSERVER_H
#define M_SVPVOLUMEUPDATEOBSERVER_H

/**
 *  Observers volume level changes 
 *
 *  @lib svp.dll
 *  @since S60 3.2
 */
class MSVPVolumeUpdateObserver
    {

public:

    /**
     * CSVPVolumeObserver calls this when volume level changes
     * changed. 
     *
     * @since S60 3.2
     * @param aVolume New volume level
     */
    virtual void VolumeChanged( TInt aVolume ) = 0;
    
    };

#endif // M_SVPVOLUMEUPDATEOBSERVER_H
