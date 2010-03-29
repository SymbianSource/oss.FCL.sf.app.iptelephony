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
* Description:  Utility class to phone functionality
*
*/


#include <sysutil.h>
#include <f32file.h>
#include <pathinfo.h>

#include "cipappphoneutils.h"

const TUint8 KSVPLineFeed2 = '\n';
const TInt KHwVersionIn3rdRow = 4;
_LIT8( KSVPSpaceMark, " " );
_LIT8( KSVPLineFeed, "\n" );
_LIT( KSVPSalesModelFileName, "\\resource\\versions\\model.txt" );
_LIT( KSVPCopyright, "\xa9" );
_LIT( KSVPCopyright2, "(C)" );
_LIT( KSVPCopyright3, "(c)" );
_LIT( KSVPSpace2, " " );
_LIT( KSVPS60,"S60" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CIpAppPhoneUtils::CIpAppPhoneUtils()
    {
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CIpAppPhoneUtils* CIpAppPhoneUtils::NewL()
    {
    CIpAppPhoneUtils* self = CIpAppPhoneUtils::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CIpAppPhoneUtils* CIpAppPhoneUtils::NewLC()
    {
    CIpAppPhoneUtils* self = new( ELeave ) CIpAppPhoneUtils;
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CIpAppPhoneUtils::~CIpAppPhoneUtils()
    {
    }

// ---------------------------------------------------------------------------
// CIpAppPhoneUtils::GetTerminalTypeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CIpAppPhoneUtils::GetTerminalTypeL( TDes& aTerminalType ) const
    {
    RFs fsSession;
    User::LeaveIfError( fsSession.Connect() );
    CleanupClosePushL( fsSession );
    
    GetTerminalTypeL( aTerminalType, fsSession );
    
    CleanupStack::PopAndDestroy( &fsSession );
    }

// ---------------------------------------------------------------------------
// CIpAppPhoneUtils::GetTerminalTypeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CIpAppPhoneUtils::GetTerminalTypeL(
        TDes& aTerminalType, RFs& aFs ) const
    {
    RBuf swVersion;
    CleanupClosePushL( swVersion );
    TInt lineFeedCount = 0;
    TBuf8< KSVPMaxTerminalTypeLength > terminalType;

    TBuf< KSVPMaxTerminalTypeLength > firstRow;
    TBuf< KSVPPhoneModelLength > phoneModel;
    TRAP_IGNORE( GetPhoneModelL( phoneModel, aFs ) );

    swVersion.CreateL( KSysUtilVersionTextLength );
    User::LeaveIfError( SysUtil::GetSWVersion( swVersion ) );

    const TInt swVersionLength( swVersion.Length() );
    for ( TInt i = 0; i < swVersionLength ; i++ )
        {
        if ( !lineFeedCount && ( KSVPLineFeed2 != swVersion[i] ) )
            {
            firstRow.Append( swVersion[i] );  // this is 1st row a.k.a SW-version
            }
        if ( KSVPLineFeed2 == swVersion[i] )
            {
            lineFeedCount++;
            if( lineFeedCount == 2 )
                {
                // Delete 1st and 2nd row.
                swVersion.Delete( 0, i );
                const TInt swVersionLength2( swVersion.Length() );

                for ( TInt k = 0; k < swVersionLength2 ; k++ )
                    {
                    if ( KSVPLineFeed2 == swVersion[k] )
                        {
                        lineFeedCount++;
                        }
                    if ( lineFeedCount == KHwVersionIn3rdRow  )
                        {
                        // HW version is in 3rd row, delete rest of SWversion
                        const TInt charsToDelete( swVersionLength2 - k );
                        swVersion.Delete( k, charsToDelete );
                        break;
                        }
                    }
                break;
                }
            }
        }

    terminalType.Append( phoneModel );     // Add HW manufacturer, e.g. our company
    terminalType.Append( swVersion );       // Add HW version
    terminalType.Append( KSVPSpaceMark );
    terminalType.Append( firstRow );        // Add SW version

    // Replace linefeeds with space marks
    TInt linefeedIndex = 0;
    for ( ; KErrNotFound != linefeedIndex ; )
        {
        linefeedIndex = terminalType.Find( KSVPLineFeed );
        if ( KErrNotFound != linefeedIndex )
            {
            // 1 is the length of KSpaceMark
            terminalType.Replace( linefeedIndex, 1, KSVPSpaceMark );
            }
        }
    aTerminalType.Copy( terminalType );
    CleanupStack::PopAndDestroy( &swVersion );
    }


// ---------------------------------------------------------------------------
// CIpAppPhoneUtils::GetPhoneModelL
// ---------------------------------------------------------------------------
//
EXPORT_C void CIpAppPhoneUtils::GetPhoneModelL( TDes& aPhoneModel ) const
    {
    RFs fsSession;
    User::LeaveIfError( fsSession.Connect() );
    CleanupClosePushL( fsSession );
    
    GetPhoneModelL( aPhoneModel, fsSession );
    
    CleanupStack::PopAndDestroy( &fsSession );
    }

// ---------------------------------------------------------------------------
// CIpAppPhoneUtils::GetPhoneModelL
// ---------------------------------------------------------------------------
//
EXPORT_C void CIpAppPhoneUtils::GetPhoneModelL( TDes& aPhoneModel,
    RFs& aFs ) const
    {
    RFile file;
    TParse parse;
    parse.Set( PathInfo::RomRootPath(), NULL, NULL);
    TFileName myfile( parse.Drive() );
    myfile.Append( KSVPSalesModelFileName ); 
    User::LeaveIfError( file.Open( aFs,
        myfile, EFileShareReadersOnly | EFileRead ) );

    CleanupClosePushL( file );

    HBufC* phoneModel = HBufC::NewLC( KSVPPhoneModelLength );
    TPtr ptr = phoneModel->Des();

    // Read the data from file.
    TFileText reader;
    reader.Set( file );
    User::LeaveIfError( reader.Read( ptr ) );

    // Phone model to aPhoneModel
    aPhoneModel.Append( *phoneModel );

    TInt copyrightPosition = aPhoneModel.Find( KSVPCopyright );
    if ( KErrNotFound != copyrightPosition ) 
        {
        // Delete copyright mark.
        aPhoneModel.Delete( copyrightPosition, 1 );
        }

    copyrightPosition = aPhoneModel.Find( KSVPCopyright2 );
    if ( KErrNotFound != copyrightPosition )
        {
        // Delete copyright mark.
        aPhoneModel.Delete( copyrightPosition, KSVPCopyright2().Length() );
        }
    copyrightPosition = aPhoneModel.Find( KSVPCopyright3 );
    if ( KErrNotFound != copyrightPosition )
        {
        // Delete copyright mark.
        aPhoneModel.Delete( copyrightPosition, KSVPCopyright3().Length() );
        }

    TInt spacePosition = aPhoneModel.Find( KSVPSpace2 );
    TInt modelLength = aPhoneModel.Length();

    while ( 0 == spacePosition )
        {
        // Delete leading space character.
        aPhoneModel.Delete( 0, 1 );
        modelLength = aPhoneModel.Length();
        spacePosition = aPhoneModel.Find( KSVPSpace2 );
        }

    if ( ( KErrNotFound != spacePosition ) && ( 0 != spacePosition ) )
        {
        // We don't need the whole HW version, just 1st word
        aPhoneModel.Delete( spacePosition, modelLength - spacePosition );
        }

    if ( !modelLength )
        {
        // If manufacturer name wasn't found add platform name
        aPhoneModel.Append( KSVPS60 );
        }

    CleanupStack::PopAndDestroy( 2, &file ); // phoneModel, file
    }

// End of file
