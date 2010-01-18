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
* Description:  
*
*/


#ifndef C_CSCSVCPLUGIN_H
#define C_CSCSVCPLUGIN_H

#include <e32base.h>

#include "cscengsettingscleanupplugininterface.h"

class CCSCSvcPluginHandler;

/**
 *  An instance of class CCSCSvcPlugin.
 *
 *  @lib
 *  @since S60 v3.2
 */
class CCSCSvcPlugin : public CCSCEngSettingsCleanupPluginInterface
    {
    public:

        static CCSCSvcPlugin* NewL();

				/**
         * Destructor.
         */
        ~CCSCSvcPlugin();
        
        
        // From base class CCSCEngSettingsCleanupPluginInterface
        
        /**
         * From CCSCEngSettingsCleanupPluginInterface
         * Get plugin type.
         *
         * @since S60 v3.2
         */
        TCSCPluginType PluginType() const;

        /**
         * From CCSCEngSettingsCleanupPluginInterface
         * Remove settings based on service id.
         *
         * @since S60 v3.2
         */
        void RemoveSettingsL( TUint32 aServiceId );


    private:
              
        CCSCSvcPlugin();

        void ConstructL();

    
    private:  // data
    
    	/*
         * Handle to CCSCSvcPluginHandler.
         * Own.
         */
        CCSCSvcPluginHandler* iSvcPluginHandler;
    };

#endif  // C_CSCSVCPLUGIN_H


