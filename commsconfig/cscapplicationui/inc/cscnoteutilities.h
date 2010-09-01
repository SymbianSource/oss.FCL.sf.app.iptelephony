/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CSC Application note utilities
*
*/


#ifndef C_CSCNOTEUTILITIES_H
#define C_CSCNOTEUTILITIES_H

/**
 *  CCSCNoteUtilities class
 *  Used to show CSC Application notes.
 *
 *  @lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CCSCNoteUtilities ) : public CBase
    {
    public: 
    
        /** Enumeration for different note types */
        enum TCSCNoteType
            {
            // Information notes..
            ECSCUnableToConfigureNote,
            ECSCUnableToDeleteNote,
            // Query dialogs..
            ECSCConfigureServiceQuery,
            ECSCDeleteServiceQuery
            };
        
            
        /**
         * Shows a confirmation note without query. 
         *
         * @since S60 v3.2
         * @param aType for note to be shown
         * @param aText for additional text information
         */ 
        static void ShowConfirmationNote( 
            TCSCNoteType aType, 
            const TDesC& aText = KNullDesC() );
        
        
        /**
         * Shows an information note without query. 
         *
         * @since S60 v3.2
         * @param aType for note to be shown
         * @param aText for additional text information
         * @param aArray for additional text information
         */ 
        static void ShowInformationNoteL( 
            TCSCNoteType aType, 
            const TDesC& aText = KNullDesC(), 
            const MDesCArray* aArray = NULL );
        
        
        /**
         * Shows an error note without query.
         *
         * @since S60 v3.2
         * @param aType for note to be shown
         * @param aText for additional text information
         */ 
        static void ShowErrorNote( 
            TCSCNoteType aType, 
            const TDesC& aText = KNullDesC() );
        
        
        /**
         * Shows a note with query.
         *
         * @since S60 v3.2
         * @param aType for note to be shown
         * @param aText for additional text information
         * @return ETrue if accepted
         */ 
        static TBool ShowCommonQueryL( 
            TCSCNoteType aType, 
            const TDesC& aText = KNullDesC() );
    
    };

#endif // C_CSCNOTEUTILITIES_H
