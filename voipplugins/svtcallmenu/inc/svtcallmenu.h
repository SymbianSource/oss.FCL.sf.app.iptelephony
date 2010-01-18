/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Main class for svtcallmenu.
*
*/


#ifndef C_SCMTELCALLMENU_H
#define C_SCMTELCALLMENU_H

#include <telmenuextension.h>

class CSvtCallStateHandler;

/**
 *  Main class for svtcallmenu.
 *
 *  @lib svtcallmenu
 *  @since S60 v5.1
 */
NONSHARABLE_CLASS(CSvtCallMenu) :
    public CTelMenuExtension
  {
public:

    /**
     * Two-phased constructor.
     */
    static CSvtCallMenu* NewL();
  
  
    /**
     * Destructor.
     */
    ~CSvtCallMenu();

    
// from base class CTelMenuExtension
        
    /**
     * Modifys given menu pane. Removes unsupported options from call menu and 
     * adds custom menu commands to the menu.
     *
     * @param aCallArray - Array holding information of all ongoing calls.
     * @param aMenuPane - A modifyable menu pane.
     */
    void ModifyExtensionsCallMenuL( 
            const RArray<CTelMenuExtension::TCallInfo>& aCallArray,
            CEikMenuPane& aMenuPane );

    /**
     * Modifys given touch pane buttons. Dimms unsupported buttons from the
     * pane.
     *
     * @param aCallArray - Array holding information of all ongoing calls.
     * @param aTouchPane - A touch pane interface.
     */
    void ModifyExtensionsTouchPaneButtonsL( 
                    const RArray<CTelMenuExtension::TCallInfo>& aCallArray,
                    MTelTouchPaneExtension& aTouchPane );
    
    /**
     * Handles custom menu commands.
     *
     * @param aCommand It is the code of the command to be handled.
     * @return boolean value was the command handled (ETrue) or not (EFalse).
     */
    TBool HandleCommandL( TInt aCommand );


private:
    
    CSvtCallMenu();

    /**
     * Creates new call state handler.
     *
     * @param aCallArray - Array holding information of all ongoing calls.
     */
    void CreateCallStateHandlerL( 
            const RArray<CTelMenuExtension::TCallInfo>& aCallArray );
      
private:    // data

    /**
     * Pointer to call state handler.
     * Own.
     */
    CSvtCallStateHandler* iCallStateHandler; 

 
#ifdef _DEBUG   
    friend class T_CSvtCallMenuPlugin;
#endif
    
  };  

#endif  // C_SCMTELCALLMENU_H


