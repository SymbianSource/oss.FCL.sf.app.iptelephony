/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CVoIPProvisioningRecognizer definition.
*
*/


#ifndef VOIPPROVISIONINGRECOGNIZER_H
#define VOIPPROVISIONINGRECOGNIZER_H

// INCLUDES
#include <apmrec.h>

// DEFINITIONS

// Uid of this dll (defined in .mmp file)
const TUid KUidMimeVoIPProvisioningRecognizer = { 0x20026FE1 };

// Implementation uid for this recognizer
const TInt KVoIPProvRecImplUIDValue = 0x20026FE2;
const TUid KUidVoIPProvRecImpl = { KVoIPProvRecImplUIDValue };

// Amount of mime types this recognizer can handle
const TInt KVoIPSupportedMimeTypes( 2 );

// Exported mime types
_LIT8( KVoIPConfMimetypeSimple, "vnd.nokia.voip.config+xml" );
_LIT8( KVoIPConfMimetypeApplication, "application/vnd.nokia.voip.config+xml" );

// Strings to be searched to recognize content
_LIT8( KVoIPXMLTag, "nokia-confdoc" );

#ifdef _DEBUG
#define EUNIT_TEST_FRIEND( x ) friend class x;
#else
#define EUNIT_TEST_FRIEND( x ) 
#endif

/**
 *  Class to make recognisation for VoIP configuration XML file.
 *  
 *  @since S60 v5.0
 *  @lib voipprovrec.lib
 */
class CVoIPProvisioningRecognizer : public CApaDataRecognizerType
    {
public:

    /**
     * Default constructor. Calls CApaDataRecognizerType in it's
     * initializer list to complete construction.
     */
    CVoIPProvisioningRecognizer();

    /**
     * Static method to create instance of this recognizer.
     * This method is called by the framework.
     * Method pointer is delivered to the framework by ImplementationTable
     * table returned by ImplementationGroupProxy.
     *
     * @since S60 v5.0
     * @return Pointer to newly created instance of the recognizer.
     */
    static CApaDataRecognizerType* CreateRecognizerL();

    /**
     * Method called by the framework.
     * This method returns the desired amount of data for recognition.
     *
     * @since S60 v5.0
     * @return Preferred buffer size for recognition.
     */
    virtual TUint PreferredBufSize();

    /**
     * Method to deliver supported data types by this recognizer.
     *
     * @since S60 v5.0
     * @param aIndex Index pointing out which data type of the supported
     *               ones is returned.
     * @return Corresponding data type of the requested index.
     */
    virtual TDataType SupportedDataTypeL( TInt aIndex ) const;

private: // From CApaDataRecognizerType

    /**
     * Overwritten method to handle recognisation.
     *
     * @since S60 v5.0
     * @param aName The name of the data; typically this is a file name
     *              containing the data to be recognized.
     * @param aBuffer Buffer containing PreferredBufSize() from 
     *                the beginning of the file to be recognized.
     */
    virtual void DoRecognizeL( const TDesC& /*aName*/, const TDesC8& aBuffer );
    
    EUNIT_TEST_FRIEND( UT_CVoIPProvisioningRecognizer )
    };

#endif // VOIPPROVISIONINGRECOGNIZER_H
