/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
*  Description : Implements svp settings reader
*
*/

#include <spsettings.h>
#include <spproperty.h>
#include <spdefinitions.h>
#include <spentry.h>

#include "svplogger.h"
#include "svpsettings.h"

// ---------------------------------------------------------------------------
// Returns the id of VCC enabled VoIP service.
// EPropertyCallProviderPluginId
// ---------------------------------------------------------------------------
//
TInt SvpSettings::IntPropertyL( TUint aServiceId, 
                TServicePropertyName aPropertyName )
    {
    SVPDEBUG1( "SvpSettings::IntPropertyL In" )
    // Create settings object and then the property one
    CSPSettings* settings = CSPSettings::NewLC();
    CSPProperty* property = CSPProperty::NewLC();

    // Now get the property of the given service
    // First, get the property and then use the property to get its value.
    settings->FindPropertyL( aServiceId, aPropertyName, *property );

    TInt value;
    TInt error = property->GetValue( value );
    SVPDEBUG2( "SvpSettings::IntPropertyL serviceId = %d", aServiceId )
    
    User::LeaveIfError( error );

    // Cleanup and return
    CleanupStack::PopAndDestroy( property );
    CleanupStack::PopAndDestroy( settings );
    
    SVPDEBUG1( "SvpSettings::IntPropertyL Out" )
    
    return value;
    }

