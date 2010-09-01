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
* Description:  ?Description
*
*/


#ifndef CWPGENERALVOIPSETTINGSADAPTER_H
#define CWPGENERALVOIPSETTINGSADAPTER_H

#include <CWPAdapter.h>
#include <MWPContextExtension.h>

class CWPCharacteristic;

/**
 *  Realizes general VoIP settings provisioning adapter.
 *
 *  General VoIP settings adapter handles general VoIP settings and stores
 *  them via Central Repository.
 *
 *  @lib CWPGeneralVoIPSettingsAdapter.lib
 *  @since S60 v3.0.
 */
class CWPGeneralVoIPSettingsAdapter : public CWPAdapter, public MWPContextExtension
    {
    #ifdef _DEBUG
    /**
     * Friend class for unit testing.
     */
    friend class UT_CWPGeneralVoIPSettingsAdapter;
    #endif

    public:

        static CWPGeneralVoIPSettingsAdapter* NewL();

        virtual ~CWPGeneralVoIPSettingsAdapter();

        /**
         * Setter for iName.
         *
         * @since S60 v3.0.
         * @param aName User viewable name of general VoIP settings.
         */
        void SetNameL( const TDesC& aName );
 
        /**
         * Setter for iToAppref. Value is changed only if iToApprefSet
         * is EFalse.
         *
         * @since S60 v3.2.3.
         * @param aToAppRef Value for TO-APPREF. Links to VoIP settings.
         */
        void SetToApprefL( const TDesC8& aToAppRef ); 
        
        /**
         * Setter for iClir.
         *
         * @since S60 v3.0.
         * @param aClir Value for VoIP CLIR enabling rule.
         */
        void SetClir( const TInt aClir );
 
        /**
         * Setter for iACBRule.
         *
         * @since S60 v3.0.
         * @param aACBRule Value for anonymous call blocking rule.
         */
        void SetACBRule( const TInt aACBRule );
 
        /**
         * Setter for iPrefTel.
         *
         * @since S60 v3.0.
         * @param aPrefTel Preferred telephony mode value.
         */
        void SetPrefTel( const TInt aPrefTel );

        /**
         * Setter for iDnd.
         *
         * @since S60 v3.0.
         * @param aDnd Value for do not disturb setting.
         */
        void SetDnd( const TInt aDnd );

        /**
         * Setter for iPSCallWaiting.
         *
         * @since S60 v3.0.
         * @param aPSCallWaiting Value for PS call waiting rule.
         */
        void SetPSCallWaiting( const TInt aPSCallWaiting );

        /**
         * Setter for iPrefTelLock.
         *
         * @since S60 v3.0.
         * @param aPrefTelLock Value for telephony preference lock.
         */
        void SetPrefTelLock( const TInt aPrefTelLock );

        /**
         * Setter for iCsRadioWifiOff.
         *
         * @since S60 v3.0.
         * @param aCsRadioWifiOff Value for turning CS radio off in WiFi mode.
         */
        void SetCsRadioWifiOff( const TInt aCsRadioWifiOff );

        /**
         * Getter for iProviderId.
         *
         * @since S60 v3.0.
         * @return Provider of general VoIP settings.
         */
        TDesC& ProviderId();

        /**
         * Getter for iName.
         *
         * @since S60 v3.0.
         * @return User viewable name of general VoIP settings.
         */
        TDesC& Name();

        /**
         * Getter for iClir.
         *
         * @since S60 v3.0.
         * @return Value for VoIP CLIR enabling rule.
         */
        TInt Clir();

        /**
         * Getter for iACBRule.
         *
         * @since S60 v3.0.
         * @return Value for anonymous call blocking rule.
         */
        TInt ACBRule();

        /**
         * Getter for iPrefTel.
         *
         * @since S60 v3.0.
         * @return Preferred telephony mode value.
         */
        TInt PrefTel();

        /**
         * Getter for iDnd.
         *
         * @since S60 v3.0.
         * @return Value for do not disturb setting.
         */
        TInt Dnd();

        /**
         * Getter for iPSCallWaiting.
         *
         * @since S60 v3.0.
         * @return Value for call waiting rule.
         */
        TInt PSCallWaiting();

        /**
         * Getter for iPrefTelLock.
         *
         * @since S60 v3.0.
         * @return Value for telephony preference lock.
         */
        TInt PrefTelLock();

        /**
         * Getter for iCsRadioWifiOff.
         *
         * @since S60 v3.0.
         * @return Value for turning CS radio off in WiFi mode.
         */
        TInt CsRadioWifiOff();

    // from base class CWPAdapter

        /**
         * From CWPAdapter.
         * Checks the number of settings items.
         *
         * @since S60 v3.0.
         * @return Quantity of items (always one).
         */
        TInt ItemCount() const;

        /**
         * From CWPAdapter.
         * Getter for the summary title of general VoIP settings item. Text is
         * shown to the user in an opened configuration message.
         *
         * @since S60 v3.0.
         * @param aIndex Not used.
         * @return Summary title.
         */
        const TDesC16& SummaryTitle( TInt /*aIndex*/ ) const;

        /**
         * From CWPAdapter.
         * Getter for the summary text of general VoIP settings. Text is shown
         * to the user in opened configuration message.
         *
         * @since S60 v3.0.
         * @param aIndex Not used.
         * @return Summary text.
         */
        const TDesC16& SummaryText( TInt /*aIndex*/ ) const;

        /**
         * From CWPAdapter.
         * Saves general VoIP settings.
         *
         * @since S60 v3.0.
         * @param aIndex Not used.
         */
        void SaveL( TInt /*aIndex*/ );

        /**
         * From CWPAdapter.
         * Returns EFalse since there can only be one set of general VoIP
         * settings.
         *
         * @since S60 v3.0.
         * @param aIndex Location of the general VoIP settings item to
         *        be queried. Not used.
         * @return Information whether these settings can be set as default.
         */
        TBool CanSetAsDefault( TInt /*aIndex*/ ) const;

        /**
         * From CWPAdapter.
         * Adapter sets the settings as default. Not supported.
         *
         * @since S60 v3.0.
         * @param aIndex Location of the general VoIP settings item to be
         *        set as default.
         */
        void SetAsDefaultL( TInt /*aIndex*/ )
            {
            };

        /**
         * From CWPAdapter.
         * Query for detailed information about the general VoIP settings.
         * This is not supported feature as in other adapters in the framework.
         *
         * @since S60 v3.0.
         * @param aItem Not used but here because of inheritance.
         * @param aVisitor Object for retrieveing details of a setting entry.
         * @return KErrNotSupported if not supported.
         */
        TInt DetailsL( TInt aItem, MWPPairVisitor& aVisitor );

        /**
         * From CWPAdapter.
         * Visit method. Visits the adapter. Adapter checks from 
         * aCharacteristic if the data is targetet to it and acts according to
         * that.
         *
         * @since S60 v3.0.
         * @param aCharacteristic The characteristic found.
         */
        void VisitL( CWPCharacteristic& aCharacteristic );        

        /**
         * From CWPAdapter.
         * Visit method. Visits the adapter. Method sets the iAppId value.
         *
         * @since S60 v3.0.
         * @param aParameter The parameter found.
         */
        void VisitL( CWPParameter& aParameter );        

        /**
         * From CWPAdapter.
         * Visit method. Not supported.
         *
         * @since S60 v3.0.
         * @param aLink Link to the characteristic.
         */
        void VisitLinkL( CWPCharacteristic& /*aLink*/ )
            {
            };

        /**
         * From CWPAdapter.
         * Informs the adapters of the saved settings. Not supported since
         * this adapter does not communicate with other adapters.
         *
         * @since S60 v3.0.
         * @param aAppIdOfSavingItem UID of the adapter component. 
         * @param aAppRef APPREF of the saved settings.
         * @param aStorageIdValue Value that identifies the settings in its
         *        storage.
         */
        void SettingsSavedL ( const TDesC8& aAppIdOfSavingItem,
            const TDesC8& aAppRef, const TDesC8& aStorageIdValue );

        /**
         * From CWPAdapter.
         * Lets the adapters know when the saving has been finalized in 
         * viewpoint of the provisioning framework. The adapters can make 
         * finalization of saving the settings after receiving this call.
         *
         * @since S60 v3.0.
         */
        void SavingFinalizedL();

    // from base class MWPContextExtension

        /**
         * From MWPContextExtension.
         * Returns a pointer to a context extension.
         *
         * @since S60 v3.0.
         * @param aExtension Contains a pointer to MWPContextExtension if
         *                   supported.
         * @return KErrNotSupported if not supported, otherwise KErrNone.
         */
        TInt ContextExtension( MWPContextExtension*& aExtension );

        /**
         * From MWPContextExtension.
         * Returns the data used for saving.
         *
         * @since S60 v3.0.
         * @param aIndex The index of the data.
         * @return The data; ownership is transferred.
         */
        const TDesC8& SaveDataL( TInt /*aIndex*/ ) const;

        /**
         * From MWPContextExtension.
         * Deletes a saved settings.
         *
         * @since S60 v3.0.
         * @param aSaveData The data used for saving.
         */
        void DeleteL( const TDesC8& aSaveData );

        /**
         * From MWPContextExtension.
         * Returns the UID of the adapter.
         *
         * @since S60 v3.0.
         * @return UID of the adapter.
         */
        TUint32 Uid() const;

    protected:

    private:

        CWPGeneralVoIPSettingsAdapter();

        void ConstructL();

    public:  // Data

    private: // Data

        /**
         * Application ID of the current characteristic.
         */
        TPtrC iAppID;
 
        /**
         * Confirms whether APPID is correct (w9033).
         */
        TBool iIsAppIdOk;

        /**
         * Confirms whether iName has been correctly set.
         */
        TBool iIsNameOk;

        /**
         * Title for general VoIP settings. Value read from the resource file 
         * in initiation phase.
         * Own.
         */
        HBufC16* iTitle;

        /**
         * Holds the ID of saved data.
         * Here because of inherited method SaveDataL.
         * Own.
         */
        HBufC8* iSavedDataId;

        /**
         * Default name of general VoIP settings visible to user when viewing
         * the provisioning message. Value is read from the resource file in
         * initiation phase.
         * Own.
         */
        HBufC* iDefaultName;

        /**
         * Name of general VoIP settings visible to user when viewing the
         * provisioning message.
         * Own.
         */
        HBufC* iName;

        /**
         * The VoIP APPREF references by TO-APPREF of general VoIP settings.
         * Own.
         */
        HBufC8* iToAppref; 
        
        /**
         * A flag indicating whether or not correct TO-APPREF value 
         * has been found.
         */
        TBool iToApprefSet;

        /**
         * VoIP CLIR. Tells whether CLIR (Calling Line Identification
         * Restriction) is enabled or disabled. See w9033.txt or
         * VoIP software settings document.
         * CP parameter: CLIR.
         */
        TInt iClir;

        /**
         * Anonymous Call Block rule. Determines the rule for Anonymous Call
         * Block feature. See w9033.txt or VoIP software settings document.
         * CP parameter: ACBRULE.
         */
        TInt iACBRule;

        /**
         * Preferred telephony. Tells which telephony mode is preferred.
         * See w9033.txt or VoIP software settings document.
         * CP parameter: PREFTEL.
         */
        TInt iPrefTel;

        /**
         * Do Not Disturb (DND) setting. See w9033.txt or VoIP software
         * settings document.
         * CP parameter: DND.
         */
        TInt iDnd;

        /**
         * Call waiting. Tells whether call waiting is enabled or disabled.
         * See w9033.txt or VoIP software settings document.
         * CP parameter: PSCALLWAITING.
         */
        TInt iPSCallWaiting;


        /**
         * Telephony preference lock. Tells whether preferred telephony mode 
         * is UI editable. See w9033.txt or VoIP software settings document.
         * CP parameter: PREFTELLOCK.
         */
        TInt iPrefTelLock;

        
        /**
         * Shut down CS radio in WiFi mode. Tells whether CS radio is turned
         * off in WiFi mode. See w9033.txt or VoIP software settings document.
         * CP parameter: CSRADIOWIFIOFF.
         */
        TInt iCsRadioWifiOff;

        /**
         * Flag telling if feature manager has been initialized.
         */
        TBool iFeatMgrInitialized;

        /**
         * VoIP ID received from cpvoipadapter in SettingsSavedL method.
         */
        TUint32 iVoipId;

    };

#endif      // CWPGENERALVOIPSETTINGSADAPTER_H

// End of File
