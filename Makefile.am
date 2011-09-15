# Copyright (c) 2007, 2008 Red Hat, Inc.
#
# This file is part of the Qpid async store library msgstore.so.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
# USA
#
# The GNU Lesser General Public License is available in the file COPYING.

AUTOMAKE_OPTIONS = 1.9.2 foreign
ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = README etc/rhmd.conf

DISTCHECK_CONFIGURE_FLAGS = --with-qpid-checkout=/home/kpvdr/mrg/qpid

sysconf_DATA = etc/rhmd.conf

SUBDIRS = lib tests tools docs

# Update libtool, if needed.
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

#
# Build RPMs from the distribution tarball.
#
RPMDIRS=rpm/BUILD rpm/RPMS rpm/SPECS rpm/SRPMS
RPMMACROS=--define "_topdir @abs_builddir@/rpm" --define "_sourcedir @abs_builddir@"
# Override this variable e.g. with -bs to produce srpm only
RPMOPTS=-ba

clean-local:
	-rm -rf $(RPMDIRS)

.PHONY: rpmbuild

rpmbuild: $(SPEC) dist-gzip
	mkdir -p $(RPMDIRS)
	rpmbuild $(RPMMACROS) $(RPMOPTS) rhm.spec
if HAS_RPMLINT
	rpmlint `find rpm -name '*.rpm'`
else
	@echo "WARNING: rpmlint not found, could not validate RPMs."
endif

check-long: all
	$(MAKE) -C tests check-long

check-short: all
	$(MAKE) -C tests check-short

