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
* Description:  CCCHPlugin declaration
*
*/


#ifndef C_CCHPLUGIN_H
#define C_CCHPLUGIN_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "cchserviceobserver.h"

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CCchService;
class MCCHServiceNotifier;

// CLASS DECLARATION

/**
 *  CCCHPlugin declaration
 *  Class contains plug-in's information
 *  @lib cchserver.exe
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CCCHPlugin ) : public CBase,
                                  public MCchServiceObserver
    {

public: // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CCCHPlugin* NewL( TUid aUid );

    /**
     * Destructor.
     */
    virtual ~CCCHPlugin();
    
public: // New functions

    /**
     * Check Plug-ins Uid
     * @since S60 3.2
     * @param aUid Plug-in's Uid
     * @return TBool ETrue if Uid is same that this Plug-in's Uid
     */
    TBool CheckUid( const TUid aUid ) const;
    
    /**
     * Return pointer to CCHService plug-in
     * @since S60 3.2
     * @return Pointer to CCHService plug-in
     */
    CCchService* Plugin() const;

    /**
     * Adds a observer for plugin events.
     * @since S60 3.2
     * @param aObserver Pointer to observer.
     */
    void SetServiceNotifier( MCCHServiceNotifier* aObserver );

    /**
     * Removes a observer for plugin events.
     * @since S60 3.2
     * @param aObserver Pointer to observer.
     */
    void RemoveServiceNotifier( MCCHServiceNotifier* aObserver );
    
private:

    /**
     * CCchService implementation have to use this callback
     * method when service state changed. 
     * @since S60 3.2
     * @param aServiceId Service which state has changed
     * @param aType Subservice's type. 
     * @param aState A new state of the service.
     */
    void ServiceStateChanged( const TServiceSelection aServiceSelection,
        TCCHSubserviceState aState, TInt aError );    
    
private:
    
    /**
     * C++ default constructor.
     */
    CCCHPlugin( TUid aUid );
    
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( TUid aUid );

private: // data
    
    /**
     * Loaded plug-in
     */
    CCchService*                        iPlugin;
    
    /**
     * Plug-ins Uid
     */
    TUid                                iUid;
    
    /**
     * Plug-ins observers
     */
    RPointerArray<MCCHServiceNotifier>  iObservers;
    
    };

#endif // C_CCHPLUGIN_H

// End of file
