###################################################################
# Makefile
# $Id: Makefile,v 1.10 2004-01-12 21:33:23 cbothamy Exp $
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

remote_update::
	sfuser=`whoami`; \
	if test "`whoami`" = bryce; then sfuser=bdenney; fi; \
        if test "`whoami`" = tbutler; then sfuser=uninet; fi; \
        if test "`whoami`" = chris; then sfuser=cbothamy; fi; \
	$(SSH) $$sfuser@$(REMOTE_HOST) "cd $(HTDOCS) && cvs update -Ad"

do_commit::
	cvs commit

commit:: update do_commit remote_update

update::
	cvs update -A -d

diff:
	cvs diff
