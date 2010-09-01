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
* Description:  Declares private API for the CCH UI component.
*
*/


#ifndef C_CCHUIPRIVATEAPI_H
#define C_CCHUIPRIVATEAPI_H

#include <e32base.h>
#include <cchui.h>

#include "cchuicommon.hrh"

class CCchUiApiImpl;
class CCch;

/**
 *  CCH UI Private API - provides means to control CCH UI.
 *  
 *  This API hides the actual implementation from clients. 
 *  In essence this API provides internal access to connectivity UI logic. 
 *
 *  @code
 *  @endcode
 *
 *  @lib cchui.lib
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( CCchUiPrivateApi ) : public CBase, public MCchUi
    {
    public:
        
        /**
         * Two-phased constructor.
         * @param aObserver reference to observer insterested in CCH UI events
         * Ownership is not taken.
         */
        static CCchUiPrivateApi* NewL( CCch& aCch );

        /**
         * Destructor.
         */
        ~CCchUiPrivateApi();


        /**
         * Tells CCH UI that manual enable has proceeded and visualization should
         * be handled according to events and provided result code.
         *
         * @since S60 5.0
         * @param aServiceId service id of the service that the dialog is 
         *        shown for. Service id is used to fetch service specific data.
         * @param aEnableResult result of the enable operation. If enable specific
                  error, error is handled here instead of event listening
         */
        void ManualEnableResultL( TUint32 aServiceId,
            TInt aEnableResult );
        
        
        // from base class MCchUi
        
        /**
         * From MCchUi. 
         * Adds observer for listening cchui events.
         *
         * @since S60 5.0
         * @param aObserver Event observing class
         */
        void AddObserverL( MCchUiObserver& aObserver );
        
        /**
         * From MCchUi.
         * Removes the observer of cchui events.
         * 
         * @since S60 5.0
         * @param aObserver Event observing class
         */
        void RemoveObserver( MCchUiObserver& aObserver );
        
        /**
         * From MCchUi.
         * Shows global note/dialog. See observer definition for available notes. 
         *
         * @since S60 5.0
         * @param aServiceId service id of the service that the dialog is 
         *        shown for. Service id is used to fetch service specific data.
         * @param aDialog defines which dialog is shown.
         * @leave KErrAlreadyExists Failed to show dialog because another 
         * note already shown.
         */
        void ShowDialogL( 
            TUint32 aServiceId, 
            MCchUiObserver::TCchUiDialogType aDialog );
        
        /**
         * From MCchUi.
         * Allows client to configure visualization of connectivity events. This
         * only effects the automatically shown connectivity notes. Client can
         * still use separate methods in this API to show specific notes by
         * itself. 
         * 
         * By default all notes are allowed.
         *
         * @since S60 5.0
         * @param aAllowedNotes array of allowed notes
         * @param aAllowedSubServices array of allowed subservices
         */
        void ConfigureVisualizationL( 
            RArray<MCchUiObserver::TCchUiDialogType>& aAllowedNotes,
            RArray<TCCHSubserviceType>& aAllowedSubServices );    
        
        /**
         * From MCchUi.
         * Cancels all notes shown by CCH UI. 
         * If there is nothing to dismiss, call to this method does nothing.
         *
         * @since S60 5.0
         */
        void CancelNotes();   
        
        /**
         * Reserved for future use.
         *
         * @since S60 5.0
         */  
        void Reserved1();  
        
        /**
         * Reserved for future use.
         *
         * @since S60 5.0
         */  
        void Reserved2();  
        
    private: // Constructors
    
    	    /**
    	    * C++ default constructor.
    	    */
    	    CCchUiPrivateApi();

    	    /**
    	    * By default Symbian 2nd phase constructor is private.
    	    */
    	    void ConstructL( CCch& aCch );
        

    private: // data

        /**
         * CCHUI API implementation.
         * Own.
         */
        CCchUiApiImpl* iImpl;
        
    CCHUI_UNIT_TEST( UT_CchUi )
    };

#endif

