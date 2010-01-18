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
* Description: 
*
*/


#ifndef C_VOIPEVENTLOG_H
#define C_VOIPEVENTLOG_H


#include <e32base.h>

class CVoipEventLogEngine;
class CVoipErrorEntry;

/**
 *  VoIP event log API class
 *  CVoipEventLog is responsible for writing and reading error information
 *  from central repository
 *  The maximu error count is saved in cenrep ini file, if there are more errors
 *  than maximum count, old error is overwritten.
 *
 *  @code
 *  _LIT( KErrorText, "something wrong, man!" );
 *  CVoipEventLog* eventLog = CVoipEventLog::NewLC();
 *  CVoipErrorEntry* entry = CVoipErrorEntry::NewLC();
 *  entry->SetErrorCode( -1 );
 *  User::LeaveIfError( entry->SetErrorText( KErrorText ));
 *  TInt err = eventLog->WriteError( *entry );
 *  User::LeaveIfError( err );
 *  CleanupStack::PopAndDestroy( entry );
 *  CleanupStack::PopAndDestroy( eventLog );
 *  @endcode
 *
 *  @lib voipeventlog.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(CVoipEventLog) : public CBase
  {
  public:

        /**
         * Two-phased constructor.
         */
        IMPORT_C static CVoipEventLog* NewL();
        IMPORT_C static CVoipEventLog* NewLC();

        /**
         * Destructor.
         */
        IMPORT_C ~CVoipEventLog();

        /**
         * Writes an error data to event log
         *
         * @since S60 v3.2
         * @param aErrorEntry Error information
         * @return error code
         */
        IMPORT_C TInt WriteError( const CVoipErrorEntry& aErrorEntry );

        /**
         * Writes an error code to event log
         * this function only writes the error code, no other infomration
         * @since S60 v3.2
         * @param aErrorCode VoIP error identification
         * @return error code
         */
        IMPORT_C TInt WriteError( TInt aErrorCode );

        /**
         * Reads error count from the log
         *
         * @since S60 v3.2
         * @return if it is positive, it means error count, if it is negative, it means error code
         */
        IMPORT_C TInt ErrorCount() const;

        /**
         * Reads an error information from the log
         *
         * @since S60 v3.2
         * @param aIndex the index of error to be read, 0 means latest error index
         * @param aErrorEntry in return, error information corresponding to aIndex
         * @return error code
         */
        IMPORT_C TInt ReadError( TInt aIndex, CVoipErrorEntry& aErrorEntry ) const;

        /**
         * Clear all voip error entries from the log
         *
         * @since S60 v3.2
         * @return error code
         */
        IMPORT_C TInt ResetLog ();

    private:

        /**
        * C++ default constructor.
        */
        CVoipEventLog();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
         * Writes an error data to event log
         * this is leave function
         *
         * @since S60 v3.2
         * @param aErrorEntry Error information
         */
        void DoWriteErrorL( const CVoipErrorEntry& aErrorEntry );

        /**
         * Writes an error code to event log
         * this is leave function
         * @since S60 v3.2
         * @param aErrorCode VoIP error identification
         */
        void DoWriteErrorL( TInt aErrorCode );


    private: // data

        /**
         * CVoipEventLogEngine instance
         */
        CVoipEventLogEngine* iEngine;

  };

#endif  // C_VOIPEVENTLOG_H
