###################################################################
# doc/docbook/Makefile
# $Id: Makefile,v 1.7 2002-03-06 15:46:16 bdenney Exp $
#
# Builds documentation in various formats from SGML source, and
# allows easy update to the Bochs web site.
#
###################################################################

# name of the major documentation sections
SECTIONS=user documentation development

# these files get installed in addition to the sections
EXTRAS=README alldocs.html

# complete list of what to install
INSTALL_LIST=$(SECTIONS) $(EXTRAS)

# ssh to this server to install the docs
REMOTE_HOST=shell.sf.net

# path of preexisting install in, on the remote server.  Each section
# will go into a subdirectory of $REMOTE_PATH, as in
# $REMOTE_PATH/user.
REMOTE_PATH=/home/groups/b/bo/bochs/htdocs/doc/docbook

# -x means don't try to forward X authorization, it won't work for SF
SSH=ssh -x

all: $(SECTIONS)

# this assumes that for each section [S], there is a subdirectory
# called [S] and the main file to render is [S]/[S].dbk.  The $@
# expression is the target name, so when doing "make user" the $@ is
# user.
$(SECTIONS)::
	# render the docbook for a section
	cd $@; docbook2ps $@.dbk
	cd $@; docbook2pdf $@.dbk
	cd $@; docbook2html $@.dbk

# install the stuff on the remote server using ssh the bryce->bdenney
# stuff is just because Bryce's local username and sourceforge
# username don't match.  If your usernames match then the "whoami"
# will work ok.
webinst: all
	# fix permissions locally so that tar will install things right
	chmod 664 `find $(INSTALL_LIST) -type f -print`
	chmod 775 `find $(INSTALL_LIST) -type d -print`
	# copy to remote
	@echo Installing documentation on $(REMOTE_PATH)
	if test "`whoami`" = bryce; then sfuser=bdenney; \
	else sfuser=`whoami`; fi; \
	tar cf - $(INSTALL_LIST) | \
	  $(SSH) $$sfuser@$(REMOTE_HOST) "cd $(REMOTE_PATH) && umask 002 && tar xvf -"

clean:
	#remove generated files
	for S in $(SECTIONS); do \
	  rm -f $$S/*.html $$S/*.ps $$S/*.pdf $$S/*.out; \
	done
