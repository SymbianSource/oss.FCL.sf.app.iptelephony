/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Static utility functions for SVP.
*
*/



#ifndef SVPUTILITY_H
#define SVPUTILITY_H

#include <e32base.h>
#include <mceaudiostream.h>
#include <mcedtmfobserver.h> // dtmf
#include <mccpcallobserver.h>
#include <mceinsession.h> 
#include <ccpdefs.h>    // error definitions
#include <mccpdtmfobserver.h> // dtmf

#include "cipappphoneutils.h" // CIpAppPhoneUtils, KSVPMaxTerminalTypeLength
#include "svpconsts.h"
#include "svputdefs.h"

// FORWARD DECLARATIONS
class CRCSEProfileEntry;
class CRCSEAudioCodecEntry;
class CMceRtpSource;
class CSIPProfile;
class CVoipEventLog;
class CVoipErrorEntry;

/**
 *  Static utility function for SVP usage.
 *
 *  @lib 
 *  @since S60 3.2
 */
class CSVPUtility : public CBase
    {

public: 
    
    /**
    * Two-phased constructor.
    */
    static CSVPUtility* NewL();
    
    /**
    * Destructor.
    */
    virtual ~CSVPUtility();


public: // new methods
    
    /**
     * Sets keep alive payload and timer when CN is provisioned
     * @since Series 60 3.2
     * @param aAudioStream Representation of Mce audio stream
     * @param aKeepAliveValue Keep-alive timer value
     * @return None
     */
    void SetCNKeepAliveL( CMceAudioStream& aAudioStream,
                          TInt aKeepAliveValue );
    /**
     * Sets keep alive payload and timer when CN is not provisioned
     * @since Series 60 3.2
     * @param aAudioStream Representation of Mce audio stream
     * @param aKeepAliveValue Keep-alive timer value
     * @return None
     */
    void SetKeepAliveL( CMceAudioStream& aAudioStream,
                        TInt aKeepAliveValue );
    
    /**
     * Sets keep alive payload and timer when CN is not provisioned
     * @since Series 60 3.2
     * @param aStream Checks if CN codec is present in stream
     * @return None
     */
    TBool IsComfortNoise( CMceAudioStream& aStream );
    
    /**
    * Set codecs for given stream in Mo call case
    * Set codecs settings from database
    * Compares database codecs to those supported by Mce
    * Leave with value KErrNotFound if codecs not found. 
    * @since Series 60 3.2
    * @param aVoIPProfileId VoIP profile ID number 
    * @param audioInStream Representation of Mce audio stream
    * @param aKeepAliveValue Keep-alive timer value
    * @return None
    */
    void SetAudioCodecsMOL( TUint32 aVoIPProfileId, 
                            CMceAudioStream& audioInStream,
                            TInt aKeepAliveValue );

    /**
    * Set codecs for given stream in Mt call case
    * Compares database codecs to those supported by Mce. 
    * @since Series 60 3.2
    * @param aVoIPProfile VoIP profile entry
    * @param audioInStream Representation of Mce audio stream
    * @param aKeepAliveValue Keepalive timer value
    * @return None
    */
    void SetAudioCodecsMTL( CRCSEProfileEntry& aVoIPProfile, 
                            CMceMediaStream& aAudioStream,
                            TInt aKeepAliveValue,
                            TBool aSessionUpdateOngoing = EFalse );

    /**
    * Sets G711 codecs (PCMA & PCMU) for emergency call when there is no VoIP 
    * profile, i.e. calling emergency by IAP ID only. RCSE default profile 
    * values are used when needed.
    * @since Series 60 3.2
    * @param audioInStream   Audio stream
    * @param aKeepAliveValue Keep-alive value
    * @param aVoipProfileId VoIP profile ID, KErrNotFound, if no VoIP profile
    */
    void SetAudioCodecsForEmergencyL( CMceAudioStream& aAudioInStream,
                                      TInt aKeepAliveValue, 
                                      TInt32 aVoipProfileId = KErrNotFound );

    /**
    * Matches Mce and Ccp errors 
    * @since S60 3.2
    * @param aErrorCode Error code - may be modified
    * @param aTone Contains tone associated to possible error condition.
    * @return TCCPError CCP error
    */                             
    TCCPError GetCCPError( TInt& aErrorCode, TCCPTone& aTone ) const;
    
                                         
    /**
     * Write mce/sip errors to the voip event logger
     * @since S60 3.2
     * @param aErrorCode Error code
     * @param aSipProfileId Id of the SIP profile
     * @param aRemoteURI Address of the remote party
     * @param aMethod Failed method e.g. INVITE          
     */                             
    void LogVoipEventL( TInt aErrorCode, 
                        TUint32 aSipProfileId,
                        const TDesC& aRemoteURI,
                        const TDesC& aMethod ) const;
    
        
    /**
    * Matches Mce and CCP DTMF events 
    * @since S60 3.2
    * @param aEvent Event received from Mce DTMF observer
    * @param aDtmfStringSengin Flag needed to determine which events are 
    *                          sent to client application
    * @return TCCPDtmfEvent Matched CCP DTMF event
    */ 
    MCCPDTMFObserver::TCCPDtmfEvent GetDtmfEvent( MMceDtmfObserver::TMceDtmfEvent aEvent,
                                                  TBool aDtmfStringSending );
         
   
    /**
    * Set provisioning data for VoIP profile.
    * @param aVoIPProfile VoIP profile entry.
    * @param aUserAgentHeaders On completion, contains collected user-agent header.
    */
    void SetProvisioningDataL( CRCSEProfileEntry& aVoipProfile, 
                               CDesC8Array& aUserAgentHeaders,
                               TUint32& aSecurityStatus,
                               TBuf<KSVPMaxTerminalTypeLength>& aTerminalType,
                               TBuf<KSVPWlanMacAddressLength>& aWlanMACAddress ) const;
    /**
    * Get the terminal type at startup.
    * @since Series60 3.2
    * @param aTerminalType
    */                                                     
    void GetTerminalTypeL( TBuf< KSVPMaxTerminalTypeLength >& aTerminalType );
    
    /**
    * Get the Wlan MAC address at startup.
    * @since Series 60 3.2
    * @param aWlanMACAddress.
    */
    static void GetWlanMACAddressL( TBuf< KSVPWlanMacAddressLength >& aWlanMACAddress );
    
    /**
    * Resolves the security mechanism, checks if TLS or SIPS is used in profile
    * @since Series 60 3.2
    * @param aProfile SIP profile which is used to call
    * @param aSecureCallPreference Secure call preference of the call ( 0, 1 or 2 ) 
    */
    void ResolveSecurityMechanismL( const CSIPProfile& aProfile,
                                    TUint32& aSecureCallPreference );

    /**
    * Check Calling Line Identification Restriction (CLIR) value.
    * @since Series 60 3.2
    * @return ETRue if CLIR is ON, otherwise EFalse.
    */
    TBool IsCLIROnL() const;

    /**
    * Set fromheader.
    * If CLIR is on add anonymous info according sec status, otherwise nothing
    * @since Series 60 3.2
    * @return HBufC8* constructed fromheader.
    */
    HBufC8* SetMoFromHeaderLC( TUint aSecurityStatus );
    
	
	/**
    * Adds P-Preferred-Identity to user headers.
    * @since Series 60 3.2
	* @param aUserAgentHeaders user agent headers.
	* @param aUri own sip uri.
    * @return void
    */
    void AddPPreferredIdentityToUserHeadersL( 
        CDesC8Array& aUserAgentHeaders, const TDesC8& aUri  );
    
    /**
     * Updates the jitterbuffer size to given RTP source. Function resets
     * the iJitterBufferSize member, which is got when finding MO/MT codecs
     * in AppendCapabilitiesDataL. If iJitterBufferSize = 0, then default
     * 10 frame jitterbuffer is used.
     * @since S60 v3.2
     * @param aRtpSource RTP source which to update.
     * @return void
     */
    void UpdateJitterBufferSizeL( CMceRtpSource& aRtpSource );
    
    
    /**
     * Gets DTMF mode, Inband mode returns EFalse and Outband mode ETrue
     * @since Series 60 3.2
     * @return TBool EFalse for Inband, ETrue for Outband
     */
    TBool GetDTMFMode();
    
    /**
     * Sets DTMF mode. This function is used when audiostreams have been
     * studied about the inband/outband DTMF status.
     * @since S60 v3.2
     * @param aOutbandEnabled. ETrue if outband DTMF is enabled.
     * @return void
     */
    void SetDtmfMode( TBool aOutbandEnabled );
    
    /**
     * Gets keep-alive timer value by IAP id
     * @since S60 v3.2
     * @param aIapId IAP id
     * @param aKeepAliveValue Keep-alive timer value
     * @return ETrue if value found, else EFalse
     */
    TBool GetKeepAliveByIapIdL( TUint32 aIapId, TInt& aKeepAliveValue ) const;
    
    /**
     * Gets keep-alive timer value by AOR
     * @since S60 v3.2
     * @param aAor AOR
     * @param aKeepAliveValue Keep-alive timer value
     * @return ETrue if value found, else EFalse
     */
    TBool GetKeepAliveByAORL( const TDesC8& aAor,
        TInt& aKeepAliveValue ) const;

     
private: // new methods

    /**
     * Checks if outband DMTF is offerd  
     * @Since S60 3.2
     * @param aAudioStream Mce audiostream
     * @return TBool, ETrue -> OB offered
     */
    TBool DtmfOBInOffer( CMceAudioStream& aAudioStream );
    
    /**
     * Sets DTMF specific settings for audiostream
     * @Since S60 v3.2
     * @param aVoIPProfile Used VoIP profile
     * @param aAudioStream Audiostream, used here to set codec order num to tel-ev.
     * @return void
     * @leave system error if setting fails
     */ 
    void SetDtmfL( const CRCSEProfileEntry& aVoIPProfile, 
                  CMceAudioStream&  aAudioStream);
    
    /**
    * Method for matching ISO-639-1 language tags to Symbian language codes
    * @since Series 60 3.2
    * @param aLanguageCode Symbian language code
    * @param aLangMatch Language tag corresponding given Symbian language code
    */
    static void MatchLanguageTags( TInt aLanguageCode,
        TDes& aLangMatch );
 

    /**
    * Get audio codecs from database for certain VoIP profile.
    * @since Series60 3.2
    * @param aProfile RCSE profile where to search the codecs ID's.
    * @param aCodecIds 
    */                               
    void GetCodecIdsByVoIPProfileIdL( 
        const CRCSEProfileEntry& aProfile,
        RArray< TUint32 >& aCodecIds ) const;
    /**
     * Set audio codec values and append new codec in array.
     * @since Series60 3.2 
     * @param aAudioInStream Representation of Mce audio stream
     * @param aVoIPProfile VoIP profile entry
     * @param aCodecProfile Audio codec entry
     * @return void
     */                                                              
    void MoCallCapabilitiesL( CMceAudioStream& aAudioInStream, 
                              const CRCSEProfileEntry& aVoIPProfile,
                              const CRCSEAudioCodecEntry& aCodecProfile );
                                
    /**
     * Set AMR related provisioned parameters.
     * @since Series60 3.2 
     * @param aAudioInStream Representation of Mce audio stream
     * @param aCodecProfile Audio codec entry
     * @param aCodecNumber AMR codec number in codec array
     * @return void
     */                                                              
    void SetAMRParameters( CMceAudioStream& aAudioInStream, 
                           const CRCSEAudioCodecEntry& aCodecProfile,
                           TInt aCodecNumber );

    /**
     * Set audio codec values and append new codec in array.
     * @since Series60 3.2 
     * @param aAudioInStream Representation of Mce audio stream
     * @param aVoIPProfile VoIP profile entry
     * @param aCodecProfile Audio codec entry
     * @return void
     */                            
    void MtCallCapabilitiesL( CMceAudioStream& aAudioInStream, 
                              const CRCSEProfileEntry& aVoIPProfile,
                              const CRCSEAudioCodecEntry& aCodecProfile );
                                       
    /**
    * Removes unnecessary codecs from Mce
    * Codecs that are supported by Mce, but are found from database
    * @since Series60 3.2
    * @param aAudioInStream Representation of Mce audio stream
    * @return None
    */                                  
    void RemoveUnnecessaryCodecsL( CMceAudioStream& aAudioInStream );
     
    /**
     * Method for setting codec order priority in offer.
     * @since S60 v3.2
     * @param aAudioStream Representation of Mce audio stream.
     * @return void
     */       
    void SetDtmfCodecOrderL( CMceAudioStream& aAudioStream ) const;
    
    /**
     * Method for setting CN on for PCMA, PCMU and ILBC if 
     * codecs provisioned and offered in incoming call
     * @since S60 v3.2
     * @param aAudioStream Representation of Mce audio stream.
     * @return void
     */     
    void SetComfortNoiseL( CMceAudioStream& aAudioStream );
   
    /**
     * Method for checking and setting red media attribute
     * @since S60 v3.2
     * @param aAudioStream Representation of Mce audio stream.
     * @return void
     */     
    void SetRedundancyL( CMceAudioStream& aAudioStream );
   
    /**
     * Sets Media level Quality Of Service. This is IP level type of service or
     * DSCP value.
     * @since S60 v3.2
     * @param aQosValue Media QoS value in settings.
     * @param aSession MCE session into which set the media QoS.
     * @return void
     */
    void SetMediaQoSL( TUint32 aQosValue, CMceSession* aSession ) const;
    
    /**
     * Enables or disables RTCP sending from RTP sinks in given media stream.
     *
     * @since S60 v3.2
     * @param aRtcp RTCP setting value read from RCSE
     * @param aStream Stream whose RTP sinks to update
     * @return void
     */
    void CheckRtcpSettingL( TUint32 aRtcp, CMceMediaStream& aStream,
                            TBool aSessionUpdateOngoing = EFalse ) const;
    
    /**
      * Method for setting VAD (Voice Activity Detection) for provisioned and
      * bound codecs.  
      * @since S60 v3.2
      * @param aInAudioStream Representation of Mce audio stream
      * @param aVADSetting value of VAD setting
      * @param aCodecName codec name
      */
    void SetVADL( CMceAudioStream& aInAudioStream, 
                            TBool aVADSetting,
                            const TDesC8& aCodecName );

     /**
     * Set AMR-WB (Adaptive Multi-Rate Wideband) related provisioned parameters.
     * @since S60 v3.2
     * @param aAudioInStream Representation of Mce audio stream.
     * @param aCodecProfile Stored audio codec entry in RCSE.
     * @param aCodecNumber Codec number in codec array.
     */                                                              
    void SetAmrWbParameters( CMceAudioStream& aAudioInStream, 
                           const CRCSEAudioCodecEntry& aCodecProfile,
                           TInt aCodecNumber );   

private: // data


    /**
     * second-phase constructor
     */
    void ConstructL();
    
    /**
     * C++ default constructor.
     */
    CSVPUtility();
    
    /**
     * Flags to determine which codecs are matched
     */
    TBool iAMR;
    TBool iILBC;
    TBool iPCMU;
    TBool iPCMA;
    TBool iG729;
    TBool iCN;
    TBool iRed;
    TBool iAMRWB;
    
    /**
     * Flag which determines which DMTF mode is used
     * ETrue => Outbound, EFalse => Inbound
     */
    TBool iTelEv;
    
    /**
     * codec preference order
     */
    TInt iPreferredCodec;
    
    /**
     * Codec match flag, determines if codec matched in Mt call case
     */
    TBool iCodecMatched;
   
    /** 
     * Flag to determine whether DTMF settin is read already
     */ 
    TBool iDTMFChecked;
    
    /**
     * Jitterbuffer length found from codec settings.
     */
    TInt32 iJitterBufferSize;
    
    /**
     * Keeps track in Mt call case when matching codecs
     */
    TInt iRoundIndex;
    
    /**
     * instance of the VoIP event logger class.
     * own.
     */
    CVoipEventLog* iVoipEventLogger;

private:

    // For testing
    SVP_UT_DEFS
   
    };

#endif // SVPUTILITY_H
