/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Static audio utility functions for SVP.
*
*/


#ifndef SVPAUDIOUTILITY_H
#define SVPAUDIOUTILITY_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CMceAudioStream;
class CSVPSessionBase;
class CSVPEmergencySession;

/**
 * SVPAudioUtility class contains static audio related utility functions.
 * 
 * @lib svp.dll
 * @since S60 v3.2
 */
class SVPAudioUtility
    {
public: // new methods

    /**
     * Determines if the given audiostream is downlink (in) stream.
     * @since S60 v3.2
     * @param aStream Audiostream whose direction to determine.
     * @return TBool ETrue if given stream is downlinkstream, EFalse otherwise.
     */
    static TBool IsDownlinkStream( CMceAudioStream& aStream );
    
    /**
     * Sets MMF priorities to streams
     * @since S60 3.2
     * @param aAudioInStream Representation of MCE audio stream
     * @param aAudioOutStream Representation of MCE audio stream
     * @return TBool ETrue if outband DTMF (tel-event) is used in
     *               audiostreams.
     *               EFalse if Inband DTMF is used in audiostreams.
     */
    static TBool SetPriorityCodecValuesL( CMceAudioStream& aAudioInStream,
                                          CMceAudioStream& aAudioOutStream );
                                          
    /**
     * Sets audio priority values to streams. Note that this does not
     * return information about DTMF status. If both streams are not type
     * KMceAudioStream, this function leaves with KErrArgument.
     * @since S60 v3.2
     * @aMediaStream Representation of MCE media stream
     * @aBoundStream Representation of MCE media stream, bound to aMediaStream
     * @return void
     */
    static void SetAudioStreamPrioritiesL( CMceMediaStream& aMediaStream,
                                           CMceMediaStream& aBoundStream );
    
    /**
     * Enables speaker sink from aSink array if found.
     * @since S60 v3.2
     * @param aSink Array of media sinks.
     * @return void
     */
    static void EnableSpeakerSinkL(
        const RPointerArray< CMceMediaSink >& aSink );
    
    /**
     * Disables speaker sink from aSink array if found.
     * @since S60 v3.2
     * @param aSink Array of media sinks.
     * @return void
     */
    static void DisableSpeakerSinkL( 
        const RPointerArray< CMceMediaSink >& aSink );
    
    /**
     * Enables aMicSource if it's type is KMceMicSource.
     * @since S60 v3.2
     * @param aMicSource Media source
     * @return void
     */
    static void EnableMicSourceL( CMceMediaSource& aMicSource );
    
    /**
     * Disables aMicSource if it's type is KMceMicSource.
     * @since S60 v3.2
     * @param aMicSource
     * @return void
     */
    static void DisableMicSourceL( CMceMediaSource& aMicSource );
    
    /**
     * Enables speaker sinks from aStreams array if found and types of media
     * streams are KMceAudio.
     * @since S60 v3.2
     * @param aStreams Array of media streams.
     * @return void
     */
    static void EnableSpeakerSinksL(
        const RPointerArray< CMceMediaStream >& aStreams );
        
    /**
     * Disables mic from given media stream or from it's bound stream if
     * mic source is found from either of the streams.
     * @since S60 v3.2
     * @param aStream Media stream
     * @return void
     */
    static void DisableMicSourceL( CMceMediaStream& aStream );
    
    /**
     * Finds a codec of name aCodecname from aAudiostream. Returns NULL if not
     * found
     * @since S60 v3.2
     * @param aAudiostream Audiostream from where to search the codec.
     * @param aCodecname Name of the codec to search
     * @return CMceAudioCodec Pointer to the found codec or NULL if not found.
     */
    static CMceAudioCodec* FindCodec(
        CMceAudioStream& aAudiostream, const TDesC8& aCodecname );
    
    /**
     * Removes a codec of name aCodecname from aAudiostream if found.
     * @since S60 v3.2
     * @param aAudiostream Audiostream from where to remove the codec.
     * @param aCodecname Name of the codec to remove
     * @return void
     */
    static void RemoveCodecL( CMceAudioStream& aAudiostream,
        const TDesC8& aCodecname );
    
    /**
     * Checks the media streams if MMF priority update is needed. Function
     * assumes that all media streams are of type KMceAudio and will leave
     * with KErrArgument if it encounters other types of media streams.
     * @since S60 v3.2
     * @param aStreams Streams to check if the priority update is needed.
     * @return TBool ETrue if priority update is needed, EFalse otherwise.
     */
    static TBool MmfPriorityUpdateNeededL(
        const RPointerArray<CMceMediaStream>& aStreams );
    
    /**
     * Checks whether given SVP session is capable to DTMF action
     * (send, stop, etc.). This means that session
     * a) does not have HoldController.
     * b) has HoldController, but is in 'connected' state.
     * @since S60 v3.2
     * @param aSession SVP session to check.
     * @return ETrue If DTMF action can be performed.
     */
    static TBool DtmfActionCapableSession(
        const CSVPSessionBase& aSession );

    /**
     * Checks whether given SVP emergency session is capable to DTMF action
     * (send, stop, etc.). This means that session
     * a) does not have HoldController.
     * b) has HoldController, but is in 'connected' state.
     * @since S60 v3.2
     * @param aSession SVP emergency session to be check.
     * @return ETrue If DTMF action can be performed.
     */
    static TBool DtmfActionCapableSession(
        const CSVPEmergencySession& aSession );
            
    /**
     * Checks whether given MCE stream is capable to DTMF action
     * (send, stop, etc.). This means that stream:
     * a) Is a MCE audio stream.
     * b) It has a source.
     * c) It's source is enabled.
     * MCE server will handle the rest of the checks and actual DTMF
     * action will return a error code.
     * @since S60 v3.2
     * @param aStream MCE stream to check.
     * @return ETrue If DTMF action can be performed.
     */
    static TBool DtmfActionCapableStream( const CMceMediaStream& aStream );
        
private: // new methods

    /**
     * C++ default constructor.
     */
    SVPAudioUtility();
    
    /**
     * Method for setting DTMF priority for DTMF codec in aAudioStream if
     * found from it.
     * @since S60 v3.2
     * @param aAudioStream Representation of Mce audio stream.
     * @return TBool ETrue if aAudioStream contained DTMF codec.
     */
    static TBool SetDtmfPriorityL( CMceAudioStream& aAudioStream );
    
    /**
     * Method for setting audio priority and preference for audio codecs.
     * @since S60 v3.2
     * @param aAudioStream Representation of Mce audio stream.
     * @param aAudioPref Audio preference value to be used for the codecs.
     * @return void
     */
    static void SetAudioCodecPrioritiesL( CMceAudioStream& aAudioStream,
        TUint aAudioPref );
        
    /**
     * Checks a uplink audiostream whether it needs an MMF priority/preference
     * update.
     * @since S60 v3.2
     * @param aUplinkStream Uplink stream to check if it needs priority or
     *                      preference update.
     * @return TBool ETrue if priority update is needed, EFalse otherwise.
     */
    static TBool MmfPriorityUpdateNeededL( CMceAudioStream& aUplinkStream );
    
    };

#endif // SVPAUDIOUTILITY_H