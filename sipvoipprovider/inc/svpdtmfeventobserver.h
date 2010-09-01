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
* Description:  Event observer for CSVPDTMFEventGenerator
*
*/


#ifndef MSVPDTMFEVENTOBSERVER_H
#define MSVPDTMFEVENTOBSERVER_H

/**
 * Enumeration for describing different SVP DTMF events
 */
enum TSVPDtmfEvent
    {
    ESvpDtmfSendStarted,
    ESvpDtmfSendStopped,
    ESvpDtmfSendCompleted
    };

/**
 *  Event observer for CSVPDTMFEventGenerator
 *
 *  @lib svp.dll
 *  @since S60 3.2
 */
class MSVPDTMFEventObserver
    {
    
public:
    
    /**
     * Called when a DTMF event needs to be sent CCE
     * @since S60 v3.2
     * @retun void
     */
    virtual void InbandDtmfEventOccurred( TSVPDtmfEvent aEvent ) = 0;
				
    };

#endif // SVPDTMFEVENTOBSERVER_H