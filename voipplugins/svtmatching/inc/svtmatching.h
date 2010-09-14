/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Main class for svtmatching.
*
*/


#ifndef C_SCMTELMATCHING_H
#define C_SCMTELMATCHING_H

#include <telmatchingextension.h>

class CSvtUriParser;
class CSvtSettingsHandler;

/**
 *  Main class for svtmatching.
 *
 *  @lib svtmatching
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CSvtMatching) :
    public CTelMatchingExtension
  {
public:

    /**
     * Two-phased constructor.
     */
    static CSvtMatching* NewL();
  
  
    /**
     * Destructor.
     */
    ~CSvtMatching();

    
// from base class CTelMatchingExtension
    
    /**
     * Initialize class after it is created with NewL.
     * @param aServiceId - This id identify what service is currently used in call.
     * @param aOrigAddress - Original address.
     */
    void InitializeL( TUint aServiceId, const TDesC& aOrigAddress );

    /**
     * Returns parsed address to be used in contact matching.
     * Sets aParsedAddress parameter as empty (KNullDesC) if original number should be used
     * in contact matching. 
     * @param aParsedAddress - Address parsed by plugin. This address will be used for matching.
     * @param aMeaningfulDigits - How many digits (starting from the right) are meaningful for matching.
     *                            Value of the parameter have to be between 4-20 otherwise the whole
     *                            of address will be used for matching (e.g. by value 0).
     *                            If length of the address is less than value of the parameter the whole
     *                            of address will be used for matching.
     * @param aAllowUserNameMatch - ETrue on return if matching using the user name part of 
     *                              the address is allowed.
     * @return - Return KErrNone if operation succeed.
     */
    TInt GetAddressForMatching( 
        RBuf& aParsedAddress, 
        TInt& aMeaningfulDigits, 
        TBool& aAllowUsernameMatch );

    /**
     * Returns contact store uris which are opened and will be used for matching.
     * @param aStoreUris - Contact store uri array.
     * @return - Return KErrNone if operation succeed.
     */
    TInt GetContactStoreUris( CDesCArray& aStoreUris );

    /**
     * Returns remote party name (remote party name is shown in the call bubble).
     * Sets aRemotePartyName parameter as empty (KNullDesC) if remote party name is not available.
     * Remote party name (aRemotePartyName) will be shown in the call bubble when contact 
     * matching not succeed.
     * @param aRemotePartyName - Remote party name.
     * @return - Return KErrNone if operation succeed.
     */
    TInt GetRemotePartyName( RBuf& aRemotePartyName );

private:
    
    CSvtMatching();

    void ConstructL();
      
private:    // data

    /**
     * Service id
     */
    TUint iServiceId;
 
    /**
     * Pointer to settings handler.
     * Own.
     */
    CSvtSettingsHandler* iSettingsHandler; 
    
    /**
     * Pointer to Uri parser.
     * Own.
     */
    CSvtUriParser* iUriParser;
    
    /**
     * Pointer for original address.
     * Own.
     */
    HBufC*  iOriginalAddress;
 
#ifdef _DEBUG   
    friend class T_CSvtMatchingPlugin;
#endif
    
  };  

#endif  // C_SCMTELMATCHING_H


