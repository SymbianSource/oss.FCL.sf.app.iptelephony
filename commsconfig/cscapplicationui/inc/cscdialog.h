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
* Description:  CSC Application´s Dialog
*
*/


#ifndef C_CSCDIALOG_H
#define C_CSCDIALOG_H

class CAknGlobalNote;
    
/**
 *  CCSCDialog class
 *  Used to show CSC Application glogal dialogs.
 *
 *  @lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CCSCDialog ) : public CActive
    {
    public:
    
        /**
         * Two-phased constructor.
         */
        static CCSCDialog* NewL();


        /**
         * Two-phased constructor.
         */
        static CCSCDialog* NewLC();
        
        
        /**
         * Destructor.
         */
        virtual ~CCSCDialog();
    

        /**
         * Launches a wait note.
         *
         * @since S60 v3.2
         */
        void LaunchWaitNoteL( 
            const TInt aResourceId );
        
        
        /**
         * Destroys a wait note.
         *
         * @since S60 v3.2
         */
        void DestroyWaitNote();
        
        
    protected:      

        // from base class CActive
    
        /**
         * @see CActive.
         */
        void RunL();


        /**
         * @see CActive.
         */
        void DoCancel();
        
    private:

        CCSCDialog();

        void ConstructL();
        
    private: // Data  

        /**
         * A global wait note.
         * Own.
         */
        CAknGlobalNote* iWaitNote;
        
       /*
        * Global note Id
        */
       TInt iNoteId;
        
#ifdef _DEBUG
    friend class UT_CSC;
#endif
     };

#endif // C_CSCDIALOG_H  
