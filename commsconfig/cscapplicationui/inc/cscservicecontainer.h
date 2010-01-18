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
* Description:  CSC Application service view´s container
*
*/


#ifndef C_CSCSERVICECONTAINER_H
#define C_CSCSERVICECONTAINER_H

#include <eikmobs.h>

class MCSCServiceContainerObserver;
class CCSCEngCCHHandler;
class CCSCEngServiceHandler;
class CCSCEngBrandingHandler;
class CCSCEngServicePluginHandler;
class CCSCEngUiExtensionPluginHandler;


const TUint KBrandIdMaxLength = 512;

/**
 *  TListBoxItem class
 *
 *  For identifying list box item types
 *
 *  @lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( TListBoxItem )
    {
    public:
    
        /**  Enumeration for listbox item type */
        enum TItemType
            {
            EGeneralService = 0,
            EServicePlugin,
            EUiExtension
            };
        
        /**
         * Constructor.
         *
         * @since S60 3.2
         */
        TListBoxItem()
            : iItemType( EGeneralService ),
              iPluginUid( KNullUid ),
              iServiceId( KErrNone )
            {}

    public: // data
    
        /**
         * Listbox item type
         */
        TItemType iItemType;
                
        /**
         * Plug-in UID for Listbox item type EServicePlugin and EUiExtension
         */
        TUid iPluginUid;
        
        /*
         * Service id ( used only with EGeneralService and EServicePlugin)
         */
        TUint iServiceId;
    };



/**
 *  CCSCServiceContainer class
 *  CSC Applications Service view container class.
 *
 *  @lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CCSCServiceContainer ) : public CCoeControl,
                                            public MEikMenuObserver
    {
    public:
        
        /**
         * Two-phased constructor.
         *
         * @param aView pointer to calling view
         * @param aRect rect
         * @param aServicePluginHandler reference to servicepluginhandler
         * @param aUiExtensionPluginHandler reference to uiextensionhandler
         * @param aServiceHandler reference to service handler
         * @param aBrandingHandler reference to branding handler
         * @param aCCHHandler reference to cch handler
         */
        static CCSCServiceContainer* NewL(
            const TRect& aRect,
            MCSCServiceContainerObserver& aContainerObserver,
            CCSCEngServicePluginHandler& aServicePluginHandler,
            CCSCEngUiExtensionPluginHandler& aUiExtensionPluginHandler,
            CCSCEngServiceHandler& aServiceHandler,
            CCSCEngBrandingHandler& aBrandingHandler,
            CCSCEngCCHHandler& aCCHHandler );
        
        
        /**
         * Destructor.
         */
        virtual ~CCSCServiceContainer();
        
                
        /**
         * Update service view
         *
         * @since S60 v3.2
         */
        void UpdateServiceViewL();
        
        
        /**
         * Update listbox
         *
         * @since S60 v3.2
         */
        void UpdateListboxL();
        
        
        /**
         * Return current listbox item index
         *
         * @since S60 v3.2
         * @return current item index
         */
        TInt CurrentItemIndex() const;
        
        
        /**
         * Return current listbox item
         *
         * @since S60 v3.2
         * @return current listbox item
         */
        TListBoxItem ListBoxItem();
        
        
        /**
         * Return reference to listbox
         *
         * @since S60 v5.2
         * @return reference to listbox
         */
        CAknDouble2LargeStyleListBox& ListBox();
        
        
        /**
         * Return pen down point
         *
         * @since S60 v5.2
         * @return pen down point
         */
        TPoint PenDownPoint();
        
        
        /**
         * Return service count
         *
         * @since S60 v3.2
         * @return service count
         */
        TInt ServiceCount();

        
        /**
         * Set current item index
         *
         * @since S60 v3.2
         * @param current item index
         */
        void SetCurrentIndex( TInt aIndex );
        
        
        /**
         * Delete service
         *
         * @since S60 v3.2
         */
        void DeleteServiceL();
        
        
        /**
         * Handle service plugin removation.
         *
         * @since S60 v5.0
         * @param aUid plugin uid.
         */
        void HandleServicePluginRemovationL( const TUid& aUid );
        
        
        /**
         * Updates container data because of layout change
         *
         * @since S60 v3.2
         * @param aType for layout change type
         */ 
        void HandleResourceChange( TInt aType );
        
        
        /**
         * Sets listbox observer.
         *
         * @since S60 v3.2
         * @param aObserver listbox observer
         */ 
        void SetListBoxObserver( MEikListBoxObserver* aObserver );
        
                
        /**
         * Hides dialog wait note
         *
         * @since S60 v3.2
         * @param aIndex index of plugin
         */ 
        void HideDialogWaitNote();


        /**
         * Launches cleanup plugin to remove settings.
         *
         * @since S60 v3.2
         * @param aServiceId for service to be removed.
         */ 
        void LaunchCleanupPluginL( TUint aServiceId ) const;
        
        
        /**
         * Resolve service id from service plugin uid
         *
         * @since S60 v3.2
         * @param aUid service plugin uid
         * @param aServiceId service id stored here
         */ 
        void ResolveServiceIdL( const TUid& aUid, TUint& aServiceId );
        
        
        // from base class CCoeControl

        /**
         * From CCoeControl
         * Offer key events for controls.
         *
         * @since S60 v3.0
         * @param aKeyEvent for pressed key
         * @param aType for type of press
         * @return ETrue if consumed
         */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

        /**
         * From CCoeControl
         * Returns component controls.
         *
         * @since S60 v3.0
         * @param aIndex for index
         * @return control of the component
         */
        CCoeControl* ComponentControl( TInt aIndex ) const;
        
        
        /**
         * From CCoeControl
         * Handle pointer event.
         *
         * @since S60 v3.0
         * @param aPointerEvent pointer event
         */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );
        
        
        // from base class MCSCEngCCHObserver

        /**
         * From MCSCEngCCHObserver.
         */
        void ServiceStatusChanged(
            TUint aServiceId, 
            TCCHSubserviceType aType, 
            const TCchServiceStatus& aServiceStatus );

        
    protected: 
        
        // from base class CCoeControl
          
        /**
         * From CCoeControl
         * Handles controls size changes
         *
         * @since S60 v3.0
         */
        void SizeChanged();
        
        
        /**
         * From CoeControl
         * Handles focus changes
         *
         * @since S60 v3.0
         * @param aDrawNow for drawing event
         */        
        virtual void FocusChanged( TDrawNow aDrawNow );
        
        
        /**
         * From CoeControl
         * Counts components controls
         *
         * @since S60 v3.0
         * @return returns number of controls
         */
        TInt CountComponentControls() const;
        
        
        /**
         * From CCoeControl
         * For getting help context
         *
         * @since S60 v3.0
         * @param aContext
         */
        void GetHelpContext( TCoeHelpContext& aContext ) const;
        
        
        /**
         * From MEikMenuObserver
         */
        void SetEmphasis( CCoeControl* /*aMenuControl*/, TBool /*aEmphasis*/ ){};
      
        
        /**
         * From MEikMenuObserver
         */
        void ProcessCommandL( TInt /*aCommandId*/ ){};

    private:
        
        CCSCServiceContainer( 
            MCSCServiceContainerObserver& aContainerObserver,
            CCSCEngServicePluginHandler& aServicePluginHandler,
            CCSCEngUiExtensionPluginHandler& aUiExtensionPluginHandler,
            CCSCEngServiceHandler& aServiceHandler,
            CCSCEngBrandingHandler& aBrandingHandler,
            CCSCEngCCHHandler& aCCHHandler );
            
            
        void ConstructL( const TRect& aRect );
        
        
        /**
         * Sets icons for listbox items.
         *
         * @since S60 v3.2
         * @param aListBoxItem lisbox item
         * @param aIconArray icon array
         * @param aIconSize icon size in pixels
         */
        void SetIconL( const TListBoxItem& aListBoxItem,
            CArrayPtr<CGulIcon>* aIconArray, TInt aIconSize );
        
        
       /**
        * Cleanup RImplInfoPtrArray
        * @since S60 3.2
        * @param aArray Which to be destroyed
        */
        static void ResetAndDestroy( TAny* aArray );
            

    private: // data
    
        /**
         * Reference to MCSCServiceContainerObserver
         */
        MCSCServiceContainerObserver& iContainerObserver;
                        
        /**
         *  Reference to CCSCServicePluginHandler
         */         
        CCSCEngServicePluginHandler& iServicePluginHandler;
        
        /*
         * Reference to CCSCUiExtensionPluginHandler
         */
        CCSCEngUiExtensionPluginHandler& iUiExtensionPluginHandler;
        
        /*
         * Reference to CCSCEngServiceHandler
         */
        CCSCEngServiceHandler& iServiceHandler;
        
        /**
         * Reference to CCSCEngBrandingHandler
         */         
        CCSCEngBrandingHandler& iBrandingHandler;
        
        /**
         * Reference to CCSCEngCCHHandler
         */         
        CCSCEngCCHHandler& iCCHHandler;
                        
        /*
         * Pointer to listbox
         * Own.
         */
        CAknDouble2LargeStyleListBox* iListBox;
        
        /*
         * Pointer to listbox item array
         * Own.
         */
        CDesCArrayFlat* iArray;
        
        /*
         * Pointer to listbox item array
         * Own.
         */
        CDesCArray* iListBoxItems;
        
        /**
         * A Global wait note for removing service.
         */
        CCSCDialog* iDialog;
        
        /**
         * Menubar.
         * Own.
         */
        CEikMenuBar* iEikMenuBar;
        
        /*
         * Array for identifying different type listbox items
         */
        RArray<TListBoxItem> iListBoxItemArray;
        
        /**
         * Ui extension plug-in count
         */
        TUint iUiExtensionCount;
        
        /**
         * Pen down point
         */
        TPoint iPenDownPoint;

#ifdef _DEBUG
    friend class UT_CSC;
#endif    

};

#endif // C_CSCSERVICECONTAINER_H
