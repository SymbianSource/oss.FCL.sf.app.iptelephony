/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  For CSC needed Service Provider Settings handling
*
*/



#ifndef C_CSCENGSERVICEHANDLER_H
#define C_CSCENGSERVICEHANDLER_H

#include <mspnotifychangeobserver.h>

class CSPEntry;
class CSPProperty;
class CSPSettings;
class CSPNotifyChange;
class CSPSettingsVoIPUtils;
class MCSCEngServiceObserver;

/**
 * An instance of CCSCEngServiceHandler
 * For CSC needed Service Provider Settings handling
 *
 * @lib cscengine.lib
 * @since Series 60 3.2
 */
NONSHARABLE_CLASS ( CCSCEngServiceHandler ) : public CBase,
                                              public MSPNotifyChangeObserver
    {    
    public:
        
        /**
         * Two-phased constructor.
         */ 
        IMPORT_C static CCSCEngServiceHandler* NewL( 
            MCSCEngServiceObserver* aObserver );


        /**
         * Destructor.
         */
        IMPORT_C virtual ~CCSCEngServiceHandler();


        /**
         * Get all service ids
         *
         * @since Series 60 3.2
         * @param aServiceIds service id array
         * @return error code
         */
        IMPORT_C void GetAllServiceIdsL( RArray<TUint>& aServiceIds );
        
        
        /**
         * Get stored service provider settings count. 
         *
         * @since Series 60 3.2
         * @return count of stored service provider settings
         */
        IMPORT_C TInt ServiceIdCountL();
        
        
        /**
         * Delete service.
         *
         * @since Series 60 3.2
         * @param aServiceId service id
         */
        IMPORT_C void DeleteServiceL( TUint aServiceId );
        
                
        /**
         * Get service name by service id
         *
         * @since Series 60 3.2
         * @param aServiceId service id
         * @return service name
         */
        IMPORT_C const TDesC16& ServiceNameL( TUint aServiceId );
        
        
        /**
         * Get brand identifier by service id
         *
         * @since Series 60 3.2
         * @param aServiceId service id
         * @param aBrandId brand id is stored to this
         */
        IMPORT_C void BrandIdL( TUint aServiceId, TDes16& aBrandId );
        
        
        /**
         * Get snap id by service id
         *
         * @since Series 60 3.2
         * @param aServiceId service id
         * @return snap id
         */
        IMPORT_C TInt SnapIdL( TUint aServiceId );
        
            
        /**
         * Get service setup plug-in id by service id
         *
         * @since Series 60 3.2
         * @param aServiceId service id
         * @return service setup plug-in id
         */
        IMPORT_C TInt32 ServiceSetupPluginIdL( TUint aServiceId );
        
        /**
         * Get service phonebook tab view id by service id
         *
         * @since Series 60 3.2
         * @param aServiceId service id
         * @return service tab view id
         */
        IMPORT_C TInt ServiceTabViewIdL( TUint aServiceId );
                
        /**
         * Get IM settings id.
         *
         * @since Series 60 5.0
         * @param aServiceId service id
         * @return service IM settings id
         */
        IMPORT_C TInt ImSettingsIdL( TUint aServiceId );
        
        /**
         * Set IM settings id.
         *
         * @since Series 60 5.0
         * @param aServiceId service id
         * @param aSettingsId service IM settings id
         */
        IMPORT_C void SetImSettingsIdL(
            TUint aServiceId, 
            TInt aSettingsId );      
        
        /**
         * Get service큦 presence request preference setting
         *
         * @since Series 60 3.2
         * @param aServiceId service id
         * @return EOn when set
         */
        IMPORT_C TOnOff PresenceReqPrefL( TUint aServiceId );
        
        
        /**
         * Set service큦 presence request preference setting
         *
         * @since Series 60 3.2
         * @param aServiceId service id
         * @param aOnOff EOn when set
         */
        IMPORT_C void SetPresenceReqPrefL( 
                TUint aServiceId, TOnOff aOnOff );
        
        /**
         * Get service큦 handover notification tone preference setting
         *
         * @since Series 60 5.0
         * @param aServiceId service id
         * @return EOn when set
         */
        IMPORT_C TOnOff HandoverNotifTonePrefL( TUint aServiceId );
        
        /**
         * Set service큦 handover notification tone preference setting
         *
         * @since Series 60 5.0
         * @param aServiceId service id
         * @param aOnOff EOn when set
         */
        IMPORT_C void SetHandoverNotifTonePrefL( 
                TUint aServiceId, TOnOff aOnOff );
        
        /**
         * Find out if service supports VCC (Voice Call Continuity).
         *
         * @since Series 60 5.0
         * @param aServiceId service id
         * @return ETrue if service supports VCC
         */
        IMPORT_C TBool IsVccSupportedL( TUint aServiceId );
        
        /**
         * Find out if service supports SIP/VoIP
         *
         * @since Series 60 3.2
         * @param aServiceId service id
         * @return ETrue if service supports SIP/VoIP
         */
        IMPORT_C TBool IsSipVoIPL( TUint aServiceId );
        
        /**
         * Find out if service is the preferred service
         *
         * @since Series 60 3.2
         * @param aServiceId service id
         * @return ETrue if service is preferred
         */
        IMPORT_C TBool IsPreferredService( TUint aServiceId ) const;
        
        /**
         * Find out if voip (PS) is the preferred telephony
         *
         * @since Series 60 3.2
         * @return ETrue if voip is preferred
         */
        IMPORT_C TBool IsPreferredTelephonyVoip() const;
       
       	/**
	     * Sets the preferred telephony value
	     *
	     * @since S60 v3.2
	     * @param aValue, ETrue means that voip (PS) will be preferred.
	     * @param aServiceId, Preferred service id.
	     * @return KErrNone if succeed.
	     */
	    IMPORT_C TInt SetVoipAsPrefTelephony( 
	            TBool aValue, TUint aServiceId ) const; 
	    
    private:

        CCSCEngServiceHandler();

        void ConstructL( MCSCEngServiceObserver* aObserver );
	    
	    /**
	     * Sets the preferred telephony value
	     * Leaves if value cannot be set.
	     *
	     * @since S60 v3.2
	     * @param aValue, ETrue means that voip (PS) will be preferred.
	     * @param aServiceId, Preferred service id.
	     */
	    void SetVoipAsPrefTelephonyL( 
	            TBool aValue, TUint aServiceId ) const;       
	    
        
        // from base class MSPNotifyChangeObserver
      
        /**
         * From MSPNotifyChangeObserver
         * Notifies change in service table
         *
         * @param aServiceId service id
         * @since S60 v3.2
         */
        void HandleNotifyChange( TUint aServiceId );
        
        /**
         * From MSPNotifyChangeObserver
         * Notifies error
         *
         * @param aServiceId service id
         * @since S60 v3.2
         */
        void HandleError( TInt /*aError*/ ){};
        
              
    private:  // data
        
        /**
         * Pointer to CSPSettings
         * Own.
         */
        CSPSettings* iServiceSettings;
        
        /**
         * Pointer to CSPNotifyChange
         * Own.
         */
        CSPNotifyChange* iNotifier;

        /**
         * Pointer to CSPSettingsVoIPUtils
         * Own.
         */        
        CSPSettingsVoIPUtils* iSettingsVoIPUtils;
        
        /**
         * Service observer
         */
        MCSCEngServiceObserver* iObserver;
        
        /**
         * Name of settings entry ( unique )
         * Own.
         */
        RBuf iServiceName;
        
#ifdef _DEBUG
    friend class UT_CSCEngServiceHandler;
#endif  
    };

#endif // C_CSCENGSERVICEHANDLER_H
