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


#ifndef C_CSCSVCPLUGINHANDLER_H
#define C_CSCSVCPLUGINHANDLER_H

#include <e32base.h>
#include <badesca.h>

class CCSCSvcPluginSipHandler;
class CCSCSvcPluginRcseHandler;
class CCSCSvcPluginCenrepHandler;
 
/**
 *  An instance of class CCSCSvcPluginHandler.
 *
 *  @lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS ( CCSCSvcPluginHandler ) : public CBase
    {
    public:

        static CCSCSvcPluginHandler* NewL();

        /**
         * Destructor.
         */
        virtual ~CCSCSvcPluginHandler();
     
     		
     		/**
         * Removes settings based on service id.
         * Function leaves on failure.
         *
         * @since S60 v3.2
         * @param aServiceId service id
         */
        void RemoveSettingsL( TUint32 aServiceId );   
                
    private:

        CCSCSvcPluginHandler();

        void ConstructL();
 
        
        /**
         * Search settings data based on service id.
         * Function leaves on failure.
         *
         * @since S60 v3.2
         * @param aServiceId service id
         */
        void SearchSettingsDataL( TUint32 aServiceId );
        
        
        /**
         * For removing SIP/VoIP profiles and NAT/FW settings.
         * Function leaves on failure.
         *
         * @since S60 v3.2
         * @param aServiceId service id
         */
        void RemoveProfilesL( TUint32 aServiceId );
        
        
    private: // data
    
        /*
         * Handles events concerning Richcall Setting Engine.
         * Own.
         */
        CCSCSvcPluginRcseHandler* iRcseHandler;
        
        /*
         * Handles events concerning SIP Profile Registry.
         * Own.
         */
        CCSCSvcPluginSipHandler* iSipHandler;
        
        /*
         * Handles events concerning Central Repository.
         * Own.
         */
        CCSCSvcPluginCenrepHandler* iCenRepHandler;
    
        /*
         * VoIP profile id.
         */
        TUint32 iVoIPProfileId;
        
        /*
         * SIP profile ids appended to the VoIP profile.
         * Own.
         */
        RArray<TUint32> iSipProfileIds;
        
        /*
         * IAP ids linked to SIP profile ids.
         * Own.
         */
        RArray<TUint32> iIapIds;
        
        /*
         * SNAP ID
         * Own.
         */
        TInt iSNAPId;
        
        /*
         * Presence ID
         * Own.
         */
        TInt iPresenceId;
        
        /*
         * Domain names linked to SIP profile ids.
         * Own.
         */
        CDesC8ArrayFlat* iDomainNames;
        
        
    };

#endif  // C_CSCSVCPLUGINHANDLER_H
