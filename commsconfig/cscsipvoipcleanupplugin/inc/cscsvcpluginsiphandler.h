/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#ifndef C_CSCSVCPLUGINSIPHANDLER_H
#define C_CSCSVCPLUGINSIPHANDLER_H


#include <e32base.h>
#include <sipprofileregistryobserver.h>

class CSIPManagedProfile;
class CSIPManagedProfileRegistry;


/**
 *  CCSCSvcPluginSipHandler class
 *
 *  @lib
 *  @since S60 v3.2
 */
class CCSCSvcPluginSipHandler : public CBase,
                                public MSIPProfileRegistryObserver
    {
    public:

        static CCSCSvcPluginSipHandler* NewL();
        
        /**
         * Destructor.
         */
        ~CCSCSvcPluginSipHandler();

                    
        /**
         * Sets SIP setting registration mode.
         * Function leaves on failure.
         *
         * @since S60 v3.2
         * @param aId for SIP profile id
         * @param aRegMode SIP registration mode
         */
        void SetRegistrationModeL( TUint32 aId, TBool aRegMode );

        
        /**
         * Gets SIP profile access point Id.
         * Function leaves on failure.
         *
         * @since S60 v3.2
         * @param aId for SIP profile id
         * @param aApId Access point Id
         */
        void GetApIdL( TUint32 aId,  TUint32& aApId );

        /**
         * Gets SIP profile user's address-of-record.
         * Function leaves on failure.
         *
         * @since S60 v3.2
         * @param aId for SIP profile id
         * @param aUserAor user's address-of-record
         */
        void GetUserAorL( TUint32 aId,  TDes8& aUserAor );
        
        /**
         * Destroys SIP profile from SIP Stack based on SIP profile id.
         * Function leaves on failure.
         *
         * @since S60 v3.2
         * @param aProfileId for SIP profile id to be deleted
         */
        void RemoveProfileL( TUint32 aProfileId );


        // from base class MSIPProfileRegistryObserver
    
        /** 
         * From MSIPProfileRegistryObserver 
         * SIP profile information event.
         *
         * @since S60 v3.0
         * @param aProfileId is id for profile
         * @param aEvent type of information event
         */
        void ProfileRegistryEventOccurred(
            TUint32 aSIPProfileId, 
            TEvent aEvent );

        
        /**
         * From MSIPProfileRegistryObserver
         * An asynchronous error has occurred related to SIP profile.
         *
         * @since S60 v3.0
         * @param aSIPProfileId the id of failed profile 
         * @param aError a error code
         */
        void ProfileRegistryErrorOccurred(
            TUint32 aSIPProfileId,
            TInt aError );
  

    private:
         
        CCSCSvcPluginSipHandler();

        void ConstructL();

    private: // data
    
        /*
         * Handle to SIP managed profile registry.
         * Own.
         */
        CSIPManagedProfileRegistry* iSipProfileRegistry;
    };

#endif  // C_CSCSVCPLUGINSIPHANDLER_H


