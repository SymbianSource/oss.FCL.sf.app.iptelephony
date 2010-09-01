/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Observer interface to be implemented by SVP controller.
*
*/


#ifndef SVPHOLDOBSERVER_H
#define SVPHOLDOBSERVER_H

/**
*  SVP Hold observer, implemented by SVP controller
*  @lib svp.dll
*  @since Series 60 3.2
*/
class MSVPHoldObserver
    {
public: // New functions

    /**
    * Notifies, that session has been holded.
    * @since Series 60 3.2
    */
    virtual void SessionLocallyHeld() = 0;

    /**
    * Notifies, that session has been resumed.
    * @since Series 60 3.2
    */
    virtual void SessionLocallyResumed() = 0;
    
    /**
    * Notifies, that session has been remotely held.
    * @since Series 60 3.2
    */
    virtual void SessionRemoteHeld() = 0;

    /**
    * Notifies, that session has been remotely resumed.
    * @since Series 60 3.2
    */
    virtual void SessionRemoteResumed() = 0;
    
    /**
    * Notifies, that hold request failed.
    * @since Series 60 3.2
    */
    virtual void HoldRequestFailed() = 0;

    /**
    * Notifies, that resume request failed.
    * @since Series 60 3.2
    */
    virtual void ResumeRequestFailed() = 0;

    };

#endif      // SVPHOLDOBSERVER_H   
            
