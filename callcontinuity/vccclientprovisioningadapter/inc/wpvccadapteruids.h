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
* Description:   UIDs for Vcc Adapter.
*
*/



#ifndef VCCCPADAPTERUID_H
#define VCCCPADAPTERUID_H

/** ECOM plugin */
#define KECOMUid 0x10009D8D

/** UID for the VCC Adapter DLL */
//	dll_uid = 
// mmp:
// UID             0x10009D8D 0x1027540A
// START RESOURCE 1027540A.rss
#define KProvisioningVccAdapterDllUid 0x2000CFAC
// 0x1027540A

/** UID for provided ECOM implementation */
//implementation_uid
//IMPLEMENTATION_PROXY_ENTRY
#define KProvisioningVccAdapterImplementationUid 0x2000CFAD
//0x1027540B

/**
* UID of the Provisisioning Adapter plugin interface
* (taken from S60\provisioning\group\ProvisioningUIDs.h)
*/
#define KProvisioningAdapterInterface 0x101F84D5


#endif // VCCCPADAPTERUID_H
