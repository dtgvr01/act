#########################################################
#mpg123 for uClinux and Asterisk, 
# Dan Amarandei Nov 2009
#
# usage: make -f mpg123.mk all
#
# Run after building uClinux-dist, copies shared libs to
# uClinux-dist/staging, ready for use in Asterisk if 
# required.
#########################################################

include rules.mk


MPG123_DIR=$(BUILD_DIR)/mips-test
STAGING_INC=$(STAGING_DIR)/usr/include
STAGING_LIB=$(STAGING_DIR)/usr/lib
MPG123_CFLAGS=-g -mfdpic -mfast-fp -ffast-math -D__FIXED_PT__ \
-D__BLACKFIN__ -I$(STAGING_INC) -fno-jump-tables 
MPG123_LDFLAGS=-mfdpic -L$(STAGING_LIB) -lpthread -ldl 
MPG123_CONFIGURE_OPTS=--host=bfin-linux-uclibc CFLAGS="$(MPG123_CFLAGS)" \
LDFLAGS="$(MPG123_LDFLAGS)"

TARGET_DIR=$(BUILD_DIR)/tmp/mpg123/ipkg/mpg123
PKG_NAME:=mpg123
PKG_VERSION:=1
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(BUILD_DIR)/tmp/mpg123



$(MPG123_DIR)/.configured:
	cd $(MPG123_DIR); ./configure $(MPG123_CONFIGURE_OPTS)
	#setup directories for package
	touch $(MPG123_DIR)/.configured

mpg123: $(MPG123_DIR)/.configured
	make -C $(MPG123_DIR)/ STAGEDIR=$(STAGING_DIR)
	make -C $(MPG123_DIR)/ STAGEDIR=$(STAGING_DIR) install DESTDIR=$(STAGING_DIR)
	

all: mpg123




