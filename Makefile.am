SUBDIRS = etc data po src
datadir = @datadir@

EXTRA_DIST = AUTHORS COPYING NEWS README garagejam.spec garagejam.spec.in meson.build

# Build ChangeLog from GIT history
ChangeLog:
	@if test -f $(top_srcdir)/.git/HEAD; then \
		git log --stat --after="Wed Jul 4 16:39:12 2018 +0200" > $@; \
	fi

dist: ChangeLog

.PHONY: ChangeLog
