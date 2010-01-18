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
* Description:  Settings handler class for svtmatching.
*
*/


#ifndef C_SVTSETTINGSHANDLER_H
#define C_SVTSETTINGSHANDLER_H

#include <e32base.h>
#include <badesca.h>

class CRCSEProfileRegistry;
class CSPSettings;

/**
 *  Settings handler class for svtmatching.
 *
 *  @lib svtmatching
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CSvtSettingsHandler ) : public CBase
    {
public:

    /**
     * Two-phased constructor.
     */
    static CSvtSettingsHandler* NewL();
  
  
    /**
     * Destructor.
     */
    virtual ~CSvtSettingsHandler();
 
    /**
     * Initializes settings for parsing.
     * @param aServiceId - This id identify what service is currently used.
     */    
    void InitializeSettingsL( TUint aServiceId );
 
    /**
     * Returns how many digits are meaningful for matching (from rcse).
     * InitializeSettingsL should to be called before this.
     * @return  - Value of meaningful digits.
     */
    TInt MeaningfulDigits() const;

    /**
     * Returns value of ignore domain part setting (from rcse).
     * InitializeSettingsL should to be called before this.
     * @return  - Value of ignore domain part setting.
     */
    TUint IgnoreDomainPartValue() const;

    /**
     * Gets services contact store uris that should
     * be used for matching.
     * InitializeSettingsL should to be called before this.
     * @param Contact store uri array.
     * @return  KErrNone if succeed.
     */
    TInt GetContactStoreUris( CDesCArray& aStoreUris ) const;
    
private:

    /**
     * Returns services contact store uris that should
     * be used for matching.
     * InitializeSettingsL should to be called before this.
     * @leave Leaves if operation not succeed.
     * @param Contact store uri array.
     */
    void GetContactStoreUrisL( CDesCArray& aStoreUris ) const;

    /**
     * Creates a new instance of rcse profile registry.
     * @return Pointer to rcse profile registry.
     */
    CRCSEProfileRegistry* CreateRcseRegistryLC() const;

    /**
     * Creates a new instance of service provider settings.
     * @return Pointer to SPSettings.
     */
    CSPSettings* CreateSpSettingsLC() const;
    
private:
    
    CSvtSettingsHandler();
    
private: // Data

    /**
     * Value of ignore domain part setting.
     */
    TUint iIgnoreDomainPart;
    
    /**
     * Meaningful digits
     */
    TInt iMeaningfulDigits;
    
    /**
     * Service id
     */
    TUint iServiceId;

#ifdef _DEBUG    
    friend class T_CSvtSettinsHandler;
#endif
    
    };

#endif //C_SVTSETTINGSHANDLER_H
