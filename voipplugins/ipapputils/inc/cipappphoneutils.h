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
* Description:  Utility class to phone functionality.
*
*/


#ifndef C_IPAPPPHONEUTILS_H
#define C_IPAPPPHONEUTILS_H

#include <e32base.h>

const TUint KSVPMaxTerminalTypeLength = 64;
const TUint KSVPPhoneModelLength = 20;

class RFs;

/**
 *  Utility class to phone functionality.
 *
 *  @code
 *        TBuf<KSVPMaxTerminalTypeLength> terminalType;
 *      CIpAppPhoneUtils* phoneUtils = CIpAppPhoneUtils::NewL();
 *      phoneUtils->GetTerminalTypeL( terminalType );
 *      delete phoneUtils;
 *  @endcode
 *
 *  @lib ipapputils.lib
 *  @since S60 v3.2 
 */
NONSHARABLE_CLASS( CIpAppPhoneUtils ) : public CBase
    {
    public:

        /**
         * Two-phased constructor.
         */
        IMPORT_C static CIpAppPhoneUtils* NewL();
        /**
         * Two-phased constructor.
         */
        IMPORT_C static CIpAppPhoneUtils* NewLC();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CIpAppPhoneUtils();
        
        /**
        * Get the terminal type.
        * @since Series60 5.0
        * @param aTerminalType terminal type
        */
        IMPORT_C void GetTerminalTypeL( TDes& aTerminalType ) const;

        /**
        * Get the terminal type.
        * @since Series60 5.0
        * @param aTerminalType terminal type
        * @param aFs file session
        */
        IMPORT_C void GetTerminalTypeL( TDes& aTerminalType, RFs& aFs ) const;
        
        
        /**
        * Get the phone model.
        * @since Series 60 3.2
        * @param aPhoneModel phone model.
        */
        IMPORT_C void GetPhoneModelL( TDes& aPhoneModel ) const;

        /**
        * Get the phone model.
        * @since Series 60 3.2
        * @param aPhoneModel phone model.
        * @param aFs file session
        */
        IMPORT_C void GetPhoneModelL( TDes& aPhoneModel, RFs& aFs ) const;
        

    private:

        /**
         * C++ default constructor.
         */
        CIpAppPhoneUtils();
        
    private: // data

    };

#endif // C_IPAPPPHONEUTILS_H
