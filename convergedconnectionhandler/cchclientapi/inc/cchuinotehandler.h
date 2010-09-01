/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Main class for note handling.
*
*/


#ifndef C_CCHUINOTEHANDLER_H
#define C_CCHUINOTEHANDLER_H

#include <e32base.h>
#include <badesca.h>

#include "cchuicommon.hrh"

class CCchUiGlobalQueryHandler;
class MCchUiNoteResultObserver;

/**
 * Note handler.
 * Note handling for cchui.
 *
 * @code
 *  // Example how to launch change connection query.
 *  iNoteController = CCCHUiNoteHandler::NewL( *this );
 *  TInt serviceId( KServiceId ); 
 *  MCchUiObserver::TCchUiDialogType dialog = 
 *      MCchUiObserver::ECchUiDialogTypeChangeConnection;
 *  iNoteController->LaunchNoteL( dialog, serviceId );  
 * @endcode
 *
 * @lib cchui.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( CCCHUiNoteHandler ): public CActive
    {
public:
    
    /**
     * Two-phased constructor.
     *
     * @param aObserver Reference to note result observer.
     */
    static CCCHUiNoteHandler* NewL( 
        MCchUiNoteResultObserver& aObserver );
    
    /**
     * Destructor.
     */
    virtual ~CCCHUiNoteHandler();
    
    /**
     * Launch note.
     *
     * @since S60 5.0
     * @param aNote Note type.
     * @param aServiceId Service id.
     * @param aIapId iap id.
     * @param aUsername service username.
     */
    void LaunchNoteL( 
        MCchUiObserver::TCchUiDialogType aNote, 
        TUint aServiceId,
        TUint aIapId,
        RBuf& aUserName );
      
    /**
     * Check if there is active note.
     *
     * @since S60 5.0
     * @retun ETrue if there is active note
     */
    TBool ActiveDialogExists();
    
    /**
     * Cancels querys.
     *
     * @since S60 5.0
     */
    void CancelOldNotes();
    
    /**
     * Returns current note.
     *
     * @since S60 5.0
     */    
    MCchUiObserver::TCchUiDialogType CurrentNote();
        
private:

    CCCHUiNoteHandler( MCchUiNoteResultObserver& aObserver );
    
    void ConstructL();
    
// from base class CActive
    
    /**
     * From CActive.
     * Called when asynchronous event occurs.
     */
    void RunL();
       
    /**
     * From CActive.
     * Called when this active object is canceled.
     */
    void DoCancel();
       
    /**
     * From CActive.
     * Asynchronous error.
     */
    TInt RunError( TInt aError );
    
private: // data

    /**
     * Global dialog handler.
     * Own.
     */
    CCchUiGlobalQueryHandler* iGlobalQueryHandler;

    /**
     * Note result observer.
     */
    MCchUiNoteResultObserver& iObserver;
    
    /**
     * Current active note.
     */
    MCchUiObserver::TCchUiDialogType iCurrentNote;

    CCHUI_UNIT_TEST( UT_CchUi )
    };

#endif // C_CCHUINOTEHANDLER_H
