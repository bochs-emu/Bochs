###################################################################
# Makefile
# $Id: Makefile,v 1.1 2001-06-17 05:20:17 bdenney Exp $
#
# Runs a cvs update on the source forge shell account.
# This will only work if you are a developer on the Bochs
# project, and have set up secure shell correctly.
#
###################################################################

SSH=ssh -x
REMOTE_HOST=shell.sf.net
BOCHS_PATH=/home/groups/b/bo/bochs
HTDOCS=$(BOCHS_PATH)/htdocs

remote_update:
	if test "`whoami`" = bryce; then sfuser=bdenney; \
	else sfuser=`whoami`; fi; \
	$(SSH) $$sfuser@$(REMOTE_HOST) "cd $(HTDOCS) && cvs update -Ad"
