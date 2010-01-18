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
* Description:  Device Management General VoIP settings adapter
*
*/


#ifndef __NSMLDMGENERALVOIPSETTINGSADAPTER_H__
#define __NSMLDMGENERALVOIPSETTINGSADAPTER_H__

#include <smldmadapter.h>

/**
 *  Cleanup item for deleting contents of RPointerArray.
 *
 */
template <class T>
class CleanupResetAndDestroy
	{
public:
	
	inline static void PushL(T& aRef);
	
private:
	
	static void ResetAndDestroy(TAny *aPtr);
	};
	
template <class T>
inline void CleanupResetAndDestroyL(T& aRef);

/**
 *  Entry point for general VoIP settings.
 *
 *  Holds general VoIP settings transiently before they are stored permanently
 *  in Central Repository.
 *  @since S60 v3.0
 */
class TGenVoIPEntry
    {

    public:

        /**
         * Status reference for this command.
         */
        TInt iStatusRef;

        /**
         * For telling whether general VoIP settings are loaded from
         * Central Repository.
         */
        TInt iAreSettingsLoaded;

        /**
         * CLIR, Calling Line Identification Restriction.
         */
        TInt iClir;

        /**
         * Anonymous Call Block rule. Determines the rule for Anonymous Call
         * Block feature.
         */
        TInt iACBRule;

        /**
         * Preferred telephony. Tells which telephony mode is preferred.
         */
        TInt iPrefTel;

        /**
         * Do Not Disturb (DND) setting.
         */
        TInt iDnd;

        /**
         * Call waiting. Tells whether call waiting is enabled or disabled.
         */
        TInt iPSCallWaiting;

        /**
         * Telephony preference lock. Tells whether preferred telephony mode 
         * is UI editable.
         */
        TInt iPrefTelLock;

        /**
         * Shut down CS radio in WiFi mode. Tells whether CS radio is turned
         * off in WiFi mode.
         */
        TInt iCsRadioWifiOff;
        
        /**
         * VoIP profile name. If preferred telephony mode is set to VoIP,
         * the service id of this VoIP profile is set as preferred service id.
         */
        TBuf8<64> iVoipProfileId;
    };

/**
 *  CNSmlDmGeneralVoIPSettingsAdapter.
 *
 *  Main implementation of Device Management General VoIP Settings Adapter.
 *  @lib cnsmldmgeneralvoipsettingsadapter.lib
 *  @since S60 v3.0
 */
class CNSmlDmGeneralVoIPSettingsAdapter : public CSmlDmAdapter
    {

#ifdef EUNIT_TESTING
    /**
     * Friend class for unit testing.
     */
    friend class UT_CNSmlDmGeneralVoIPSettingsAdapter;
#endif

    public:
        
        static CNSmlDmGeneralVoIPSettingsAdapter* NewL( MSmlDmCallback* aDmCallback );

        static CNSmlDmGeneralVoIPSettingsAdapter* NewLC( MSmlDmCallback* aDmCallback );

        virtual ~CNSmlDmGeneralVoIPSettingsAdapter();

    private:

        CNSmlDmGeneralVoIPSettingsAdapter( TAny* aEcomArguments );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();
        
        /**
         * Fetches leaf object values
         *
         * @since S60 v3.0.
         * @param aURI URI of the object.
         * @param aLUID LUID of the object.
         * @param aObject Data of the object.
         * @return Status code of operation
         */       
        CSmlDmAdapter::TError FetchObjectL( 
            const TDesC8& aURI, 
            CBufBase& aObject );

        /**
         * Loads general VoIP settings from Central Repository.
         *
         * @since S60 v3.0.
         * @param aEntry General VoIP settings entry.
         * @return Error code for the operation.
         */
        TInt LoadSettingsL();

        /**
         * Converts descriptor to int.
         *
         * @since S60 v3.0.
         * @param aDes Descriptor to convert.
         * @return Converted value.
         */
        TInt DesToInt( const TDesC8& aDes );

        /**
         * Removes last URI segment from aURI
         *
         * @since S60 v3.0.
         * @param aDes Descriptor to handle.
         * @return Pointer to URI.
         */        
        TPtrC8 RemoveLastURISeg( const TDesC8& aURI );

        /**
         * Gets number of URI segments.
         *
         * @since S60 v3.0.
         * @param aURI Descriptor to handle.
         * @return Number of URI segments.
         */
        TInt NumOfURISegs( const TDesC8& aURI );

        /**
         * Gets last URI segment.
         *
         * @since S60 v3.0.
         * @param aURI Descriptor to handle.
         * @return Pointer to beginning of last URI segment.
         */
        TPtrC8 LastURISeg( const TDesC8& aURI ) const;

        /**
         * Removes ./ from URI
         *
         * @since S60 v3.0.
         * @param aURI Descriptor to handle.
         * @return Pointer to URI.
         */
        TPtrC8 RemoveDotSlash( const TDesC8& aURI );

        // from base class MSmlDmAdapter

        /**
         * From MSmlDmAdapter.
         *
         * @since S60 v3.0.
         * @param aDDFVersion for returning DDF version of adapter.
         */
        void DDFVersionL( CBufBase& aDDFVersion );

        /**
         * From MSmlDmAdapter.
         *
         * @since S60 v3.0.
         * @param aDDF for returning DDF structure of adapter.
         */
        void DDFStructureL( MSmlDmDDFObject& aDDF );
   
        /**
         * From MSmlDmAdapter.
         *
         * @since S60 v3.0.
         * @param aURI URI of the object.
         * @param aLUID LUID of the object.
         * @param aObject Data of the object.
         * @param aType MIME type of the object.
         * @param aStatusRef Reference to correct command.
         */                                                     
        void UpdateLeafObjectL( 
            const TDesC8& aURI, 
            const TDesC8& aLUID, 
            const TDesC8& aObject,
            const TDesC8& aType, 
            const TInt aStatusRef );
    
        /**
         * From MSmlDmAdapter.
         * Not supported.
         *
         * @param aURI URI of the object.
         * @param aLUID LUID of the object.
         * @param aStream Data of the object.
         * @param aType MIME type of the object.
         * @param aStatusRef Reference to correct command.
         */
        void UpdateLeafObjectL( 
            const TDesC8& aURI, 
            const TDesC8& aLUID, 
            RWriteStream*& aStream,
            const TDesC8& aType, 
            TInt aStatusRef );

        /**
         * From MSmlDmAdapter.
         * Deletes (resets) General VoIP Settings profile.
         *
         * @since S60 v3.0.
         * @param aURI URI of the object.
         * @param aLUID LUID of the object.
         * @param aStatusRef Reference to correct command.
         */        
        void DeleteObjectL( 
            const TDesC8& aURI, 
            const TDesC8& aLUID, 
            const TInt aStatusRef );
    
        /**
         * From MSmlDmAdapter.
         * Fetches the values of leaf objects.
         *
         * @since S60 v3.0.
         * @param aURI URI of the object.
         * @param aLUID LUID of the object.
         * @param aType MIME type of the object.
         * @param aResultsRef Reference to correct results.
         * @param aStatusRef Reference to correct command.
         */                      
        void FetchLeafObjectL( 
            const TDesC8& aURI, 
            const TDesC8& aLUID, 
            const TDesC8& aType, 
            const TInt aResultsRef, 
            const TInt aStatusRef );
    
        /**
         * From MSmlDmAdapter.
         * Fetches the size of a leaf object.
         *
         * @since S60 v3.0.
         * @param aURI URI of the object.
         * @param aLUID LUID of the object.
         * @param aType MIME type of the object.
         * @param aResultsRef Reference to correct results.
         * @param aStatusRef Reference to correct command.
         */                      
        void FetchLeafObjectSizeL( 
            const TDesC8& aURI, 
            const TDesC8& aLUID, 
            const TDesC8& aType, 
            TInt aResultsRef, 
            TInt aStatusRef );

        /**
         * From MSmlDmAdapter.
         * Asks for the list of child objects of the node.
         *
         * @since S60 v3.0.
         * @param aURI URI from DM module.
         * @param aLUID LUID of the parent object.
         * @param aPreviousURISegmentList from DM module.
         * @param aResultsRef results reference from DM module.
         * @param aStatusRef Reference to correct command.
         */
        void ChildURIListL( 
            const TDesC8& aURI, 
            const TDesC8& aLUID, 
            const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, 
            const TInt aResultsRef, 
            const TInt aStatusRef );
    
        /**
         * From MSmlDmAdapter.
         * Not supported (there can only be one general VoIP settings node).
         *
         * @since S60 v3.0.
         * @param aURI URI of the object.
         * @param aParentLUID LUID of the parent object.
         * @param aStatusRef Reference to correct command.
         */        
        void AddNodeObjectL( 
            const TDesC8& aURI, 
            const TDesC8& aParentLUID, 
            const TInt aStatusRef );
    
        /**
         * From MSmlDmAdapter.
         * Not supported.
         *
         * @since S60 v3.0.
         * @param aURI URI from DM module.
         * @param aLUID LUID of the object.
         * @param aArgument Argument for the command.
         * @param aType MIME type of the object.
         * @param aStatusRef Reference to correct command.
         */
        void ExecuteCommandL( 
            const TDesC8& aURI, 
            const TDesC8& aLUID, 
            const TDesC8& aArgument, 
            const TDesC8& aType, 
            const TInt aStatusRef );
    
        /**
         * From MSmlDmAdapter.
         * Not supported.
         *
         * @since S60 v3.0.
         * @param aURI URI from DM module.
         * @param aLUID LUID of the object.
         * @param aStream Argument for the command.
         * @param aType MIME type of the object.
         * @param aStatusRef Reference to correct command.
         */
        void ExecuteCommandL( 
            const TDesC8& aURI, 
            const TDesC8& aLUID, 
            RWriteStream*& aStream, 
            const TDesC8& aType, 
            const TInt aStatusRef );
    
        /**
         * From MSmlDmAdapter.
         * Not supported.
         *
         * @since S60 v3.0.
         * @param aTargetURI Target URI for the command.
         * @param aTargetLUID LUID of the target object.
         * @param aSourceURI Source URI for the command.
         * @param aSourceLUID LUID of the source object.
         * @param aType MIME type of the objects.
         * @param aStatusRef Reference to correct command.
         */
        void CopyCommandL( 
            const TDesC8& aTargetURI, 
            const TDesC8& aTargetLUID, 
            const TDesC8& aSourceURI, 
            const TDesC8& aSourceLUID, 
            const TDesC8& aType, 
            TInt aStatusRef );
    
        /**
         * From MSmlDmAdapter.
         * Not supported.
         *
         * @since S60 v3.0.
         */     
        void StartAtomicL();

        /**
         * From MSmlDmAdapter.
         * Not supported.
         *
         * @since S60 v3.0.
         */
        void CommitAtomicL();

        /**
         * From MSmlDmAdapter.
         * Not supported.
         *
         * @since S60 v3.0.
         */
        void RollbackAtomicL();

        /**
         * From MSmlDmAdapter.
         * Not supported.
         *
         * @since S60 v3.0.
         * @param aItemSize Size limit for stream usage.
         * @return Streaming support.
         */
        TBool StreamingSupport( TInt& aItemSize );

        /**
         * From MSmlDmAdapter.
         * Not supported.
         *
         * @since S60 v3.0.
         */
        void StreamCommittedL();

        /**
         * From MSmlDmAdapter.
         * Saves modified profiles back to permanent store.
         *
         * @since S60 v3.0.
         */
        void CompleteOutstandingCmdsL();

        /**
         * Gets VoIP profile id.
         *
         * @since S60 v3.2.3.
         * @param aObject URI of the object to fetch.
         */
        TUint32 VoIPProfileIdL( const TDesC8& aObject ) const;

        /**
         * Resolves the name of the preferred service and saves it to an
         * internal variable.
         *
         * @since S60 v3.2.3.
         * @param aRepository Reference to a previously opened repository.
         */
        TInt ResolvePreferredServiceNameL( CRepository& aRepository );

    private: // data

        /**
         * For returning data to DM module.
         */
        MSmlDmCallback* iDmCallback;

        /**
         * Profile ID currently being modified.
         */
        TInt iProfileID;

        /**
         * For knowing status of DM module.
         */
        TInt iStatus;

        /**
         * Entry point for general VoIP settings.
         */
        TGenVoIPEntry iEntry;

        /**
         * For returning results to DM module.
         */
        CBufBase* iResults;

    };

#endif // __NSMLDMGENERALVOIPSETTINGSADAPTER_H__
