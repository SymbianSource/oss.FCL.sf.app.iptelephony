/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  cscgsplugin implementation.
*
*/


#ifndef C_CSCGSPLUGIN_H
#define C_CSCGSPLUGIN_H

#include <AknServerApp.h>
#include <ConeResLoader.h>
#include <gsplugininterface.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "apadoc.h"
#endif

class CAknNullService;


class TAppInfo
    {
    public:
        TAppInfo( TUid aUid, const TDesC& aFile )
            : iUid( aUid ), iFile( aFile )
            {}
        TUid iUid;
        TFileName iFile;
    };


/**
 * Commsconfig GS plugin implementation. 
 * Launches CSC application from control panel.
 *
 * @code
 * @endcode
 *
 * @since S60 5.0
 */
class CCscGsPlugin : public CGSPluginInterface,
                     public MAknServerAppExitObserver // Embedding
    {
    public: 

        static CCscGsPlugin* NewL( TAny* aInitParams );

        ~CCscGsPlugin();
        
        
        // from base class CAknView

        /**
         * From CAknView.
         *
         * @since S60 v3.0
         * @return The ID of view.
         */
        TUid Id() const;

        
        // from base class CGSPluginInterface

        /**
         * From CGSPluginInterface.
         */
        void GetCaptionL( TDes& aCaption ) const;
        
        /**
         * From CGSPluginInterface.
         */
        TInt PluginProviderCategory() const;
      
        /**
         * From CGSPluginInterface.
         */
        TGSListboxItemTypes ItemType();
     
        /**
         * From CGSPluginInterface.
         */
        void GetValue( 
            const TGSPluginValueKeys aKey,
            TDes& aValue );

        /**
         * From CGSPluginInterface.
         */
        void HandleSelection( const TGSSelectionTypes aSelectionType );
        
        /**
         * From CGSPluginInterface.
         */
        CGulIcon* CreateIconL( const TUid aIconType );
        
        /**
         * From CGSPluginInterface.
         */
        TBool Visible() const;

    protected:


        CCscGsPlugin();

        void ConstructL();

        
        // from base class CAknView

        /**
         * From CAknView.
         * Creates the Container class object.
         *
         * @param aPrevViewId is not used.
         * @param aCustomMessageId is not used.
         * @param aCustomMessage is not used.
         */
        void DoActivateL( 
            const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId,
            const TDesC8& aCustomMessage );

        /**
         * From CAknView.
         * Deletes the Container class object.
         *
         */
        void DoDeactivate();

    private:

        /**
         * Opens localized resource file.
         *
         * @since S60 5.0
         * @param aResourceFileName resource file name
         * @param aResourceLoader resource loader
         */
        void OpenLocalizedResourceFileL(
            const TDesC& aResourceFileName,
            RConeResourceLoader& aResourceLoader );

        /**
         * Launches csc application.
         *
         * @since S60 5.0
         */
        void LaunchCscAppL();
        
        
        /**
         * Launches application as embedded.
         * 
         * @since S60 5.0
         */
        void EmbedAppL( const TAppInfo& aApp );
        
    protected:

        /**
         * Resource laoder.
         * Own.
         */
        RConeResourceLoader iResources;
        
        /**
         * Nullservice.
         * Own.
         */
        CAknNullService* iNullService;
        
        /**
         * ApaDocument.
         * Own.
         */
        CApaDocument* iEmbedded;
        
#ifdef _DEBUG
    friend class ut_cscgsplugin;
#endif 
    };

#endif // C_CSCGSPLUGIN_H
