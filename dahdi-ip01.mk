# dahdi-ip01.mk 
# Makes dahdi ipkg for IP01
# Authors: Dan Amarandei
#
# usage:
#   make -f dahdi-ip01.mk dahdi-package
# 
# Prerequisites:
#   make -f uClinux.mk && make -f oslec.mk

DAHDI_EXTRA_CFLAGS=-DCONFIG_4FX_SPI_INTERFACE

include rules.mk

DAHDI_VERSION=2.2.0.2
DAHDI_NAME=dahdi-linux-$(DAHDI_VERSION)
DAHDI_DIR=$(BUILD_DIR)/$(DAHDI_NAME)
DAHDI_SOURCE=$(DAHDI_NAME).tar.gz
DAHDI_SITE=http://downloads.asterisk.org/pub/telephony/dahdi-linux
DAHDI_UNZIP=zcat
OSLEC_DIR=$(TOPDIR)/oslec/

DAHDI_IP01_SRCDIR=$(BUILD_DIR)/src/dahdi-ip01

STAGING_INC=$(STAGING_DIR)/usr/include
STAGING_LIB=$(STAGING_DIR)/usr/lib

MOD_PATH:=$(UCLINUX_DIST)/root/lib/modules
MOD_DIR:=$(shell ls $(UCLINUX_DIST)/root/lib/modules)

PKG_NAME:=dahdi-ip01
PKG_VERSION:=2.2.0.2
PKG_RELEASE:=1

TARGET_DIR=$(TOPDIR)/tmp/$(PKG_NAME)/ipkg/$(PKG_NAME)

PKG_BUILD_DIR:=$(TOPDIR)/tmp/$(PKG_NAME)/

$(DL_DIR)/$(DAHDI_SOURCE):
	mkdir -p $(DL_DIR)
	$(WGET) -P $(DL_DIR) $(DAHDI_SITE)/$(DAHDI_SOURCE)

$(DAHDI_DIR)/.unpacked: $(DL_DIR)/$(DAHDI_SOURCE) 
	$(DAHDI_UNZIP) $(DL_DIR)/$(DAHDI_SOURCE) | \
	tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(DAHDI_DIR)/.unpacked

$(DAHDI_DIR)/.configured: $(DAHDI_DIR)/.unpacked
	cd $(DAHDI_DIR); ./configure --host=bfin-linux-uclibc

	# Add new files we need to support Blackfin.  Note we
	# use sym-links so that any changes we make to dahdi
	# get captured by SVN.

	# DR: Many of these files are the same as for the IP04, remaining
	# changes could possibly be merged in as #ifdefs or
	# patches.

	ln -sf $(DAHDI_IP01_SRCDIR)/wcfxs.c $(DAHDI_DIR)/wcfxs.c
	ln -sf $(DAHDI_IP01_SRCDIR)/fx.c $(DAHDI_DIR)/fx.c
	ln -sf $(DAHDI_IP01_SRCDIR)/bfsi.c $(DAHDI_DIR)/bfsi.c
	ln -sf $(DAHDI_IP01_SRCDIR)/wcfxs.h $(DAHDI_DIR)/wcfxs.h
	ln -sf $(DAHDI_IP01_SRCDIR)/bfsi.h $(DAHDI_DIR)/bfsi.h

	# patch for Oslec

	cd $(DAHDI_DIR); \
	patch < $(OSLEC_DIR)/kernel/zaptel-$(DAHDI_VERSION).patch
	patch -p0 < patch/dahdi-ip01.patch
	touch $(DAHDI_DIR)/.configured

dahdi: $(DAHDI_DIR)/.configured

	# build libtonezone, reqd for Asterisk

	cd $(DAHDI_DIR); make libtonezone.a

	# install files needed for other apps

	mkdir -p $(STAGING_INC)
	mkdir -p $(STAGING_INC)/dahdi
	mkdir -p $(STAGING_LIB)
	cp $(DAHDI_DIR)/tonezone.h $(STAGING_INC)/dahdi
	cp $(DAHDI_DIR)/dahdicfg.h $(STAGING_INC)
	cp $(DAHDI_DIR)/dahdi.h $(STAGING_INC)/dahdi
	cp $(DAHDI_DIR)/libtonezone.a $(STAGING_LIB)

	# build dahdicfg and dahdiscan

	cd $(DAHDI_DIR); make dahdicfg
	bfin-linux-uclibc-gcc -I$(STAGING_INC) src/dahdiscan.c \
	-o $(DAHDI_DIR)/dahdiscan -Wall

	# build dahdi.ko & wcfxs.ko

	cd $(DAHDI_DIR); \
	gcc -o gendigits gendigits.c -lm; \
	make tones.h; \
	make version.h
	make -C $(UCLINUX_DIST) EXTRA_CFLAGS=$(DAHDI_EXTRA_CFLAGS) \
	SUBDIRS=$(DAHDI_DIR) modules V=1

	# set up dir structure for package

	rm -Rf $(TARGET_DIR)
	mkdir -p $(TARGET_DIR)/lib/modules/$(MOD_DIR)
	mkdir -p $(TARGET_DIR)/bin
	mkdir -p $(TARGET_DIR)/etc/init.d 
	mkdir -p $(TARGET_DIR)/etc/asterisk

	# install

	cp -f $(DAHDI_DIR)/dahdi.ko $(DAHDI_DIR)/wcfxs.ko $(DAHDI_DIR)/bfsi.ko \
	$(TARGET_DIR)/lib/modules/$(MOD_DIR)
	cp -f $(DAHDI_DIR)/dahdicfg $(DAHDI_DIR)/dahdiscan $(TARGET_DIR)/bin
	cp files/dahdi.init $(TARGET_DIR)/etc/init.d/dahdi
	chmod a+x $(TARGET_DIR)/etc/init.d/dahdi
	cp -f files/dahdi.conf.in $(TARGET_DIR)/etc/dahdi.conf
	cp -f files/dahdi.conf.in $(TARGET_DIR)/etc/asterisk

	# doc

	mkdir -p $(TARGET_DIR)/usr/doc
	cp -f doc/dahdi.txt $(TARGET_DIR)/usr/doc

	touch $(PKG_BUILD_DIR)/.built

all: dahdi

dirclean:
	rm -Rf $(DAHDI_DIR)

define Package/$(PKG_NAME)
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Dahdi
  DESCRIPTION:=\
        Telephony hardware drivers for Atcom IP01
  DEPENDS:=oslec
  URL:=http://www.asterisk.org
endef

# post installation - add the modules.dep entries

define Package/$(PKG_NAME)/postinst
#!/bin/sh
cd /lib/modules/$(MOD_DIR)
cat modules.dep | sed '/.*dahdi.ko:/ d' > modules.tmp
cat modules.tmp | sed '/.*wcfxs.ko:/ d' > modules.tmp1
cat modules.tmp1 | sed '/.*bfsi.ko:/ d' > modules.dep
rm -f modules.tmp modules.tmp1
echo /lib/modules/$(MOD_DIR)/bfsi.ko: >> modules.dep
echo /lib/modules/$(MOD_DIR)/dahdi.ko: /lib/modules/$(MOD_DIR)/oslec.ko >> modules.dep
echo /lib/modules/$(MOD_DIR)/wcfxs.ko: /lib/modules/$(MOD_DIR)/bfsi.ko /lib/modules/$(MOD_DIR)/dahdi.ko >> modules.dep
rm -Rf /dev/dahdi
mkdir -p /dev/dahdi
mknod /dev/dahdi/ctl c 196 0
mknod /dev/dahdi/timer c 196 253
mknod /dev/dahdi/channel c 196 254
mknod /dev/dahdi/pseudo c 196 255
mknod /dev/dahdi/1 c 196 1
mknod /dev/dahdi/2 c 196 2
mknod /dev/dahdi/3 c 196 3
mknod /dev/dahdi/4 c 196 4

/etc/init.d/dahdi enable
endef

# pre-remove - remove the modules.dep entries

define Package/$(PKG_NAME)/prerm
#!/bin/sh
cd /lib/modules/$(MOD_DIR)
cat modules.dep | sed '/.*dahdi.ko:/ d' > modules.tmp
cat modules.tmp | sed '/.*wcfxs.ko:/ d' > modules.tmp1
cat modules.tmp1 | sed '/.*bfsi.ko:/ d' > modules.dep
rm -f modules.tmp modules.tmp1
/etc/init.d/dahdi disable
rm -f -r /dev/dahdi
endef

$(eval $(call BuildPackage,$(PKG_NAME)))

dahdi-package: dahdi $(PACKAGE_DIR)/$(PKG_NAME)_$(VERSION)_$(PKGARCH).ipk

