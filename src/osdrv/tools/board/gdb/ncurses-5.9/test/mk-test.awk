# $Id: mk-test.awk,v 1.12 2010/11/06 23:06:48 tom Exp $
##############################################################################
# Copyright (c) 2006-2007,2010 Free Software Foundation, Inc.                #
#                                                                            #
# Permission is hereby granted, free of charge, to any person obtaining a    #
# copy of this software and associated documentation files (the "Software"), #
# to deal in the Software without restriction, including without limitation  #
# the rights to use, copy, modify, merge, publish, distribute, distribute    #
# with modifications, sublicense, and/or sell copies of the Software, and to #
# permit persons to whom the Software is furnished to do so, subject to the  #
# following conditions:                                                      #
#                                                                            #
# The above copyright notice and this permission notice shall be included in #
# all copies or substantial portions of the Software.                        #
#                                                                            #
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR #
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   #
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    #
# THE ABOVE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      #
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    #
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        #
# DEALINGS IN THE SOFTWARE.                                                  #
#                                                                            #
# Except as contained in this notice, the name(s) of the above copyright     #
# holders shall not be used in advertising or otherwise to promote the sale, #
# use or other dealings in this Software without prior written               #
# authorization.                                                             #
##############################################################################
#
# Author: Thomas E. Dickey
#
# generate Makefile for ncurses tests.
BEGIN	{
		first = 1;
		count = 0;
	}
/^#/	{
		next;
	}
/^$/	{
		next;
	}
	{
		if (first) {
			print "# generated by mk-test.awk\n";
			first = 0;
		}
		progs[count] = $1;
		flags[count] = $2;
		using[count] = $3;
		files[count] = "";
		for (n = 4; n <= NF; ++n) {
			files[count] = sprintf("%s $(MODEL)/%s$o", files[count], $n);
		}
		count = count + 1;
	}
END	{
	for (n = 0; n < count; ++n) {
		if (n == 0) {
			printf "TESTS\t= ";
		} else {
			printf "\t  ";
		}
		printf "$(destdir)%s$x", progs[n];
		if (n < count - 1) {
			printf " \\";
		}
		print "";
	}
	print	""
	print	"all: $(TESTS)"
	print	""
	print	"sources:"
	print	""
	print	"tags:"
	print	"	$(CTAGS) *.[ch]"
	print	""
	print	"# no libraries here"
	print	"libs \\"
	print	"install.libs \\"
	print	"uninstall.libs:"
	print	""
	if (INSTALL == "yes") {
		print	"# we might install the test-programs"
		print	"install \\"
		print	"install.test: $(BINDIR) $(TESTS)"
		print	"	$(SHELL) -c 'for src in $(TESTS); do \\"
		print	"	dst=`echo $$src | $(TRANSFORM)`; \\"
		print	"	$(INSTALL_PROG) $$src $(BINDIR)/$$dst; \\"
		print	"	done'"
		print	""
		print	"uninstall \\"
		print	"uninstall.test:"
		print	"	$(SHELL) -c 'for src in $(TESTS); do \\"
		print	"	dst=`echo $$src | $(TRANSFORM)`; \\"
		print	"	rm -f $(BINDIR)/$$dst; \\"
		print	"	done'"
	} else {
		print	"install \\"
		print	"install.test \\"
		print	"uninstall \\"
		print	"uninstall.test:"
	}
	print	""
	print	"mostlyclean ::"
	print	"	-rm -f core tags TAGS *~ *.bak *.i *.ln *.atac trace"
	print	""
	print	"clean :: mostlyclean"
	print	"	-$(SHELL) -c \"if test -n '$x' ; then $(MAKE) clean x=''; fi\""
	print	"	-rm -rf *$o screendump *.lis $(TESTS) .libs"
	print	""
	print	"distclean :: clean"
	print	"	-rm -f Makefile ncurses_cfg.h config.status config.log"
	print	""
	print	"realclean :: distclean"
	print	""
	print	"lint:"
	print	"	$(SHELL) -c 'for N in $(TESTS); do echo LINT:$$N; $(LINT) $(LINT_OPTS) $(CPPFLAGS) $(srcdir)/$$N.c $(LINT_LIBS); done'"
	print	"$(BINDIR) :"
	print	"	mkdir -p $@"


	if (ECHO_LINK != "") {
		ECHO_LINK="@ echo linking $@ ... ;"
	}
	for (n = 0; n < count; ++n) {
		print "";
		printf "$(destdir)%s$x:%s %s\n", progs[n], files[n], using[n];
		printf "\t%s$(LINK) -o $@%s %s\n", ECHO_LINK, files[n], flags[n];
	}

	}
