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
* Description:  Interface to SIP profiles.
*
*/


#ifndef WPPROTOCOLPROFILEHANDLERSIP_H
#define WPPROTOCOLPROFILEHANDLERSIP_H

//  INCLUDES
#include    <e32base.h>
#include    <sipobserver.h>
#include    <sipprofileregistryobserver.h>

// FORWARD DECLARATIONS
class CSIP;
class CSIPProfile;
class CSIPProfileRegistry;
class CSIPManagedProfile;

// CLASS DECLARATION

/**
*  Interface to SIP profiles.
*
*  @lib ProvisioningFile.lib
*  @since Series 60 3.0
*/
NONSHARABLE_CLASS(CWPProtocolProfileHandlerSIP) : public CBase,
                                                  public MSIPObserver,
                                                  public MSIPProfileRegistryObserver
    {
    
#ifdef PROVISIONINGFILE_TEST
    
    friend class UT_ProvisioningFile;
    friend class UT_ProtocolProvider;
    
#endif
    
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CWPProtocolProfileHandlerSIP* NewL();

        /**
        * Two-phased constructor.
        */
        static CWPProtocolProfileHandlerSIP* NewLC();
        
        /**
        * Destructor.
        */
        virtual ~CWPProtocolProfileHandlerSIP();

    private: 
                              
        /**
        * Find SIP profile from array by profile id.
        * @since Series 60 3.0
        * @param aProfileId Profile id to search for
        * @param aIndex     On completion, contains index of profile.
        * @return Pointer to SIP profile
        */
        CSIPProfile* FindSIPProfileByIdL( TUint32 aProfileId,
                                          TInt& aIndex );                              
                              
    public:
        
        /**
        * Set Protocol Profile use this IAP.
        * @since Series 60 3.0
        * @param aProfileId  Protocol Profiles array place.
        * @param aProfileIapId  Profiles new IAP Id.
        */                                     
        void SetProfilesIapL( TInt aProfileId, 
                              TUint32 aProfileIapId );
                              
        /**
        * Set provisioning Protocol Profiles Id in array.
        * @since Series 60 3.0
        */                                     
        void SetProvisioningIdL();
        
        /**
        * Get provisioned Protocol Profiles count.
        * @since Series 60 3.0
        * @return TInt count of provisioned profiles.
        */                                     
        TInt ProvisionedCount();
                              
    public: // Functions from base classes
        
        /**
        * From MSIPObserver
        * A SIP request has been received from the network.        
        * @pre aTransaction != 0
        * @param aIapId The IapId from which the SIP request was received.
        * @param aTransaction contains local address, remote address of a SIP
        *        message, as well as optional SIP message method, headers and
        *        body. The ownership is transferred.
        */
        void IncomingRequest( TUint32 aIapId,
                              CSIPServerTransaction* aTransaction );

        /**
        * From MSIPObserver
        * The received SIP request time-outed and it is invalid i.e. cannot be used
        * anymore.
        * This will be called if the user fails to create a SIP connection
        * and does not send an appropriate SIP response.
        * @param aTransaction The time-outed transaction.
        */
        void TimedOut( CSIPServerTransaction& aTransaction );
        
        /** 
        * From MSIPProfileRegistryObserver
        * An event related to SIP Profile has accorred
        * @param aProfileId a profile Id
        * @param aEvent an occurred event
        **/
        void ProfileRegistryEventOccurred( TUint32 aProfileId,
                                           MSIPProfileRegistryObserver::TEvent aEvent );

        /**
        * From MSIPProfileRegistryObserver
        * An asynchronous error has occurred related to SIP profile
        * Event is send to those observers, who have the
        * corresponding profile instantiated.
        * @param aProfileId the id of failed profile 
        * @param aError an occurred error
        */
        void ProfileRegistryErrorOccurred( TUint32 aProfileId,
                                           TInt aError );
                                           
    private:

        /**
        * C++ default constructor.
        */
        CWPProtocolProfileHandlerSIP();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data

        // SIP objects.
        CSIP*                       iSIP;
        
        CSIPProfileRegistry*        iSIPProfileRegistry;

        // Array of SIP profile instances
        RPointerArray<CSIPProfile>  iSIPProfiles;
        
        // Array of SIP profile ids
        RArray<TInt32>              iSIPProfileIds;

    };

#endif // WPPROTOCOLPROFILEHANDLERSIP_H   
            
// End of File
