/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declarition of CAcpDialog
*
*/


#ifndef ACPDIALOG_H
#define ACPDIALOG_H

#include <e32base.h>

class CAknGlobalNote;
class CAknGlobalMsgQuery;
class MAcpDialogObserver;
    
/**
 *  CAcpDialog class
 *  Declarition of CAcpDialog.
 *
 *  @lib accountcreationplugin.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAcpDialog ) : public CActive
    {
public:

    /**
     * Two-phased constructor.
     *
     * @param aObserver Dialog observer.
     */
    static CAcpDialog* NewL( MAcpDialogObserver& aObserver );

    /**
     * Two-phased constructor.
     *
     * @param aObserver Dialog observer.
     */
    static CAcpDialog* NewLC( MAcpDialogObserver& aObserver );

    /**
     * Destructor.
     */
    virtual ~CAcpDialog();

    /**
     * Launches a wait note.
     *
     * @since S60 v3.2
     * @param aWaitNote Appropriate text.
     */
    void LaunchWaitNoteL( TInt aWaitNote );

    /**
     * Launches a wait note with a custom string.
     *
     * @since S60 v3.2
     * @param aWaitNote Appropriate text.
     * @param aString Custom string.
     */
    void LaunchWaitNoteL( TInt aWaitNote, const TDesC& aString );

    /**
     * Destroys a wait note.
     *
     * @since S60 v3.2
     */
    void DestroyWaitNote();

    /**
     * Shows query with the appropriate text.
     *
     * @since S60 v3.2
     * @param aQuery Text to be displayed.
     * @return Key pressed.
     */
    TInt ShowQueryL( TInt aQuery );
    
    /**
     * Shows message query with the appropriate text.
     *
     * @since S60 v5.0
     * @param aMessageQuery Text to be displayed.
     */
    void ShowMessageQueryL( TInt aMessageQuery, const TDesC& aString );

    /**
     * Shows note with the appropriate text.
     *
     * @since S60 v3.2
     * @param aNote Text to be displayed.
     * @param aString Text used in resource loading.
     */
    void ShowNoteL( TInt aNote, const TDesC& aString );    

    /**
     * Shows note with the appropriate text.
     *
     * @since S60 v3.2
     * @param aNote Text to be displayed.
     */
    void ShowGlobalNoteL( TInt aNote );

    /**
     * Shows global note with the appropriate text.
     *
     * @since S60 v3.2
     * @param aNote Resource id of the note.
     * @param aString Text used in resource loading.
     */
    void ShowGlobalNoteL( TInt aNote, const TDesC& aString );
    
    /**
     * Shows dialog with several lines for selection.
     * 
     * @since S60 v3.2
     * @param aDialog Dialog resource to use.
     * @param aArray Array of selections.
     * @return Selected index.
     */
    TInt ShowSelectionDialogL( TInt aDialog, MDesC16Array* aArray );

protected:

// from base class CActive

    /**
     * From CActive.
     * Handles request completion event.
     * 
     * @since S60 v3.2
     */
    void RunL();

    /**
     * From CActive.
     * Cancels an outstanding request.
     * 
     * @since S60 v3.2
     */
    void DoCancel();

private:

    CAcpDialog( MAcpDialogObserver& aObserver );

private: // data  

    /**
     * Reference for dialog observer.
     */
     MAcpDialogObserver& iObserver;

    /**
     * Global wait note.
     * Own.
     */
    CAknGlobalNote* iWaitNote;
    
    /**
     * Akn global message query 
     * Own.
     */
    CAknGlobalMsgQuery* iGlobalMsgQuery;

    /**
     * Global note Id
     */
    TInt iNoteId;
    };

#endif // ACPDIALOG_H  

// End of file.
