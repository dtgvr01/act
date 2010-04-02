#########################################################
# spandsp for uClinux and Asterisk, 
# Jeff Knighton Feb 2008
#
# usage: make -f spandsp.mk spandsp 
#
# Run after building uClinux-dist, copies shared libs to
# uClinux-dist/staging, ready for use in Asterisk if 
# required.
#########################################################

include rules.mk

SPANDSP_SITE=http://www.soft-switch.org/downloads/spandsp/
SPANDSP_VERSION=0.0.6
SPANDSP_SOURCE=spandsp-0.0.6pre17.tgz
TIFF_VERSION=3.8.2
SPANDSP_DIR=$(BUILD_DIR)/spandsp-$(SPANDSP_VERSION)
SPANDSP_CONFIGURE_OPTS=--host=bfin-linux-uclibc --enable-fixed-point --disable-itutests

TARGET_DIR=$(BUILD_DIR)/tmp/spandsp/ipkg/spandsp
PKG_NAME:=spandsp
PKG_VERSION:=$(SPANDSP_VERSION)
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(BUILD_DIR)/tmp/spandsp

TIFF_LDFLAGS=-L$(BUILD_DIR)/tiff-$(TIFF_VERSION)/libtiff/.libs
TIFF_CFLAGS=-I$(BUILD_DIR)/tiff-$(TIFF_VERSION)/libtiff


$(DL_DIR)/$(SPANDSP_SOURCE):
	$(WGET) -P $(DL_DIR) $(SPANDSP_SITE)/$(SPANDSP_SOURCE)

spandsp-source: $(DL_DIR)/$(SPANDSP_SOURCE)

$(SPANDSP_DIR)/.unpacked: $(DL_DIR)/$(SPANDSP_SOURCE)
	tar -xzvf $(DL_DIR)/$(SPANDSP_SOURCE)
	touch $(SPANDSP_DIR)/.unpacked

$(SPANDSP_DIR)/.configured: $(SPANDSP_DIR)/.unpacked
	cp -v -f $(BUILD_DIR)/patch/config.sub-spandsp-$(SPANDSP_VERSION) $(SPANDSP_DIR)/config/config.sub
	cp -v -f $(BUILD_DIR)/patch/configure-spandsp-$(SPANDSP_VERSION) $(SPANDSP_DIR)/configure
	chmod a+x $(SPANDSP_DIR)/configure
	export ac_cv_func_malloc_0_nonnull=yes
	cd $(SPANDSP_DIR); LDFLAGS=$(TIFF_LDFLAGS) CFLAGS=$(TIFF_CFLAGS) ./configure $(SPANDSP_CONFIGURE_OPTS)
	touch $(SPANDSP_DIR)/.configured

spandsp: $(SPANDSP_DIR)/.configured
	make LDFLAGS=$(TIFF_LDFLAGS) CFLAGS=$(TIFF_CFLAGS) STAGEDIR=$(STAGING_DIR) -C $(SPANDSP_DIR)/
	#copy header files to staging directory
	mkdir -p $(STAGING_DIR)/usr/include/spandsp
	cp -r -f $(SPANDSP_DIR)/src/spandsp/* $(STAGING_DIR)/usr/include/spandsp
	cp -f $(SPANDSP_DIR)/src/.libs/libspandsp* $(STAGING_DIR)/usr/lib/

	# setup directories for package
	rm -Rf $(TARGET_DIR)
	mkdir -p $(TARGET_DIR)/lib

	#copy to the package location
	cp -f $(SPANDSP_DIR)/src/.libs/libspandsp.so.0 $(TARGET_DIR)/lib
	cd $(TARGET_DIR)/lib; ln -sf libspandsp.so.0 libspandsp.so
	$(TARGET_STRIP) $(TARGET_DIR)/lib/libspandsp.so.0
	touch $(PKG_BUILD_DIR)/.built

all: spandsp

spandsp-dirclean:
	rm -rf $(SPANDSP_DIR)

define Package/$(PKG_NAME)
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Spandsp
  DESCRIPTION:=\
	Telephony Algorithms and Digital Signal Processing Routines.
  URL:=http://soft-switch.org
endef

#post installation - do nothing

define Package/$(PKG_NAME)/postinst
endef

#pre-remove
define Package/$(PKG_NAME)/prerm
endef

$(eval $(call BuildPackage,$(PKG_NAME)))

spandsp-package: spandsp $(PACKAGE_DIR)/$(PKG_NAME)_$(VERSION)_$(PKGARCH).ipk

