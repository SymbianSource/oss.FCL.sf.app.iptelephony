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
* Description:  Observer for SVP timer
*
*/


#ifndef SVPTIMEROBSERVER_H
#define SVPTIMEROBSERVER_H


/**
 *  SVP timer observer
 *
 *  Observer for SVP timer
 *
 *  @lib 
 *  @since S60 3.2
 */
class MSVPTimerObserver
    {

public: // New functions
        
    /**
    * Notifies, that the previously set timer has now expired.
    * Implemented by the class using the timer.
    * @since Series 60 3.2
    * @param TInt aTimerId The ID of the timer. Observer can identify 
    * timers from each other with this ID.
    * @return 
    */
    virtual void TimedOut( TInt aTimerId ) = 0;       
    				
  		
    };

#endif // SVPTIMEROBSERVER_H
