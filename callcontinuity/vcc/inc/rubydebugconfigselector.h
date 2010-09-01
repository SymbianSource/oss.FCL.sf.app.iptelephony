/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Switches sets of the RubyDebug options. 
*                @warning DO NOT change the file. At least DO NOT check in the modified version.
*                Variants should be switched by replacing the whole rubydebugconfigselector.h 
*                with the help of a Synergy task. 
*
*/



#ifndef RUBYDEBUGCONFIGSELECTOR_H
#define RUBYDEBUGCONFIGSELECTOR_H


// CONSTANTS

/** 
 * If defined, rubydebug will be tuned with a set of options designed for external users
 * Typically it means showing all the warnings for hardware builds and only errors for the
 * SW builds
 *
 * If not defined, rubydebug will be tuned for the internal Multimodal UI team development.
 * Typically it means all the warnings always enabled
 *
 * The meaning of the variant is defined in the rubydebugcfg.h
 * @warning DO NOT change the line. At least DO NOT check in the modified version.
 *          Variants should be switched by replacing the whole rubydebugconfigselector.h 
 *          with the help of a Synergy task. 
 *
 */
#define RUBY_EXTERNAL_VARIANT

#endif // RUBYDEBUGCONFIGSELECTOR_H
            
// End of File
