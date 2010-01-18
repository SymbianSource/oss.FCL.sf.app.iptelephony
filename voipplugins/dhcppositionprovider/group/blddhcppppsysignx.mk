#
# Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:  Sign a SIS file for dhcppositionprovider
#

#
# Build	a stub SIS file	to go into ROM.
#
SOURCEDIR=..\install
CERTKEYDIR=..\internal
TARGETDIR=..\install

SISNAME=dhcppppsy_udeb
PKGNAME=dhcppppsy_udeb

$(TARGETDIR) :
	@perl -S emkdir.pl "$(TARGETDIR)"

do_nothing :
	rem do_nothing

SISFILE=$(SOURCEDIR)\$(PKGNAME).pkg
SISFILEX=$(TARGETDIR)\$(SISNAME).sisx

$(SISFILEX) : $(SOURCEDIR)\$(PKGNAME).pkg
	createsis create -cert $(CERTKEYDIR)\rd.cer -key $(CERTKEYDIR)\rd-key.pem  $(SISFILE)
#
# The targets invoked by bld...
#

MAKMAKE	: do_nothing

RESOURCE : do_nothing

SAVESPACE :	do_nothing

BLD	: do_nothing

FREEZE : do_nothing

LIB	: do_nothing

CLEANLIB : do_nothing

FINAL :$(TARGETDIR) $(SISFILEX)
	

CLEAN :	
	-erase $(SISFILEX)

RELEASABLES	: 
	@echo $(SISFILE)
