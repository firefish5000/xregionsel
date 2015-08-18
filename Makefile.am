## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in xregionsel.spec

LDFLAGS           = -L/usr/X11R6/lib -L/usr/lib -L/usr/local/lib
INCLUDES          = -I/usr/X11R6/include \
$(X_CFLAGS) -I$(prefix)/include -I$(includedir) -I.

man_MANS = xregionsel.1

docs_DATA = README AUTHORS ChangeLog TODO
docsdir = $(prefix)/doc/xregionsel

EXTRA_DIST = \
xregionsel.spec xregionsel.1 $(docs_DATA)

SUBDIRS = src
