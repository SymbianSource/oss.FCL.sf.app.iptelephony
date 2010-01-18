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
* Description:  CSC Application´s note utilities
*
*/


#include <csc.rsg>
#include <StringLoader.h>
#include <aknnotedialog.h>
#include <aknnotewrappers.h>

#include "csclogger.h"
#include "cscnoteutilities.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Shows a confirmation note without query. 
// ---------------------------------------------------------------------------
//
void CCSCNoteUtilities::ShowConfirmationNote( 
    TCSCNoteType /*aType*/, const TDesC& /*aText*/ )
    {
    CSCDEBUG( "CCSCNoteUtilites::ShowConfirmationNote - begin" );
    
     // Not used
    
    CSCDEBUG( "CCSCNoteUtilites::ShowConfirmationNote - end" );
    }


// ---------------------------------------------------------------------------
// Shows an information note without query. 
// ---------------------------------------------------------------------------
//
void CCSCNoteUtilities::ShowInformationNoteL( 
    TCSCNoteType aType, const TDesC& aText, const MDesCArray* /*aArray*/ )
    {
    CSCDEBUG( "CCSCNoteUtilites::ShowInformationNoteL" );
    
    // Create note.
    HBufC* string = NULL;
    
    // Select text for note.
    switch ( aType )
        { 
        case ECSCUnableToConfigureNote:
            string = StringLoader::LoadL( 
                R_QTN_CSC_UNABLE_TO_CONFIGURE_NOTE );
            break;
        case ECSCUnableToDeleteNote:
            string = StringLoader::LoadL( 
                R_QTN_CSC_UNABLE_TO_DELETE_NOTE, aText );
            break;
        default:
            break;    
        }
    
   if ( string )
       {
       // Set text and show note.
       CleanupStack::PushL( string );   
       CAknInformationNote* note = 
       new ( ELeave ) CAknInformationNote( ETrue );
       note->ExecuteLD( *string );
       CleanupStack::PopAndDestroy( string );
       }
   }


// ---------------------------------------------------------------------------
// Shows an error note without query. 
// ---------------------------------------------------------------------------
//
void CCSCNoteUtilities::ShowErrorNote( 
    TCSCNoteType /*aType*/, const TDesC& /*aText*/ )
    {
    // Not used
    }

    
// ---------------------------------------------------------------------------
// Shows a note with query.
// ---------------------------------------------------------------------------
//
TBool CCSCNoteUtilities::ShowCommonQueryL( 
    TCSCNoteType aType, const TDesC& aText )
    {
    CSCDEBUG( "CCSCNoteUtilites::ShowCommonQueryL - begin" );
    
    TBool ret( EFalse );
    TBool showQuery( ETrue );
    
    // Create confirmation query dialog.
    HBufC* string = NULL;
    CAknQueryDialog* query = 
        new( ELeave ) CAknQueryDialog( CAknQueryDialog::ENoTone );
        
    CleanupStack::PushL( query );
    
    // Prepare and set text for query.
    switch ( aType )
        {
        case ECSCConfigureServiceQuery:
            query->PrepareLC( R_CSC_CONFIGURE_SERVICE_QUERY );
            string = StringLoader::LoadLC( 
                R_QTN_CSC_CONFIGURE_SERVICE_QUERY, aText );
            query->SetPromptL( *string );
            CleanupStack::PopAndDestroy( string );
            break;
        case ECSCDeleteServiceQuery:
            query->PrepareLC( R_CSC_DELETE_SERVICE_QUERY );
            string = StringLoader::LoadLC( 
                R_QTN_CSC_DELETE_SERVICE_QUERY, aText );
            query->SetPromptL( *string );
            CleanupStack::PopAndDestroy( string );
            break;
        default:
            // in default case query is not shown
            showQuery = EFalse;
            break;    
        }
    
    CleanupStack::Pop( query );
    
    // Show query.
    if ( showQuery )
    	{
    	if( query->RunLD() )
	        {
	        ret = ETrue;
	        }
		}    
    
    CSCDEBUG( "CCSCNoteUtilites::ShowCommonQueryL - end" );
        
    return ret;
    }
    
