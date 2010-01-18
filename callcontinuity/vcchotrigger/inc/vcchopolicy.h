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
* Description:   Vcc handover policy definitions
*
*/



#ifndef T_VCCHOPOLICY_H
#define T_VCCHOPOLICY_H

#include <e32def.h> 

/*
* VCC Handover policy attr
* Preferred Domain Attributes 
*/
enum TVccHoPolicyPreferredDomain
    {
    ECsPreferred = 0,
    EPsPreferred,
    ECsOnly,
    EPsOnly
    };    

enum TVccHoPolicyHoDirection
    {
    ECsToPsAllowed = 0x000001,
    EPsToCsAllowed = 0x000002
    };
    
/**
 *  Class indicating the Vcc handover policy
 *
 *  @lib vcchotrigger.dll
 *  @since S60 v3.2
 */
class TVccHoPolicy
    {
    
public:    
    /**
     * Constructor.
     *
     * @since S60 3.2
     */  
    TVccHoPolicy();
    
     /**
     * Preferred domain for mobile device.
     *
     * @since S60 3.2
     * @return TVccHoPolicyPreferredMode 
     */
    TVccHoPolicyPreferredDomain PreferredDomain() const;
    
    /**
     * Set preferred domain for mobile device.
     *
     * @since S60 3.2
     * @param aPreferredDomain Prefered domain value
     */
    void SetPreferredDomain( 
            const TVccHoPolicyPreferredDomain& aPreferredDomain ); 
     
     /**
     * Allowed direction for handover.
     *
     * @since S60 3.2
     * @return TVccHoPolicyHoDirection 
     */
    TInt AllowedDirection() const;

     /**
     * Set allowed direction for handover on/off.
     * @since S60 3.2 
     * @param aAllowedDirection Allowed direction value
     */
    void SetAllowedDirection( const TInt aAllowedDirection );
     
     /**
     * Indicates that immediate domain transfer should be
     * performed when preferred network comes available.
     *
     * @since S60 3.2
     * @return TBool ETrue when Dt should be performed immediately
     */
    TBool DoImmediateHo() const;
     
    /**
     * Set immediate domain transfer flag on/off
     * @since S60 3.2
     * @param aImmediate Immediate HO value
     */
    void SetDoImmediateHo( const TBool& aImmediate );
    
     /**
     * Indicates can the domain transfer be performed
     * when active/held/waiting calls in trasnferring-out domain.
     *
     * @since S60 3.2
     * @return TBool ETrue when Dt should be performed
     */
    TBool DoHoInHeldWaitingCalls() const;
     
     /**
     * Set can domain transfer be performed
     * when active/held/waiting calls in trasnferring-out domain
     * on/off.
     *
     * @since S60 3.2
     * @param aHeldWaitingCalls Held waiting calls value
     */
    void SetDoHoInHeldWaitingCalls( const TBool& aHeldWaitingCalls );
    
    /**
     * Indicates can HO be performed when cs originated call
     * 
     * @since S60 3.2
     * @return TBool ETrue when HO can be performed
     */
    TBool DtAllowedWhenCsOriginated() const;
    
    /**
     * Set is HO allowed when the call is CS originated
     * 
     * @since S60 3.2
     * @param aDtAllowedCsOrig Is Dt allowed value
     */
    void SetDtAllowedWhenCsOriginated( 
                    const TBool& aDtAllowedWhenCsOriginated );
    
private:
    /** Preferred domain */
    TVccHoPolicyPreferredDomain iPreferredDomain;
    
    /** Allowed direction */
    TInt                        iAllowedDirection;
    
    /** Immediate HO value */
    TBool                       iImmediate;
    
    /** Held waiting calls value*/
    TBool                       iHeldWaitingCalls;
    
    /** Dt allowed in cs originated call */
    TBool                       iDtAllowedWhenCsOriginated;
    };

#endif //T_VCCHOPOLICY_H
