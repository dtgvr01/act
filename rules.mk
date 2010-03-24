
#################################################################
#
#                           General
#
#################################################################

TOPDIR:=$(shell pwd)

KERNEL_SOURCE = $(UCLINUX_DIST)/linux-2.6.x
VERSION = 0.2.0

BUILD_DIR = $(TOPDIR)
DL_DIR = $(TOPDIR)/dl
UCLINUX_DIST=$(TOPDIR)/uClinux-dist
STAGING_DIR=$(UCLINUX_DIST)/staging
INCLUDE_DIR=$(TOPDIR)/include
PACKAGE_DIR=$(TOPDIR)/ipkg
PKG_BUILD_DIR=$(TOPDIR)/ipkg
IPKG_STATE_DIR:=$(TOPDIR)/root/usr/lib/ipkg
SCRIPT_DIR=$(TOPDIR)/scripts
TOOLCHAIN_PATH=/opt/uClinux/bfin-linux-uclibc/bin:/opt/uClinux/bfin-uclinux/bin

ARCH=bfin
export PATH:= $(TOOLCHAIN_PATH):$(PATH):scripts:scripts/ipkg-utils:include
export SHELL=/usr/bin/env bash -c '. $(TOPDIR)/include/shell.sh; eval "$$2"' --
PATCH_KERNEL=$(TOPDIR)/scripts/kernel-patch.sh
TARGET_CROSS=bfin-linux-uclibc-
STRIP=bfin-linux-uclibc-strip
TARGET_STRIP=$(STRIP)
HOSTCC:=gcc
TAR_OPTIONS=-xf
WGET=wget
SVN=svn co

export ROMFSINST:=$(TOPDIR)/scripts/romfs-inst.sh

export CONFIGURE_OPTS=--host=bfin-linux-uclibc \
CFLAGS=-I$(STAGING_DIR)/usr/include \
LDFLAGS=-L$(STAGING_DIR)/usr/lib \


################################################################
# code to support ipkg Makefiles, I can't help thinking using
# makefile code to build ipkgs is doing this the hard way!

# where to build (and put) .ipk packages
IPKG:= \
  PATH="$(STAGING_DIR)/bin:$(PATH)" \
  IPKG_TMP=$(BUILD_DIR)/tmp \
  IPKG_INSTROOT=$(TOPDIR)/root \
  IPKG_CONF_DIR=$(STAGING_DIR)/etc \
  IPKG_OFFLINE_ROOT=$(TOPDIR)/root \
  $(SCRIPT_DIR)/ipkg -force-defaults -force-depends

# invoke ipkg-build with some default options
IPKG_BUILD:= \
  ipkg-build -c -o 0 -g 0

# strip an entire directory
RSTRIP:= \
  NM="$(TARGET_CROSS)nm" \
  STRIP="$(STRIP)" \
  STRIP_KMOD="$(TARGET_CROSS)strip --strip-unneeded --remove-section=.comment" \
  $(SCRIPT_DIR)/rstrip.sh

#DUMP=0

define shvar
V_$(subst .,_,$(subst -,_,$(subst /,_,$(1))))
endef

define shexport
$(call shvar,$(1))=$$(call $(1))
export $(call shvar,$(1))
endef

include include/package.mk
