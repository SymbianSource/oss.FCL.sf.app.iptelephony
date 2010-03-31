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
* Description:  Application UI definition.
*
*/


#ifndef VOIPPROVISIONINGAPPUI_H
#define VOIPPROVISIONINGAPPUI_H

#ifdef _DEBUG
#define EUNIT_FRIEND_CLASS( x ) friend class x;
#else
#define EUNIT_FRIEND_CLASS( x )
#endif

// INCLUDES
#include <aknappui.h>
#include <downloadmgrclient.h>

class CAknWaitDialog;

/**
*  CVoIPProvisioningAppUi class.
*  This class handles the operation betweed provisioning processor,
*  notifier framework (asks if user want to install parsed service)
*  and service api (used to add service to the database if user has
*  decided so)
*/
class CVoIPProvisioningAppUi : public CAknAppUi,
                               public MHttpDownloadMgrObserver
    {
public:

    CVoIPProvisioningAppUi();
    virtual ~CVoIPProvisioningAppUi();

    /**
     * Method to be called for CVoIPProvisioningDocument when
     * that classes OpenFileL method has been called and file
     * containing the data to be parsed is received.
     *
     * @since S60 v5.0
     * @param aFile File handle to XML file.
     */
    void HandleFileL( RFile& aFile );

    /**
     * Overwritten method from CEikAppUi. This method gets called
     * when application is started by framework when file, which has
     * mime type this applicaion knows, has been selected to be
     * opened.
     * 
     * @since S60 v5.0
     * @param aCommand Type of the command
     * @param aDocumentName Name of the document to be opened
     *                      When starting up application this value
     *                      is empty
     * @param aTail Command line's tail. Not used in this implementation.
     * @return ETrue if aDocument represents an existing file, otherwise
     *         EFalse.
     */
    TBool ProcessCommandParametersL( TApaCommand aCommand,
     TFileName& aDocumentName, const TDesC8& aTail );

protected:

    /**
     * Indicates an event.
     *
     * @since S60 v5.0
     * @param aDownload The download with which the event occured.
     * @param aEvent Event type.
     */
    void HandleDMgrEventL( RHttpDownload& aDownload,
        THttpDownloadEvent aEvent );

private:

    /**
     * Show invalid data note.
     * 
     * @since S60 v5.0
     */
    void ShowFailNoteL() const;

    /**
     * Show done note.
     * 
     * @since S60 v5.0
     */
   void ShowDoneNoteL() const;

   /**
    * Fetches, i.e. downloads branding data from given URL.
    * 
    * @since S60 v5.0
    * @param aUrl URL from which branding data is downloaded.
    */
   TInt FetchBrandingDataL( const TDesC8& aUrl );

   /**
    * Uncompresses (unzips) files from GZip file.
    * 
    * @since S60 v5.0
    */
   void UncompressFilesL();

private:
    void ConstructL();
private:
    /**
     * Download manager.
     */
    RHttpDownloadMgr iDownloadMgr;

    /**
     * File name of branding zip.
     */
    TFileName iBrandPackageName;

    /**
     * Wait dialog.
     * Own.
     */
    CAknWaitDialog* iWaitDialog;
    
    EUNIT_FRIEND_CLASS( UT_CVoIPProvisioningAppUi )
    };

#endif // VOIPPROVISIONINGAPPUI_H

// End of File
