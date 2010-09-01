/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements MSIPProfileRegistryObserver interface.
*
*/


#ifndef CSIPPROFILEREGISTRYOBSERVER_H
#define CSIPPROFILEREGISTRYOBSERVER_H

//  INCLUDES
#include <sipprofileregistryobserver.h>

// CLASS DECLARATION
/**
 * Class implements MSIPProfileRegistryObserver interface. 
 * Observs changes in the SIP profile registry.
 *
 * @lib nsmldmvopipadapter.lib
 * @since S60 3.0
 */
class CSIPProfileRegistryObserver : 
    public CBase, public MSIPProfileRegistryObserver
    {
    #ifdef _DEBUG
    /**
     * Friend class for unit testing.
     */
    friend class UT_CNSmlDmVoIPAdapter; 
    #endif


    public: // Constructors and destructor.

        static CSIPProfileRegistryObserver* NewLC();
        static CSIPProfileRegistryObserver* NewL();
        virtual ~CSIPProfileRegistryObserver();

    public: // Methods from base classes
    
		/** 
         * An event related to SIP Profile has occurred
         *
         * @param aProfileId a profile Id
         * @param aEvent an occurred event
         **/
        void ProfileRegistryEventOccurred( 
            TUint32 aProfileId, TEvent aEvent );

        /**
         * An asynchronous error has occurred related to SIP profile
         * Event is send to those observers, who have the
         * corresponding profile instantiated.
         *
         * @param aProfileId the id of failed profile 
         * @param aError an occurred error
         */
        void ProfileRegistryErrorOccurred( TUint32 aProfileId, TInt aError );

  private:
        CSIPProfileRegistryObserver(); 
        void ConstructL();
    };

#endif // CSIPPROFILEREGISTRYOBSERVER_H

// End of file.