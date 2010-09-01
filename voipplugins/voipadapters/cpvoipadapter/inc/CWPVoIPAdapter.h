/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Realizes VoIP provisioning adapter. The VoIP adapter 
*                handles VoIP settings and stores them in 
*                CWPVoIPItem instances.
*
*/


#ifndef CWPVOIPADAPTER_H
#define CWPVOIPADAPTER_H

#include <CWPAdapter.h>
#include <MWPContextExtension.h>

class CWPCharacteristic;
class CWPVoIPItem;
class CWPVoIPCodec;

/**
 *  Realizes VoIP provisioning adapter.
 *  VoIP adapter handles VoIP settings and stores them via RCSE.DLL 
 *  and serviceprovidersettings.dll.
 *
 *  @lib CWPVoIPAdapter.lib
 *  @since Series60 3.0.
 */
class CWPVoIPAdapter :  public CWPAdapter, public MWPContextExtension
    {
    #ifdef _DEBUG
    /**
     * Friend class for unit testing.
     */
     friend class UT_CWPVoIPAdapter;
    #endif

public:  // Constructor and destructor

    /**
     * Two-phased constructor.
     */
    static CWPVoIPAdapter* NewL();

    /**
     * Destructor.
     */
    virtual ~CWPVoIPAdapter();

public:

// from base class CWPAdapter

    /**
     * Gets the saving information from the
     * adapter that has saved settings.
     *
     * @since S60 3.2
     * @param TInt aIndex. Index of the previously saved item.
     * @param aSavingInfo Saving information (APPID, APPREF, settings ID).
     */
    void GetSavingInfoL( TInt aIndex, 
        RPointerArray<HBufC8>& aSavingInfo );

    /**
     * Returns the number of the summary lines to be shown in 
     * BIO control.
     *
     * @since S60 v3.0
     * @return TInt. Quantity of items (SIP profiles).
     */
    TInt ItemCount() const;

    /**
     * Getter for the summary text of the SIP item (SIP profile) 
     * located in the array. Text is shown to the user in 
     * opened configuration message.
     *
     * @since S60 v3.0
     * @param TInt aIndex. Location of the SIP item.
     * @return TDesC16&. Summary title.
     */
    const TDesC16& SummaryTitle( TInt /*aIndex*/ ) const;

    /**
     * Getter for the summary text of the VoIP item (VoIP settings) 
     * located in the array. Text is shown to the user in 
     * opened configuration message.
     *
     * @since S60 v3.0
     * @param TInt aIndex. Location of the SIPVoIP item.
     * @return TDesC16&. Summary text.
     */
    const TDesC16& SummaryText( TInt aIndex ) const;

    /**
     * Saves VoIP item (VoIP settings) located in array.
     *
     * @since S60 v3.0
     * @param TInt aIndex. Location of the VoIP item to be saved.
     */
    void SaveL( TInt aIndex );

    /**
     * Query if the VoIP item on place pointed by parameter can 
     * be set as default.
     *
     * @since S60 v3.0
     * @param TInt aIndex. Not used.
     * @return False since defaultness is not supported.
     */
    TBool CanSetAsDefault( TInt /*aIndex*/ ) const;

    /**
     * Sets the VoIP item on place pointed by parameter as default.
     * 
     * @since S60 v3.0
     * @param TInt aIndex. Location of the VoIP item to be queried.
     */
    void  SetAsDefaultL( TInt aIndex );

    /**
     * Query for the detail information about the VoIP settings. 
     * MWPPairVisitor is used for retrieving the details of a single 
     * setting entry. This is not supported feature as in the other 
     * adapters in the framework.
     *
     * @since S60 v3.0
     * @param TInt aItem.
     * @param MWPPairVisitor aVisitor.
     * @return TInt. 
     */
    TInt DetailsL( TInt /*aItem*/, MWPPairVisitor& /*aVisitor*/ );

    /**
     * Visit method. Visits the adapter. Adapter then checks from the 
     * aCharacteristic if the data is targetet to it and acts according to
     * that.
     *
     * @since S60 v3.0
     * @param CWPCharacteristic& aCharacteristic.
     */
    void VisitL( CWPCharacteristic& aCharacteristic );		

    /**
     * Visit method. Visits the adapter. Methods set the iAppId value.
     *
     * @since S60 v3.0
     * @param CWPParameter& aParameter.
     */
    void VisitL( CWPParameter& aParameter );		

    /**
     * Visit method. For linking VoiceMailBox settings to a certain IAP.
     * @since S60 v3.2
     * @param CWPCharacteristic& aLink.
     */
    void VisitLinkL( CWPCharacteristic& aLink );

    /**
     * Informs the adapters of the saved settings.
     *
     * @since S60 v3.0
     * @param TDesC8& aAppIdOfSavingItem. UID of the adapter component. 
     * @param HBufC8& aAppRef. APPREF of the saved settings.
     * @param TAny& aStorageIdValue. Value that identifies the settings in
     *        its storage.
     * @param TBool aIsLastOfItsType. If the settings saved is the last of
     *        its kind, the value of the parameter is ETrue (or 1, because
     *        the TBool type does not work correctly via interface class 
     *        definition (based on information from MVC documentation).
     */
    void SettingsSavedL (const TDesC8& aAppIdOfSavingItem,
        const TDesC8& aAppRef, const TDesC8& aStorageIdValue );

    /**
     * Lets the adapters know when the saving has been finalized in 
     * viewpoint of the provisioning framework. The adapters can make 
     * finalization of saving the settings after receiving this call.
     *
     * @since S60 v3.0
     */
    void SavingFinalizedL();

    /**
     * Returns a pointer to a context extension.
     * @param aExtension Contains a pointer to MWPContextExtension 
     * if supported
     * @return KErrNotSupported if not supported, KErrNone otherwise
     */
    TInt ContextExtension(MWPContextExtension*& aExtension);

// from base class MWPContextExtension

    /**
     * Returns the data used for saving.
     * @param aIndex The index of the data
     * @return The data. Ownership is transferred.
     */
    const TDesC8& SaveDataL(TInt aIndex) const;

    /**
     * Deletes a saved item.
     * @param aSaveData The data used for saving
     */
    void DeleteL(const TDesC8& aSaveData);

    /**
     * Returns the UID of the adapter.
     * @return UID
     */
    TUint32 Uid() const;

private:

    /**
     * C++ default constructor.
     */
    CWPVoIPAdapter();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private: // New functions

    /**
     * Converts descriptor to integer.
     * @since Series60 3.2
     * @param aDescValue Descriptor data.
     * @param aIntValue Refenced integer value.
     * @return KErrNone if converting is done.
     */
    TInt DescToInt( HBufC8* aDescValue, TInt& aIntValue ) const;

    /**
     * Gets IAP ID according to WAP ID.
     * @since Series60 3.2
     * @param aWapId WAP ID of access point.
     * @return IAP ID of access point
     */
    TUint32 IapIdFromWapIdL( TUint32 aWapId );

private: // Data

    /*
     * Holds the received VoIP item data.
     * Own.
     */
    RPointerArray<CWPVoIPItem> iDatas;

    /*
     * Current VoIP setting data.
     * Own.
     */
    CWPVoIPItem* iCurrentVoIPItem;

    /*
     * The application id of the current characteristic.
     */
    TPtrC iAppID;

    /*
     * Title for VoIP settings. Value read from the resource file 
     * in initiation phase. 
     * Own.
     */
    HBufC*  iTitle;

    /*
     * Holds the value of current characteristic type (state).
     */
    TUint iCurrentCharacteristic;

    /*
     * Holds the parameters of the currently handled codec item.
     * Own.
     */
    CWPVoIPCodec* iCurrentCodec;

    /*
     * Descriptor containing uids of the saved items (as TUint32).
     */
    mutable HBufC8* iUids;

    /*
     * Booleans for telling if certain settings can be set.
     */
    TBool iSetCodec;
    TBool iSetVoipUris;

    /*
     * Flag telling if feature manager has been initialized.
     */
    TBool iFeatMgrInitialized;
    };

#endif      // CWPVOIPADAPTER_H

// End of File
