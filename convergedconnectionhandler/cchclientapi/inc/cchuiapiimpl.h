/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_CCHUIAPIIMPL_H
#define C_CCHUIAPIIMPL_H

#include <cch.h>
#include <e32base.h>
#include <cchuiobserver.h>

#include "mcchuinoteresultobserver.h"

class CCchUiSpsHandler;
class CCchUiCchHandler;
class CCCHUiNoteHandler;
class CCchUiClientObserver;
class CCchUiConnectionHandler;

/**
 *  CchUiApiImpl class
 *  Implementation class for CchUiApi.
 *  Do the actual things of CchUiApi
 *
 *  @since @since S60 5.0
 */
NONSHARABLE_CLASS( CCchUiApiImpl ) : public CBase,
                                     public MCchUiNoteResultObserver,
                                     public MCchServiceStatusObserver
    {
public:
    
    /**
     * Two-phased constructor.
     * @param aObserver reference to observer insterested in CCH UI events
     */
    static CCchUiApiImpl* NewL( CCch& aCch );

    /**
     * Destructor.
     */
    virtual ~CCchUiApiImpl();
       
    /**
     * Adds observer for listening cchui events.
     *
     * @since S60 5.0
     * @param aObserver Event observing class
     */  
    void AddObserverL( MCchUiObserver& aObserver );
    
    /**
     * Removes the observer of cchui events.
     *
     * @since S60 5.0
     * @param aObserver Event observing class
     */ 
    void RemoveObserver( MCchUiObserver& aObserver );
    
    /**
     * Shows specific dialog or note.
     *
     * @since S60 5.0
     * @param aServiceId service id of the service that the
     *        note is shown for. Service id is used to fetch
     *        service specific data. 
     * @param aDialog specifies dialog to be shown
     */        
    void ShowDialogL( 
        TUint32 aServiceId, 
        MCchUiObserver::TCchUiDialogType aDialog );
       
    /**
     * Configures visualization params.
     *
     * @since S60 5.0
     * @param aAllowedNotes array of allowed notes
     * @param aAllowedSubServices allowed sub services
     * @return None
     */            
    void ConfigureVisualizationL( 
        RArray<MCchUiObserver::TCchUiDialogType>& aAllowedNotes,
        RArray<TCCHSubserviceType>& aAllowedSubServices );
       
    /**
     * Cancels all notes. Same behaviour can be achieved by deleting
     * this instance. Client can use this method to dismiss note and keep this 
     * API instance in order to show some other notes in the future.
     * If there is nothing to dismiss, call to this method does nothing.
     *
     * @since S60 5.0
     */
    void CancelNotes();    
        
    /**
     * Starts observing events for specific service defined by aServiceId
     *
     * @since S60 5.0
     * @param aService, service identifier
     * @param aEnableResult, enable result error code
     */    
    void ManualEnableResultL( TUint aServiceId, TInt aEnableResult );
    
    
// from base class MCchUiNoteResultObserver
    
    /**
     * From MCchUiNoteResultObserver.
     * Called when certain dialog has completed without errors.
     *
     * @since S60 5.0
     * @param aCompleteCode Complete code.
     * @param aResultParams Result params of the dialog.
     */
    void DialogCompletedL( 
        TInt aCompleteCode, 
        TCCHUiNotifierParams aResultParams );
            
// from base class MCchServiceStatusObserver    

    /**
     * Observer implementation for service specific events.
     * by aServiceId.
     *
     * @since S60 5.0
     * @param aServiceId service id
     * @param aType sub service type
     * @param aServiceStatus service status
     */ 
    void ServiceStatusChanged( 
        TInt aServiceId, 
        TCCHSubserviceType aType, 
        const TCchServiceStatus& aServiceStatus );
    
protected: // constructor

    CCchUiApiImpl();
    
    void ConstructL( CCch& aCch );
    
protected: // functions    
    /**
     * Handles authentication failed dialog result.
     *
     * @since S60 5.0
     * @param aCompleteCode Complete code of the dialog.
     * @param aResultParams Result parameters from the dialog.
     */    
    void DoHandleAuthenticationFailedCompleteL( 
        TInt aCompleteCode, 
        TCCHUiNotifierParams aResultParams );
                
    /**
     * Handles no connections dialog result.
     *
     * @since S60 5.0
     * @param aCompleteCode Complete code of the dialog.
     * @param aResultParams Result parameters from the dialog.
     */          
    void DoHandleNoConnectionsCompleteL( 
        TInt aCompleteCode, 
        TCCHUiNotifierParams aResultParams );
    
    /**
     * Handles change connection confirmation note result.
     *
     * @since S60 5.0
     * @param aCompleteCode Complete code of the dialog.
     * @param aResultParams Result parameters from the dialog.
     */          
    void DoHandleConfirmChangeConnectionCompleteL( 
        TInt aCompleteCode, 
        TCCHUiNotifierParams aResultParams );
    
    /**
     * Handles change connection query result.
     *
     * @since S60 5.0
     * @param aCompleteCode Complete code of the dialog.
     * @param aResultParams Result parameters from the dialog.
     */          
    void DoHandleChangeConnectionCompleteL( 
        TInt aCompleteCode, 
        TCCHUiNotifierParams aResultParams );
            
    /**
     * Handles service specific error.
     *
     * @since S60 5.0
     * @param aServiceId service id
     * @param aType subservice type
     * @param aServiceStatus service status
     */                      
    void DoHandleServiceErrorL( 
        TInt aServiceId, 
        TCCHSubserviceType aType,
        const TCchServiceStatus& aServiceStatus );

    /**
     * Handles service specific status event.
     *
     * @since S60 5.0
     * @param aServiceId service id
     * @param aType subservice type
     * @param aServiceStatus service status
     */                      
    void DoHandleServiceEventL( 
        TInt aServiceId, 
        TCCHSubserviceType aType,
        const TCchServiceStatus& aServiceStatus );   
    
    /**
     * Handles search wlan dialog result.
     *
     * @since S60 5.0
     * @param aCompleteCode Complete code of the dialog.
     * @param aResultParams Result parameters from the dialog.
     */   
    void HandleSearchWlanCompleteL( 
        TInt& aCompleteCode, 
        TCCHUiNotifierParams& aResultParams );
    
    /**
     * Handles search access points error code.
     *
     * @since S60 5.0
     * @param aErr error code to handle.
     * @param aCompleteCode Complete code of the dialog.
     * @param aResultParams Result parameters from the dialog.
     */  
    void HandleSearchAccessPointsErrorL( 
        TInt aErr,
        TInt& aCompleteCode, 
        TCCHUiNotifierParams& aResultParams );
    
    /**
     * Handles search wlan dialog result when use gprs selected.
     *
     * @since S60 5.0
     * @param aCompleteCode Complete code of the dialog.
     * @param aResultParams Result parameters from the dialog.
     */   
    void HandleCopyGprsCompleteL( 
        TInt& aCompleteCode, 
        TCCHUiNotifierParams& aResultParams );
    
    /**
     * Handle changing to new connection.
     *
     * @since S60 5.0
     * @param aCompleteCode Complete code of the dialog.
     * @param aResultParams Result parameters from the dialog.
     */   
    void HandleChangeConnectionL( 
        TInt& aCompleteCode, 
        TCCHUiNotifierParams& aResultParams );
    
    /**
     * Informs event to all observers.
     *
     * @since S60 5.0
     * @param aServiceId service id.
     */                                 
    void InformObserversL( TInt aServiceId );
            
    /**
     * Checks if specific dialog is allowed. If client has not configured,
     * all dialogs are allowed.
     *
     * @since S60 5.0
     * @param aDialog dialog to be checked.
     * @return TBool, ETrue if aDialog is allowed
     */                                         
    TBool DialogIsAllowed( MCchUiObserver::TCchUiDialogType aDialog );
    
    /**
     * Checks if specific sub service is allowed. If client has not configured,
     * only voip sub service is allowed.
     *
     * @since S60 5.0
     * @param aSubService sub service to be checked.
     * @return TBool, ETrue if aSubService is allowed
     */                                     
    TBool SubServiceIsAllowed( TCCHSubserviceType aSubService );
    
    /**
     * Checks subservice state is allowed for showing dialogs.
     *
     * @since S60 5.0
     * @param aState sub service state
     * @return TBool, ETrue if state is allowed for showing dialogs
     */                                     
    TBool StateIsAllowed( const TCCHSubserviceState aState );
    
    /**
     * Handles re-enabling of service.
     *
     * @since S60 5.0
     * @param aServiceId service id
     */  
    void HandleServiceReEnablingL( TUint aServiceId );
    
    /**
     * Handles manual enable errors.
     *
     * @since S60 5.0
     * @param aService, service identifier
     * @param aEnableResult, enable result error code
     */    
    void HandleManualEnableErrorL( TUint aServiceId, TInt aEnableResult );
           
private: // data

    /**
     * Array of encapsulated observers (clients)
     * Own.
     */
	RPointerArray<CCchUiClientObserver> iObservers;

    /**
     * CCH handler.
     * Own.
     */
	CCchUiCchHandler* iCCHHandler;

    /**
     * Controller class for dialogs.
     * Own.
     */
    CCCHUiNoteHandler* iNoteController;
    
    /**
     * Service settings handler.
     * Own.
     */
    CCchUiSpsHandler* iSpsHandler;    
    
    /**
     * Connection handler. Handles iaps & snaps.
     * Own.
     */
    CCchUiConnectionHandler* iConnectionHandler;
        
    /**
     * Own. Stores service ids of the monitored services
     */        
    RArray<TUint> iObservervedServices;
    
    /**
     * Own. Stores allowed notes (client provided)
     */            
    RArray<MCchUiObserver::TCchUiDialogType> iAllowedNotes;
    
    /**
     * Own. Stores allowed sub services (client provided)
     */            
    RArray<TCCHSubserviceType> iAllowedSubServices; 
    
    /**
     * Flag for indicating if re-enabling of service needs  
     * after service disabled event.
     */  
    TBool iReEnableService;
    
    /**
     * For iap id which can be removed when change connection 
     * performed succesfully.
     */  
    TInt iRemovableConnection;
    
    /**
     * Service which we have added cch observer.
     */  
    TInt iObservedService;
    
    /**
     * Stores last cchui operation result.
     */  
    MCchUiObserver::TCchUiOperationResult iLastOperationResult;
    
    CCHUI_UNIT_TEST( UT_CchUi )
    };

#endif  // C_CCHUIAPIIMPL_H


