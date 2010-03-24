#########################################################
# dahdi-tools for uClinux and Asterisk, 
# Dan Amarandei Nov 2009
#
# usage: make -f dahdi-tools.mk dahdi-tools-package 
#
# Run after building uClinux-dist, copies shared libs to
# uClinux-dist/staging, ready for use in Asterisk if 
# required.
#########################################################

include rules.mk

DAHDI-TOOLS_SITE=http://downloads.asterisk.org/pub/telephony/dahdi-tools/releases
DAHDI-TOOLS_VERSION=2.2.0
DAHDI-TOOLS_SOURCE=dahdi-tools-2.2.0.tar.gz
DAHDI-TOOLS_DIR=$(BUILD_DIR)/dahdi-tools-$(DAHDI-TOOLS_VERSION)
STAGING_INC=$(STAGING_DIR)/usr/include
STAGING_LIB=$(STAGING_DIR)/usr/lib
DAHDI-TOOLS_CFLAGS=-g -mfdpic -mfast-fp -ffast-math -D__FIXED_PT__ \
-D__BLACKFIN__ -I$(STAGING_INC) -fno-jump-tables 
DAHDI-TOOLS_LDFLAGS=-mfdpic -L$(STAGING_LIB) -lpthread -ldl 
DAHDI-TOOLS_CONFIGURE_OPTS=--host=bfin-linux-uclibc CFLAGS="$(DAHDI-TOOLS_CFLAGS)" \
LDFLAGS="$(DAHDI-TOOLS_LDFLAGS)"

TARGET_DIR=$(BUILD_DIR)/tmp/dahdi-tools/ipkg/dahdi-tools
PKG_NAME:=dahdi-tools
PKG_VERSION:=$(DAHDI-TOOLS_VERSION)
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(BUILD_DIR)/tmp/dahdi-tools

$(DL_DIR)/$(DAHDI-TOOLS_SOURCE):
	$(WGET) -P $(DL_DIR) $(DAHDI-TOOLS_SITE)/$(DAHDI-TOOLS_SOURCE)

dahdi-tools-source: $(DL_DIR)/$(DAHDI-TOOLS_SOURCE)

$(DAHDI-TOOLS_DIR)/.unpacked: $(DL_DIR)/$(DAHDI-TOOLS_SOURCE)
	tar -xzvf $(DL_DIR)/$(DAHDI-TOOLS_SOURCE)
	touch $(DAHDI-TOOLS_DIR)/.unpacked

$(DAHDI-TOOLS_DIR)/.configured: $(DAHDI-TOOLS_DIR)/.unpacked
	cd $(DAHDI-TOOLS_DIR); ./configure $(DAHDI-TOOLS_CONFIGURE_OPTS)
	#setup directories for package
	touch $(DAHDI-TOOLS_DIR)/.configured

dahdi-tools: $(DAHDI-TOOLS_DIR)/.configured
	make -C $(DAHDI-TOOLS_DIR)/ STAGEDIR=$(STAGING_DIR)
	make -C $(DAHDI-TOOLS_DIR)/ STAGEDIR=$(STAGING_DIR) install DESTDIR=$(STAGING_DIR)
	bfin-linux-uclibc-gcc -I$(STAGING_INC) $(DAHDI-TOOLS_DIR)/timertest.c -o $(DAHDI-TOOLS_DIR)/timertest -Wall
	bfin-linux-uclibc-gcc -I$(STAGING_INC) $(DAHDI-TOOLS_DIR)/fxstest.c -o $(DAHDI-TOOLS_DIR)/fxstest -lm -Wall 
	#cp -f $(DAHDI-TOOLS_DIR)/dahdi-tools/.libs/dahdi-tools* $(STAGING_DIR)/usr/lib/

	#copy to package location
	#rm -Rf $(TARGET_DIR)
	#mkdir -p $(TARGET_DIR)/lib
	#cp -f $(DAHDI-TOOLS_DIR)/dahdi-tools/.libs/dahdi-tools.so.3 $(TARGET_DIR)/lib
	#$(TARGET_STRIP) $(TARGET_DIR)/lib/dahdi-tools.so.3
	#cd $(TARGET_DIR)/lib/; ln -sf dahdi-tools.so.3 dahdi-tools.so
	#touch $(PKG_BUILD_DIR)/.built

all: dahdi-tools

dahdi-tools-dirclean:
	rm -rf $(DAHDI-TOOLS_DIR)


define Package/$(PKG_NAME)
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Dahdi-Tools
  DESCRIPTION:=\
	Dahdi Tools.
  URL:=http://www.asterisk.org
endef

#post installation - do nothing
define Package/$(PKG_NAME)/postinst
endef

#pre-remove
define Package/$(PKG_NAME)/prerm
endef

$(eval $(call BuildPackage,$(PKG_NAME)))

dahdi-tools-package: dahdi-tools $(PACKAGE_DIR)/$(PKG_NAME)_$(VERSION)_$(PKGARCH).ipk

