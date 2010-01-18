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
* Description:  Implements CAcpProviderSpecificContainer methods
*
*/


#include <aknlists.h>
#include <StringLoader.h>
#include <accountcreationplugin.rsg>
// SKIN: includes
#include <AknsDrawUtils.h>
#include <AknsBasicBackgroundControlContext.h>
#include <gdi.h>

#include "acpproviderspecificcontainer.h"
#include "accountcreationpluginlogger.h"
#include "accountcreationpluginconstants.h"
#include "acpcontroller.h"

const TInt KComponentControlCount = 6;
const TInt KScrollMin = -200;
const TInt KScrollMax = 10;
const TInt KEight = 8;
const TInt KPositionX = 10;
const TInt KScrollSpeed = 10;

// ---------------------------------------------------------------------------
// CAcpProviderSpecificContainer::CAcpProviderSpecificContainer
// ---------------------------------------------------------------------------
//
CAcpProviderSpecificContainer::CAcpProviderSpecificContainer( 
    CAcpController& aController )
    : iController( aController ), iScrollY( KScrollMax )
    {
    }

// ---------------------------------------------------------------------------
// CAcpProviderSpecificContainer::ConstructL
// ---------------------------------------------------------------------------
//
void CAcpProviderSpecificContainer::ConstructL( const TRect& aRect )
    {
    ACPLOG( "CAcpProviderSpecificContainer::ConstructL begin" );

    CreateWindowL();

    // Skin background.
    //SKIN: create the skin context for our control
    iBgContext = CAknsBasicBackgroundControlContext::NewL( 
        KAknsIIDQsnBgAreaMain, aRect, ETrue );

    // Provider.
    TInt index = iController.ActiveIndex();
    TPtrC providerName = iController.ProviderNameFromIndexL( index );
    TPtrC providerType = iController.ProviderTypeFromIndexL( index );
    TPtrC providerDescription = 
        iController.ProviderDescriptionFromIndexL( index );
    
    // Name label.
    iLabel = new (ELeave) CEikLabel;
    iLabel->SetContainerWindowL( *this );
    HBufC* typeText;
    typeText = StringLoader::LoadLC( R_SERVTAB_CD_DETAIL_NAME );
    iLabel->SetTextL( *typeText );
    CleanupStack::PopAndDestroy( typeText );

    // Type label.
    iLabel2 = new (ELeave) CEikLabel;
    iLabel2->SetContainerWindowL( *this );
    HBufC* typeText2;
    typeText2 = StringLoader::LoadLC( R_SERVTAB_CD_DETAIL_TYPE );
    iLabel2->SetTextL( *typeText2 );
    CleanupStack::PopAndDestroy( typeText2 );

    // Description label
    iLabel3 = new (ELeave) CEikLabel;
    iLabel3->SetContainerWindowL( *this );
    HBufC* typeText3;
    typeText3 = StringLoader::LoadLC( R_SERVTAB_CD_DETAIL_DESCRIPTION );
    iLabel3->SetTextL( *typeText3 );
    CleanupStack::PopAndDestroy( typeText3 ); 

    // Edwin for name.
    TResourceReader reader; 
    iCoeEnv->CreateResourceReaderLC( reader, R_ACP_TYPE );
    iEdwin = new (ELeave) CEikEdwin;
    iEdwin->SetContainerWindowL( *this );
    iEdwin->ConstructFromResourceL( reader );       
    CleanupStack::PopAndDestroy();
    if ( !providerName.Length() )
    	{
    	HBufC* typeText6;
        typeText6 = StringLoader::LoadLC( R_SERVTAB_CD_VALUE_UNDEFINED );
        iEdwin->SetTextL( typeText6 );
        CleanupStack::PopAndDestroy( typeText6 );
    	}
    else
        {
    	iEdwin->SetTextL( &providerName );
        }

    // Edwin for type.
    TResourceReader reader2; 
    iCoeEnv->CreateResourceReaderLC( reader2, R_ACP_TYPE );
    iEdwin2 = new (ELeave) CEikEdwin;
    iEdwin2->SetContainerWindowL( *this );
    iEdwin2->ConstructFromResourceL( reader2 );       
    CleanupStack::PopAndDestroy();
    if ( !providerType.Length() )
    	{
    	HBufC* typeText7;
        typeText7 = StringLoader::LoadLC( R_SERVTAB_CD_VALUE_UNDEFINED );
        iEdwin2->SetTextL( typeText7 );
        CleanupStack::PopAndDestroy( typeText7 );
    	}
    else
        {
    	iEdwin2->SetTextL( &providerType );
        }

    // Edwin for description.
    TResourceReader reader3; 
    iCoeEnv->CreateResourceReaderLC( reader3, R_ACP_TYPE );
    iEdwin3 = new (ELeave) CEikEdwin;
    iEdwin3->SetContainerWindowL( *this );
    iEdwin3->ConstructFromResourceL( reader3 );       
    CleanupStack::PopAndDestroy();
    if ( !providerDescription.Length() )
    	{
    	HBufC* typeText8;
        typeText8 = StringLoader::LoadLC( R_SERVTAB_CD_VALUE_UNDEFINED );
        iEdwin3->SetTextL( typeText8 );
        CleanupStack::PopAndDestroy( typeText8 );
    	}
    else
        {
    	iEdwin3->SetTextL( &providerDescription );
        }
    
    // Get label colours from skin.
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb color( 0, 0, 0 );
    
    AknsUtils::GetCachedColor( skin, color, KAknsIIDQsnTextColors,
        EAknsCIQsnTextColorsCG6 );
    iLabel->OverrideColorL( EColorLabelText, color );
    iLabel->SetEmphasis( CEikLabel::ENoEmphasis );
    iLabel2->OverrideColorL(EColorLabelText, color );
    iLabel2->SetEmphasis( CEikLabel::ENoEmphasis );
    iLabel3->OverrideColorL(EColorLabelText, color );
    iLabel3->SetEmphasis( CEikLabel::ENoEmphasis );
    
    CCharFormatLayer* formatLayer = CEikonEnv::NewDefaultCharFormatLayerL();
    CleanupStack::PushL( formatLayer );
    
    TCharFormat format;
    TCharFormatMask formatMask;
    formatLayer->Sense( format, formatMask );
    format.iFontPresentation.iTextColor = color;
    formatMask.SetAttrib( EAttColor );
    formatLayer->SetL( format, formatMask );
    
    iEdwin->SetCharFormatLayer( formatLayer->CloneL() );
    iEdwin2->SetCharFormatLayer( formatLayer->CloneL() );
    
    CleanupStack::Pop( formatLayer );
    iEdwin3->SetCharFormatLayer( formatLayer );
        
    SetRect( aRect );
    ActivateL();

    ACPLOG( "CAcpProviderSpecificContainer::ConstructL end" );
    }

// ---------------------------------------------------------------------------
// CAcpProviderSpecificContainer::NewL
// ---------------------------------------------------------------------------
//      
CAcpProviderSpecificContainer* CAcpProviderSpecificContainer::NewL( 
    CAcpController& aController,
    const TRect& aRect )
    {    
    CAcpProviderSpecificContainer* self = 
        CAcpProviderSpecificContainer::NewLC( aController, aRect );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAcpProviderSpecificContainer::NewLC
// ---------------------------------------------------------------------------
//
CAcpProviderSpecificContainer* CAcpProviderSpecificContainer::NewLC(
    CAcpController& aController,
    const TRect& aRect )
    {    
    CAcpProviderSpecificContainer* self = 
        new ( ELeave ) CAcpProviderSpecificContainer( aController );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// ---------------------------------------------------------------------------
// CAcpProviderSpecificContainer::~CAcpProviderSpecificContainer
// ---------------------------------------------------------------------------
//
CAcpProviderSpecificContainer::~CAcpProviderSpecificContainer()
    {
    ACPLOG( 
      "CAcpProviderSpecificContainer::~CAcpProviderSpecificContainer begin" );

    delete iLabel;
    delete iLabel2;
    delete iLabel3;
    delete iEdwin;
    delete iEdwin2;    
    delete iEdwin3;
    delete iBgContext;

    ACPLOG( 
        "CAcpProviderSpecificContainer::~CAcpProviderSpecificContainer end" );
    }

// ---------------------------------------------------------------------------
// CAcpProviderSpecificContainer::ComponentControl
// From class CoeControl.
// ---------------------------------------------------------------------------
//
CCoeControl* CAcpProviderSpecificContainer::ComponentControl( 
    TInt aIndex ) const
    {
    const TInt KZeroCase  = 0;
    const TInt KOneCase   = 1;
    const TInt KTwoCase   = 2;
    const TInt KThreeCase = 3;
    const TInt KFourCase  = 4;
    const TInt KFiveCase  = 5;

    switch ( aIndex )
        {
            case KZeroCase:
                return iLabel;
            case KOneCase:
                return iLabel2;
    		case KTwoCase:
    			return iLabel3;
    		case KThreeCase:
    			return iEdwin;
    		case KFourCase:
    			return iEdwin2;	
    		case KFiveCase: 
    			return iEdwin3;
            default:
                return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CAcpProviderSpecificContainer::MopSupplyObject
// From class CCoeControl.
// SKIN: pass context to child controls.
// ---------------------------------------------------------------------------
//
TTypeUid::Ptr CAcpProviderSpecificContainer::MopSupplyObject( TTypeUid aId )
    {
    if ( iBgContext )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }
    return CCoeControl::MopSupplyObject( aId );
    }

// ---------------------------------------------------------------------------
// CAcpProviderSpecificContainer::Draw
// From class CoeControl.
// ---------------------------------------------------------------------------
//
void CAcpProviderSpecificContainer::Draw( const TRect& aRect ) const
    {
    // Get the standard graphics context.
    CWindowGc& gc = SystemGc();

    // Draw skin backgound.
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
  	MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
    AknsDrawUtils::Background( skin, cc, this, gc, aRect );
    }

// ---------------------------------------------------------------------------
// CAcpProviderSpecificContainer::CountComponentControls
// From class CoeControl.
// ---------------------------------------------------------------------------
//
TInt CAcpProviderSpecificContainer::CountComponentControls() const
    {
    return KComponentControlCount;
    }

// ---------------------------------------------------------------------------
// CAcpProviderSpecificContainer::SizeChanged
// From class CoeControl.
// ---------------------------------------------------------------------------
//
void CAcpProviderSpecificContainer::SizeChanged()
    {
    RepositionLabels();

    // Redraw the control.
    DrawDeferred();
    }

// ---------------------------------------------------------------------------
// CAcpProviderSpecificContainer::HandleResourceChange
// From class CCoeControl.
// Called by framework when layout is changed.
// ---------------------------------------------------------------------------
//
void CAcpProviderSpecificContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );

    // *****************************
    // ADDED FOR SCALABLE UI SUPPORT
    // *****************************
    if ( KEikDynamicLayoutVariantSwitch == aType )
        {
        TRect rect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, rect );
        SetRect( rect );
        iBgContext->SetRect( rect );
        }
    }

// ---------------------------------------------------------------------------
// CAcpProviderSpecificContainer::ScrollText
// Scrolls text labels up or down.
// ---------------------------------------------------------------------------
//
void CAcpProviderSpecificContainer::ScrollText( TInt aDelta )
    {
    ACPLOG( "CAcpProviderSpecificContainer::ScrollText begin" );
    
    iScrollY += aDelta;
    if ( iScrollY > KScrollMax ) 
        {
        iScrollY = KScrollMax;
        }
    else if ( iScrollY < KScrollMin )
        {
        iScrollY = KScrollMin;
        }
    
    RepositionLabels();
    
    // Redraw the control.
    DrawDeferred();
    
    ACPLOG( "CAcpProviderSpecificContainer::ScrollText end" );
    }

// ---------------------------------------------------------------------------
// CAcpProviderSpecificContainer::RepositionLabels
// ---------------------------------------------------------------------------
//
void CAcpProviderSpecificContainer::RepositionLabels()
    {
    const TInt KWidthX = Rect().Width() - KPositionX;
    const TInt KDeltaY = Rect().Height() / KEight;
    
    TInt yPos = iScrollY;
    iLabel->SetExtent( TPoint( KPositionX, yPos ), 
        TSize( KWidthX, KDeltaY ) );
    yPos += KDeltaY;

    iEdwin->SetExtent( TPoint( KPositionX, yPos ), 
        TSize( KWidthX, KDeltaY ) );
    yPos += KDeltaY;

    iLabel2->SetExtent( TPoint( KPositionX, yPos ), 
        TSize( KWidthX, KDeltaY ) );
    yPos += KDeltaY;

    iEdwin2->SetExtent( TPoint( KPositionX, yPos ), 
        TSize( KWidthX, KDeltaY ) );
    yPos += KDeltaY;

    iLabel3->SetExtent( TPoint( KPositionX, yPos ), 
        TSize( KWidthX, KDeltaY ) );
    yPos += KDeltaY;

    iEdwin3->SetExtent( TPoint( KPositionX, yPos ), 
        TSize( KWidthX, Rect().Height() ) );
    }

// ---------------------------------------------------------------------------
// CAcpProviderSpecificContainer::OfferKeyEventL
// Handles key events.
// ---------------------------------------------------------------------------
//
TKeyResponse CAcpProviderSpecificContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    ACPLOG( "CAcpProviderSpecificContainer::OfferKeyEventL begin" );
    
    TKeyResponse ret = EKeyWasNotConsumed;
    
    switch ( aKeyEvent.iScanCode )
        {
        case EStdKeyUpArrow:
            if ( aType != EEventKeyUp )
                {
                ScrollText( KScrollSpeed );
                ret = EKeyWasConsumed;
                }
            break;
            
        case EStdKeyDownArrow:
            if ( aType != EEventKeyUp )
                {
                ScrollText( -KScrollSpeed );
                ret = EKeyWasConsumed;
                }
            break;
            
        default:
            break;
        }
    
    ACPLOG( "CAcpProviderSpecificContainer::OfferKeyEventL end" );
    return ret;
    }


// End of file.
