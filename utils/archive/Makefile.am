SUBDIRS = \
	docs

bin_SCRIPTS = \
	lepton-archive

EXTRA_DIST = \
	lepton-archive.scm

do_subst = sed -e 's,[@]libdir[@],$(libdir),g' \
	-e 's,[@]GUILE[@],$(GUILE),g' \
	-e 's,[@]LEPTON_SCHEME_MODULE_DIRECTORY[@],$(LEPTON_SCHEME_MODULE_DIRECTORY),g' \
	-e 's,[@]localedir[@],$(localedir),g' \
	-e 's,[@]ccachedir[@],@LEPTON_SCM_PRECOMPILE_DIR@,g'

lepton-archive: lepton-archive.scm
	$(do_subst) < $(srcdir)/$@.scm > $@
	chmod +x $@

CLEANFILES = \
	lepton-archive
