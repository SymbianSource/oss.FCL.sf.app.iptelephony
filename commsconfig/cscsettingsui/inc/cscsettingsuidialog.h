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
* Description:  Declarition of CSCSettingsUiDialog
*
*/


#ifndef C_CSCSETTINGSUIDIALOG_H
#define C_CSCSETTINGSUIDIALOG_H

class CAknGlobalNote;
class CAknGlobalMsgQuery;
class MCSCSettingsUiDialogObserver;
    
/**
 *  CCSCSettingsUiDialog class
 *  Declarition of CCSCSettingsUiDialog.
 *
 *  @lib CSCSettingsUi.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CCSCSettingsUiDialog ) : public CActive
    {
    public:
                
        /**
         * Two-phased constructor.
         * @param aObserver settings ui dialog observer
         */
        static CCSCSettingsUiDialog* NewL(
            MCSCSettingsUiDialogObserver& aObserver );


        /**
         * Two-phased constructor.
         * @param aObserver settings ui dialog observer
         */
        static CCSCSettingsUiDialog* NewLC(
            MCSCSettingsUiDialogObserver& aObserver );
        
        
        /**
         * Destructor.
         */
        virtual ~CCSCSettingsUiDialog();
    
        
        /**
         * Launches a service information message query.
         *
         * @since S60 v3.2
         * @param aUsername for protocol profile username
         * @param aDomain for protocol profile domain
         * @param aAddress for connected address
         * @param aSsid for connected network
         * @param aMac for WLAN MAC address
         * @param aDisabled ETrue if service is disabled
         */
        void LaunchMessageQueryL(
            const TDesC& aUsername,
            const TDesC& aDomain,
            const TDesC& aAddress,
            const TDesC& aSsid,
            const TDesC& aMac,
            TBool aDisabled );
        
        
        /**
         * Destroys a service information message query.
         *
         * @since S60 v3.2
         */
        void DestroyMessageQuery();
        
    protected:

        // from base class CActive
   
        /**
         * @see CActive.
         */
        void RunL();


        /**
         * @see CActive.
         */
        void DoCancel();
        
    private:

        CCSCSettingsUiDialog( MCSCSettingsUiDialogObserver& aObserver );

        void ConstructL();
        
        
        /**
         * Creates text for service information message query.
         *
         * @since S60 v3.2
         * @param aText for service information text
         * @param aUsername for protocol profile username
         * @param aDomain for protocol profile domain
         * @param aAddress for connected address
         * @param aSsid for connected network
         * @param aMac for WLAN MAC address
         * @param aDisabled ETrue if service is disabled
         */
        void CreateTextForMsgQueryL( 
            HBufC* aText,
            const TDesC& aUsername,
            const TDesC& aDomain,
            const TDesC& aAddress,
            const TDesC& aSsid,
            const TDesC& aMac,
            TBool aDisabled ) const;
            
        
        /**
         * Creates an item for service information message query.
         *
         * @since S60 v3.2
         * @param aText for service information text
         * @param aItem for setting item text
         * @param aAdditionalValue for additional setting item value
         * @param aData for setting item value
         */
        void CreateItemToMsgQuery( 
            HBufC* aText,
            HBufC* aItem,
            HBufC* aAdditionalValue,
            const TDesC& aData ) const;
                
    private: // Data  

        /**
         * Reference to settings ui dialog observer.
         */
         MCSCSettingsUiDialogObserver& iObserver;
                        
        /**
         * A global message query for service information.
         * Own.
         */
        CAknGlobalMsgQuery* iMsgQuery;
        
        /**
         * Global note Id
         */
        TInt iNoteId;
        
     };

#endif // C_CSCSETTINGSUIDIALOG_H  
