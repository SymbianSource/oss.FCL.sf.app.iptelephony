/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Handles Vcc settings in provisioning.
*
*/



#ifndef C_WPVCCADAPTER_H
#define C_WPVCCADAPTER_H

#include <CWPAdapter.h>
#include <MWPContextExtension.h>

#include "vccunittesting.h"
#include <sipprofileregistryobserver.h>

class CWPCharacteristic;
class CVccSPSettings;

/**
 * CVccAdapter handles Vcc settings received via provisioning framework.
 *
 * CVccAdapter is a plugin for provisioning framework. It indicates to
 * framwork that it is interested in Vcc settings, and when they're
 * available CVccAdapter stores them into instance of CVccItem.
 *
 * @lib wpvccadapter
 * @since S60 3.2
 */
class CWPVccAdapter : public CWPAdapter, public MWPContextExtension, public MSIPProfileRegistryObserver
    {

public:
    
    /**
    * Symbian OS two-phased constructor
    */
    static CWPVccAdapter* NewL();

    /**
    * Destructor.
    */
    virtual ~CWPVccAdapter();

// from base class CWPAdapter

    /**
     * @see CWPAdapter::ItemCount()
     */
    TInt ItemCount() const;

    /**
     * @see CWPAdapter::SummaryTitle()
     */
    const TDesC16& SummaryTitle( TInt aIndex ) const;

    /**
     * @see CWPAdapter::SummaryText()
     */
    const TDesC16& SummaryText( TInt aIndex ) const;

    /**
     * @see CWPAdapter::SaveL()
     */
    void SaveL( TInt aIndex );

    /**
     * @see CWPAdapter::CanSetAsDefault()
     */
    TBool CanSetAsDefault( TInt aIndex ) const;

    /**
     * @see CWPAdapter::SetAsDefaultL()
     */
    void SetAsDefaultL( TInt aIndex );

    /**
     * @see CWPAdapter::DetailsL()
     */
    TInt DetailsL( TInt aItem, MWPPairVisitor& aVisitor );

    /**
     * @see CWPAdapter::ContextExtension()
     */
    TInt ContextExtension( MWPContextExtension*& aExtension );

    /**
     * @see CWPAdapter::SettingsSavedL()
     */
    void SettingsSavedL(const TDesC8& aAppIdOfSavingItem,
                        const TDesC8& aAppRef, 
                        const TDesC8& aStorageIdValue);
    
// From base class MWPVisitor
    
    /**
     * @see MWPVisitor::VisitL()
     */
    void VisitL( CWPCharacteristic& aCharacteristic );
    
    /**
     * @see MWPVisitor::SavingFinalizedL()
     */
    void SavingFinalizedL();


    /**
     * @see MWPVisitor::VisitL()
     */
    void VisitL( CWPParameter& aParameter );

    /**
     * @see MWPVisitor:VisitLinkL()
     */
    void  VisitLinkL( CWPCharacteristic& aLink );

// from base class MWPContextExtension
    
    /**
     * @see MWPContextExtension::SaveDataL()
     */
    const TDesC8& SaveDataL( TInt aIndex ) const;

    /**
     * @see MWPContextExtension::DeleteL()
     */
    void DeleteL( const TDesC8& aSaveData );

    /**
     * @see MWPContextExtension::Uid()
     */
    TUint32 Uid() const;
  
    /** 
    * From MSIPProfileRegistryObserver
    * SIP profile information event.
    *
    * @since S60 v3.1
    * @param aSIPProfileId id for profile
    * @param aEvent type of information event
    */
     void ProfileRegistryEventOccurred(TUint32 aProfileId,
                                              TEvent aEvent);
     /**
     * From MSIPProfileRegistryObserver
     * An asynchronous error has occurred related to SIP profile.
     *
     * @since S60 v3.1
     * @param aSIPProfileId the id of failed profile
     * @param aError a error code
     * @return none
     */
     void ProfileRegistryErrorOccurred(TUint32 aProfileId,
                                              TInt aError);
      
private:

    /**
    * C++ constructor 
    */
    CWPVccAdapter();
    
    /**
    * Symbian 2nd phase constructor
    */
    void ConstructL();

    /**
    * Remove old VCC related settings if found
    */
    void RemoveVCCSettingsL();
    
    /**
    * Remove VCC enabled VoIP network destination from snaplist
    */
    void RemoveNetworkDestinationL();

private: // data

    /**
     * The application id of the current characteristic
     * Own
     */
    HBufC* iAppID;

    /**
     * Title for the Vcc settings.
     * Own.
     */
    HBufC*  iTitle;

    /**
     * Holds the value of the current characteristic type (state).
     */
    TUint iCurrentCharacteristic;

    /**
     * Type of the current characteristics
     */
    TInt iCurrentCharacteristicType;
    
    /**
     * Application references
     * Own
     */
    RArray<TPtrC> iToAppReferences;
    
    /**
     * Item that saves the provisioned settings
     * Own
     */
    CVccSPSettings* iVccItem;
    
    /**
     * Item that saves the provisioned settings
     * Own
     */
    CVccSPSettings* iNewVccItem;

    /**
     * RCSE Profile ID
     * Own
     */
    TUint iVoipProfileId;
    
     /**
     * Old VCC enabled VoIP service name
     * needed for deleting network destination before saving new VCC enabled VoIP service
     */
    TBuf<200> iOldVoipServiceName;
    
    
    
    VCC_UNITTEST( T_WpVccAdapter )
    };

#endif	// C_WPVCCADAPTER_H
