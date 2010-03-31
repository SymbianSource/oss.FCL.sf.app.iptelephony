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
* Description:  Device Management VoIP Settings Adapter
*
*/


#ifndef __NSMLDMVOIPADAPTER_H__
#define __NSMLDMVOIPADAPTER_H__

// INCLUDES
#include <e32base.h>
#include <smldmadapter.h>

// CONSTANTS
const TUint KMaxTerminalTypeLength = 64;
const TUint KWlanMacAddressLength  = 50;
const TUint KPhoneModelLength      = 20;
const TInt KMaxUriLength           = 64;
const TInt KMaxDebugPrintLength    = 256;

// CLASS DECLARATION
class CRCSEProfileRegistry;
class CRCSEProfileEntry;
class CRCSEAudioCodecRegistry;
class CRCSEAudioCodecEntry;
class MSIPProfileRegistryObserver;
class CSIPProfileRegistryObserver;
class CSIPManagedProfileRegistry;
class CSIPManagedProfile;
class CCoeEnv;
class CDMVoIPSPSettings;

/**
 *  CNSmlDmVoIPAdapter
 *  ?other_description_lines
 *
 *  @lib ?library
 *  @since Series 60_3.0
 */
class CNSmlDmVoIPAdapter : public CSmlDmAdapter
    {
    #ifdef _DEBUG
    /**
     * Friend class for unit testing.
     */
    friend class UT_CNSmlDmVoIPAdapter;
    #endif

    public:

        static CNSmlDmVoIPAdapter* NewL( MSmlDmCallback* aDmCallback );

        virtual ~CNSmlDmVoIPAdapter();

    /**  For setting telephony preference */
	enum TTelephonyPreference
			{
            ECSPreferred,
            EPSPreferred
			};

    private:

// From base class NSmlDmAdapter

        /**
         * From CNsmlDmAdapter
         * @since Series 60_3.0
         * @param aVersion for returning DDF version of adapter
         */
        void DDFVersionL( CBufBase& aDDFVersion );

        /**
         * From CNsmlDmAdapter
         * @since Series 60_3.0
         * @param aDDF for returning DDF structure of adapter
         */
        void DDFStructureL( MSmlDmDDFObject& aDDF );

        /**
         * From CNsmlDmAdapter
         * @since Series 60_3.0
         * @param aURI URI from dm-module
         * @param aParentLUID luid mapping from dm-module
         * @param aType object type from dm-module
         * @param aStatusRef status from dm-module
         */                            
        void UpdateLeafObjectL( 
            const TDesC8& aURI, 
            const TDesC8& aLUID, 
            const TDesC8& aObject,
            const TDesC8& /*aType*/, 
            const TInt aStatusRef );

        /**
         * From CNsmlDmAdapter
         * @since Series 60_3.0
         * @param aURI URI from dm-module
         * @param aParentLUID luid mapping from dm-module
         * @param aType object type from dm-module
         * @param aStatusRef status from dm-module
         */                            
        void UpdateLeafObjectL( 
            const TDesC8& /*aURI*/, 
            const TDesC8& /*aLUID*/, 
            RWriteStream*& /*aStream*/,
            const TDesC8& /*aType*/, 
            TInt aStatusRef );

        /**
         * From CNsmlDmAdapter
         * @since Series 60_3.0
         * @param aURI URI from dm-module
         * @param aLUID luid mapping from dm-module
         * @param aStatusRef status from dm-module
         */        
        void DeleteObjectL( 
            const TDesC8& aURI, 
            const TDesC8& aLUID, 
            const TInt aStatusRef );

        /**
         * From CNsmlDmAdapter
         * @since Series 60_3.0
         * @param aURI URI from dm-module
         * @param aLUID luid mapping from dm-module
         * @param aType object type from dm-module
         * @param aResultsRef results reference from dm-module
         * @param aStatusRef status reference from dm-module
         */        
        void FetchLeafObjectL( 
            const TDesC8& aURI, 
            const TDesC8& aLUID, 
            const TDesC8& aType, 
            const TInt aResultsRef, 
            const TInt aStatusRef );

        /**
         * From CNsmlDmAdapter
         * @since Series 60_3.0
         * @param aURI URI from dm-module
         * @param aLUID luid mapping from dm-module
         * @param CArrayFix previousURISegmentList from dm-module
         * @param aResultsRef results reference from dm-module
         * @param aStatusRef status reference from dm-module
         */
        void ChildURIListL( 
            const TDesC8& aURI, 
            const TDesC8& aLUID, 
            const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, 
            const TInt aResultsRef, 
            const TInt aStatusRef );

        /**
         * From CNsmlDmAdapter
         * @since Series 60_3.0
         * @param aURI URI from dm-module
         * @param aParentLUID luid mapping from dm-module
         * @param aStatusRef status from dm-module
         */        
        void AddNodeObjectL( 
            const TDesC8& aURI, 
            const TDesC8& aParentLUID, 
            const TInt aStatusRef );

        /**
         * From CNsmlDmAdapter
         * @since Series 60_3.0
         * @param ?arg1 ?description
         * 
         */
        void ExecuteCommandL( 
            const TDesC8& aURI, 
            const TDesC8& aLUID, 
            const TDesC8& aArgument, 
            const TDesC8& aType, 
            const TInt aStatusRef );

        /**
         * From CNsmlDmAdapter
         * @since Series 60_3.0
         * @param ?arg1 ?description
         * 
         */
        void ExecuteCommandL( 
            const TDesC8& aURI, 
            const TDesC8& aLUID, 
            RWriteStream*& aStream, 
            const TDesC8& aType, 
            const TInt aStatusRef );

        /**
         * From CNsmlDmAdapter
         * @since Series 60_3.0
         * @param ?arg1 ?description
         * 
         */   
        void CopyCommandL( 
            const TDesC8& aTargetURI, 
            const TDesC8& aTargetLUID, 
            const TDesC8& aSourceURI, 
            const TDesC8& aSourceLUID, 
            const TDesC8& aType, 
            TInt aStatusRef );

        /**
         * From CNsmlDmAdapter
         * @since Series 60_3.0
         * @param ?arg1 ?description
         * 
         */
        void FetchLeafObjectSizeL( const TDesC8& aURI, 
            const TDesC8& aLUID, 
            const TDesC8& aType, 
            TInt aResultsRef, 
            TInt aStatusRef );

        /**
         * From CNsmlDmAdapter
         * @since Series 60_3.0
         * @param ?arg1 ?description
         * 
         */
        void StartAtomicL();

        /**
         * From CNsmlDmAdapter
         * @since Series 60_3.0
         * @param ?arg1 ?description
         * 
         */
        void CommitAtomicL();

        /**
         * From CNsmlDmAdapter
         * @since Series 60_3.0
         * @param ?arg1 ?description
         * 
         */
        void RollbackAtomicL();

        /**
         * From CNsmlDmAdapter
         * @since Series 60_3.0
         * @param ?arg1 ?description
         * 
         */
        TBool StreamingSupport( TInt& aItemSize );

        /**
         * From CNsmlDmAdapter
         * @since Series 60_3.0
         * @param ?arg1 ?description
         * 
         */
        void StreamCommittedL();

        /**
         * From CNsmlDmAdapter
         * @since Series 60_3.0
         * @param ?arg1 ?description
         * 
         */
        void CompleteOutstandingCmdsL();

    private: 

        CNSmlDmVoIPAdapter( TAny* aEcomArguments );

        void ConstructL();

        /**
         * Fetches object based on URI and LUID.
         * @since Series 60_3.0
         * @param aURI URI of the object.
         * @param aLUID LUID of the object.
         * @param aObject The object.
         * @return DM error status.
         */
        CSmlDmAdapter::TError FetchObjectL( 
            const TDesC8& aURI,
            const TDesC8& aLUID, 
            CBufBase& aObject );

        /**
         * Loads SIP profile information
         * @since Series 60_3.0
         */        
        void LoadProfilesL();

        /**
         * Converts 8 bit descriptor to integer.
         * @since Series 60_3.0
         * @param aDes Descriptor to convert
         * @return integer
         */        
        TInt DesToInt( const TDesC8& aDes ) const;

        /**
         * Converts descriptor to unsigned integer
         * @since Series 60_3.0
         * @param aDes Descriptor to convert
         * @return unsigned integer
         */        
        TUint DesToTUint( const TDesC8& aDes ) const;

        /**
         * Removes last uri segment from aURI
         * @since Series 60_3.0
         * @param aDes Descriptor to handle
         * @return pointer to uri
         */        
        const TPtrC8 RemoveLastURISeg( const TDesC8& aURI ) const;

        /**
         * Gets number of uri segments.
         * @since Series 60_3.0
         * @param aURI Descriptor to handle
         * @return pointer to uri
         */
        TInt NumOfURISegs( const TDesC8& aURI ) const;

        /**
         * Gets last uri segment.
         * @since Series 60_3.0
         * @param aURI Descriptor to handle
         * @return integer
         */
        const TPtrC8 LastURISeg( const TDesC8& aURI ) const;

        /**
         * Removes ./ from uri
         * @since Series 60_3.0
         * @param aURI Descriptor to handle
         * @return pointer to uri
         */
        const TPtrC8 RemoveDotSlash( const TDesC8& aURI ) const;

        /**
         * Get Sip URI reference from SIP adapter
         * @since Series 60_3.0
         * @param aObject Object to insert found URI.
         * @param aID SIP profile ID to get URI
         * @return errorcode to tell wether operation was successful
         */        
        MSmlDmAdapter::TError GetSipIdL( CBufBase& aObject, 
            TUint32 aID ) const;

        /**
         * Set SipRef value according to aObject
         * Converts aObject to TUint 
         * @since Series 60_3.0
         * @param aObject SIP reference to change
         * @return SIP profile ID as TUint
         */
        TUint32 SetSipRefL( const TDesC8& aObject ) const;

        /**
         * Get Sccp URI reference from SIP adapter
         * @since Series 60_3.0
         * @param aObject Object to insert found URI.
         * @param aID SIP profile ID to get URI
         * @return errorcode to tell wether operation was successful
         */        
        MSmlDmAdapter::TError GetSccpIdL( CBufBase& aObject, 
            TUint32 aID ) const;

        /**
         * Set SccpRef value according to aObject
         * Converts aObject to TUint 
         * @since Series 60_3.0
         * @param aObject SIP reference to change
         * @return SIP profile ID as TUint
          */
        TUint SetSccpRefL( const TDesC8& aObject ) const;

        /**
         * Check for duplicate settingsName and rename if same
         * @since Series 60_3.0
         * @param aProfile Profile to check.
         */
        TBool CheckDuplicateNameL( CRCSEProfileEntry& aEntry );

        /**
         * Check for duplicate providerName and rename if same
         * @since Series 60_3.0
         * @param aProfile Profile to check.
         */
        TBool CheckDuplicateProviderL( CRCSEProfileEntry& aEntry );

        /**
         * Finds profile location in profile array.
         * @since Series 60_3.0
         * @param aURI Descriptor to look for
         * @return Profile location TInt
         */
        TInt FindProfileLocation( const TDesC8& aURI ) const;

        /**
         * Finds codec location in codec array.
         * @since Series 60_3.0
         * @param aID CodecId
         * @return Profile location TInt
         */
        TInt FindCodecLocation( TUint32 aID ) const;

	    /**
	     * Sets CS/PS telephony preference
	     * @since Series 60_3.0
	     * @param aTelephonyPreference state to set
	     */
	    void SetTelephonyPreferenceL( const TTelephonyPreference& 
	        aTelephonyPreference );

        /**
         * Get Terminal Type for SIP User Agent Header.
         * @since S60 v3.0
         * @param aTeminalType Terminal type.
         */
        void GetTerminalTypeL( TBuf<KMaxTerminalTypeLength>& aTerminalType ) 
            const;

        /**
         * Get WLAN MAC address for SIP User Agent Header.
         * @since S60 v3.0
         * @param aMac WLAN MAC address.
         */
        void GetWlanMacAddressL( TBuf<KWlanMacAddressLength>& aMac ) const;

        /**
         * Get NAT/FW Domain specific URI according to given profile ID.
         * @since S60 3.2
         * @param aObject URI to be set.
         * @param aProfileId Profile ID from which the URI is to be found.
         * @return DM error status.
         */
        MSmlDmAdapter::TError GetNatFwUriL( TBuf16<KMaxUriLength>& aObject, 
            TInt aProfileId ) const;

        /**
         * Get NAT/FW Domain specific ID according to given URI.
         * @since S60 3.2
         * @param aObject URI with which to resolve the ID.
         * @return NAT/FW Domain specific ID.
         */
        TUint32 NatFwIdL( const TDesC8& aObject ) const;

        /**
         * Get Access Point ID (connection reference) according to aObject.
         * @since S60 3.2
         * @param aObject URI to look from AP adapter.
         * @return Access point ID number.
         */    
        TInt ConRefL( const TDesC8& aObject ) const;

        /**
         * Get Access Point URI reference from AP adapter.
         * @since S60 3.2
         * @param aObject Object into which to insert found URI.
         * @param aIapID IAP ID to look for.
         * @return DM error status.
         */
        MSmlDmAdapter::TError GetConRefL( CBufBase& aObject, 
            TInt aIapId ) const;

        /**
         * Get Presence profile URI according to given profile ID.
         * @since S60 3.2
         * @param aObject URI to be set.
         * @param aProfileId Profile ID from which the URI is to be found.
         * @return DM error status.
         */
        MSmlDmAdapter::TError GetPresenceUriL( TBuf16<KMaxUriLength>& aObject,
            TUint32 aProfileId ) const;

        /**
         * Get Presence profile ID according to given URI.
         * @since S60 3.2
         * @param aObject URI with which to resolve the ID.
         * @return Presence profile ID.
         */
        TUint32 PresenceIdL( const TDesC8& aObject ) const;
		
        /**
         * Checks if Presence Settings ID is valid.
         * @since S60 3.2
         * @param aSetId Presence Settings ID.
         * @return ETrue if settings exists, EFalse if not.
         */
        TBool IsPresIDValidL( TUint32 aSetId ) const;
        
        /**
         * Get SNAP ID.
         * @since S60 3.2.
         * @param aUri URI from which to find SNAP ID.
         * @return SNAP ID.
         */
        TInt SnapIdL( const TDesC8& aUri );

        /**
         * Set SNAP URI.
         * @since S60 3.2.
         * @param aObject Object in which found URI is set.
         * @param aSnapId SNAP ID from which to find URI.
         * @return ETrue if successful, EFalse if not.
         */
        MSmlDmAdapter::TError GetSnapUriL( TDes8& aObject, TInt aSnapId );

        /**
         * Updates Codec specific object.
         * @since S60 3.2
         * @param aLUID luid mapping from dm-module
         * @param aUriSeg Last URI segment.
         * @param aObject Data of the object.
         * @return Value indicating if object was found.
         */
        CSmlDmAdapter::TError UpdateCodecSpecificObject( 
            const TDesC8& aLUID, const TDesC8& aUriSeg, 
            const TDesC8& aObject );

        /**
         * Fetches the values of Codec specific leaf objects.
         * @since S60 3.2
         * @param aLUID luid mapping from dm-module
         * @param aUriSeg Last URI segment.
         * @param aSegmentResult Storage for fetched objects.
         * @return Value indicating if object was found.
         */
        CSmlDmAdapter::TError FetchCodecObject( const TDesC8& aLUID,
            const TDesC8& aUriSeg, TDes8& aSegmentResult );

        /**
         * Inserts Codec specific leaf objects.
         * @since S60 3.2
         * @param aCurrentURISegmentList List for leaf objects.
         */
        void CodecChildListL( CBufBase* const aCurrentURISegmentList );
            
        /**
         * Updates Setting Ids specific object.
         * @since S60 3.2
         * @param aLUID luid mapping from dm-module
         * @param aUriSeg Last URI segment.
         * @param aObject Data of the object.
         * @return Value indicating if object was found.
         */
        CSmlDmAdapter::TError UpdateSettingIdsSpecificObjectL( 
            const TDesC8& aLUID, const TDesC8& aUriSeg, 
            const TDesC8& aObject );
            
        /**
         * Inserts Voip leaf objects.
         * @since S60 3.2
         * @param aCurrentURISegmentList List for leaf objects.
         */
        void VoipChildListL( CBufBase* const aCurrentURISegmentList );

    private:    // Data

        /**
         * RCSE profile registry handle.
         * Own.
         */
        CRCSEProfileRegistry* iCRCSEProfile;

        /**
         * RCSE codec registry handle.
         * Own.
         */
        CRCSEAudioCodecRegistry* iCRCSEAudioCodec;

        /**
         * Handle for DM callback interface.
         * Not own.
         */
        MSmlDmCallback* iDmCallback;

        /**
         * For telling status of command to the adapter.
         */
        TInt iStatusRef;

        /**
         * Array of pointers to all profile entries.
         * Own.
         */
        RPointerArray<CRCSEProfileEntry> iProfileEntries;

        /**
         * Array of pointers to all codec entries.
         * Own.
         */
        RPointerArray<CRCSEAudioCodecEntry> iCodecEntries;

        /**
         * For knowing if there are modifications to entries.
         */
        RArray<TInt> iProfileModifs;

        /**
         * For knowing if there are modifications to entries.
         */
        RArray<TInt> iCodecModifs;

        /**
         * Current editable profileID in array of profiles.
         */
        TInt iProfileID;

        /**
         * Array for service provider settings.
         */
        RPointerArray<CDMVoIPSPSettings> iSPSettings;
    };

#endif // __NSMLDMVOIPADAPTER_H__

// End of File.
