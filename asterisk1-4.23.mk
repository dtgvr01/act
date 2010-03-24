# pretty much straight from Astfin packages/asterisk.mk, with some
# extra code to support building ipkg

include rules.mk

ASTERISK_VERSION=1.4.23
ASTERISK_NAME=asterisk-$(ASTERISK_VERSION)
ASTERISK_DIR=$(BUILD_DIR)/$(ASTERISK_NAME)
ASTERISK_SOURCE=$(ASTERISK_NAME).tar.gz
ASTERISK_SITE=http://downloads.digium.com/pub/asterisk/releases
ASTERISK_UNZIP=zcat
TARGET_DIR=$(TOPDIR)/tmp/asterisk/ipkg/asterisk

PKG_NAME:=asterisk
PKG_VERSION:=$(ASTERISK_VERSION)
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(TOPDIR)/tmp/asterisk
LIBTIFF_INC:=$(TOPDIR)/tiff-3.8.2/libtiff
SPANDSP_INC:=$(TOPDIR)/spandsp-0.0.4/src
STAGING_INC=$(STAGING_DIR)/usr/include
STAGING_LIB=$(STAGING_DIR)/usr/lib
ASTERISK_CFLAGS=-g -mfdpic -mfast-fp -ffast-math -D__FIXED_PT__ \
-D__BLACKFIN__ -I$(STAGING_INC) -I$(LIBTIFF_INC) -I$(SPANDSP_INC) -fno-jump-tables \
-DUSE_SPANDSP_CALLERID -DUSE_UWB_EXTENSIONS
ASTERISK_LDFLAGS=-mfdpic -L$(STAGING_LIB) -lpthread -ldl -ltonezone -lspandsp -ltiff
ASTERISK_CONFIGURE_OPTS=--host=bfin-linux-uclibc --disable-largefile \
--without-pwlib --without-curl CFLAGS="$(ASTERISK_CFLAGS)" \
LDFLAGS="$(ASTERISK_LDFLAGS)"

$(DL_DIR)/$(ASTERISK_SOURCE):
	mkdir -p $(DL_DIR)
	$(WGET) -P $(DL_DIR) $(ASTERISK_SITE)/$(ASTERISK_SOURCE)

$(ASTERISK_DIR)/.unpacked: $(DL_DIR)/$(ASTERISK_SOURCE) 
	$(ASTERISK_UNZIP) $(DL_DIR)/$(ASTERISK_SOURCE) | \
	tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
#	$(PATCH_KERNEL) $(ASTERISK_DIR) patch dtmf-1.4.20.patch
#	$(PATCH_KERNEL) $(ASTERISK_DIR) patch gui-crash.patch
#	$(PATCH_KERNEL) $(ASTERISK_DIR) patch asterisk.patch
	$(PATCH_KERNEL) $(ASTERISK_DIR) patch asterisk.patch
	$(PATCH_KERNEL) $(ASTERISK_DIR) patch cid.patch
	ln -sf $(BUILD_DIR)/src/codec_g729.c $(ASTERISK_DIR)/codecs
	ln -sf $(BUILD_DIR)/src/g729ab_codec.h $(ASTERISK_DIR)/codecs
	ln -sf $(BUILD_DIR)/src/codec_speex.c $(ASTERISK_DIR)/codecs
#	ln -sf $(BUILD_DIR)/src/cdr_sqlite3.c $(ASTERISK_DIR)/cdr
	touch $(ASTERISK_DIR)/.unpacked

$(ASTERISK_DIR)/.configured: $(ASTERISK_DIR)/.unpacked $(STAGING_LIB)/libgsm.a
	ln -sf $(BUILD_DIR)/patch/menuselect.makeopts $(ASTERISK_DIR)/
#	ln -sf $(SOURCES_DIR)/res_sqlite3.c $(ASTERISK_DIR)/res/
	cd $(ASTERISK_DIR); ./configure $(ASTERISK_CONFIGURE_OPTS)
	touch $(ASTERISK_DIR)/.configured

$(STAGING_LIB)/libgsm.a:
	-patch -f -p0 < patch/blackfin-gsm.patch
	make -C $(UCLINUX_DIST)/lib/blackfin-gsm \
	CC=bfin-linux-uclibc-gcc AR=bfin-linux-uclibc-ar
	ln -sf $(UCLINUX_DIST)/lib/blackfin-gsm/gsm/lib/libgsm.a $(STAGING_LIB)
	ln -sf $(UCLINUX_DIST)/lib/blackfin-gsm/gsm/inc/gsm.h $(STAGING_INC)

asterisk: $(ASTERISK_DIR)/.configured $(ASTERISK_DIR) 

	OPTIMIZE="-O4" ASTCFLAGS="$(ASTERISK_CFLAGS)" \
	ASTLDFLAGS="$(ASTERISK_LDFLAGS)" \
	$(MAKE) -C $(ASTERISK_DIR) codecs NOISY_BUILD=1

	OPTIMIZE="-Os" ASTCFLAGS="$(ASTERISK_CFLAGS)" \
	ASTLDFLAGS="$(ASTERISK_LDFLAGS)" \
	$(MAKE) -C $(ASTERISK_DIR) NOISY_BUILD=1

	rm -Rf $(TARGET_DIR)
	mkdir -p $(TARGET_DIR)/bin/
	mkdir -p $(TARGET_DIR)/var/lib/asterisk/agi-bin
	mkdir -p $(TARGET_DIR)/var/log/asterisk/cdr-csv
	mkdir -p $(TARGET_DIR)/var/lib/asterisk/firmware/iax
	mkdir -p $(TARGET_DIR)/var/lib/asterisk/keys
	mkdir -p $(TARGET_DIR)/var/lib/asterisk/sounds
	mkdir -p $(TARGET_DIR)/var/lib/asterisk/sounds/moh
	mkdir -p $(TARGET_DIR)/var/lib/asterisk/sounds/meetme
	mkdir -p $(TARGET_DIR)/var/lib/asterisk/sounds/voicemail
	mkdir -p $(TARGET_DIR)/var/spool/asterisk
	mkdir -p $(TARGET_DIR)/usr/lib/asterisk/modules
	ln -sf /var/lib/asterisk/sounds/moh $(TARGET_DIR)/var/lib/asterisk/
	ln -sf /var/lib/asterisk/sounds/meetme $(TARGET_DIR)/var/spool/asterisk/
	ln -sf /var/lib/asterisk/sounds/voicemail $(TARGET_DIR)/var/spool/asterisk/
	cp -v $(ASTERISK_DIR)/main/asterisk $(TARGET_DIR)/bin/
	ln -sf /bin/asterisk $(TARGET_DIR)/bin/rasterisk
	find $(ASTERISK_DIR) -name '*.so' -exec cp -v "{}" $(TARGET_DIR)/usr/lib/asterisk/modules/ \;
	$(TARGET_STRIP) $(TARGET_DIR)/bin/asterisk
	$(TARGET_STRIP) $(TARGET_DIR)/usr/lib/asterisk/modules/*.so

	mkdir -p $(TARGET_DIR)/etc/asterisk
	cp -Rf files/asterisk-defaults/etc/asterisk/* $(TARGET_DIR)/etc/asterisk
	mkdir -p $(TARGET_DIR)/etc/asterisk/tools
	cp files/asterisk-defaults/etc/asterisk/tools/* $(TARGET_DIR)/etc/asterisk/tools
	#mkdir -p $(TARGET_DIR)/etc/asterisk/scripts
	#cp files/asterisk-defaults/etc/asterisk/scripts/* $(TARGET_DIR)/etc/asterisk/scripts
	#mkdir -p $(TARGET_DIR)/var/lib/asterisk/sounds
	#cp files/asterisk-defaults/var/lib/asterisk/sounds/*.gsm \
	#$(TARGET_DIR)/var/lib/asterisk/sounds
	#mkdir -p $(TARGET_DIR)/var/lib/asterisk/static-http
	#cp files/asterisk.index.html \
	#$(TARGET_DIR)/var/lib/asterisk/static-http/index.html

	# need this codecs.conf for our Speex patch to work
	#cp files/codecs.conf.profile $(TARGET_DIR)/etc/asterisk/codecs.conf

	mkdir -p $(TARGET_DIR)/etc/init.d 
	cp files/asterisk.init $(TARGET_DIR)/etc/init.d/asterisk
	#cp files/zaptel.conf.in $(TARGET_DIR)/etc/asterisk
	chmod a+x $(TARGET_DIR)/etc/init.d/asterisk

	touch $(PKG_BUILD_DIR)/.built

all: asterisk

asterisk-configure: $(ASTERISK_DIR)/.configured

asterisk-config: $(ASTERISK_DIR)/.configured
	$(MAKE) -C $(ASTERISK_DIR) menuconfig

asterisk-dirclean:
	rm -rf $(ASTERISK_DIR)
	rm -rf $(PKG_BUILD_DIR)

#---------------------------------------------------------------------------
#                              CREATING PATCHES     
#---------------------------------------------------------------------------

# Generate patches between vanilla asterisk tar ball and our asterisk
# version.  Run this target after you have made any changes to
# asterisk to capture.

AO = asterisk-$(ASTERISK_VERSION)-orig
A = asterisk-$(ASTERISK_VERSION)

asterisk-make-patch:

        # untar original, to save time we check if the orig is already there

	if [ ! -d $(ASTERISK_DIR)-orig ] ; then \
		cd $(DL_DIR); tar xzf $(ASTERISK_SOURCE); \
		mv $(A) $(ASTERISK_DIR)-orig; \
	fi

        # Don't build muted as it has unsupported fork() and daemon() calls.
        # I suspect there might also be a way to delselect this utility using
        # the menu config utility.

	-cd $(BUILD_DIR); diff -uN \
	$(AO)/utils/Makefile \
	$(A)/utils/Makefile \
	> $(PWD)/patch/asterisk.patch

	# turn off glob support

	-cd $(BUILD_DIR); diff -uN \
	$(AO)/main/config.c \
	$(A)/main/config.c \
	>> $(PWD)/patch/asterisk.patch

	# This makes sure we turn off the #define HAVE_WORKING_FORK
	# as fork() and daemon() are not supported on the Blackfin.
	# Look for HAVE_WORKING_FORK in asterisk.c to see how this
	# is handled.  We needed this with uClinux-dist-2007R1 as 
	# the FDPIC compiler name changed and the Asterisk ./configure
	# mistakenly assumed fork() (and therefore daemon) worked.

	-cd $(BUILD_DIR); diff -uN \
	$(AO)/configure \
	$(A)/configure \
	>> $(PWD)/patch/asterisk.patch

	# Make Speex work in 15 kbit/s mode, was broken by default.  This
	# is the mode that is optimised for the Blackfin.

	-cd $(BUILD_DIR); diff -uN \
	$(AO)/main/frame.c \
	$(A)/main/frame.c \
	>> $(PWD)/patch/asterisk.patch

	# disable g729 by default, as we may not have libg729.so

	-cd $(BUILD_DIR); diff -uN \
	$(AO)/configs/modules.conf.sample \
	$(A)//configs/modules.conf.sample \
	>> $(PWD)/patch/asterisk.patch

	# caller ID patches

	-cd $(BUILD_DIR); diff -uN \
	$(AO)/main/callerid.c \
	$(A)/main/callerid.c \
	>> $(PWD)/patch/asterisk.patch

	# fork-vfork patch to make AGIs work

	-cd $(BUILD_DIR); diff -uN \
	$(AO)/res/res_agi.c \
	$(A)/res/res_agi.c \
	>> $(PWD)/patch/asterisk.patch

#---------------------------------------------------------------------------
#                              CREATING PACKAGE    
#---------------------------------------------------------------------------

define Package/asterisk
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Asterisk PBX
  DESCRIPTION:=\
        Asterisk is a complete PBX in software. It provides all \\\
        of the features you would expect from a PBX and more. \\\
        Asterisk does voice over IP in three protocols, and can \\\
        interoperate with almost all standards-based telephone \\\
        equipment using relatively inexpensive hardware.
  URL:=http://www.asterisk.org/
  ARCHITECTURE:=bfin-uclinux

endef

# post installation - add the sym link for auto start

define Package/asterisk/postinst
#!/bin/sh
/etc/init.d/asterisk enable
endef

# pre-remove - remove sym link

define Package/asterisk/prerm
#!/bin/sh
/etc/init.d/asterisk disable
endef

$(eval $(call BuildPackage,asterisk))

asterisk-package: asterisk $(PACKAGE_DIR)/asterisk_$(VERSION)_$(PKGARCH).ipk
	

