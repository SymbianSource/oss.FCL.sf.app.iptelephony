/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This file contains the header file of the MCchUi class.
*
*/


#ifndef M_CCHUI_H
#define M_CCHUI_H

#include <cch.h>
#include <cchuiobserver.h>

/**
 *  Virtual interface of cchui.
 *
 *  @code
 *   // One way to use CCHUI API is to configure which notes it shows
 *   // and after that do normal enable through CCH
 *   iCch = CCch::NewL();
 *   
 *   // Allow connectivity notes to be shown when enabling service
 *   iCch->SetConnectivityDialogsAllowed( ETrue );
 * 
 *   // Get handle to cchui api
 *   iCchUi = &iCch->CchUiApi(); // iCchUi is pointer to MCchUi
 * 
 *   // Set observer to listen cchui events.
 *   iCchUi->AddObserverL( *this );  // parameter is MCchUiObserver
 *   
 *   // Configure CCHUI API to show all notes/dialogs except connecting note.
 *   // Also configure that only VoIP and IM subservices are allowed (notes are
 *   // shown only for those).
 *   RArray<MCchUiObserver::TCchUiDialogType> allowedNotes;
 *   RArray<MCchUiObserver::TCchUiDialogType> allowedSubServices;
 *   CleanupClosePushL( allowedNotes );
 *   CleanupClosePushL( allowedSubServices );
 * 
 *   allowedNotes.AppendL( MCchUiObserver::ECchUiDialogTypeAuthenticationFailed );
 *   allowedNotes.AppendL( MCchUiObserver::ECchUiDialogTypeNoConnectionDefined );
 *   allowedNotes.AppendL( MCchUiObserver::ECchUiDialogTypeNoConnectionAvailable );
 *   allowedNotes.AppendL( MCchUiObserver::ECchUiDialogTypeChangeConnection );
 *   allowedNotes.AppendL( MCchUiObserver::ECchUiDialogTypeDefectiveSettings );     
 *   
 *   allowedSubServices.AppendL( ECCHVoIPSub );
 *   allowedSubServices.AppendL( ECCHIMSub );
 * 
 *   iCchUi->ConfigureVisualizationL( 
 *       allowedNotes, allowedSubServices );
 *   
 *   CleanupStack::PopAndDestroy( &allowedNotes );
 *   
 *   CCchService* service = iCch->GetService( serviceId );
 *   service->Enable( ECCHUnknown );
 * 
 * 
 * 
 * 
 *   // Another way to use CCHUI API is to just show specific notes.
 *   iCch = CCch::NewL();
 *   
 *   // Get handle to cchui api
 *   iCchUi = &iCch->CchUiApi(); // iCchUi is pointer to MCchUi
 * 
 *   // Set observer to listen cchui events.
 *   iCchUi->AddObserverL( *this );  // parameter is MCchUiObserver
 * 
 *   // Show connecting dialog.
 *   iCchUi->ShowDialogL( 
 *       serviceId, 
 *       MCchUiObserver::ECchUiDialogTypeConnecting );
 * 
 *   // disable note by cancelling
 *   iCchUi->CancelNotes();
 * 
 *   // Remove observer
 *   iCchUi->RemoveObserver();  
 * 
 *  @endcode
 * 
 *  @lib cch.lib
 *  @since S60 5.0
 */
class MCchUi
    {
    public:
        /**
        * destructor
        */
        virtual ~MCchUi( ) {};
        
    public:
        
        /**
         * Adds observer for listening cchui events.
         *
         * @since S60 5.0
         * @param aObserver Event observing class
         */
        virtual void AddObserverL( MCchUiObserver& aObserver ) = 0;
        
        /**
         * Removes the observer of cchui events.
         * 
         * @since S60 5.0
         * @param aObserver Event observing class
         */
        virtual void RemoveObserver( MCchUiObserver& aObserver ) = 0;
        
        /**
         * Shows global note/dialog. See observer definition for available notes. 
         *
         * @since S60 5.0
         * @param aServiceId service id of the service that the dialog is 
         *        shown for. Service id is used to fetch service specific data.
         * @param aDialog defines which dialog is shown.
         * @leave KErrAlreadyExists Failed to show dialog because another 
         * note already shown.
         */
        virtual void ShowDialogL( 
            TUint32 aServiceId, 
            MCchUiObserver::TCchUiDialogType aDialog ) = 0;
           
        /**
         * Allows client to configure visualization of connectivity events. This
         * only effects the automatically shown connectivity notes. Client can
         * still use seperate methods in this API to show specific notes by
         * itself. 
         * 
         * By default all notes are allowed.
         *
         * @since S60 5.0
         * @param aAllowedNotes array of allowed notes
         */
        virtual void ConfigureVisualizationL( 
            RArray<MCchUiObserver::TCchUiDialogType>& aAllowedNotes,
            RArray<TCCHSubserviceType>& aAllowedSubServices ) = 0;    
           
        /**
         * Cancels all notes shown by CCH UI. 
         * If there is nothing to dismiss, call to this method does nothing.
         *
         * @since S60 5.0
         */
        virtual void CancelNotes() = 0;         
           
        /**
         * Reserved for future use.
         *
         * @since S60 5.0
         */    
        virtual void Reserved1() = 0;         

        /**
         * Reserved for future use.
         *
         * @since S60 5.0
         */    
        virtual void Reserved2() = 0;
    };

#endif // M_CCHUI_H
