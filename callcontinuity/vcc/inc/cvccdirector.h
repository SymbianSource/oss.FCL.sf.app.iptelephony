/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Voice Call Continuity proxy
*
*/

#ifndef CVCCDIRECTOR_H
#define CVCCDIRECTOR_H



#include <cconvergedcallprovider.h>
#include <mccpobserver.h>
#include <mccpcsobserver.h>  // MCCPCSObserver
#include <mspnotifychangeobserver.h>
#include <mccpcall.h>
#include <cccpcallparameters.h>

#include "mvccengpspropertylistenerobserver.h"
#include "ccpdefs.h"

//state machine, in order to maintain only one instance
#include "tvccstateinit.h"
#include "tvccstatecalling.h"
#include "tvccstatereleasing.h"
#include "tvccstatefailing.h"

class CVccPerformer;
class CVccHoTrigger;
class CVccEngPsPropertyListener;
class CVccEngPsProperty;
class CSPNotifyChange;
class CVccConferenceCall;
class CVccDtmfProvider;

/**  Service Provider Name */
_LIT( KCSPName,"VCC" );

/**  VCC implementation UID */
const TInt KVCCImplementationUidInt = 0x2000CFAA;

/**
* Implementation uid of VCC Plugin.
*/
const TUid KVCCImplementationUid = { KVCCImplementationUidInt };

/**
 *  
 *
 * Proxies all calls to the relevant call providers
 * and keeps the house hold.
 *
 *  @lib svp.dll
 *  @since S60 3.2 
 */
class CVccDirector : public CConvergedCallProvider,
                     public MCCPCSObserver,
                     public MVccEngPsPropertyListenerObserver,
                     public MSPNotifyChangeObserver

    {


public:

    /**
    * Two-phased constructor.
    */
    static CVccDirector* NewL();
  
    /**
    * Destructor.
    */
    ~CVccDirector();
       
    /**
    * Gets the instance of the MCCPMonitor
    * @return Reference to MCCPMonitor
    */
    const MCCPCSObserver* CPPObserver() const;
    
// from base class CConvergedCallProvider 
    
   	/**
    * From CConvergedCallProvider
    * Initializes plug-in. All required parameters must be passed
    * with this function in order to use services.
    * Simplifies pluging construct removing need to pass parameters
    * to ECom interface
    * 
    * @since S60 v3.2
    * @param aMonitor CCP observer
    * @param aSsObserver SS observer
    */
    void InitializeL( const MCCPObserver& aMonitor,
                      const MCCPSsObserver& aSsObserver );

    /**
    * From CConvergedCallProvider
    * Creates a new session and checks secure status from profile.
    * 
    * @since S60 v3.2
    * @param aParameters Call parameters, SVP uses Service ID parameter
    * @param aRecipient Recipients address/number
    * @param aObserver Observer
    * @return Reference to created MCCPCall object.
    */
    MCCPCall* NewCallL( const CCCPCallParameters& aParameters,
					    const TDesC& aRecipient,
					    const MCCPCallObserver& aObserver );
                                
    /**
    * From CConvergedCallProvider
    * Returns Uid of plug-in which is on use.
    * 
    * @since S60 v3.2
    * @return Uid of plug-in
    */                        
    const TUid& Uid() const;
                           
    /**
    * From CConvergedCallProvider
    * Creates a new Emergency call and add user-agent header.
    * 
    * @since S60 v3.2
    * @param aServiceId Service ID, obsolete!
    * @param aAddress Emergency number
    * @param aObserver Observer
    * @return MCCPEmergencyCall
    */
    MCCPEmergencyCall* NewEmergencyCallL( const TUint32 aServiceId,
                                          const TDesC&  aAddress,
                                          const MCCPCallObserver& aObserver );
                                          
    /**
    * From CConvergedCallProvider
    * Creates a new conference session.
    * 
    * @since S60 v3.2
    * @param aObserver Observer
    * @return MCCPConferenceCall
    */
    MCCPConferenceCall* NewConferenceL( const TUint32 aServiceId,
                              const MCCPConferenceCallObserver& aObserver );
                             
    /**
    * From CConvergedCallProvider
    * Releases call
    * 
    * @since S60 v3.2
    * @param aCall Call to be removed
    * @return Symbian error code
    */
    TInt ReleaseCall( MCCPCall& aCall );

    /**
    * From CConvergedCallProvider
    * Releases emergency call
    * 
    * @since S60 v3.2
    * @param aCall Call to be removed
    * @return Symbian error code
    */
    TInt ReleaseEmergencyCall( MCCPEmergencyCall& aCall );

    /**
    * From CConvergedCallProvider
    * Releases conference call
    * 
    * @since S60 v3.2
    * @param aCall Call to be removed
    * @return Symbian error code
    */
    TInt ReleaseConferenceCall( MCCPConferenceCall& aCall );

    /**
    * From CConvergedCallProvider
    * Get Plug-In capabilities
    * 
    * @since Series 60 v3.2
    * @return TUint32 On return contains the capability flags of plug-In
    */
    TUint32 Caps() const;
    
	/**
    * From CConvergedCallProvider
	* Get DTMF provider
    * 
	* @since Series 60 v3.2
	* @param aObserver CCP Dtmf observer for informing events
	* @return Pointer to MCCPDTMFProvider if succesfull,
    * NULL if not available
	*/
	MCCPDTMFProvider* DTMFProviderL( const MCCPDTMFObserver& aObserver );

	/**
    * From CConvergedCallProvider
	* Get extension provider
    * 
	* @since Series 60 v3.2
    * @param aObserver observer for extension(custom) events
	* @return Pointer to MCCPExtensionProvider if succesfull,
    * NULL if not available
	*/
	MCCPExtensionProvider* ExtensionProviderL(
            const MCCPExtensionObserver& aObserver );

    /**
    * From CConvergedCallProvider
    * Add an observer for DTMF related events.
    * Plug-in dependent feature if duplicates or more than one observers 
    * are allowed or not. Currently CCE will set only one observer.
    * 
    * @since S60 v3.2
    * @param aObserver Observer
    * @return none
    * @leave system error if observer adding fails
    */
    void AddObserverL( const MCCPDTMFObserver& aObserver );

    /**
    * From CConvergedCallProvider
    * Remove an observer.
    * 
    * @since S60 v3.2
    * @param none
    * @param aObserver Observer
    * @return KErrNone if removed succesfully. KErrNotFound if observer
    * was not found.
    * Any other system error depending on the error.
    */
    TInt RemoveObserver( const MCCPDTMFObserver& aObserver );

    /**
    * @see CConvergedCallProvider::GetLifeTime
    */ 
    TBool GetLifeTime( TDes8& aLifeTimeInfo );
    
    /**
    * @see CConvergedCallProvider::GetCSInfo
    */ 
    TBool GetCSInfo( CSInfo& aCSInfo );
    
// from base class MCCPCSObserver

        
   
    
    
// from base class MCCPObserver (inhereded by MCCPCSObserver)

    /**
     * From MCCPObserver (inhereded by MCCPCSObserver)
     * 
     * @see MCCPObserver::ErrorOccurred
     */
    
    void ErrorOccurred( TCCPError aError );

    /** From MCCPObserver (inhereded by MCCPCSObserver)
    * 
    * @see MCCPObserver::IncomingCall
    */
    
    void IncomingCall( MCCPCall* aCall, MCCPCall& aTempCall );

    /**
     * From MCCPObserver (inhereded by MCCPCSObserver)
     * 
     * 
     * @see MCCPObserver::IncomingCall
     */
    
    void IncomingCall( MCCPCall* aCall );

    /**
     * The actual implementation for IncomingCall.
     */

    void IncomingCallL( MCCPCall* aCall );
    
    /**
     * From MCCPObserver (inhereded by MCCPCSObserver)
     * 
     * @see MCCPObserver::CallCreated
     */
    
    void CallCreated( MCCPCall* aCall,
            MCCPCall* aOriginator,
            TBool aAttented );

    /**
     * From MCCPCSObserver
     * see MCCPCSObserver::PluginInitiatedCSMoCallCreated
     */
     //void PluginInitiatedCSMoCallCreated( MCCPCall* aCall ) const;
     void MoCallCreated( MCCPCall& aCall );

     /**
      * Notify of conference call that is not requested.
      * @since S60 3.2
      * @param aConferenceCall Created conference
      * @return none
      */
     void ConferenceCallCreated( MCCPConferenceCall& aConferenceCall );
    
    /**
     * From MCCPObserver (inhereded by MCCPCSObserver)
     *
     * @see MCCPObserver::DataPortName
     */
    void DataPortName( TName& aPortName );
    
// from base class MVccEngPsPropertyListenerObserver
    
    /**
     * From MVccEngPsPropertyListenerObserver
     * Observer interface. Called when a P&S property value 
     * has been updated.
     * 
     * @since S60 v3.2
     * @param aCategoryId The category uid of the property
     * @param akeyId The key uid of the property
     * @param aValue Current value of the property.
     */ 
    void PropertyChangedL(
            const TUid aCategoryId,
            const TUint aKeyId,
            const TInt aValue );

//from base class MSPNotifyChangeObserver
    
    /**
     * From MSPNotifyChangeObserver.
     * Handle notify change event from the service provider table observer.
     * 
     * @param aServiceId the service ID of added/changed/deleted service
     * ( only notification about the changes to VCC service are revceived) 
     */
    void HandleNotifyChange( TServiceId aServiceId );
    
    /**
     * From MSPNotifyChangeObserver.
     * Handle error notification received from the service provider table 
     * observer.
     * 
     * @param aError error code
     * @since S60 3.2
     */
    void HandleError( TInt aError );
    
    /**
     * Gets the call type of the currently active call from the performer array.
     * 
     * @return Call type of the currently active call, KErrNotFound if no active call
     * @since S60 3.2
     */    
    TInt CurrentCallTypeForDTMF();
    
    /**
     * Gets the call type of the currently active call and returns it's provider.
     * 
     * @return CConvergedCallProvider of the active call
     * @since S60 5.0
     */   
    CConvergedCallProvider* GetProvider();
    
private:

    /**
    * C++ default constructor.
    */
    CVccDirector();
    
    /**
    * Symbian second-phase constructor
    */    
    void ConstructL();
    
	/**
	* for destroying an array
	*/    
    static void PointerArrayCleanup( TAny* aArray );
    
    /**
 	* Creates call provider array 
 	*/    
    void FillCallProviderArrayL();

    /**
  	* Checks if given plugin is used by VCC 
	* 
	* @aparam aPluginId	call provider plugin id
	* @return ETrue if plugin is used by VCC
  	*/    
    TBool IsVccOwnedPlugin( TInt aPluginId );
    
    /**
   	* Tries to initialize all call provider plugins VCC uses.  
 	* 
 	* @param aMonitor CCP observer
    * @param aSsObserver SS observer
 	* @return ETrue if all plugins were initialized
   	*/  
    TBool Initialize( const MCCPObserver& aMonitor,
            const MCCPSsObserver& aSsObserver );
    
    /**
    * Tries to initialize the given call provider plugin. 
    * Leaves if initialization fails.
  	* 
  	* @param aMonitor CCP observer
    * @param aSsObserver SS observer
    * @param aPlugin	call provider plugin
    */  
    void InitializeL( const MCCPObserver& aMonitor,
            const MCCPSsObserver& aSsObserver, 
            CConvergedCallProvider& aPlugin );
    
    /**
     *  for checking state of calls and making handover
     */
    void SwitchL( const TInt aValue );
    
    /**
     *  Checks are converged call provider plugins initialized
    */
    TBool IsPluginInitialized();
    
    /*
     * Starts notifications from service provider table observer.
     * Is notified about VCC service settings changes. 
     */
    void StartSpNotifierL();

    /*
     * Stops notifications from service provider table observer. 
     */
    void StopSpNotifierL();
    
    /*
     * Triesto initialize call provider plugins used by VCC that
     * were not initialized .
     * 
     * */
    void RetryInitialization();
    
    /*
     * The leaving-part implementation on ConferenceCallCreated.
     * @since S60 3.2
     * @param aConferenceCall Created conference
     */
    void CreateConferenceL( MCCPConferenceCall& aConferenceCall );
    
   
    
private: // data

    /**
     * Implementation UID
     */
    TUid iImplementationUid;
    
    /**
     * Array of Call providers
     * Own.
     */    
    RPointerArray <CConvergedCallProvider> iProviders;
    
    /*
     * VCC needs to handle both cs and ps calls.
     * MCCPCSObserver can do both
     * Not own.
     */ 
    MCCPCSObserver* iCCPObserver;
    
    /*
     *  Supplementary service observer.
     * Not own.
     */
    MCCPSsObserver* iCCPSsObserver;
    
     /*
     * Handle to handover trigger.
     * Own.
     */
    CVccHoTrigger* iHoTrigger;
    
    /**
     * Array of Performers
     * Own.
     */   
   	RPointerArray<CVccPerformer> iPerfArray;
    
    /*
     * Handle to Conference call object.
     * Own.
     */
    CVccConferenceCall* iConference;
   	
    /*
     * Listener for the hand-over commands
     * Own.
     */
   	CVccEngPsPropertyListener* iPropListener; 
   	
   	//VCC state machine implementation
   	TVccStateInit iStateInit;
	TVccStateCalling iStateCalling;
	TVccStateReleasing iStateReleasing;
	TVccStateFailing iStateFailing;
    
	/**
     * Used for checking in ReleaseCall, that was this ReleaseCall due to
     * multicall handover situation
     */ 
	TInt iHoKeyValue;
	
	/**
     * Notifies service provider setting change
     * Own
     */
    CSPNotifyChange* iSpNotifier;
    
    /*
     * Array of initialized plugins
     * Own.
     */    
    RArray<TInt> iInitialisedPlugins;
    
    /**
     * Proxy object for DTMF providers
     * Own.
     */
    CVccDtmfProvider* iDtmfProvider;
    
     
	friend class T_CVccDirector;
    friend class T_CVccDTMFProvider;
	};

#endif // CVCCDIRECTOR_H
