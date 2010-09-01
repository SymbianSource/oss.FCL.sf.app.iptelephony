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


#ifndef C_VOIPEVENTLOGENGINE_H
#define C_VOIPEVENTLOGENGINE_H

#include <e32base.h>

class CRepository;
class CVoipErrorEntry;

/**
 *  Engine class for VoIP event log API.
 *  CVoipEventLogEngine do the real work for reading and
 *  writing error from/to central repository
 *
 *  @lib voipeventlog.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(CVoipEventLogEngine) : public CBase
  {
  public:

        /**
         * Two-phased constructor.
         */
        static CVoipEventLogEngine* NewL();
        static CVoipEventLogEngine* NewLC();

        /**
         * Destructor.
         */
        ~CVoipEventLogEngine();

        /**
         * Writes an error data to event log
         *
         * @since S60 v3.2
         * @param aErrorEntry Error information
         */
        void WriteErrorL( const CVoipErrorEntry& aErrorEntry );

        /**
         * Reads an error informaiton from the log
         *
         * @since S60 v3.2
         * @param aIndex the index of error to be read
         * @param aErrorEntry in return, error information corresponding to aIndex
         */
        void ReadErrorL( TInt aIndex, CVoipErrorEntry& aErrorEntry );

        /**
         * Reads error count from the log
         *
         * @since S60 v3.2
         * @param on return, error count
         */
        void ErrorCountL( TInt& aCount );

        /**
         * Reset all erros from the central repository.
         *
         * @since S60 v3.2
         * @return  Operation code.
         */
        TInt ResetLogHistory ();

        /**
         * Starts transaction in central repository
         *
         * @since S60 v3.2
         */
        void BeginTransactionLC();

        /**
         * Commits changes in repository
         *
         * @since S60 v3.2
         */
        void CommitTransactionL();

    private:

        /**
        * C++ default constructor.
        */
        CVoipEventLogEngine();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
         * Generate time stamp with current time
         *
         * @since S60 v3.2
         * @param on return, current time stamp, e.g. 11/09/2007 14:19:40
         */
        void GenerateTimeStampL( TDes& aTimeStamp ) const;

        /**
         * Get latest index number
         *
         * @since S60 v3.2
         * @return latest index number
         */
        TInt GetLatestIndexL();

        /**
         * Get new index number
         *
         * @since S60 v3.2
         * @return new index number
         */
        TInt GetNewIndexL();

        /**
         * Get maximum error count
         *
         * @since S60 v3.2
         * @return maximu error count
         */
        TInt GetMaxErrorCountL();

        /**
         * release semaphore, this function is used in custom cleanup
         *
         * @since S60 v3.2
         * @param aEngine pointer to an object which is the target of the cleanup operation
         */
        static void ReleaseSemaphore( TAny* aEngine );

        /**
         * release semaphore
         *
         * @since S60 v3.2
         */
        void DoReleaseSemaphore();

    private: // data
        /**
         * Central Repository object.
         * Own.
         */
        CRepository* iRepository;

        /**
         * RSemaphore object.
         * Own.
         */
        RSemaphore iSemaphore;

  };

#endif  // C_VOIPEVENTLOGENGINE_H


