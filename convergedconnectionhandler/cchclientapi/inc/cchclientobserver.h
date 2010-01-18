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
* Description:  cch client api implementation class
*
*/



#ifndef C_CCHCLIENTOBSERVER_H
#define C_CCHCLIENTOBSERVER_H

#include <e32base.h>
#include <ccherror.h>
#include <cchtypes.h>

#include "cch.h"


/**
 *  Class for basic cch functionality, getting services
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib cch
 */
class CCchClientObserver : public CActive
    {

public:

    /**
     * Two-phased constructor.
     * 
     * @param aCchService Cch service factory
     */
    static CCchClientObserver* NewL( CCchServiceImpl& aCchService ) ;
    
    /**
     * Two-phased constructor.
     * 
     * @param aCchService Cch service factory
     */
    static CCchClientObserver* NewLC( CCchServiceImpl& aCchService );
    
    /**
     * Destructor.
     */
    ~CCchClientObserver();

    /**
     * @deprecated Do not use this anymore, change to AddObserver!
     * 
     * Adds observer for listening service events
     *
     * @param aObserver Event observing class
     */
    void SetObserver( MCchServiceStatusObserver& aObserver );
    
    /**
     * @deprecated Do not use this anymore, change to 
     * RemoveObserver method with paramater
     * 
     * Removes the observer of service events
     */
    void RemoveObserver( );

    /**
     * Adds observer for listening service events
     *
     * @param aObserver Event observing class
     * @return KErrAlreadyExist Observer already added
     *         KErrNone Observer was added
     */
    TInt AddObserver( MCchServiceStatusObserver& aObserver );
    
    /**
     * Removes the observer of service events
     *
     * @param aObserver Event observing class
     * @return KErrNotFound Observer was not found
     *         KErrNone Observer was removed
     */
    TInt RemoveObserver( MCchServiceStatusObserver& aObserver );
    
private:
    
    /**
     * The constructor
     * 
     * @param aCchService Cch service factory
     */
	CCchClientObserver( CCchServiceImpl& aCchService );
    
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private: //from CActive

	/**
	 * @see CActive.
	 */
	void RunL();
	
	/**
	 * @see CActive.
	 */
	void DoCancel();
	
	/**
	 * @see CActive.
	 */
	TInt RunError( TInt aError );
	
private: // data
	
	/**
	 * cch service factory, not own
	 */
	CCchServiceImpl& iCchService;
	
	/**
	 * Service observer, not own
	 */
	MCchServiceStatusObserver* iBackwardSupportObserver;
	
	/**
	 * Stores service status information
	 */
    TPckgBuf<TServiceStatus> iServiceStatus;

    // Observers
    RPointerArray<MCchServiceStatusObserver> iObservers;
    
#ifdef CCHUNITTEST
    friend class UT_CCchService;
#endif
    };

#endif // C_CCHCLIENTOBSERVER_H
