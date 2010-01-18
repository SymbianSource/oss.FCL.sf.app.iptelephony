/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SPSettings class for Device Management VoIP Settings Adapter
*
*/

#ifndef CDMVOIPSPSETTINGS_H
#define CDMVOIPSPSETTINGS_H

#include <e32base.h>
#include <e32cmn.h>

/**
 * CDMVoIPSPSettings
 * Class for transiently holding Service Provider Setting values.
 *
 * @since S60 3.2
 */
class CDMVoIPSPSettings: public CBase
    {
public:
    static CDMVoIPSPSettings* NewL();
    static CDMVoIPSPSettings* NewLC();
    virtual ~CDMVoIPSPSettings();

private:
    CDMVoIPSPSettings();
    void ConstructL();

public: // data

    /**
     * VoiceMailBox MWI URI.
     * Own.
     */
    HBufC* iVmbxMwiUri;

    /**
     * VoiceMailBox listening address (URI).
     * Own.
     */
    HBufC* iVmbxListenUri;

    /**
     * VoiceMailBox IAP ID.
     */
    TUint32 iVmbxIapId;

    /**
     * VoiceMailBox Re-SUBSCRIBE interval.
     */
    TInt iReSubscribeInterval;

    /**
     * VoiceMailBox SIP ID.
     */
    TUint32 iVmbxSipId;

    /**
     * Service provider bookmark URI.
     * Own.
     */
    HBufC* iServiceProviderBookmark;

    /**
     * Branding data URI.
     * Own.
     */
    HBufC* iBrandingDataUri;

    /**
     * Presence profile ID.
     */
    TUint32 iPresenceId;

    /**
     * RCSE profile ID.
     */
    TUint32 iRcseProfileId;

    /**
     * Presence auto accept buddy subscription request.
     */
    TInt iAutoAcceptBuddy;

    /**
     * Auto enable state of service.
     */
    TInt iAutoEnableService;

    /**
     * Snap profile ID.
     */
    TUint32 iSnapId;

    /**
     * IM enabled.
     */
    TInt iImEnabled;
    };

#endif // CDMVOIPSPSETTINGS_H

// End of file.
