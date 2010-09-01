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
* Description:  Provides observer methods for SVP
*
*/



#ifndef SVPSESSIONOBSERVER_H
#define SVPSESSIONOBSERVER_H


class CSVPSessionBase;
class CMceInSession;


/**
*  An interface to be implemented by users of MCE if they wish to receive
*  information about SVP sessions: Used mainly for session cleanup procedures
*
*
*  @lib svp.dll
*/
class MSVPSessionObserver
    {

public: // New functions
        
        /**
        * Removes session from array.
        * @since Series60 3.2
        * @param aSession
        * @return None.
        */
        virtual void RemoveFromArray( CSVPSessionBase& aSession ) = 0;
        
        /**
        * Terminates session when Mt session creation has failed in early
        * stages of session creation and CCE does not have information
        * about incoming session
        * @since Series60 3.2
        * @param aSession Session to be terminated.
        * @return None.
        */
        virtual void TerminateSession( CMceInSession& aSession ) = 0;				
    };

#endif // SVPSESSIONOBSERVER_H
