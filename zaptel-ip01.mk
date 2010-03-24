# zaptel-ip01.mk 
# Makes zaptel ipkg for IP01
# Authors: Atcom
#
# usage:
#   make -f zaptel-ip01.mk zaptel-package
# 
# Prerequisites:
#   make -f uClinux.mk && make -f oslec.mk

ZAPTEL_EXTRA_CFLAGS=-DCONFIG_4FX_SPI_INTERFACE

include rules.mk

ZAPTEL_VERSION=1.4.3
ZAPTEL_NAME=zaptel-$(ZAPTEL_VERSION)
ZAPTEL_DIR=$(BUILD_DIR)/$(ZAPTEL_NAME)
ZAPTEL_SOURCE=$(ZAPTEL_NAME).tar.gz
#ZAPTEL_SITE=http://ftp.digium.com/pub/zaptel/releases
ZAPTEL_SITE=http://downloads.digium.com/pub/zaptel/releases
ZAPTEL_UNZIP=zcat
OSLEC_DIR=$(TOPDIR)/oslec/

ZAP_IP01_SRCDIR=$(BUILD_DIR)/src/zaptel-ip01

STAGING_INC=$(STAGING_DIR)/usr/include
STAGING_LIB=$(STAGING_DIR)/usr/lib

MOD_PATH:=$(UCLINUX_DIST)/root/lib/modules
MOD_DIR:=$(shell ls $(UCLINUX_DIST)/root/lib/modules)

PKG_NAME:=zaptel-ip01
PKG_VERSION:=1.4.3
PKG_RELEASE:=1

TARGET_DIR=$(TOPDIR)/tmp/$(PKG_NAME)/ipkg/$(PKG_NAME)

PKG_BUILD_DIR:=$(TOPDIR)/tmp/$(PKG_NAME)/

$(DL_DIR)/$(ZAPTEL_SOURCE):
	mkdir -p $(DL_DIR)
	$(WGET) -P $(DL_DIR) $(ZAPTEL_SITE)/$(ZAPTEL_SOURCE)

$(ZAPTEL_DIR)/.unpacked: $(DL_DIR)/$(ZAPTEL_SOURCE) 
	$(ZAPTEL_UNZIP) $(DL_DIR)/$(ZAPTEL_SOURCE) | \
	tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(ZAPTEL_DIR)/.unpacked

$(ZAPTEL_DIR)/.configured: $(ZAPTEL_DIR)/.unpacked
	cd $(ZAPTEL_DIR); ./configure --host=bfin-linux-uclibc

	# Add new files we need to support Blackfin.  Note we
	# use sym-links so that any changes we make to zaptel
	# get captured by SVN.

	# DR: Many of these files are the same as for the IP04, remaining
	# changes could possibly be merged in as #ifdefs or
	# patches.

	ln -sf $(ZAP_IP01_SRCDIR)/wcfxs.c $(ZAPTEL_DIR)/wcfxs.c
	ln -sf $(ZAP_IP01_SRCDIR)/fx.c $(ZAPTEL_DIR)/fx.c
	ln -sf $(ZAP_IP01_SRCDIR)/bfsi.c $(ZAPTEL_DIR)/bfsi.c
	ln -sf $(ZAP_IP01_SRCDIR)/wcfxs.h $(ZAPTEL_DIR)/wcfxs.h
	ln -sf $(ZAP_IP01_SRCDIR)/bfsi.h $(ZAPTEL_DIR)/bfsi.h

	# patch for Oslec

	cd $(ZAPTEL_DIR); \
	patch < $(OSLEC_DIR)/kernel/zaptel-$(ZAPTEL_VERSION).patch
	patch -p0 < patch/zaptel-ip01.patch
	touch $(ZAPTEL_DIR)/.configured

zaptel: $(ZAPTEL_DIR)/.configured

	# build libtonezone, reqd for Asterisk

	cd $(ZAPTEL_DIR); make libtonezone.a

	# install files needed for other apps

	mkdir -p $(STAGING_INC)
	mkdir -p $(STAGING_INC)/zaptel
	mkdir -p $(STAGING_LIB)
	cp $(ZAPTEL_DIR)/tonezone.h $(STAGING_INC)/zaptel
	cp $(ZAPTEL_DIR)/ztcfg.h $(STAGING_INC)
	cp $(ZAPTEL_DIR)/zaptel.h $(STAGING_INC)/zaptel
	cp $(ZAPTEL_DIR)/libtonezone.a $(STAGING_LIB)

	# build ztcfg and zapscan

	cd $(ZAPTEL_DIR); make ztcfg
	bfin-linux-uclibc-gcc -I$(STAGING_INC) src/zapscan.c \
	-o $(ZAPTEL_DIR)/zapscan -Wall

	# build zaptel.ko & wcfxs.ko

	cd $(ZAPTEL_DIR); \
	gcc -o gendigits gendigits.c -lm; \
	make tones.h; \
	make version.h
	make -C $(UCLINUX_DIST) EXTRA_CFLAGS=$(ZAPTEL_EXTRA_CFLAGS) \
	SUBDIRS=$(ZAPTEL_DIR) modules V=1

	# set up dir structure for package

	rm -Rf $(TARGET_DIR)
	mkdir -p $(TARGET_DIR)/lib/modules/$(MOD_DIR)
	mkdir -p $(TARGET_DIR)/bin
	mkdir -p $(TARGET_DIR)/etc/init.d 
	mkdir -p $(TARGET_DIR)/etc/asterisk

	# install

	cp -f $(ZAPTEL_DIR)/zaptel.ko $(ZAPTEL_DIR)/wcfxs.ko $(ZAPTEL_DIR)/bfsi.ko \
	$(TARGET_DIR)/lib/modules/$(MOD_DIR)
	cp -f $(ZAPTEL_DIR)/ztcfg $(ZAPTEL_DIR)/zapscan $(TARGET_DIR)/bin
	cp files/zaptel.init $(TARGET_DIR)/etc/init.d/zaptel
	chmod a+x $(TARGET_DIR)/etc/init.d/zaptel
	cp -f files/zaptel.conf.in $(TARGET_DIR)/etc/zaptel.conf
	cp -f files/zapata.conf.in $(TARGET_DIR)/etc/asterisk

	# doc

	mkdir -p $(TARGET_DIR)/usr/doc
	cp -f doc/zaptel.txt $(TARGET_DIR)/usr/doc

	touch $(PKG_BUILD_DIR)/.built

all: zaptel

dirclean:
	rm -Rf $(ZAPTEL_DIR)

define Package/$(PKG_NAME)
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Zaptel
  DESCRIPTION:=\
        Telephony hardware drivers for Atcom IP01
  DEPENDS:=oslec
  URL:=http://www.asterisk.org
endef

# post installation - add the modules.dep entries

define Package/$(PKG_NAME)/postinst
#!/bin/sh
cd /lib/modules/$(MOD_DIR)
cat modules.dep | sed '/.*zaptel.ko:/ d' > modules.tmp
cat modules.tmp | sed '/.*wcfxs.ko:/ d' > modules.tmp1
cat modules.tmp1 | sed '/.*bfsi.ko:/ d' > modules.dep
rm -f modules.tmp modules.tmp1
echo /lib/modules/$(MOD_DIR)/bfsi.ko: >> modules.dep
echo /lib/modules/$(MOD_DIR)/zaptel.ko: /lib/modules/$(MOD_DIR)/oslec.ko >> modules.dep
echo /lib/modules/$(MOD_DIR)/wcfxs.ko: /lib/modules/$(MOD_DIR)/bfsi.ko /lib/modules/$(MOD_DIR)/zaptel.ko >> modules.dep
rm -Rf /dev/zap
mkdir -p /dev/zap
mknod /dev/zap/ctl c 196 0
mknod /dev/zap/timer c 196 253
mknod /dev/zap/channel c 196 254
mknod /dev/zap/pseudo c 196 255
mknod /dev/zap/1 c 196 1
mknod /dev/zap/2 c 196 2
mknod /dev/zap/3 c 196 3
mknod /dev/zap/4 c 196 4

/etc/init.d/zaptel enable
endef

# pre-remove - remove the modules.dep entries

define Package/$(PKG_NAME)/prerm
#!/bin/sh
cd /lib/modules/$(MOD_DIR)
cat modules.dep | sed '/.*zaptel.ko:/ d' > modules.tmp
cat modules.tmp | sed '/.*wcfxs.ko:/ d' > modules.tmp1
cat modules.tmp1 | sed '/.*bfsi.ko:/ d' > modules.dep
rm -f modules.tmp modules.tmp1
/etc/init.d/zaptel disable
rm -f -r /dev/zap
endef

$(eval $(call BuildPackage,$(PKG_NAME)))

zaptel-package: zaptel $(PACKAGE_DIR)/$(PKG_NAME)_$(VERSION)_$(PKGARCH).ipk

