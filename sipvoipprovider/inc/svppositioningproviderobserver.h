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
* Description:  Observes positioning provider
*
*/


#ifndef M_SVPPOSITIONINGPROVIDEROBSERVER_H
#define M_SVPPOSITIONINGPROVIDEROBSERVER_H


/**
 *  Observes positioning provider
 *
 *  Observes positioning provider whether position information has been 
 *  acquired or an error has occurred
 *
 *  @lib svp.dll
 *  @since S60 3.2
 */
class MSVPPositioningProviderObserver
    {

public:

    /**
     * Notifies observer that positioning request has completed successfully
     *
     * @since S60 3.2
     * @param aPosition Acquired position information
     */
    virtual void PositioningRequestComplete( const TDesC8& aPosition ) = 0;

    /**
     * Notifies observer that positioning request has failed
     *
     * @since S60 3.2
     * @param aError Symbian error code
     */
    virtual void PositioningErrorOccurred( TInt aError ) = 0;

    };


#endif // M_SVPPOSITIONINGPROVIDEROBSERVER_H
