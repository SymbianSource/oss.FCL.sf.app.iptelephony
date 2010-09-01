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
* Description:  MWakeUpEventObserver defines interface that is used 
*                when reporting wake-up events.
*
*/


#ifndef MCCHWAKEUPEVENTOBSERVER_H
#define MCCHWAKEUPEVENTOBSERVER_H

// INCLUDES

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  MCchWakeUpEventObserver
*  
*  @lib   cchserver
*  @since 3.2
*/

class MCchWakeUpEventObserver
{
public:

    /**
    * Called when some requested event has occured
    */         
    virtual void WakeUp( ) = 0;

private:

    
};

#endif // MCCHWAKEUPEVENTOBSERVER_H

// End of File
