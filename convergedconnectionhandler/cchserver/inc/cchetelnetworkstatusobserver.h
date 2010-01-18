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
* Description:  MCchEtelNetworkStatusObserver handles cch's CTelephony 
*              : related observing functionality.
*
*/


#ifndef CCHETELNETWORKSTATUSOBSERVER_H
#define CCHETELNETWORKSTATUSOBSERVER_H

// INCLUDES

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  MCchEtelNetworkStatusObserver
*  
*  @lib   cchserver
*  @since 3.2
*/

class MCchEtelNetworkStatusObserver
{
public:

    /**
    * Called when CS coverage out
    */         
    virtual void MobileNetworkNoService( ) = 0;

private:

    
};

#endif // CCHETELNETWORKSTATUSOBSERVER_H

// End of File
