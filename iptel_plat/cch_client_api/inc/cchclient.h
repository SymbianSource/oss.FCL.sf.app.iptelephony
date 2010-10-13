/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  RCCHClient declaration
*
*/


#ifndef R_CCHCLIENT_H
#define R_CCHCLIENT_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "CCHServer.h"
#include "CCHClientServer.h"

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION
    
/**
 *  RCCHClient declaration.
 *  A CCH sub-session object. This sub-session provides the functionality
 *  for CCH.
 *  @lib cchclient.lib
 *  @since S60 3.2
 */
class RCCHClient : public RSubSessionBase
    {

public: // Constructors and destructor
    
    /**
     * C++ default constructor
     */
    IMPORT_C RCCHClient();
    
    /**
     * Destructor.
     */
    IMPORT_C virtual ~RCCHClient();

public: // New functions

    /**
     * Opens a new sub-session to CCH within an existing session.
     * @since S60 3.2
     * @param aRCCHServer Open CCH session
     * @return General Symbian error code
     */
    IMPORT_C TInt Open( const RCCHServer& aRCCHServer );
    
    /**
     * Close sub-session to CCH server
     * @since S60 3.2
     */
    IMPORT_C void Close();
    
    /**
     * Register to server
     * If CCH server crashes, Symbian will complete this message with KErrServerTerminated,
     * CCH client is possible to create new handle to CCH server
     * @since S60 5.0
     * @param aReqStatus KErrNone if successful, otherwise another of the 
     * system-wide error codes
     */
    IMPORT_C void RegisterToServer( TRequestStatus& aReqStatus ) const;

    /**
     * Cancel register server request
     * @since S60 5.0
     */
    IMPORT_C void RegisterToServerCancel( ) const;
    
    /**
     * Subscribe to specific service events, if any service's subservices 
     * status has changed, CCH sends notify to the client. 
     * @since S60 3.2
     * @param aServiceSelection Which service's or type of subservices
     * client wants to be notified, if aServiceSelection iServiceId is zero and
     * iType is ECCHUnknown then all subservice events are selected
     * @param aServiceStatus On completion contains selected service's status
     * @param aReqStatus KErrNone if successful, otherwise another of the 
     * system-wide error codes
     */
    IMPORT_C void SubscribeToEvents( const TServiceSelection& aServiceSelection,
                                     TPckgBuf<TServiceStatus>& aServiceStatus,
                                     TRequestStatus& aReqStatus ) const;
                                     
    /**
     * Cancel all subscribe event request.
     * @since S60 3.2
     */
    IMPORT_C void SubscribeToEventsCancel() const;
      
    /**
     * Get services. Client queries for all services and sub-services, 
     * network scan is performed.
     * @since S60 3.2
     * @param aServices On completion contains all services and subservices
     * @param aReqStatus KErrNone if successful, otherwise another of the 
     * system-wide error codes
     */
    IMPORT_C void GetServicesL( CArrayFixFlat<TCCHService>& aServices,
                                TRequestStatus& aReqStatus );
    
    /**
     * Get services. Client queries for certain services or type of 
     * subservices, network scan is not performed.
     * @since S60 3.2
     * @param aServiceSelection Which service's or type of subservices
     * client wants to get, if aServiceSelection iServiceId is zero and 
     * iType is ECCHUnknown then all services are selected
     * @param aServices On completion contains all selected service and 
     * subservices
     * @param aReqStatus KErrNone if successful, otherwise another of the 
     * system-wide error codes.
     */
    IMPORT_C void GetServicesL( const TServiceSelection& aServiceSelection,
                                CArrayFixFlat<TCCHService>& aServices,
                                TRequestStatus& aReqStatus );
                               
    /**
     * Cancel get services request.
     * @since S60 3.2
     */
     IMPORT_C void GetServicesCancel() const;
                                
    /**
     * Client asks for the state of service or subservice of a certain 
     * service. To perform operation quickly, network scan is not performed.
     * @since S60 3.2
     * @param aServiceSelection Which service's state client wants to get.
     * aServiceSelection's iServiceId and iType are compulsory
     * @param aState On completion contains selected service's state. 
     * @return General symbian error code
     */
    IMPORT_C TInt GetServiceState( const TServiceSelection& aServiceSelection,
                                   TCCHSubserviceState& aState ) const;
          
    /**
     * Enable services. All sub-services under the service are enabled.
     * @since S60 3.2
     * @param aServiceSelection Which service's or type of subservices
     * client wants to enable, aServiceSelection's iServiceId and iType 
     * are compulsory
     * @param aReqStatus KErrNone if successful, otherwise another of the 
     * system-wide error codes.
     * @param aConnectivityCheck If value is True Connectivity Plug-in will
     * make test call to check is the connection truly working.
     */
    IMPORT_C void EnableService( const TServiceSelection& aServiceSelection,
                                 TRequestStatus& aReqStatus,
                                 TBool aConnectivityCheck = EFalse ) const;
                                 
    /**
     * Cancel enable services request.
     * @since S60 3.2
     */
    IMPORT_C void EnableServiceCancel() const;                                 
                                 
    /**
     * Client disables service. All sub-services under the service are 
     * disabled.
     * @since S60 3.2
     * @param aServiceSelection Which service's or type of subservices
     * client wants to disable, aServiceSelection's iServiceId and iType 
     * are compulsory
     * @param aReqStatus KErrNone if successful, otherwise another of the 
     * system-wide error codes.
     */
    IMPORT_C void DisableService( const TServiceSelection& aServiceSelection,
                                  TRequestStatus& aReqStatus ) const;
                                  
    /**
     * Cancel disable service request.
     * @since S60 3.2
     */
    IMPORT_C void DisableServiceCancel() const;                                  
    
    /**
     * Get service specifics information about service and protocol
     * @since S60 3.2
     * @param aServiceSelection Which service's or type of subservices
     * client wants get service information, aServiceSelection's iServiceId 
     * and iType are compulsory
     * @param aBuffer On completion contains selected service's protocol
     * information, aBuffer's max length is in KCCHMaxProtocolNameLength
     * which is defined in CCHClientServer.h-file
     * @return General symbian error code.  
     */
    IMPORT_C TInt GetServiceInfo( const TServiceSelection& aServiceSelection,
                                  TDes& aBuffer ) const;
    
    /**
     * Client set's connection information of sub-service of a certain 
     * service.
     * @since S60 3.2
     * @param aServiceConnInfo Service where client wants to set connection
     * information. Note that iSNAPLocked is read only.
     * @param aReqStatus KErrNone if successful, otherwise another of the 
     * system-wide error codes.
     */
    IMPORT_C void SetConnectionInfo( 
        const TPckgBuf<TServiceConnectionInfo>& aServiceConnInfo,
        TRequestStatus& aReqStatus );
                                     
    /**
     * Cancel set connection information request.
     * @since S60 3.2
     */
    IMPORT_C void SetConnectionInfoCancel() const;                                     
                                     
    /**
     * Client ask connection information of subservice of a certain service.
     * @since S60 3.2
     * @param aServiceSelection Service which client wants to get connection 
     * information, aServiceSelection's iServiceId and iType are compulsory
     * @param aServiceConnInfo On completion contains selected service's 
     * connection information
     * @param aReqStatus KErrNone if successful, otherwise another of the 
     * system-wide error codes.
     */
    IMPORT_C void GetConnectionInfo( 
        const TServiceSelection& aServiceSelection,
        TPckgBuf<TServiceConnectionInfo>& aServiceConnInfo,
        TRequestStatus& aReqStatus ) const;
                                     
    /**
     * Cancel get connection information request.
     * @since S60 3.2
     */
    IMPORT_C void GetConnectionInfoCancel() const;
    
    /**
     * Client asks for exclusive reservation of the service e.g. to block 
     * ALR during a VoIP call
     * @since S60 3.2
     * @param aServiceSelection Which service client wants to reserve.
     * aServiceSelection's iServiceId and iType are compulsory
     * @param aReqStatus KErrNone if successful, otherwise another of the 
     * system-wide error codes.
     */
    IMPORT_C TInt ReserveService( const TServiceSelection& aServiceSelection );
    
    /**
     * Client asks to free the exclusive service reservation.
     * @since S60 3.2
     * @param aServiceSelection Which service client wants to free.
     * aServiceSelection's iServiceId and iType are compulsory
     * @param aReqStatus KErrNone if successful, otherwise another of the 
     * system-wide error codes.
     */
    IMPORT_C TInt FreeService( const TServiceSelection& aServiceSelection );
    
    /**
     * Returns service's reservation status.
     * @since S60 3.2
     * @param aServiceSelection Which service's reference count client wants 
     * to get.
     * aServiceSelection's iServiceId and iType are compulsory
     * @return Reservation status
     */
    IMPORT_C TBool IsReserved( const TServiceSelection& aServiceSelection ) 
                    const;
    
private:

    // Prohibit copy constructor if not deriving from CBase.
    RCCHClient( const RCCHClient& );

    // Prohibit assigment operator if not deriving from CBase.
    RCCHClient& operator=( const RCCHClient& );                                        
    
private:

    TPtr8* iService;   
    
    };

#endif // R_CCHCLIENT_H

// End of file
