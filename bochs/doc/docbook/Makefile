SECTIONS=user documentation development
EXTRA_INSTALL_LIST=README alldocs.html
REMOTE_HOST=shell.sf.net
REMOTE_PATH=/home/groups/b/bo/bochs/htdocs/tmp/docbooktest
SSH_PATH=ssh

all: $(SECTIONS)

# this assumes that for each section [S], there is a subdirectory called
# [S] and the main file to render is [S]/[S].dbk.
$(SECTIONS)::
	# render the docbook for each section
	cd $@; docbook2ps $@.dbk
	cd $@; docbook2pdf $@.dbk
	cd $@; docbook2html $@.dbk

webinst: 
	@echo Installing documentation on $(REMOTE_PATH)
	if test "`whoami`" = bryce; then sfuser=bdenney; \
	else sfuser=`whoami`; fi; \
	tar cf - $(SECTIONS) $(EXTRA_INSTALL_LIST) | \
	  ssh -x $$sfuser@$(REMOTE_HOST) "cd $(REMOTE_PATH) && tar xvf -"

clean:
	#remove generated files
	for S in $(SECTIONS); do \
	  rm -f $$S/*.html $$S/*.ps $$S/*.pdf $$S/*.out; \
	done
