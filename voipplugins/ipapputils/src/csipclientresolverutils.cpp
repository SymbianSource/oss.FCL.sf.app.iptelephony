/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SIP client resolver utility class
*
*/


#include <centralrepository.h>
#include <sipclientresolverconfigcrkeys.h>
#include <spsettings.h>
#include <spentry.h>
#include <spproperty.h>

#include "csipclientresolverutils.h"


// ======== LOCAL FUNCTIONS ========
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CSipClientResolverUtils::CSipClientResolverUtils()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CSipClientResolverUtils::ConstructL()
    {
    iRepository = CRepository::NewL( KCRUidSIPClientResolverConfig );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CSipClientResolverUtils* CSipClientResolverUtils::NewL()
    {
    CSipClientResolverUtils* self = CSipClientResolverUtils::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CSipClientResolverUtils* CSipClientResolverUtils::NewLC()
    {
    CSipClientResolverUtils* self = new( ELeave ) CSipClientResolverUtils;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CSipClientResolverUtils::~CSipClientResolverUtils()
    {
    delete iRepository;
    }

// ---------------------------------------------------------------------------
// Add line to client resolver cenrep table
// ---------------------------------------------------------------------------
//
EXPORT_C void CSipClientResolverUtils::RegisterClientWithUserL(
    const TUid& aImplementation,
    const TDesC8& aProfileContactHeaderUser, 
    const TUid& aResolver ) const
    {
    __ASSERT_ALWAYS( aImplementation.iUid > 0, User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( aProfileContactHeaderUser.Length() > 0,
            User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( aResolver.iUid > 0, User::Leave( KErrArgument ));

    // Remove all earlier instances of client&user combination
    UnRegisterClientWithUserL( aImplementation, aProfileContactHeaderUser );

    User::LeaveIfError( 
        iRepository->StartTransaction( CRepository::EConcurrentReadWriteTransaction ) );
    TUint32 newKey = 0;
    CreateNewKeyL( newKey );
    // Add the new row
    TInt uid = aImplementation.iUid;
    User::LeaveIfError( 
        iRepository->Create(newKey|KSIPClientResolverClientUIDMask, uid ) );    
    User::LeaveIfError( 
        iRepository->Create(newKey|KSIPClientResolverUserNameMask, aProfileContactHeaderUser ) );
    uid = aResolver.iUid;    
    User::LeaveIfError( 
        iRepository->Create(newKey|KSIPClientResolverPluginUIDMask, uid ) ); 

    // Commit the transaction
    User::LeaveIfError( iRepository->CommitTransaction( newKey ) );
    }

// ---------------------------------------------------------------------------
// Remove line from client resolver cenrep table
// ---------------------------------------------------------------------------
//
EXPORT_C void CSipClientResolverUtils::UnRegisterClientWithUserL(
    const TUid& aImplementation, 
    const TDesC8& aProfileContactHeaderUser ) const
    {
    __ASSERT_ALWAYS( aImplementation.iUid > 0, User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( aProfileContactHeaderUser.Length() > 0,
            User::Leave( KErrArgument ) );  

    RArray<TUint32> keys;
    CleanupClosePushL( keys );
    GetClientWithUserL( aProfileContactHeaderUser, keys );
    for ( TInt i = 0; i < keys.Count(); i++ )
        {        
        if ( CheckImplementationUidL( keys[ i ], aImplementation ) )
            {
            TUint32 errorKey;
            User::LeaveIfError( 
                iRepository->StartTransaction( 
                    CRepository::EConcurrentReadWriteTransaction ) );
            iRepository->Delete( keys[i], KSIPClientResolverKeyMask, errorKey );
            // Commit the transaction
            User::LeaveIfError( iRepository->CommitTransaction( errorKey ) );            
            }
        }
    CleanupStack::PopAndDestroy( &keys );
    }

// ---------------------------------------------------------------------------
// Resolve correct implementation UID for contact header
// ---------------------------------------------------------------------------
//    
EXPORT_C void CSipClientResolverUtils::GetImplementationUidWithUserL( 
    const TDesC8& aProfileContactHeaderUser,
    TUid& aImplementation ) const
    {
    RArray<TUint32> keys;    
    CleanupClosePushL( keys );
    // Find all rows where KSIPClientResolverUserNameMask is aProfileContactHeaderUser
    iRepository->FindEqL( KSIPClientResolverUserNameMask, 
                    KSIPClientResolverFieldTypeMask,
                    aProfileContactHeaderUser, keys );
    
    TInt implementationValue = KErrNotFound;
    TInt count( keys.Count() );
    if ( count > 1 )
        {
        // Call provider UIDs should be read from spsettings table and if some
        // UID found from spsettings match one uid in client resolver cenrep,
        // that should be returned to caller.
        RArray<TUint> uidArray;
        CleanupClosePushL( uidArray );
        CallProviderUidsL( uidArray );
        __ASSERT_ALWAYS( uidArray.Count() > 0, User::Leave( KErrNotFound ) );
        TInt i( 0 );
        for( ; i < count; i++ )
            {
            // Get implementation uid of found user name
            iRepository->Get( ( keys[i]^KSIPClientResolverUserNameMask ) |
                    KSIPClientResolverClientUIDMask, implementationValue );
            
            User::LeaveIfError( implementationValue );
            if ( KErrNotFound != uidArray.Find( implementationValue ) )
                {
                aImplementation.iUid = implementationValue;
                break;
                }
            }
        CleanupStack::PopAndDestroy( &uidArray );
        if ( i == count && aImplementation.iUid != implementationValue )
            {
            // No matching Uid found
            User::Leave( KErrNotFound );            
            }
        }
        
    else if ( count == 1 )
        {        
        // Get implementation uid of found user name
        iRepository->Get( ( keys[0]^KSIPClientResolverUserNameMask ) |
        KSIPClientResolverClientUIDMask, implementationValue );

        User::LeaveIfError( implementationValue );
        aImplementation.iUid = implementationValue;
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    CleanupStack::PopAndDestroy( &keys );    
    }

// ---------------------------------------------------------------------------
// Return all implementation Uids bases on contact header
// ---------------------------------------------------------------------------
//    
EXPORT_C void CSipClientResolverUtils::GetAllImplementationUidsWithUserL( 
    const TDesC8& aProfileContactHeaderUser,
    RArray<TUid>& aImplementationUids ) const
    {
    RArray<TUint32> keys;    
    CleanupClosePushL( keys );
    
    // Find all rows where KSIPClientResolverUserNameMask
    // is aProfileContactHeaderUser
    User::LeaveIfError( iRepository->FindEqL( KSIPClientResolverUserNameMask, 
        KSIPClientResolverFieldTypeMask,
        aProfileContactHeaderUser, keys ) );
    
    for( TInt i( 0 ) ; i < keys.Count() ; i++ )
        {
        TInt implementationValue = KErrNotFound;
        
        // Get implementation uid of found user name
        User::LeaveIfError( iRepository->Get( 
            ( keys[i]^KSIPClientResolverUserNameMask ) |
            KSIPClientResolverClientUIDMask, implementationValue ) );
        
        if ( implementationValue )
            { 
            TUid implementationUid;
            implementationUid.iUid = implementationValue;
            aImplementationUids.AppendL( implementationUid );
            }
        }
    CleanupStack::PopAndDestroy( &keys );
    }

// ---------------------------------------------------------------------------
// Find clients from resolver cenrep table
// ---------------------------------------------------------------------------
//
void CSipClientResolverUtils::GetClientWithUserL(
    const TDesC8& aProfileContactHeaderUser, 
    RArray<TUint32>& aFoundKeys ) const
    {
    RArray<TUint32> keys;
    
    CleanupClosePushL( keys );
    // Find all rows where KSIPClientResolverUserNameMask is aProfileUserName
    iRepository->FindEqL( KSIPClientResolverUserNameMask, 
                            KSIPClientResolverFieldTypeMask,
                            aProfileContactHeaderUser, keys );
    for ( TInt i = 0; i < keys.Count(); i++ )
        {
        // Identification is now done only by aProfileContactHeaderUser
        aFoundKeys.AppendL( keys[i]^KSIPClientResolverUserNameMask );
        }
    CleanupStack::PopAndDestroy( &keys );
    }

// ---------------------------------------------------------------------------
// Create a new key for the new row
// ---------------------------------------------------------------------------
//
void CSipClientResolverUtils::CreateNewKeyL(
    TUint32& aNewKey ) const
    {
    RArray<TUint32> keys;
    CleanupClosePushL( keys );
    TInt err = KErrNone;
    
    TRAPD( leaveCode, err = iRepository->FindL( KSIPClientResolverUserNameMask, 
                                  KSIPClientResolverFieldTypeMask, 
                                  keys ) );
    if ( KErrNotFound == leaveCode )
        {
        err = KErrNotFound;
        leaveCode = KErrNone;
        }
    User::LeaveIfError( leaveCode );
    if ( ( KErrNotFound == err ) || ( keys.Count() == 0 ) )
        {
        aNewKey = 1;
        }
    else
        {
        User::LeaveIfError( err );
        // Find the biggest key and increment it by one
        keys.SortUnsigned();
        aNewKey = KSIPClientResolverUserNameMask^keys[ keys.Count() - 1 ] + 1;       
        }
    CleanupStack::PopAndDestroy( &keys );
    }


// -----------------------------------------------------------------------------
// CSipClientResolverUtils::CallProviderUidsL
// -----------------------------------------------------------------------------
//    
 void CSipClientResolverUtils::CallProviderUidsL( RArray<TUint>& aUidArray ) const
    {
    CSPSettings* spSettings = CSPSettings::NewLC();
    CSPProperty* property = CSPProperty::NewLC();
    
    RArray<TUint> idArray;
    CleanupClosePushL( idArray );
    User::LeaveIfError( spSettings->FindServiceIdsL( idArray ) );
    TInt count = idArray.Count();
    TInt result( KErrNotFound );
    
    for( TInt i( 0 ); i < count; i++ )
        {
        result =  spSettings->FindPropertyL(
                idArray[i], EPropertyCallProviderPluginId, *property );
        
        if ( result == KErrNone && property )
            {
            TInt value( 0 );
            property->GetValue( value );
            aUidArray.Append( value );
            }
        }
    
    CleanupStack::PopAndDestroy( &idArray );
    CleanupStack::PopAndDestroy( property );
    CleanupStack::PopAndDestroy( spSettings );
    }
 
 // -----------------------------------------------------------------------------
 // CSipClientResolverUtils::CheckImplementationUidL
 // -----------------------------------------------------------------------------
 // 
 TBool CSipClientResolverUtils::CheckImplementationUidL( 
     const TUint32& aKey, const TUid& aImplementation ) const
     {     
     TBool match( EFalse );
     TInt implementationValue( KErrNotFound );
     
     iRepository->Get( ( aKey^KSIPClientResolverUserNameMask )  |
                         KSIPClientResolverClientUIDMask,
                         implementationValue );
      
     if ( implementationValue == aImplementation.iUid )
         { 
         match = ETrue;
         }
     
     return match;
     }
