###############################################################################
#    x86/src/mtd-utils
#    Create By 
#    2012.04.23
###############################################################################
# parameter
PREFIX         ?= $(shell pwd)

###############################################################################
export CHIP

ifndef CHIP
CHIP = hi3516cv200
endif

CC             := gcc
BUILD_DIR      := $(PREFIX)/tmp
ZLIB           := $(shell pwd)/../zlib
ZLIB_INSTALL   := $(PREFIX)/../zlib
ZLIB_DIR       := $(ZLIB_INSTALL)/tmp
STRIP          := strip
MTD_UTILS      := mtd-utils-1.5.2
# this lib should be install to host
LZO            := lzo-2.09

TARGETS        := mkfs.jffs2

###############################################################################
all: $(addprefix $(PREFIX)/,$(TARGETS))

$(PREFIX)/mkubiimg.sh: $(PWD)/mkubiimg.sh
	@test -d $(@D) || mkdir -p $(@D)
	install -v --mode=755 -t $(PREFIX) $<

$(PREFIX)/%: $(BUILD_DIR)/sbin/%
	@test -d $(@D) || mkdir -p $(@D)
	install -vs -t $(PREFIX) $<
	$(STRIP) $<

###############################################################################

$(addprefix $(BUILD_DIR)/sbin/,$(TARGETS)): \
    $(ZLIB_DIR)/lib/libz.a    \
    $(BUILD_DIR)/lib/liblzo2.a \
    $(BUILD_DIR)/$(MTD_UTILS)/tar
	cd $(BUILD_DIR)/$(MTD_UTILS) && \
		make CFLAGS="-I$(BUILD_DIR)/include"  \
			LDFLAGS="-L$(BUILD_DIR)/lib" \
			ZLIBCPPFLAGS="-I$(BUILD_DIR)/include" \
			WITHOUT_XATTR=1 \
			ZLIBLDFLAGS="-L$(BUILD_DIR)/lib"&& \
		make DESTDIR=$(BUILD_DIR) SBINDIR=sbin install && \
	cd -
	cp $(pwd)tmp/sbin/ubinize ./ -rf
	cp $(pwd)tmp/sbin/mkfs.ubifs ./ -rf
ifeq ($(CHIP),hi3516cv200)
	cp $(pwd)../ubifs_config/ubi_hi3516cv200.cfg ./ubi.cfg -rf
endif
ifeq ($(CHIP),hi3518ev200)
	cp $(pwd)../ubifs_config/ubi_hi3518ev200.cfg ./ubi.cfg -rf
endif
ifeq ($(CHIP),hi3518ev201)
	cp $(pwd)../ubifs_config/ubi_hi3518ev201.cfg ./ubi.cfg -rf
endif

$(BUILD_DIR)/lib/liblzo2.a: \
    $(BUILD_DIR)/$(LZO)/tar
	cd $(BUILD_DIR)/$(LZO) && \
		./configure --prefix=$(BUILD_DIR) CC=gcc && \
		make -j 16 CC=gcc && \
		make install CC=gcc && \
	cd -

$(ZLIB_DIR)/lib/libz.a:
	@test -f $(ZLIB_DIR)/lib/libz.a || \
	make -j1 -C $(ZLIB) PREFIX=$(ZLIB_INSTALL)

###############################################################################

clean: zlib_clean
	@test ! -d $(BUILD_DIR) || rm -rfv $(BUILD_DIR)
	@test ! -e $(PREFIX)/mkfs.jffs2 || rm -rfv $(PREFIX)/mkfs.jffs2
	@test ! -e $(PREFIX)/mkfs.ubifs || rm -rfv $(PREFIX)/mkfs.ubifs
	@test ! -e $(PREFIX)/ubinize || rm -rfv $(PREFIX)/ubinize
	@test ! -e $(PREFIX)/ubi.cfg || rm -rfv $(PREFIX)/ubi.cfg
	@test "$(PREFIX)" = "$(PWD)" || rm -rfv $(PREFIX)/mkubiimg.sh

zlib_clean:
	@test ! -d $(ZLIB_DIR) || \
	make -j1 -C $(ZLIB) PREFIX=$(ZLIB_INSTALL) distclean

distclean: clean

$(BUILD_DIR)/%/tar:
	test -d $(@D) || mkdir -p $(@D)
	tar -xf $(notdir $(@:/tar=)).tar.* -C $(BUILD_DIR)
	@touch $@

###############################################################################
.PHONY: clean
###############################################################################
