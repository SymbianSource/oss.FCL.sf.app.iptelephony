/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCchFeatureManager
*                Reads phone features
*
*/


#ifndef CCHFEATUREMANAGER_H
#define CCHFEATUREMANAGER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CRepository;

// CLASS DECLARATION
/**
*  CCchFeatureManager
*  
*  @lib   cchserver
*  @since 3.2* 
*/
class CCchFeatureManager: public CBase
{
public:

    /**
     * Two-phased constructor.
     */
    static CCchFeatureManager* NewL();

    /**
     * Destructor.
     */
    virtual ~CCchFeatureManager();
    
    /**
     * Returns whether VoIP is supported.
     *
     * @since S60 3.2
     * @return ETrue if feature is supported
     */  
    TBool VoIPSupported() const;
    
    /**
     * Checks is phone in offline-mode.
     *
     * @since S60 3.2
     * @return ETrue if phone is in offline-mode
     */  
    TBool OfflineMode() const;
       
    /**
     * Returns whether Cover Display is supported.
     *
     * @since S60 3.2
     * @return ETrue if feature is supported
     */  
    TBool CoverDisplaySupported() const;

private:

    /**
     * Constructors
     */         
    CCchFeatureManager();
    
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:
    
    /**
    * VoIP support status.
    */
    TBool iVoIPSupported;
    
    /**
    * Cover display support status.
    */
    TBool iCoverDisplaySupported;
    
    /**
    * Offline mode
    */
    CRepository* iOfflineRepository;
};

#endif // CCHFEATUREMANAGER_H

// End of File
