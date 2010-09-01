/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MCCHCommDbWatcherObserver class definition. A virtual class.
*
*/


#ifndef MCCHCOMMDBWATCHEROBSERVER_H
#define MCCHCOMMDBWATCHEROBSERVER_H

// CLASS DECLARATION
/**
*  MCCHCommDbWatcherObserver
*  
*  @lib   cchserver
*  @since 3.2
*/

class MCCHCommDbWatcherObserver
	{
public:
    /**
    * Called when new WLAN IAP(s) are added to comms db
    * @param aSnapID ID of the SNAP where IAP count increased
    * @since S60 3.2
    */
	virtual void HandleWLANIapAdded( TInt aSnapID ) = 0;		
	};


#endif // MCCHCOMMDBWATCHEROBSERVER_H

// End of File
