/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCCHSubserviceInfo declaration
*
*/


#ifndef C_CCHSUBSERVICEINFO_H
#define C_CCHSUBSERVICEINFO_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <cchclientserver.h>

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CCCHServerBase;

// CLASS DECLARATION

/**
 *  CCCHSubserviceInfo declaration
 *  Contains Subservice information
 *  @lib cchserver.exe
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CCCHSubserviceInfo ) : public CBase
    {

public: // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CCCHSubserviceInfo* NewL( TUint aServiceId, 
                                     CCCHServerBase& aServer );

    /**
     * Two-phased constructor.
     */
    static CCCHSubserviceInfo* NewLC( TUint aServiceId, 
                                      CCCHServerBase& aServer );

    /**
     * Destructor.
     */
    virtual ~CCCHSubserviceInfo();

public: // New functions
    
    /**
     * SubserviceId setter
     * @since S60 3.2
     * @param aSubserviceId Subservice Id
     */
    void SetSubserviceId( const TUint aSubserviceId );
    
    /** 
     * Plug-in Uid's setter
     * @since S60 3.2
     * @param aPluginUid Subservice's Plug-in's Uid
     */
    void SetPluginUid( const TUid aPluginUid );
    
    /** 
     * Plug-in Uid's getter
     * @since S60 3.2
     * @param TUid Subservice's Plug-in's Uid
     */
    TUid GetPluginUid() const;
    
    /**
     * Subservice type setter
     * @since S60 3.2
     * @param aType Type of Subservice
     */
    void SetType( const TCCHSubserviceType aType );
    
    /**
     * Subservice type getter
     * @since S60 3.2
     * @return TCCHSubserviceType type of subservice
     */
    TCCHSubserviceType Type();
    
    /**
     * Subservice state setter
     * @since S60 3.2
     * @param aState Current state of Subservice
     */
    void SetState( const TCCHSubserviceState aState );
    
    /**
     * Error setter
     * @since S60 3.2
     * @param aState Current error of Subservice
     */
    void SetError( const TInt aError  );
    
    /**
     * IAP Id setter
     * @since S60 3.2
     * @param aIapId Currently used IAP Id
     */
    void SetIapId( const TUint32 aIapId );
    
    /**
     * SNAP Id setter
     * @since S60 3.2
     * @param aSNAPid Currently used SNAP Id
     */
    void SetSNAPId( const TUint32 aSNAPid );
    
    /**
     * Update state of Subservice
     * @since S60 3.2
     */
    void Update();
    
    /**
     * Is this Subservice enabled
     * @since S60 3.2
     * @return ETrue if Subservice is enabled
     */
    TBool IsEnabled() const;
    
    /**
     * Subservice error getter
     * @since S60 3.2
     * @return TCCHSubserviceStates Current state of Subservice
     */     
    TInt GetError();
    
    /**
     * Subservice state getter
     * @since S60 3.2
     * @return TCCHSubserviceStates Current state of Subservice
     */     
    TCCHSubserviceState GetState();
    
    /**
     * Get Subservice information
     * @since S60 3.2
     * @param aSubservice On completion contains subservice information
     */
    void FillSubserviceInfo( TCCHSubservice& aSubservice );
    
    /**
     * Subservice's enable at startup info setter
     * @since S60 3.2
     * @param aLoadAtStartUp Enable Subservice at the startup
     */
    void SetStartupFlag( TBool aLoadAtStartUp );
    
    /**
     * Subservice's enable at startup info getter
     * @since S60 3.2
     * @param ETrue If Subservice have to enable at the startup
     */
    TBool GetStartUpFlag() const;
    
    /**
     * Status has changed
     * @since S60 3.2
     */
    void StatusChanged();
    
private:

    /**
     * C++ default constructor.
     */
    CCCHSubserviceInfo( TUint aServiceId, CCCHServerBase& aServer );

    /**
     * Update connection related member variables from SIP Connection Provider
     * @since S60 3.2
     */
    void GetServiceNetworkInfo();
    
private: // data

    /**
     * Service Id, this service owns this subservice
     */
    TUint32                         iServiceId;
    
    /**
     * Handle to server
     */
    CCCHServerBase&                 iServer;
    
    /**
     * Subservice's Id
     */
    TUint                           iSubserviceId;

    /**
     * Subservice's Plug-in's Uid
     */
    TUid                            iPluginUid;
    
    /**
     * Subservice's type
     */
    TCCHSubserviceType              iType;
    
    /**
     * Current state of subservice
     */
    TCCHSubserviceState             iState;
    
    /**
     * Current error of subservice
     */
    TInt                            iError;
    
    /**
     * Subservice's IAP
     */
    TUint32                         iIapId;
    
    /**
     * Subservice's SNAP
     */
    TUint32                         iSNAPId;
    
    /**
     * Can SNAP be edited
     */
    TBool                           iSNAPLocked;
    
    /**
     * Is password set.
     */
    TBool                           iPasswordSet;
    
     /**
     * Enable at start-up
     */
    TBool                           iEnableAtStartUp;

     /**
     * Previous state of subservice
     */
    TCCHSubserviceState             iPreviousState;    
    };

#endif // C_CCHSUBSERVICEINFO_H

// End of file
