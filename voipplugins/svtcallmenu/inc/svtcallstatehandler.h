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
* Description:  Call handler class for svtcallmenu.
*
*/


#ifndef C_SVTCALLSTATEHANDLER_H
#define C_SVTCALLSTATEHANDLER_H

#include <e32base.h>
#include <telmenuextension.h>

/**
 *  Call state handler class for svtmatching.
 *
 *  @lib svtmatching
 *  @since S60 v5.1
 */
NONSHARABLE_CLASS( CSvtCallStateHandler ) : public CBase
    {
public: // Data
    
    /**
     * Features
     */
    enum TFeature
        {
        ESvmFeatureNone,
        ESvmFeatureConference,
        ESvmFeatureUnattendedTransfer,
        ESvmFeatureAttendedTransfer
        };
    
    
public:

    /**
     * Two-phased constructor.
     * @param aCallArray Current call info array.
     */
    static CSvtCallStateHandler* NewL( 
            const RArray<CTelMenuExtension::TCallInfo>& aCallArray );
  
  
    /**
     * Destructor.
     */
    virtual ~CSvtCallStateHandler();
 
    /**
     * Checks is feature supported.
     * @param aFeature - Feature to be checked.
     * @return ETrue if feature supported.
     */    
    TBool FeatureSupported( TFeature aFeature ) const;
 
    
private:

    CSvtCallStateHandler();

    void ConstructL( const RArray<CTelMenuExtension::TCallInfo>& aCallArray );
    
    /**
     * Checks is unattended transfer currently supported.
     * @return ETrue if supported.
     */  
    TBool UnattendedTransferPossible() const;
    
private: // Data

    /**
     * Current call information.
     */
    RArray<CTelMenuExtension::TCallInfo> iCallArray;
    

#ifdef _DEBUG    
    friend class T_CSvtCallStateHandler;
#endif
    
    };

#endif // C_SVTCALLSTATEHANDLER_H
