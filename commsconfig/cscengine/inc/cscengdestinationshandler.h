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
* Description:  For handling interactions betweed UI and CMM.
*
*/


#ifndef C_CSCENGDESTINATIONSHANDLER_H
#define C_CSCENGDESTINATIONSHANDLER_H

#include <cmmanagerext.h>

/**
 * An instance of CCSCEngDestinationsHandler
 * For handling interactions betweed UI and CMM.
 *
 * @lib cscengine.lib
 * @since Series 60 3.2
 */
NONSHARABLE_CLASS( CCSCEngDestinationsHandler ) : public CBase
    {    
    public:
        
        /**
         * Two-phased constructor.
         */ 
        IMPORT_C static CCSCEngDestinationsHandler* NewL();


        /**
         * Destructor.
         */
        IMPORT_C virtual ~CCSCEngDestinationsHandler();

        
        /**
         * Returns available destination ids in the device.
         *
         * @since S60 v3.2
         * @param aSnapIds for destination ids
         */
        IMPORT_C void AvailableSnapIdsL( RArray<TUint32>& aSnapIds );          
    
        
        /**
         * Returns destination name based on destination id.
         *
         * @since S60 v3.2
         * @param aSnapId for destination id 
         * @return HBufC pointer to destination name, 
         *          ownership is transferred to caller
         */
        IMPORT_C HBufC* SnapNameL( TUint32 aSnapId );
        
        
        /**
         * Returns ETrue if snap is in use.
         *
         * @since S60 v3.2
         * @param aSnapId for destination id 
         * @return ETrue when snap is in use
         */
        IMPORT_C TBool IsSnapInUseL( TUint32 aSnapId );
     
    private:

        CCSCEngDestinationsHandler();

        void ConstructL();
              
    private:  // data
        
        /**
         * Handle to Connection Method Manager.
         * Own.
         */
        RCmManagerExt iCmManager;
        
    };

#endif // C_CSCENGDESTINATIONSHANDLER_H
