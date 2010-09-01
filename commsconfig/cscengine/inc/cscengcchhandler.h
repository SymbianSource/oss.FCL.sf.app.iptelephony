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
* Description:  For handling interactions betweed UI and CCH.
*
*/



#ifndef C_CSCENGCCHHANDLER_H
#define C_CSCENGCCHHANDLER_H

#include <cch.h>

class MCSCEngCCHObserver;

/**
 * TSupportedSubServices class
 *
 * @lib cscengine.lib
 * @since Series 60 3.2
 */
NONSHARABLE_CLASS ( TSupportedSubServices )
    {
    public:
    
        /**
         * Constructor.
         *
         * @since S60 3.2
         */
        TSupportedSubServices()
            : iVoIP( EFalse ),
            iPresence( EFalse ),
            iIm( EFalse ),
            iVmbx( EFalse )
            {}

    public: // data
        
        /*
         * VoIP
         */
        TBool iVoIP;
    
        /*
         * Presence
         */
        TBool iPresence;
    
        /*
         * Instant Messaging
         */
        TBool iIm;
        
        /*
         * Voice mail box
         */
        TBool iVmbx;
    };


/**
 * An instance of CCSCEngCCHHandler
 * For handling interactions betweed UI and CCH.
 *
 * @lib cscengine.lib
 * @since Series 60 3.2
 */
NONSHARABLE_CLASS( CCSCEngCCHHandler ) : public CBase, 
                                                MCchServiceStatusObserver
    {    
    public:
                
        /**
         * Two-phased constructor.
         */ 
        IMPORT_C static CCSCEngCCHHandler* NewL( 
            MCSCEngCCHObserver& aObserver );


        /**
         * Destructor.
         */
        virtual ~CCSCEngCCHHandler();
        
        
        /**
         * Disable service
         *
         * @since S60 v5.0
         * @param aServiceId service id
         * @return Error code.
         */
        IMPORT_C TInt DisableService( TInt aServiceId );
        
                
        /**
         * Get supported subservices
         *
         * @since S60 v5.0
         * @param aServiceId service id
         * @param aSupSubServices supported subservices
         */
        IMPORT_C void SupportedSubServicesL( 
                TInt aServiceId, 
                TSupportedSubServices& aSupSubservices );
        
        
        /**
         * Get cch connection parameter (RBuf)
         *
         * @since S60 v5.0
         * @param aServiceId service id
         * @param aConnParam cch connection parameter
         * @param aConnParamValue connection parameter value
         * @return Error code.
         */
        IMPORT_C TInt GetConnectionParameter( 
            TInt aServiceId, 
            TCchConnectionParameter aConnParam,
            RBuf& aConnParamValue );
        
        
        /**
         * Get cch connection parameter( TInt )
         *
         * @since S60 v5.0
         * @param aServiceId service id
         * @param aConnParam cch connection parameter
         * @param aConnParamValue connection parameter value
         * @return Error code.
         */
        IMPORT_C TInt GetConnectionParameter( 
            TInt aServiceId, 
            TCchConnectionParameter aConnParam,
            TInt& aConnParamValue );
        
        
        /**
         * Set cch connection parameter
         *
         * @since S60 v5.0
         * @param aServiceId service id
         * @param aConnParam cch connection parameter
         * @param aConnParamValue connection parameter value
         * @return Error code.
         */
        IMPORT_C TInt SetConnectionParameter( 
            TInt aServiceId, 
            TCchConnectionParameter aConnParam,
            const TDesC& aConnParamValue );
        
        
        /**
         * For checking if service is disabled
         *
         * @since S60 v3.2
         * @param aServiceId service id
         * @return ETrue if service is disabled
         */
        IMPORT_C TBool IsServiceDisabled( TInt aServiceId );
               
        
        /**
         * For checking if service is valid (some subservice(s) is supported)
         *
         * @since S60 v5.0
         * @param aServiceId service id
         * @return ETrue if service is valid
         */
        IMPORT_C TBool IsServiceValidL( TInt aServiceId );
        
    private:

        CCSCEngCCHHandler( 
            MCSCEngCCHObserver& aObserver );

        void ConstructL();
        
        
        /**
         * Returns ETrue if subservice is disabled
         * 
         * @since S60 v5.0
         * @param aServiceId Service id.
         * @param aType Subservice type.
         * @return ETrue if subservice is disabled
         */
        TBool IsDisabled( TInt aServiceId, TCCHSubserviceType aType );
        
        
        // from base class MCchServiceStatusObserver
        
        /**
         *  From MCchServiceStatusObserver
         * 
         * @since S60 v5.0
         * @param aServiceId Service id.
         * @param aType Subservice type.
         * @return aServiceStatus service status.
         */
        void ServiceStatusChanged( 
            TInt aServiceId, 
            TCCHSubserviceType aType, 
            const TCchServiceStatus& aServiceStatus );
        
    private:  // data
        
        /**
         * Reference for observer.
         */
        MCSCEngCCHObserver& iObserver;
               
        /**
         * Handle to CCH Client API.
         * Own.
         */
        CCch* iCchClientApi;
        
#ifdef _DEBUG
    friend class UT_CSCEngCchHandler;
#endif 
    };

#endif // C_CSCENGCCHHANDLER_H
