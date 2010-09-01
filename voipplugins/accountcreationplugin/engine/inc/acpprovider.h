/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declarition of CAcpProvider
*
*/

 
#ifndef ACPPROVIDER_H
#define ACPPROVIDER_H

#include <e32base.h>

class CGulIcon;
class CFbsBitmap;

/**
 *  CAcpProvider class
 *  Declarition of CAcpProvider.
 *
 *  @lib accountcreationplugin.lib
 *  @since S60 v3.2
 */
class CAcpProvider : public CBase
    {
public: 

    static CAcpProvider* NewL();
    static CAcpProvider* NewLC();
    virtual ~CAcpProvider();

    /**
     * Copies provider data from given parameter to member data.
     *
     * @since S60 v3.2
     * @param aProvider for provider to be copies
     */ 
    void CopyL( const CAcpProvider& aProvider );

    /**
     * Returns name of the provider.
     *
     * @since S60 v3.2
     * @return name of the provider
     */ 
    TPtrC ProviderName() const;

    /**
     * Sets name of the provider.
     *
     * @since S60 v3.2
     * @param aProviderName for provider name
     */ 
    void SetProviderNameL( const TDesC& aProviderName );

    /**
     * Returns the location of icons.
     *
     * @since S60 v3.2
     * @return location of icons
     */ 
    TPtrC8 IconUrl() const;

    /**
     * Sets the location of icons.
     *
     * @since S60 v3.2
     * @param aIconUrl for location of icons
     */ 
    void SetIconUrlL( const TDesC8& aIconUrl );

    /**
     * Returns priority of the provider.
     *
     * @since S60 v3.2
     * @return priority of the provider
     */ 
    TInt Priority() const;

    /**
     * Sets priority of the provider.
     *
     * @since S60 v3.2
     * @param aPriority for provider
     */ 
    void SetPriority( TInt aPriority );

    /**
     * Returns the location of SIS file.
     *
     * @since S60 v3.2
     * @return location of SIS file
     */ 
    TPtrC8 SisUrl() const;

    /**
     * Sets the location of SIS file.
     *
     * @since S60 v3.2
     * @param aSisUrl for location of SIS file
     */ 
    void SetSisUrlL( const TDesC8& aSisUrl );

    /**
     * Returns the location of creation URL.
     *
     * @since S60 v3.2
     * @return location of creation URL
     */ 
    TPtrC8 CreationUrl() const;

    /**
     * Sets the location of creation URL.
     *
     * @since S60 v3.2
     * @param aSisUrl for location of creation URL
     */ 
    void SetCreationUrlL( const TDesC8& aCreationUrl );

    /**
     * Returns the location of activation URL.
     *
     * @since S60 v3.2
     * @return location of activation URL
     */ 
    TPtrC8 ActivationUrl() const;

    /**
     * Sets the location of activation URL.
     *
     * @since S60 v3.2
     * @param aSisUrl for location of activation URL
     */ 
    void SetActivationUrlL( const TDesC8& aActivationUrl );

    /**
     * Returns the content type of image file.
     *
     * @since S60 v3.2
     * @return mime type of image file.
     */ 
    TPtrC8 ContentData() const;

    /**
     * Sets the content type of image file.
     *
     * @since S60 v3.2
     * @param aContentData for type of image file
     */ 
    void SetContentDataL( const TDesC8& aContentData );

    /**
     * Returns the provider type
     *
     * @since S60 v3.2
     * @return type of the provider
     */ 
    TPtrC ProviderType() const;

    /**
     * Sets type of the provider
     *
     * @since S60 v3.2
     * @param aProviderType for provider type
     */ 
    void SetProviderTypeL( const TDesC& aProviderType );        

    /**
     * Returns the provider description
     *
     * @since S60 v3.2
     * @return description of the provider
     */ 
    TPtrC ProviderDescription() const;

    /**
     * Sets the description of the provider
     *
     * @since S60 v3.2
     * @param aProviderDescription for provider description
     */ 
    void SetProviderDescriptionL( const TDesC& aProviderDescription );        

    /**
     * Sets bitmap and mask for provider
     *
     * @since S60 v3.2
     * @param aBitmap Bitmap.
     * @param aMask Mask.
     */
    void SetBitmapL( CFbsBitmap* aBitmap, CFbsBitmap* aMask ); 

    /**
     * Returns bitmap
     *
     * @since S60 v3.2
     * @param aBitmap Gets a pointer to the bitmap.
     * @param aMask Gets a pointer to the mask.
     */
    void GetBitmaps( CFbsBitmap*& aBitmap, CFbsBitmap*& aMask );

private: // Constructor

    CAcpProvider();

private: // data

    /**
     * Provider name.
     * Own.
     */
    HBufC* iProviderName;

    /**
     * Location of the provider specific icon.
     * Own.
     */
    HBufC8* iIconUrl;

    /**
     * Priority of the provider.
     */
    TInt iPriority;

    /**
     * Location of the provider specific SIS file.
     * Own.
     */
    HBufC8* iSisUrl;

    /**
     * Url to be used in account creation.
     * Own.
     */
    HBufC8* iCreationUrl;

    /**
     * Url to be used in account activation.
     * Own.
     */
    HBufC8* iActivationUrl;

    /**
     * Mime type to be used when decoding image to bitmap.
     * Own.
     */
    HBufC8* iMimeType;

    /**
     * Provider typr.
     * Own.
     */
    HBufC* iProviderType;        

    /**
     * Provider description.
     * Own.
     */
    HBufC* iProviderDescription;        

    /**
     * Bitmap reference
     * Own.
     */
    CFbsBitmap* iBitmap;

    /**
     * Mask reference
     * Own.
     */
    CFbsBitmap* iMask;
    };

#endif // ACPPROVIDER_H
