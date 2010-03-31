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
* Description:  Document definition.
*
*/


#ifndef VoIPPROVISIONINGDOCUMENT_H
#define VoIPPROVISIONINGDOCUMENT_H

#ifdef _DEBUG
#define EUNIT_FRIEND_CLASS( x ) friend class x;
#else
#define EUNIT_FRIEND_CLASS( x )
#endif

// INCLUDES
#include <AknDoc.h>

// FORWARD DECLARATIONS
class  CEikAppUi;
class CVoIPProvisioningAppUi;

// CLASS DECLARATION
/**
*  CVoIPProvisioningDocument application class.
*/
class CVoIPProvisioningDocument : public CAknDocument
    {
    public:
        /**
        * C++ constructor.
        * @param CEikApplication& aApp
        * @return void
        */
        CVoIPProvisioningDocument( CEikApplication& aApp ) :
        							CAknDocument(aApp) { }

        /**
        * Destructor.
        * @param None
        * @return void
        */
        virtual ~CVoIPProvisioningDocument();


	// From CEikDocument.
    CFileStore* OpenFileL( TBool aDoOpen, const TDesC& aFilename, RFs& aFs );
	void OpenFileL( CFileStore*& aFileStore, RFile& aFile );

    private:
        /**
        * C++ default constructor.
        * @param None
        * @return void
        */
        CVoIPProvisioningDocument();

        /**
        * This method makes an application hidden so that it is not visible
        * for example in Applications list and FastSwap window.
        * @param CApaWindowGroupName* aWgName
        * @return void        
        */
        void UpdateTaskNameL( CApaWindowGroupName* aWgName );
        
    private:// from CAknDocument
        /**
        * Create CVoIPProvisioningDocument "App UI" object.
        * @param None
        * @return CEikAppUi*        
        */
        CEikAppUi* CreateAppUiL();
        
        EUNIT_FRIEND_CLASS( UT_CVoIPProvisioningDocument )
    };

#endif      // VoIPPROVISIONINGDOCUMENT_H
            
// End of File
