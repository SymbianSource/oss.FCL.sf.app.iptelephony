/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for CCSCEngSettingsCleanupPluginInterface class.
*
*/

#ifndef CSCSETTINGSCLEANUPPLUGININTERFACE_H
#define CSCSETTINGSCLEANUPPLUGININTERFACE_H

#include <e32base.h>
#include <ecom/ecom.h>

// Constant for plugin interface:
const TUid KCSCSettingsCleanupPluginInterfaceUid = { 0x2000B009 };


class CCSCEngSettingsCleanupPluginInterface : public CBase
    {
    public:
    
        enum TCSCPluginType
            {
            ESipVoIPCleanupPlugin = 0
            };
            

        /**
        * Creates new CSC settings cleanup plugin having the given UID.
        * Uses Leave code KErrNotFound if implementation is not found.
        *
        * @param aImplementationUid Implementation UID of the plugin to be
        *        created.
        */
        static CCSCEngSettingsCleanupPluginInterface* NewL(
            const TUid aImplementationUid );


        virtual ~CCSCEngSettingsCleanupPluginInterface();


        // New methods

        virtual TCSCPluginType PluginType() const;

   
        virtual void RemoveSettingsL( TUint32 aServiceId ) = 0;

    
    public: // Data

    /// Unique key for implementations of this interface.
    TUid iInstanceKey;    
    };

#include <cscengsettingscleanupplugininterface.inl>

#endif // CSCSETTINGSCLEANUPPLUGININTERFACE_H
