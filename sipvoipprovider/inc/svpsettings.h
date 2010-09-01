/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*  Description : Definition of service provider settings
*
*/


#ifndef SVPSETTINGS_H
#define SVPSETTINGS_H

#include <e32base.h>
#include <spdefinitions.h>
#include <e32cmn.h>

class CSPSettings;
class CSPProperty;

/**
 *  Defines settings reader for VCC
 *
 *  @code
 *  @endcode
 *
 *  @since S60 v3.2
 */
class SvpSettings
 {
 public:
 
   /**
    * 
    * 
    * @since S60 v3.2
    * @return 
    */
    static TInt IntPropertyL( TUint aServiceId, 
                TServicePropertyName aPropertyName );
    
  
 // No copy, assigment or creation of this class.

private:    
    /**
     * Assigment operator
     * 
     * @since S60 v3.2
     */
	SvpSettings& operator = ( const SvpSettings& );
    
    /**
     * Default C++ constructor
     * 
     * @since S60 v3.2
     */
	SvpSettings();
    
    
    /**
     * Copy constructor
     * 
     * @since S60 v3.2
     */
	SvpSettings( const SvpSettings& );
    
 };

#endif // SVPSETTINGS_H

