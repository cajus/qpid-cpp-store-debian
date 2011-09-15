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

#
# Run doxygen to generate HTML doc.
# Generate dependency files so its rebuilt only when needed. 
#

DOXYGEN = @DOXYGEN@

EXTRA_DIST = jrnl_tmpl.dox html man #rhmj.pdf rhmj.ps

all: doxygen

man: doxygen

#rhmj: doxygen

#rhmj: doxygen

doxygen:
if DOXYGEN
	@doxygen jrnl_tmpl.dox
# FIXME: doxygen seems to create files that do not compile under latex on 64-bit
# so the following section is disabled until this is sorted out.
#	@make -C latex
#	@cd latex; dvipdf refman.dvi refman.pdf
#	@cd latex; dvips refman.dvi -o refman.ps
#	@ln -fs latex/refman.pdf rhmj.pdf
#	@ln -fs latex/refman.ps rhmj.ps
else
	@echo "Doxygen not found during configuration; documentation skipped."
endif

clean-local:
	@rm -f rhmj.pdf
	@rm -f rhmj.ps
	@rm -rf latex
	@rm -rf html
	@rm -rf man
