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
* Description:  CCH Client / Server common interface header
*
*/



#ifndef T_CCHCLIENTSERVER_H
#define T_CCHCLIENTSERVER_H

// INCLUDES
#include <e32base.h>
#include <ccherror.h>
#include <cchtypes.h>
// CONSTANTS
const TUint KCCHMaxServiceNameLength  = 64; 
const TUint KCCHMaxSubservicesCount   = 4;
const TUint KCCHMaxProtocolNameLength = 512; 
const TUint KCCHMaxUsernameLength = 128;
const TUint KCCHMaxPasswordLength = 64;
  
  
// CLASS DECLARATION
class TServiceSelection
    {
public:
    inline TServiceSelection();
    inline TServiceSelection( TUint aServiceId, 
                      TCCHSubserviceType aType,
                      TCchConnectionParameter aParameter = ECchInitial );
public:
    /**
     * Service Id
     */
    TUint                   iServiceId;
    /**
     * Subservice's type
     */
    TCCHSubserviceType      iType;
    /**
     * Identifies operation target for get/set.
     */
    TCchConnectionParameter iParameter;
    };
    
class TServiceConnectionInfo
    {
public:
    inline TServiceConnectionInfo();
    inline TServiceConnectionInfo( TUint aServiceId, 
                                   TCCHSubserviceType aType, 
                                   TUint32 aIapId, 
                                   TUint32 aSNAPId );

    inline TServiceConnectionInfo( TUint aServiceId, 
                                   TCCHSubserviceType aType, 
                                   TUint32 aIapId, 
                                   TUint32 aSNAPId,
                                   TDesC& aUsername,
                                   TDesC& aPassword );
    inline TUint ServiceId() const;
    inline TCCHSubserviceType Type() const;
    inline void SetServiceId( TUint aServiceId );
    inline void SetType( TCCHSubserviceType aType );
    inline void SetParameter( TCchConnectionParameter aParameter );
    
public:    
    /**
     * Service Id
     */
    TServiceSelection       iServiceSelection;
    /**
     * Service's SNAP Id
     */
    TUint32                 iSNAPId;
    /**
     * Services IAP Id
     */
    TUint32                 iIapId;
     /**
     * Is SNAP editable, read only
     */
    TBool                   iSNAPLocked;
    /**
    * Is password set.
    */
    TBool                   iPasswordSet;

    /**
     * Username
     */
    TBuf<KCCHMaxUsernameLength> iUsername;

    /**
     * Password
     */
    TBuf<KCCHMaxPasswordLength> iPassword;
    
    /**
    * Reserved, obsolote. When adding next new parameter (TInt)
    * rename Reserved to that, implementation is ready then.
    */
    TBool                   iReserved;
    
    };

class TServiceStatus
    {
public:
    inline TServiceStatus();
    inline TUint ServiceId() const;
    inline TCCHSubserviceType Type() const;
    inline TUint32 IapId() const;
    inline TUint32 SNAPId() const;
    inline TBool SNAPLocked() const;
    inline TBool PasswordSet() const;
    inline void SetServiceId( TUint aServiceId );
    inline void SetType( TCCHSubserviceType aType );
    inline void SetIapId( TUint32 aIapId );
    inline void SetSNAPId( TUint32 aSNAPId );
    inline void SetSNAPLocked( TBool aSNAPLocked );
public:
    /**
     * Service Id
     */
    TServiceConnectionInfo  iConnectionInfo;
    /**
     * Current state of subservice
     */
    TCCHSubserviceState     iState;
    /**
     * Current error code of subservice
     */
    TInt                    iError;
    };

class TCCHSubservice
    {
public:
    inline TCCHSubservice();
    inline TUint ServiceId() const;
    inline TCCHSubserviceType Type() const;
    inline TUint32 IapId() const;
    inline TUint32 SNAPId() const;
    inline TBool SNAPLocked() const;
    inline void SetServiceId( TUint aServiceId );
    inline void SetType( TCCHSubserviceType aType );
    inline void SetIapId( TUint32 aIapId );
    inline void SetSNAPId( TUint32 aSNAPId );
    inline void SetSNAPLocked( TBool aSNAPLocked );
public:
    /**
     * Service Id
     */
    TServiceConnectionInfo          iConnectionInfo;
    /**
     * Subservice's Id
     */
    TUint                           iSubserviceId;
    /**
     * Current state of subservice
     */
    TCCHSubserviceState             iState;
    /**
     * Subservice's name
     */
    TBuf<KCCHMaxServiceNameLength>  iSubserviceName;
    /**
     * Current error code of subservice
     */
    TInt                            iError;
    };

class TCCHService
    {
public:
    inline TCCHService();

public:
    /**
     * Service Id
     */
    TUint                                   iServiceId;
    /**
     * Service's name
     */
    TBuf<KCCHMaxServiceNameLength>          iServiceName;
    /**
     * Service's subservices
     */
    TFixedArray<TCCHSubservice, KCCHMaxSubservicesCount>    iSubservices;
    };

#include "cchclientserver.inl"

#endif // T_CCHCLIENTSERVER_H

// End of File
