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


#ifndef C_CSCSVCPLUGINRCSEHANDLER_H
#define C_CSCSVCPLUGINRCSEHANDLER_H

class CRCSEProfileEntry;
class CRCSEProfileRegistry;
class CCSCSvcPluginSpSettHandler;

/**
 *  An instance of class CCSCSvcPluginRcseHandler.
 *
 *  @lib
 *  @since S60 v3.2
 */
class CCSCSvcPluginRcseHandler : public CBase
    {
    public:

        static CCSCSvcPluginRcseHandler* NewL();

	    /**
         * Destructor.
         */
        ~CCSCSvcPluginRcseHandler();

         
        /**
         * Returns VoIP profile id for corresponding EasyVoIP plugin.
         * Also returns SIP profile reference ids if any 
         * was found from VoIP profile entry.
         * Function leaves on failure.
         *
         * @since S60 v3.2
         * @param aServiceId service id
         * @param aProfileId for VoIP profile id to be returned
         * @param aSipProfileIds for SIP profile references appended to entry
         */
        void GetProfileIdsL( 
            TUint32 aServiceId, 
            TUint32& aProfileId,
            RArray<TUint32>& aSipProfileIds );
        
        /**
         * Destroys VoIP profile from RCSE based on VoIP profile id.
         * Function leaves on failure.
         *
         * @since S60 v3.2
         * @param aProfileId for VoIP profile id to be deleted
         */
        void RemoveProfileL( TUint32 aProfileId );
                

    private:
         
        CCSCSvcPluginRcseHandler();

        void ConstructL();
        
        
       /**
        * For deleting RPointerArray in case of leave.
        *
        * @since S60 v3.2
        * @param aPointerArray for pointer array to be deleted
        */
        static void ResetAndDestroy( TAny* aPointerArray );

    
    private: // data
            
        /*
         * Handle to Richcall Setting Engine profile registry.
         * Own.
         */
        CRCSEProfileRegistry* iRcseProfileRegistry;  
        
        /*
         * Handle to CCSCSvcPluginRcseHandler.
         * Own.
         */
        CCSCSvcPluginSpSettHandler* iSpSettHandler;  
    };

#endif  // C_CSCSVCPLUGINRCSEHANDLER_H


