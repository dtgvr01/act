# uClinux.mk
# Makefile for Blackfin uClinux-dist
#
# NOTE: You need to install the 08 toolchain and uClibc:
#
#   (i)  blackfin-toolchain-08r1.5-14.i386.tar.bz2 to build.
#
#   (ii) blackfin-toolchain-uclibc-default-08r1.5-14.i386.tar.bz2
#
# The output is a bootable uImage in images/uImage.
#
# Supported vendors: Rowetel
# Supported products: IP01, IP04, IP08
#

VENDOR=Rowetel
PRODUCT=IP01

include rules.mk

UCLINUX_DIRNAME=uClinux-dist
UCLINUX_DIR=$(BUILD_DIR)/$(UCLINUX_DIRNAME)
UCLINUX_KERNEL_SRC=$(BUILD_DIR)/uClinux-dist/linux-2.6.x
UCLINUX_SOURCE=uClinux-dist-2008R1.5-RC3.tar.bz2
UCLINUX_SITE=http://download.analog.com/27516/frsrelease/5/0/8/5088
UCLINUX_UNZIP=bzcat
TARGET_DIR=$(UCLINUX_DIR)/root

PKG_NAME:=uclinux
PKG_VERSION:=1.5
PKG_RELEASE:=3
PKG_BUILD_DIR:=$(TOPDIR)/tmp/uclinux

#---------------------------------------------------------------------------
#                    Downloaded source file Target
#---------------------------------------------------------------------------

$(DL_DIR)/$(UCLINUX_SOURCE):
	$(WGET) -P $(DL_DIR) $(UCLINUX_SITE)/$(UCLINUX_SOURCE)

#---------------------------------------------------------------------------
#                    Unpack and patch to support product & ipkg
#---------------------------------------------------------------------------

$(UCLINUX_DIR)/.unpacked: $(DL_DIR)/$(UCLINUX_SOURCE)
	tar xjf $(DL_DIR)/$(UCLINUX_SOURCE) -C $(BUILD_DIR)
	mv uClinux-dist-2008R1.5-RC3 uClinux-dist
	patch -d $(BUILD_DIR) -uN -p0 < patch/$(PRODUCT).patch
	patch -d $(BUILD_DIR) -uN -p0 < patch/busybox.patch
	touch $(UCLINUX_DIR)/.unpacked

#---------------------------------------------------------------------------
#                    Configure for product
#---------------------------------------------------------------------------

$(UCLINUX_DIR)/.configured: $(UCLINUX_DIR)/.unpacked
	mkdir -p $(UCLINUX_DIR)/vendors/$(VENDOR)/$(PRODUCT)/
	cp -af patch/vendors/$(VENDOR)/$(PRODUCT)/* $(UCLINUX_DIR)/vendors/$(VENDOR)/$(PRODUCT)
	cp -af patch/vendors/$(VENDOR)/vendor.mak $(UCLINUX_DIR)/vendors/$(VENDOR)
	$(MAKE) -C $(UCLINUX_DIR) $(VENDOR)/$(PRODUCT)_config
	touch $(UCLINUX_DIR)/.configured

#---------------------------------------------------------------------------
#                    create uImage
#---------------------------------------------------------------------------

uClinux: $(UCLINUX_DEP) $(UCLINUX_DIR)/.configured
	$(MAKE) -C $(UCLINUX_DIR) ROMFSDIR=$(TARGET_DIR)
	gcc src/zeropad.c -o src/zeropad -Wall
	./src/zeropad uClinux-dist/images/uImage uClinux-dist/images/uImage_r3.$(PRODUCT) 0x20000
	mkdir -p $(PKG_BUILD_DIR)/ipkg/uclinux/var/tmp
	cp uClinux-dist/images/uImage_r3.$(PRODUCT) $(PKG_BUILD_DIR)/ipkg/uclinux/var/tmp

	touch $(PKG_BUILD_DIR)/.built

uClinux-unpacked: $(UCLINUX_DIR)/.unpacked

uClinux-configure: $(UCLINUX_DIR)/.configured

uClinux-clean:
	-$(MAKE) -C $(UCLINUX_DIR) clean

all: uClinux

#---------------------------------------------------------------------------
#                              CREATING PATCHES     
#---------------------------------------------------------------------------

# Generate patches between vanilla uClinux-dist tar ball and $(PRODUCT)
# version ($(PRODUCT) only at this stage, pending approval by the astfin
# team). Run this target after you have made any changes to
# uClinux-dist to capture them to the patch and conf files.  This
# target captures the changes required to get the $(PRODUCT) to boot
# uClinux, but doesn't capture any of the Asterisk/Zaptel stuff (see
# below for that).

UDO = uClinux-dist-orig
UD = uClinux-dist

uClinux-make-patch:

	if [ ! -d $(UCLINUX_DIR)-orig ] ; then \
		mkdir -p tmp; cd tmp; \
	        tar xjf $(DL_DIR)/$(UCLINUX_SOURCE); \
		mv uClinux-dist-2008R1.5-RC3 $(UCLINUX_DIR)-orig; \
	fi

	-cd $(BUILD_DIR); diff -uN \
	$(UDO)/linux-2.6.x/arch/blackfin/Kconfig \
	$(UD)/linux-2.6.x/arch/blackfin/Kconfig \
	> $(PWD)/patch/$(PRODUCT).patch

	-cd $(BUILD_DIR); diff -uN \
	$(UDO)/linux-2.6.x/arch/blackfin/mach-bf533/boards/Kconfig \
	$(UD)/linux-2.6.x/arch/blackfin/mach-bf533/boards/Kconfig \
	>> $(PWD)/patch/$(PRODUCT).patch

	-cd $(BUILD_DIR); diff -uN \
	$(UDO)/linux-2.6.x/arch/blackfin/mach-bf533/boards/ip0x.c \
	$(UD)/linux-2.6.x/arch/blackfin/mach-bf533/boards/ip0x.c \
	>> $(PWD)/patch/$(PRODUCT).patch

	-cd $(BUILD_DIR); diff -uN \
	$(UDO)/linux-2.6.x/include/asm-blackfin/mach-bf533/mem_init.h \
	$(UD)/linux-2.6.x/include/asm-blackfin/mach-bf533/mem_init.h \
	>> $(PWD)/patch/$(PRODUCT).patch

	-cd $(BUILD_DIR); diff -uN \
	$(UDO)/linux-2.6.x/arch/blackfin/mach-bf533/boards/Makefile \
	$(UD)/linux-2.6.x/arch/blackfin/mach-bf533/boards/Makefile \
	>> $(PWD)/patch/$(PRODUCT).patch

	-cd $(BUILD_DIR); diff -uN \
	$(UDO)/linux-2.6.x/drivers/mtd/maps/Kconfig \
	$(UD)/linux-2.6.x/drivers/mtd/maps/Kconfig \
	>> $(PWD)/patch/$(PRODUCT).patch

	-cd $(BUILD_DIR); diff -uN \
	$(UDO)/linux-2.6.x/drivers/mtd/maps/bf5xx-flash.c \
	$(UD)/linux-2.6.x/drivers/mtd/maps/bf5xx-flash.c  \
	>> $(PWD)/patch/$(PRODUCT).patch

	-cd $(BUILD_DIR); diff -uN \
	$(UDO)/linux-2.6.x/drivers/mtd/nand/bfin_nand.c \
	$(UD)/linux-2.6.x/drivers/mtd/nand/bfin_nand.c \
	>> $(PWD)/patch/$(PRODUCT).patch

	-cd $(BUILD_DIR); diff -uN \
	$(UDO)/linux-2.6.x/drivers/mtd/nand/Kconfig \
	$(UD)/linux-2.6.x/drivers/mtd/nand/Kconfig \
	>> $(PWD)/patch/$(PRODUCT).patch

	-cd $(BUILD_DIR); diff -uN \
	$(UDO)/linux-2.6.x/drivers/net/dm9000.c \
	$(UD)/linux-2.6.x/drivers/net/dm9000.c \
	>> $(PWD)/patch/$(PRODUCT).patch

	-cd $(BUILD_DIR); diff -uN \
	$(UDO)/linux-2.6.x/drivers/serial/bfin_5xx.c \
	$(UD)/linux-2.6.x/drivers/serial/bfin_5xx.c \
	>> $(PWD)/patch/$(PRODUCT).patch

	mkdir -p patch/vendors/$(VENDOR)/$(PRODUCT)
	cp -af $(UCLINUX_DIR)/vendors/$(VENDOR)/$(PRODUCT)/* patch/vendors/$(VENDOR)/$(PRODUCT)
	cp -f $(UCLINUX_DIR)/.config patch/vendors/$(VENDOR)/$(PRODUCT)/config.device
	cp -f $(UCLINUX_DIR)/linux-2.6.x/.config patch/vendors/$(VENDOR)/$(PRODUCT)/config.linux-2.6.x	
	cp -f $(UCLINUX_DIR)/config/.config patch/vendors/$(VENDOR)/$(PRODUCT)/config.vendor-2.6.x 

	# files needed for adding ipkg to busybox.  This was unchanged from
	# uClinux-dist 2007 so we use the same patch file

	-cd $(BUILD_DIR); diff -uN -x *.o -x *cmd -x *.a \
	$(UDO)/user/busybox/archival/libipkg \
	$(UD)/user/busybox/archival/libipkg \
	> $(PWD)/patch/busybox.patch

	-cd $(BUILD_DIR); diff -uN \
	$(UDO)/user/busybox/archival/ipkg.c \
	$(UD)/user/busybox/archival/ipkg.c \
	>> $(PWD)/patch/busybox.patch

	-cd $(BUILD_DIR); diff -uN \
	$(UDO)/user/busybox/archival/Config.in \
	$(UD)/user/busybox/archival/Config.in \
	>> $(PWD)/patch/busybox.patch

	-cd $(BUILD_DIR); diff -uN \
	$(UDO)/user/busybox/archival/Kbuild \
	$(UD)/user/busybox/archival/Kbuild \
	>> $(PWD)/patch/busybox.patch

	-cd $(BUILD_DIR); diff -uN \
	$(UDO)/user/busybox/archival/libunarchive/data_extract_all.c \
	$(UD)/user/busybox/archival/libunarchive/data_extract_all.c \
	>> $(PWD)/patch/busybox.patch

	-cd $(BUILD_DIR); diff -uN \
	$(UDO)/user/busybox/archival/libunarchive/open_transformer.c \
	$(UD)/user/busybox/archival/libunarchive/open_transformer.c \
	>> $(PWD)/patch/busybox.patch

	-cd $(BUILD_DIR); diff -uN \
	$(UDO)/user/busybox/archival/libunarchive/Kbuild \
	$(UD)/user/busybox/archival/libunarchive/Kbuild \
	>> $(PWD)/patch/busybox.patch

	-cd $(BUILD_DIR); diff -uN \
	$(UDO)/user/busybox/uclinux-configs/archival/Config.in \
	$(UD)/user/busybox/uclinux-configs/archival/Config.in \
	>> $(PWD)/patch/busybox.patch

	-cd $(BUILD_DIR); diff -uN \
	$(UDO)/user/busybox/include/applets.h \
	$(UD)/user/busybox/include/applets.h \
	>> $(PWD)/patch/busybox.patch

	-cd $(BUILD_DIR); diff -uN \
	$(UDO)/user/busybox/include/unarchive.h \
	$(UD)/user/busybox/include/unarchive.h \
	>> $(PWD)/patch/busybox.patch

	-cd $(BUILD_DIR); diff -uN \
	$(UDO)/user/busybox/include/usage.h \
	$(UD)/user/busybox/include/usage.h \
	>> $(PWD)/patch/busybox.patch

	-cd $(BUILD_DIR); diff -uN \
	$(UDO)/user/busybox/Makefile \
	$(UD)/user/busybox/Makefile \
	>> $(PWD)/patch/busybox.patch

#---------------------------------------------------------------------------
#                              CREATING PACKAGE    
#---------------------------------------------------------------------------

define Package/uclinux
  SECTION:=net
  CATEGORY:=Network
  TITLE:=uclinux
  DESCRIPTION:=\
        This is the uClinux distribution project for the Blackfin \\\
        processor.
  URL:=http://blackfin.uclinux.org/gf/project/uclinux-dist
  ARCHITECTURE:=bfin-uclinux
endef

# post installation

define Package/uclinux/postinst
#!/bin/sh
eraseall /dev/mtd1
cp /var/tmp/uImage_r3.$(PRODUCT) /dev/mtd1
reboot
endef

# pre-remove

define Package/uclinux/prerm
#!/bin/sh
echo "This package cannot be removed"
endef

$(eval $(call BuildPackage,uclinux))

uClinux-package: uClinux $(PACKAGE_DIR)/uclinux_$(VERSION)_$(PKGARCH).ipk
	rcp -r /home/dan/dev/uClinux-dist/root/lib/modules/2.6.22.19-ADI-2008R1.5/kernel/drivers/dahdi root@192.168.1.80:/lib/modules/2.6.22.19-ADI-2008R1.5/kernel/drivers
	

