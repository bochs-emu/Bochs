###################################################################
# Makefile
# $Id: Makefile,v 1.6 2001-07-23 01:25:42 bdenney Exp $
#
# Runs a cvs update on the source forge shell account.
# This will only work if you are a developer on the Bochs
# project, and have set up secure shell correctly.
#
###################################################################

SSH=ssh -x
REMOTE_HOST=shell.sourceforge.net
BOCHS_PATH=/home/groups/b/bo/bochs
HTDOCS=$(BOCHS_PATH)/htdocs

remote_update::
	if test "`whoami`" = bryce; then sfuser=bdenney; \
	else sfuser=`whoami`; fi; \
        if test "`whoami`" = tbutler; then sfuser=uninet; fi; \
	$(SSH) $$sfuser@$(REMOTE_HOST) "cd $(HTDOCS) && cvs update -Ad"

do_commit::
	cvs commit

commit:: update do_commit remote_update

update::
	cvs update -A -d

diff:
	cvs diff
