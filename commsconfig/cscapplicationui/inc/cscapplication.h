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
* Description:  CSC Application
*
*/



#ifndef C_CSCAPPLICATION_H
#define C_CSCAPPLICATION_H

/**
 *  An instance of CCSCApplication
 *  The application part of AVKON application framework for CSC application
 *
 *  @lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CCSCApplication ) : public CAknApplication
    {
    public:  
        
        // from base class CAknApplication

        /** 
         * From CAknApplication
         * Return the application DLL UID value
         *
         * @since S60 v3.0
         * @return the UID of this Application/Dll
         */
        TUid AppDllUid() const;

    protected: 
    
        // from base class CAknApplication
    
        /**
         * From CAknApplication
         * Create a CApaDocument object and return a pointer to it
         *
         * @since S60 v3.0
         * @return a pointer to the created document
         */
        CApaDocument* CreateDocumentL();
        
#ifdef _DEBUG
    friend class UT_CSC;
#endif

    };

#endif // C_CSCAPPLICATION_H

