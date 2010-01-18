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



#ifndef C_VOIPERRORENTRY_H
#define C_VOIPERRORENTRY_H

#include <e32base.h>

/**
 *  VoIP error entry. 
 *  Container class for VoIP error information
 *
 *  @lib voipeventlog.lib
 *  @since S60 V3.2
 */
NONSHARABLE_CLASS( CVoipErrorEntry ): public CBase
    {

	public: // Constructors and destructor
	    /**
	    * Two-phased constructor.
	    */
	    IMPORT_C static CVoipErrorEntry* NewL();

	    /**
	    * Two-phased constructor.
	    */
	    IMPORT_C static CVoipErrorEntry* NewLC();

	    /**
	    * Two-phased constructor.
	    */
	    IMPORT_C static CVoipErrorEntry* NewLC( TInt aErrorCode, const TDesC& aErrorText );

	    /**
	    * Destructors.
	    */
	    IMPORT_C virtual ~CVoipErrorEntry();

	public: // Member functions

	    /**
	     * Returns error VoIP code.
	     *
	     * @since S60 V3.2
	     * @return error code of VoIP
	     */
	    IMPORT_C TInt ErrorCode() const;

	    /**
	     * Sets VoIP error code.
	     *
	     * @since S60 V3.2
	     * @param aErrorCode VoIP error code
	     */
	    IMPORT_C void SetErrorCode( TInt aErrorCode ); 

	    /**
	     * Returns error text
	     *
	     * @since S60 V3.2
	     * @return error text
	     */
	    IMPORT_C const TDesC& ErrorText() const;

	    /**
	     * Sets error text
	     *
	     * @since S60 V3.2
	     * @param aErrorText error text to be set
	     */
	    IMPORT_C TInt SetErrorText( const TDesC& aErrorText );

	    /**
	     * Returns time stamp
	     *
	     * @since S60 V3.2
	     * @return time stamp
	     */
	    IMPORT_C const TDesC& TimeStamp() const;

	    /**
	     * Sets time stamp
	     * this function is not exported, time stamp is set inside of 
	     * voipeventlog component
	     *
	     * @since S60 V3.2
	     * @param aTimeStamp time stamp to be set
	     */
	    TInt SetTimeStamp( const TDesC& aTimeStamp );
	    

	private:

	    /**
	    * C++ default constructor.
	    */
	    CVoipErrorEntry();

	    /**
	    * By default Symbian 2nd phase constructor is private.
	    */
	    void ConstructL();

	private: // data

	    /**
	     * VoIP error code
	     */
	    TInt iErrorCode;

	    /**
	     * VoIP error text
	     */
	    RBuf iErrorText;

	    /**
	     * VoIP error stamp
	     */
		RBuf iTimeStamp;
    };


#endif // C_VOIPERRORENTRY_H
