/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Application UI implementation.
*
*/


// INCLUDES
#include <eikdoc.h>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <voipprovisioningapp.rsg>
#include <aknnotewrappers.h>
#include <apgcli.h> // RApaLsSession
#include <bautils.h>
#include <xSPViewServices.h>
#include <browseruisdkcrkeys.h>
#include <pathinfo.h>
#include <driveinfo.h>
#include <zipfile.h>
#include <zipfilememberinputstream.h>
#include <AknWaitDialog.h>

#include "voipxmlparser.h"
#include "VoIPProvisioningAppUi.h"
#include "VoIPProvisioningApp.h"
#include "VoIPProvisioningDocument.h"

const TInt KOneSecond( 1000000 );
const TUid KMecoServiceTabUid = {0x20012423};
const TInt KFiveTimes( 5 );
_LIT( KDownloadFileName, "tempbranddatapackage.zip" );
_LIT( KBrandMifPath,     ":\\private\\102828DD\\import\\" );
_LIT( KBrandInstallPath, ":\\private\\102828DD\\import\\install\\" );
_LIT( KBrandXmlPath,     ":\\private\\102828E1\\import\\" );
_LIT( KXmlExtension,     ".xml" );
_LIT( KMifExtension,     ".mif" );
_LIT( KInstallExtension, ".install" );
_LIT8( KZipExtension,    ".zip" );

// ----------------------------------------------------------------------------
// CVoIPProvisioningAppUi::CVoIPProvisioningAppUi
// ----------------------------------------------------------------------------
//
CVoIPProvisioningAppUi::CVoIPProvisioningAppUi()
    {
    }

// ----------------------------------------------------------------------------
// CVoIPProvisioningAppUi::ConstructL
// ----------------------------------------------------------------------------

void CVoIPProvisioningAppUi::ConstructL()
    {
    // Call base classes ConstructL method.
    BaseConstructL( ENoScreenFurniture | EAknEnableMSK );

    StatusPane()->MakeVisible( EFalse );
    HideApplicationFromFSW( ETrue );

    iBrandPackageName.Copy( PathInfo::PhoneMemoryRootPath() );
    iBrandPackageName.Append( PathInfo::OthersPath() );
    iBrandPackageName.Append( KDownloadFileName );
    }

// ----------------------------------------------------------------------------
// CVoIPProvisioningAppUi::~CVoIPProvisioningAppUi
// ----------------------------------------------------------------------------
//
CVoIPProvisioningAppUi::~CVoIPProvisioningAppUi()
    {
    iDownloadMgr.Close();
    if ( iWaitDialog )
        {
        TRAP_IGNORE( iWaitDialog->ProcessFinishedL() );
        iWaitDialog = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CVoIPProvisioningAppUi::ProcessCommandParametersL
// ----------------------------------------------------------------------------
//
TBool CVoIPProvisioningAppUi::ProcessCommandParametersL(
    TApaCommand /*aCommand*/, TFileName& /*aDocumentName*/,
    const TDesC8& /*aTail*/ )
    {
    // Always return ETrue to make CVoIPProvisioningDocument's
    // OpenFileL to be called.
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CVoIPProvisioningAppUi::HandleFileL
// ----------------------------------------------------------------------------
//
void CVoIPProvisioningAppUi::HandleFileL( RFile& aFile )
    {
    RFile file;
    User::LeaveIfError( file.Duplicate( aFile ) );
    CVoipXmlParser* parser = CVoipXmlParser::NewLC(); // CS:1
    TBuf8<KMaxFileName> brandUrl( KNullDesC8 );
    HBufC* queryText = parser->ReadDescriptionLC( aFile, brandUrl ); // CS:2
    if ( !queryText->Des().Length() )
        {
        CleanupStack::PopAndDestroy( queryText );
        queryText = NULL;
        queryText = StringLoader::LoadLC( R_VOIP_DEFAULT_SAVE_QUERY );
        }
    CAknQueryDialog* query = CAknQueryDialog::NewL();
    CleanupStack::PushL( query ); // CS:3
    query->SetPromptL( queryText->Des() );
    TBool launchTab( EFalse );
    TUint32 serviceTabId;
    if ( query->ExecuteLD( R_VOIP_PROVISIONING_QUERY_INSTALL_SERVICE ) )
        {
        iWaitDialog = new( ELeave ) CAknWaitDialog( 
            REINTERPRET_CAST( CEikDialog**, &iWaitDialog ) );
        iWaitDialog->SetTone( CAknNoteDialog::ENoTone );
        iWaitDialog->ExecuteLD( R_WAITNOTE_CONFIGURE );

        FetchBrandingDataL( brandUrl );

        // Start phonebook into backgound.
        RxSPViewServices viewServices;
        CleanupClosePushL( viewServices ); // CS:4
        TInt viewSrvErr = viewServices.Open();
        if ( KErrServerTerminated == viewSrvErr || KErrNotFound == viewSrvErr )
            {
            viewServices.Close();
            viewSrvErr = viewServices.Open();
            }
        if ( KErrNone == viewSrvErr )
            {
            viewServices.Close();
            }

        // Ownership of file is trasferred.
        TInt error = parser->ImportDocument( file, serviceTabId );
        if ( KErrNone == error )
            {
            iWaitDialog->ProcessFinishedL();
            iWaitDialog = NULL;
            ShowDoneNoteL();
            launchTab = ETrue;
            }
        else
            {
            iWaitDialog->ProcessFinishedL();
            iWaitDialog = NULL;
            ShowFailNoteL();
            }
        CleanupStack::PopAndDestroy( &viewServices ); // CS:3
        }
    CleanupStack::Pop( query ); // CS:2
    // queryText, parser
    CleanupStack::PopAndDestroy( 2, parser ); // CS:0

    // Launch service tab.
    if ( launchTab )
        {
        TInt err = KErrNotFound;
        TInt counter( 0 );
        RxSPViewServices viewServices;
        CleanupClosePushL( viewServices ); // CS:1
        while ( KErrNone != err && KFiveTimes > counter )
            {
            err = viewServices.Activate( 
                KMecoServiceTabUid.iUid, (TInt32)serviceTabId );
            if ( KErrServerTerminated == err || KErrNotFound == err )
                {
                err = viewServices.Activate( 
                    KMecoServiceTabUid.iUid, (TInt32)serviceTabId );
                }
            // Wait...
            User::After( KOneSecond );
            counter++;
            }

        CleanupStack::PopAndDestroy( &viewServices ); // CS:0
        TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
        TApaTask browserTask = taskList.FindApp( KCRUidBrowser );
        if ( browserTask.Exists() )
            {
            browserTask.EndTask();
            }
        }

    if ( iWaitDialog )
        {
        iWaitDialog->ProcessFinishedL();
        iWaitDialog = NULL;
        }

    iDownloadMgr.Close();
#ifndef __EUNIT_TESTING
    User::Exit( KErrNone );
#endif
    }

// ----------------------------------------------------------------------------
// CVoIPProvisioningAppUi::ShowFailNoteL
// ----------------------------------------------------------------------------
//
void CVoIPProvisioningAppUi::ShowFailNoteL() const
    {
    HBufC* text = StringLoader::LoadLC( R_VOIP_INVALID_DATA );
    CAknErrorNote* note = new (ELeave) CAknErrorNote( R_AKN_ERROR_NOTE );
    note->ExecuteLD( *text );
    CleanupStack::PopAndDestroy( text );
    }

// ----------------------------------------------------------------------------
// CVoIPProvisioningAppUi::ShowDoneNoteL
// ----------------------------------------------------------------------------
//
void CVoIPProvisioningAppUi::ShowDoneNoteL() const
    {
    HBufC* text = StringLoader::LoadLC( R_VOIP_INSTALLED );
    CAknInformationNote* note =
        new (ELeave) CAknInformationNote( R_AKN_INFORMATION_NOTE );
    note->ExecuteLD( *text );
    CleanupStack::PopAndDestroy( text );
    }

// ---------------------------------------------------------------------------
// From base class MHttpDownloadMgrObserver.
// Handles download manager events.
// ---------------------------------------------------------------------------
//
void CVoIPProvisioningAppUi::HandleDMgrEventL( RHttpDownload& aDownload,
    THttpDownloadEvent aEvent )
    {
    if ( EHttpDlPaused == aEvent.iDownloadState && 
        EHttpContentTypeReceived == aEvent.iProgressState )
        {
        TInt32 length;
        TInt err = aDownload.GetIntAttribute( EDlAttrLength, length );
        if ( KDefaultContentLength != length && 0 < length )
            {
            // Continue.
            aDownload.Start();
            }
        else
            {
            CActiveScheduler::Stop();
            aDownload.Delete();
            }
        }
    else if ( EHttpDlCompleted == aEvent.iDownloadState )
        {
        CActiveScheduler::Stop();
        // We don't need the leave code; un-branded service is OK.
        TRAP_IGNORE( UncompressFilesL() );
        aDownload.Delete();
        }
    else if ( EHttpDlFailed == aEvent.iDownloadState )
        {
        CActiveScheduler::Stop();
        aDownload.Delete();
        }
    else if ( EHttpDlInprogress == aEvent.iDownloadState && 
        EHttpProgDisconnected == aEvent.iProgressState )
        {
        CActiveScheduler::Stop();
        aDownload.Delete();
        }
    }

// ---------------------------------------------------------------------------
// Starts branding data download.
// ---------------------------------------------------------------------------
//
TInt CVoIPProvisioningAppUi::FetchBrandingDataL( const TDesC8& aUrl )
    {
    if ( !aUrl.Length() || KErrNotFound == aUrl.Find( KZipExtension ) )
        {
        return KErrNotFound;
        }
    iDownloadMgr.ConnectL( KUidVoIPProvisioningAppUid, *this, EFalse );
    RHttpDownload& download = iDownloadMgr.CreateDownloadL( aUrl );
    download.SetStringAttribute( EDlAttrDestFilename, iBrandPackageName );
    TInt err = download.Start();
    CActiveScheduler::Start();
    return err;
    }

// ---------------------------------------------------------------------------
// Uncompresses files from a zip file.
// ---------------------------------------------------------------------------
//
void CVoIPProvisioningAppUi::UncompressFilesL()
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs ); // CS:1
    CZipFile* zipfile = CZipFile::NewL( fs, iBrandPackageName );
    CleanupStack::PushL( zipfile ); // CS:2
    CZipFileMemberIterator* iterator = zipfile->GetMembersL();
    CleanupStack::PushL( iterator ); // CS:3
    CZipFileMember* fileMember = iterator->NextL();
    TChar drive;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultPhoneMemory, drive );

    RFile output;
    CleanupClosePushL( output ); // CS:4

    while ( fileMember )
        {
        TFileName memberFileName = *fileMember->Name();
        TFileName outputPath;
        outputPath.Append( drive );
        if ( KErrNotFound != memberFileName.Find( KXmlExtension ) )
            {
            outputPath.Append( KBrandXmlPath );
            outputPath.Append( memberFileName );
            }
        else if ( KErrNotFound != memberFileName.Find( KInstallExtension ))
            {
            outputPath.Append( KBrandInstallPath );
            outputPath.Append( memberFileName );
            }
        else if ( KErrNotFound != memberFileName.Find( KMifExtension ))
            {
            outputPath.Append( KBrandMifPath );
            outputPath.Append( memberFileName );
            }
        else
            {
            // Ignore this file and get next file from zip.
            fileMember = iterator->NextL();
            break;
            }

        TInt err = fs.MkDirAll( outputPath );
        err = output.Replace( fs, outputPath, EFileShareAny|EFileWrite );

        TUint32 size = fileMember->UncompressedSize();
        RBuf8 outputData;
        outputData.CreateL( size );
        CleanupClosePushL( outputData ); // CS:5
        RZipFileMemberReaderStream* stream;
        zipfile->GetInputStreamL( fileMember, stream );
        CleanupStack::PushL( stream ); // CS:6

        err = stream->Read( outputData, size );
        err = output.Write( outputData );

        output.Flush();

        // stream, &outputData
        CleanupStack::PopAndDestroy( 2, &outputData ); // CS:4

        fileMember = iterator->NextL();
        }
    // &output, iterator, zipfile, &fs
    CleanupStack::PopAndDestroy( 3, zipfile ); // CS:1

    // Finally, delete temp zip file.
    fs.Delete( iBrandPackageName );    
    CleanupStack::PopAndDestroy( &fs ); // CS:0
    }

// End of file.
