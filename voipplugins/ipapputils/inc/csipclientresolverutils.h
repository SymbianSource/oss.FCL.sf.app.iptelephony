/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Utility class to handle SIP client resolver cenrep table.
*
*/


#ifndef C_SIPCLIENTRESOLVERUTILS_H
#define C_SIPCLIENTRESOLVERUTILS_H

#include <e32base.h>
#include <spdefinitions.h>

class CRepository;

/**
 *  Utility class to handle SIP client resolver cenrep table.
 *
 *  @code
 *      iSipClientResolver = CSipClientResolverUtils::NewL();
 *      iSipClientResolver->RegisterClientWithUserL( 
 *              KImplementation, iContactHeaderUser, KResolver );
 *      delete iSipClientResolver;
 *  @endcode
 *
 *  @lib ipapputils.lib
 *  @since S60 v3.2 
 */
NONSHARABLE_CLASS( CSipClientResolverUtils ) : public CBase
    {
public:

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CSipClientResolverUtils* NewL();
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CSipClientResolverUtils* NewLC();

    /**
    * Destructor.
    */
    IMPORT_C virtual ~CSipClientResolverUtils();

    /**
     * Register to SIP client resolver to be able to receive incoming sessions.
     *
     * @since S60 3.2
     * @param aImplementation UID of client who handles incoming session
     * @param aProfileContactHeaderUser Globally unique identifier of SIP profile
     * @param aResolver UID of client resolver plug-in
     * @return 
     */
    IMPORT_C void RegisterClientWithUserL( 
                const TUid& aImplementation,
                const TDesC8& aProfileContactHeaderUser, 
                const TUid& aResolver ) const;

    /**
     * Un-register from SIP client resolver to deny accepting incoming sessions.
     *
     * @since S60 3.2
     * @param aImplementation UID of client who handles incoming session
     * @param aProfileContactHeaderUser Globally unique identifier of SIP profile
     * @return 
     */
    IMPORT_C void UnRegisterClientWithUserL( 
                const TUid& aImplementation,
                const TDesC8& aProfileContactHeaderUser ) const;

    /**
     * Resolve implementation Uid for incoming session.
     *
     * @since S60 3.2
     * @param aImplementation UID of client who handles incoming session
     * @param aProfileContactHeaderUser Globally unique identifier of SIP profile
     * @return 
     */
    IMPORT_C void GetImplementationUidWithUserL( 
        const TDesC8& aProfileContactHeaderUser,
        TUid& aImplementation ) const;
    
    /**
     * Return all implementation Uids bases on contact header
     *
     * @since S60 5.0
     * @param aProfileContactHeaderUser Globally unique identifier of SIP profile
     * @param aImplementationUids UIDs of clients which handle incoming session
     * @return 
     */
    IMPORT_C void GetAllImplementationUidsWithUserL( 
        const TDesC8& aProfileContactHeaderUser,
        RArray<TUid>& aImplementationUids ) const;
    
private:

    /**
     * C++ default constructor.
     */
    CSipClientResolverUtils();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Finds all rows that match given search criterias.
     *
     * @since S60 3.2
     * @param aProfileContactHeaderUser Globally unique identifier of SIP profile
     * @param aFoundKeys On return contains keys that matched requested rows
     * @return 
     */
    void GetClientWithUserL(
                const TDesC8& aProfileContactHeaderUser, 
                RArray<TUint32>& aFoundKeys ) const;
                    
    /**
     * Creates unique key
     *
     * @since S60 3.2
     * @param aNewKey New unique key to be used when inserting row to table
     * @return 
     */
    void CreateNewKeyL( TUint32& aNewKey ) const;
    
    /**
     * Gets call provider Uids.
     * @param aUidArray Array containing all call provider Uids.
     */
    void CallProviderUidsL( RArray<TUint>& aUidArray ) const;   
     
    /**
     * Checks if implementation uid found by key in repository
     * matches given uid.
     *
     * @since S60 5.0
     * @param aKey unique key identifieng row in repository
     * @param aImplementation implementation uid
     * @return ETrue if uid matches
     */
    TBool CheckImplementationUidL( 
        const TUint32& aKey, const TUid& aImplementation ) const;

private: // data

    /**
     * Instance of central repository
     * Own.
     */
    CRepository* iRepository;

#ifdef _DEBUG
        friend class T_CSipClientResolverUtils;
#endif
    };

#endif // C_SIPCLIENTRESOLVERUTILS_H
