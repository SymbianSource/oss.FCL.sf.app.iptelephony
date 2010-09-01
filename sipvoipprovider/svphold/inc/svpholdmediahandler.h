/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Pauses and resumes audio stream.
*
*/


#ifndef SVPHOLDMEDIAHANDLER_H
#define SVPHOLDMEDIAHANDLER_H

#include <e32base.h>

class CSVPHoldContext;
class CMceMediaStream;


/**
*  Media handler class for SVP hold extension.
*  @lib svp.dll
*  @since S60 3.2
*/
class CSVPHoldMediaHandler : public CBase
    {
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    static CSVPHoldMediaHandler* NewL();
    
    /**
    * Destructor.
    */
    virtual ~CSVPHoldMediaHandler();

public:  // New functions
    
    /**
    * Disables/enables media stream based on the request.
    * @since S60 3.2
    * @param aMediaStream MCE Media stream
    * @param aRequest Request type
    */
    void PerformMediaActionL( CMceMediaStream& aMediaStream,
                              TSVPHoldRequestType aRequest );
    
    /**
    * Disables needed sinks/sources.
    * @since S60 3.2
    * @param aContext SVP Hold context
    * @param aLeaveSpeakerEnabled ETrue, when only mic source is disabled
    */
    void DisableAudioL( CSVPHoldContext& aContext,
                        TBool aLeaveSpeakerEnabled = EFalse );
    
    /**
    * Disables needed sinks/sources.
    * @since S60 3.2
    * @param aContext SVP Hold context
    */
    void EnableAudioL( CSVPHoldContext& aContext );


private:

    /**
    * C++ default constructor.
    */
    CSVPHoldMediaHandler();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL() const;


private: // New functions

    /**
    * Handles streams in local hold request cases.
    * @since S60 3.2
    * @param aMediaStream MCE Media stream
    * @param aBoundStream MCE Media stream
    */
    void HandleLocalHoldingL( CMceMediaStream& aMediaStream,
                              CMceMediaStream& aBoundStream );

    /**
    * Handles streams in local resume request cases.
    * @since S60 3.2
    * @param aMediaStream MCE Media stream
    * @param aBoundStream MCE Media stream
    * @param aRequest Request type
    */
    void HandleLocalResumingL( CMceMediaStream& aMediaStream,
                               CMceMediaStream& aBoundStream,
                               TSVPHoldRequestType aRequest );

    /**
    * Handles streams in remote hold request cases.
    * @since S60 3.2
    * @param aMediaStream MCE Media stream
    * @param aBoundStream MCE Media stream
    */
    void HandleRemoteHoldingL( CMceMediaStream& aMediaStream,
                               CMceMediaStream& aBoundStream,
                               TSVPHoldRequestType aRequest );

    /**
    * Handles streams in local resume request cases.
    * @since S60 3.2
    * @param aMediaStream MCE Media stream
    * @param aBoundStream MCE Media stream
    * @param aRequest Request type
    */
    void HandleRemoteResumingL( CMceMediaStream& aMediaStream,
                                CMceMediaStream& aBoundStream,
                                TSVPHoldRequestType aRequest );
    
    /**
    * Enables audio
    * @since S60 3.2
    * @param aMediaStream MCE Media stream
    * @param aContext SVP Hold context
    */
    void EnableAudioL( CMceMediaStream& aMediaStream,
                       CSVPHoldContext& aContext );

                                             
    /**
    * Disables used audio stream (mic & speaker)
    * @since S60 3.2
    * @param aAudioStream MCE Media stream
    */
    void DisableAudioStreamL( CMceMediaStream& aAudioStream,
                              TBool aLeaveSpeakerEnabled );

    /**
    * Disables used audio stream (mic & speaker)
    * @since S60 3.2
    * @param aAudioStream MCE Media stream
    * @param aAllowMicEnable Boolean value
    */
    void EnableAudioStreamL( CMceMediaStream& aAudioStream,
                             TBool aAllowMicEnable = ETrue );

    };

#endif      // SVPHOLDMEDIAHANDLER_H  
            
