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
* Description:  CSC Applications Document
*
*/


#ifndef C_CSCDOCUMENT_H
#define C_CSCDOCUMENT_H

#include <AknDoc.h>

class CCSCAppUi;
class CEikAppUi;
class CEikApplication;
class CCSCEngServicePluginHandler;
class CCSCEngUiExtensionPluginHandler;


/**
 *  An instance of class CCSCDocument
 *  The Document part of AVKON application framework for CSC application
 *
 *  @lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CCSCDocument ) : public CAknDocument
    {
    public:
        
        /**
         * Two-phased constructor.
         * @param aApp CEikApplication
         */
        static CCSCDocument* NewL( CEikApplication& aApp );
        
        
        /**
         * Two-phased constructor.
         * @param aApp CEikApplication
         */
        static CCSCDocument* NewLC( CEikApplication& aApp );


        /**
         * Destructor
         */
        virtual ~CCSCDocument();

        
        /**
         * Transfers ownership to document.
         * 
         * @since S60 v3.2
         * @param aServicePluginHandler for handle to be transferred
         * @param aUiExtensionPluginHandler for handle to be transferred
         */
        void SetOwnershipOfPluginHandlers( 
            CCSCEngServicePluginHandler* aServicePluginHandler,
            CCSCEngUiExtensionPluginHandler* aUiExtensionPluginHandler );  
        
        
        /**
         * Return reference to service plug-in handler
         * 
         * @since S60 v3.0
         * @return reference to service plug-in handler
         */
        CCSCEngServicePluginHandler& ServicePluginHandler();
        
        
        // from base call CAknDocument

        /**
         * From CAknDocument 
         * Create a CCSCAppUi object and return a pointer to it
         *
         * @return CEikAppUi A pointer to the created instance of the
         *                   AppUi created
         */
        CEikAppUi* CreateAppUiL();
        
        /**
         * From CAknDocument 
         * Hides icon from task list
         * 
         * @param aWgName window group name
         */
        void UpdateTaskNameL( CApaWindowGroupName* aWgName );

    private:

        void ConstructL();

        CCSCDocument( CEikApplication& aApp );
    
    private: //data
        
        /**
         * Handle to CSC ServicePluginHandler.
         * Own.
         */
        CCSCEngServicePluginHandler* iServicePluginHandler;
        
        /**
         * Handle to CSC UiExtensionPluginHandler.
         * Own.
         */
        CCSCEngUiExtensionPluginHandler* iUiExtensionPluginHandler;
        
#ifdef _DEBUG
    friend class UT_CSC;
#endif
        
    };

#endif // C_CSCDOCUMENT_H
