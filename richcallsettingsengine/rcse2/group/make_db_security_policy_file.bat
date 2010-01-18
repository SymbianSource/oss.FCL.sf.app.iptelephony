rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:
rem

REM Copy the policy file from ..\data -folder to \epoc32\Winscw\c -folder.
echo off
xcopy ..\data\voip_profile_db_security_policy.txt \epoc32\winscw\c /Y

echo Running the policy file converter.
\epoc32\release\winscw\udeb\edbspconv /f=c:\voip_profile_db_security_policy.txt /b=c:\10202869.spd

echo copying the security policy file to s60\rcse\data
xcopy \epoc32\winscw\c\10202869.spd \s60\rcse\data\ /Y

echo Removing copied files
del \epoc32\winscw\c\voip_profile_db_security_policy.txt
del \epoc32\winscw\c\10202869.spd 
