/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Inline functions for ClientServer
*
*/


// --------------------------------------------------------------------------
// TCCHService::TCCHService
// --------------------------------------------------------------------------
//
inline TCCHService::TCCHService()
    {
    iServiceId      = 0;
    }

// --------------------------------------------------------------------------
// TCCHSubservice::TCCHSubservice
// --------------------------------------------------------------------------
//
inline TCCHSubservice::TCCHSubservice()
    {
    iConnectionInfo.iServiceSelection.iServiceId = 0;
    iConnectionInfo.iServiceSelection.iType      = ECCHUnknown;
    iConnectionInfo.iIapId                       = 0;
    iConnectionInfo.iSNAPId                      = 0;
    iConnectionInfo.iSNAPLocked                  = EFalse;
    iConnectionInfo.iPasswordSet                  = EFalse;
    iConnectionInfo.iUsername.Zero();
    iConnectionInfo.iPassword.Zero();
    iSubserviceId                                = 0;
    iState                                       = ECCHUninitialized;
    iError                                       = KErrNone;
    }

inline TUint TCCHSubservice::ServiceId() const
    {
    return iConnectionInfo.iServiceSelection.iServiceId;    
    }
    
inline TCCHSubserviceType TCCHSubservice::Type() const
    {
    return iConnectionInfo.iServiceSelection.iType;     
    }

inline void TCCHSubservice::SetServiceId( TUint aServiceId )
    {
    iConnectionInfo.iServiceSelection.iServiceId = aServiceId;    
    }
    
inline void TCCHSubservice::SetType( TCCHSubserviceType aType )
    {
    iConnectionInfo.iServiceSelection.iType      = aType; 
    }

inline TUint32 TCCHSubservice::IapId() const
    {
    return iConnectionInfo.iIapId;    
    }
    
inline TUint32 TCCHSubservice::SNAPId() const
    {
    return iConnectionInfo.iSNAPId;     
    }
    
inline TBool TCCHSubservice::SNAPLocked() const
    {
    return iConnectionInfo.iSNAPLocked;     
    }

inline void TCCHSubservice::SetIapId( TUint32 aIapId )
    {
    iConnectionInfo.iIapId = aIapId;    
    }
    
inline void TCCHSubservice::SetSNAPId( TUint32 aSNAPId)
    {
    iConnectionInfo.iSNAPId = aSNAPId;     
    }
    
inline void TCCHSubservice::SetSNAPLocked( TBool aSNAPLocked )
    {
    iConnectionInfo.iSNAPLocked = aSNAPLocked;     
    }

// --------------------------------------------------------------------------
// TServiceStatus::TServiceStatus
// --------------------------------------------------------------------------
//
inline TServiceStatus::TServiceStatus()
    {
    iConnectionInfo.iServiceSelection.iServiceId = 0;
    iConnectionInfo.iServiceSelection.iType      = ECCHUnknown;
    iConnectionInfo.iIapId                       = 0;
    iConnectionInfo.iSNAPId                      = 0;
    iConnectionInfo.iSNAPLocked                  = EFalse;
    iConnectionInfo.iUsername.Zero();
    iConnectionInfo.iPassword.Zero();
    iState                                       = ECCHUninitialized;
    iError                                       = KErrNone;
    }

inline TUint TServiceStatus::ServiceId() const
    {
    return iConnectionInfo.iServiceSelection.iServiceId;    
    }
    
inline TCCHSubserviceType TServiceStatus::Type() const
    {
    return iConnectionInfo.iServiceSelection.iType;     
    }

inline void TServiceStatus::SetServiceId( TUint aServiceId )
    {
    iConnectionInfo.iServiceSelection.iServiceId = aServiceId;    
    }
    
inline void TServiceStatus::SetType( TCCHSubserviceType aType )
    {
    iConnectionInfo.iServiceSelection.iType      = aType; 
    }

inline TUint32 TServiceStatus::IapId() const
    {
    return iConnectionInfo.iIapId;    
    }
    
inline TUint32 TServiceStatus::SNAPId() const
    {
    return iConnectionInfo.iSNAPId;     
    }
    
inline TBool TServiceStatus::SNAPLocked() const
    {
    return iConnectionInfo.iSNAPLocked;     
    }
inline TBool TServiceStatus::PasswordSet() const
    {
    return iConnectionInfo.iPasswordSet;     
    }

inline void TServiceStatus::SetIapId( TUint32 aIapId )
    {
    iConnectionInfo.iIapId = aIapId;    
    }
    
inline void TServiceStatus::SetSNAPId( TUint32 aSNAPId)
    {
    iConnectionInfo.iSNAPId = aSNAPId;     
    }
    
inline void TServiceStatus::SetSNAPLocked( TBool aSNAPLocked )
    {
    iConnectionInfo.iSNAPLocked = aSNAPLocked;     
    }
        
// --------------------------------------------------------------------------
// TServiceSelection::TServiceSelection
// --------------------------------------------------------------------------
//
inline TServiceSelection::TServiceSelection()
    {
    iServiceId      = 0;
    iType           = ECCHUnknown;
    iParameter      = ECchInitial;
    }

inline TServiceSelection::TServiceSelection( TUint aServiceId, 
                                             TCCHSubserviceType aType,
                                             TCchConnectionParameter aParameter )
    {
    iServiceId      = aServiceId;
    iType           = aType;
    iParameter      = aParameter;
    }

// --------------------------------------------------------------------------
// TServiceConnectionInfo::TServiceConnectionInfo
// --------------------------------------------------------------------------
//
inline TServiceConnectionInfo::TServiceConnectionInfo()
    {
    iServiceSelection.iServiceId = 0;
    iServiceSelection.iType      = ECCHUnknown;
    iSNAPId                      = 0;
    iIapId                       = 0;
    iSNAPLocked                  = EFalse;
    iUsername.Zero();
    iPassword.Zero();
    }

inline TServiceConnectionInfo::TServiceConnectionInfo( TUint aServiceId, 
                                                       TCCHSubserviceType aType,
                                                       TUint32 aIapId, 
                                                       TUint32 aSNAPId )
    {
    iServiceSelection.iServiceId = aServiceId;
    iServiceSelection.iType      = aType;
    iSNAPId                      = aSNAPId;
    iIapId                       = aIapId;
    iSNAPLocked                  = EFalse;
    iPasswordSet                 = EFalse;
    }

inline TServiceConnectionInfo::TServiceConnectionInfo( TUint aServiceId, 
                                                       TCCHSubserviceType aType,
                                                       TUint32 aIapId, 
                                                       TUint32 aSNAPId,
                                                       TDesC& aUsername,
                                                       TDesC& aPassword )
    {
    iServiceSelection.iServiceId = aServiceId;
    iServiceSelection.iType      = aType;
    iSNAPId                      = aSNAPId;
    iIapId                       = aIapId;
    iSNAPLocked                  = EFalse;
    iUsername.Append( aUsername );
    iPassword.Append( aPassword );
    }
inline TUint TServiceConnectionInfo::ServiceId() const
    {
    return iServiceSelection.iServiceId;    
    }
    
inline TCCHSubserviceType TServiceConnectionInfo::Type() const
    {
    return iServiceSelection.iType;     
    }

inline void TServiceConnectionInfo::SetServiceId( TUint aServiceId )
    {
    iServiceSelection.iServiceId = aServiceId;    
    }
    
inline void TServiceConnectionInfo::SetType( TCCHSubserviceType aType )
    {
    iServiceSelection.iType      = aType; 
    }
    
inline void TServiceConnectionInfo::SetParameter( TCchConnectionParameter aParameter )
    {
    iServiceSelection.iParameter = aParameter;    
    }
