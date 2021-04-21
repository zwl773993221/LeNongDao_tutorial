
##########################################################################################
#	prepare param
##########################################################################################
export OSDRV_DIR=$(shell pwd)
export OSDRV_CROSS
export CHIP
export ARCH=arm
export CROSS_COMPILE=$(OSDRV_CROSS)-
export OSDRV_CROSS_CFLAGS

# if CHIP is support, we set SUPPORT_CHIP = "yes"
SUPPORT_CHIP := "no"

ifeq ($(OSDRV_CROSS), )
$(error you must set OSDRV_CROSS first!)
endif

ifeq ($(CHIP), )
$(error you must set CHIP first!)
endif

ifeq ($(OSDRV_CROSS),arm-hisiv300-linux)
LIB_TYPE:=uclibc
BUSYBOX_CFG:=config_v300_arm9
TOOLCHAIN_DIR:=arm-hisiv300-linux
CROSS_SPECIFIED:=y
endif

ifeq ($(OSDRV_CROSS),arm-hisiv400-linux)
LIB_TYPE:=glibc
BUSYBOX_CFG:=config_v400_arm9
TOOLCHAIN_DIR:=arm-hisiv400-linux
CROSS_SPECIFIED:=y
endif

BUSYBOX_VER:=busybox-1.20.2
TOOLCHAIN_RUNTIME_LIB_C:=lib.tgz
TOOLCHAIN_RUNTIME_LIB:=armv5te_arm9_soft
OSDRV_CROSS_CFLAGS:=-march=armv5te -mcpu=arm926ej-s

ifneq ($(CROSS_SPECIFIED),y)
all:
	@echo "---------------------------------------------------------------------"
	@echo "ERROR: you should have specified an OSDRV_CROSS!                     "
	@echo "e.g., make OSDRV_CROSS=arm-hisiv300-linux	all 	    	    "
	@echo "e.g., make OSDRV_CROSS=arm-hisiv400-linux 	all	    	    "
	@echo "---------------------------------------------------------------------"
endif

ifeq ($(CHIP),hi3516cv200)
export	UBOOT_CONFIG:=hi3516cv200_config
export KERNEL_CFG:=hi3516cv200_full_defconfig
UIMAGE:=uImage_hi3516cv200
UBOOT:=u-boot-hi3516cv200.bin
UBOOT_REG_BIN:=reg_info_hi3516cv200.bin
JFFS2_IMAGE_BIN_64K:=rootfs_hi3516cv200_64k.jffs2
JFFS2_IMAGE_BIN_128K:=rootfs_hi3516cv200_128k.jffs2
JFFS2_IMAGE_BIN_256K:=rootfs_hi3516cv200_256k.jffs2
YAFFS2_IMAGE_BIN:=rootfs_hi3516cv200_2k_4bit.yaffs2
EXT4_IMAGE_BIN:=rootfs_hi3516cv200_32M.ext4
UBIFS_UBI_IMAGE_BIN:=rootfs_hi3516cv200_2k_128k_32M.ubiimg
UBIFS_IMAGE_BIN:=rootfs_hi3516cv200_2k_128k_32M.img
SUPPORT_CHIP = "yes"
endif

ifeq ($(CHIP),hi3518ev200)
export	UBOOT_CONFIG:=hi3518ev200_config
export KERNEL_CFG:=hi3518ev200_full_defconfig
UIMAGE:=uImage_hi3518ev200
UBOOT:=u-boot-hi3518ev200.bin
UBOOT_REG_BIN:=reg_info_hi3518ev200.bin
JFFS2_IMAGE_BIN_64K:=rootfs_hi3518ev200_64k.jffs2
JFFS2_IMAGE_BIN_128K:=rootfs_hi3518ev200_128k.jffs2
JFFS2_IMAGE_BIN_256K:=rootfs_hi3518ev200_256k.jffs2
YAFFS2_IMAGE_BIN:=rootfs_hi3518ev200_2k_4bit.yaffs2
EXT4_IMAGE_BIN:=rootfs_hi3518ev200_32M.ext4
UBIFS_UBI_IMAGE_BIN:=rootfs_hi3518ev200_2k_128k_32M.ubiimg
UBIFS_IMAGE_BIN:=rootfs_hi3518ev200_2k_128k_32M.img
SUPPORT_CHIP = "yes"
endif

ifeq ($(CHIP),hi3518ev201)
export	UBOOT_CONFIG:=hi3518ev201_config
export KERNEL_CFG:=hi3518ev201_full_defconfig
UIMAGE:=uImage_hi3518ev201
UBOOT:=u-boot-hi3518ev201.bin
UBOOT_REG_BIN:=reg_info_hi3518ev201.bin
JFFS2_IMAGE_BIN_64K:=rootfs_hi3518ev201_64k.jffs2
JFFS2_IMAGE_BIN_128K:=rootfs_hi3518ev201_128k.jffs2
JFFS2_IMAGE_BIN_256K:=rootfs_hi3518ev201_256k.jffs2
YAFFS2_IMAGE_BIN:=rootfs_hi3518ev201_2k_4bit.yaffs2
EXT4_IMAGE_BIN:=rootfs_hi3518ev201_32M.ext4
UBIFS_UBI_IMAGE_BIN:=rootfs_hi3518ev201_2k_128k_32M.ubiimg
UBIFS_IMAGE_BIN:=rootfs_hi3518ev201_2k_128k_32M.img
SUPPORT_CHIP = "yes"
endif

YAFFS_TOOL:=mkyaffs2image100
EXT4_TOOL:=make_ext4fs

export PUB_BOARD:=board_$(LIB_TYPE)
export PUB_IMAGE:=image_$(LIB_TYPE)
export PUB_ROOTFS:=rootfs_$(LIB_TYPE)

ifneq ($(SUPPORT_CHIP), "yes")
$(error $(CHIP) not surpport!)
endif

##########################################################################################
#	set task
##########################################################################################
ifeq ($(CROSS_SPECIFIED),y)
all: prepare hiboot hikernel hirootfs_prepare hibusybox hipctools hiboardtools hirootfs_build
notools: hiboot hikernel hinotools_prepare hirootfs_notools_build

clean: hiboot_clean hikernel_clean \
       hibusybox_clean hipctools_clean hiboardtools_clean hirootfs_clean

distclean:clean pub_clean
endif

##########################################################################################
#task [0]	prepare
##########################################################################################
prepare:
	mkdir $(OSDRV_DIR)/pub/$(PUB_IMAGE) -p
	mkdir $(OSDRV_DIR)/pub/bin/$(PUB_BOARD) -p
	mkdir $(OSDRV_DIR)/pub/bin/pc -p
	mkdir $(OSDRV_DIR)/pub/bin/$(PUB_BOARD)/hifat -p

hinotools_prepare:
	pushd $(OSDRV_DIR)/pub;tar xzf $(PUB_ROOTFS).tgz;popd
##########################################################################################
#task [1]	build uboot
##########################################################################################
hiboot: prepare
	@echo "---------task [1]	build boot"
	rm $(OSDRV_DIR)/opensource/uboot/u-boot-2010.06 -rf
	tar xzf $(OSDRV_DIR)/opensource/uboot/u-boot-2010.06.tgz -C $(OSDRV_DIR)/opensource/uboot
	find $(OSDRV_DIR)/opensource/uboot/u-boot-2010.06 | xargs touch
	make -C $(OSDRV_DIR)/opensource/uboot/u-boot-2010.06 ARCH=arm CROSS_COMPILE=$(OSDRV_CROSS)- $(UBOOT_CONFIG)
	pushd $(OSDRV_DIR)/opensource/uboot/u-boot-2010.06;make ARCH=arm CROSS_COMPILE=$(OSDRV_CROSS)- -j 20 >/dev/null;popd
	cp $(OSDRV_DIR)/opensource/uboot/u-boot-2010.06/u-boot.bin $(OSDRV_DIR)/pub/$(PUB_IMAGE)
	cp $(OSDRV_DIR)/tools/pc/uboot_tools/mkboot.sh $(OSDRV_DIR)/pub/$(PUB_IMAGE)
	cp $(OSDRV_DIR)/tools/pc/uboot_tools/$(UBOOT_REG_BIN) $(OSDRV_DIR)/pub/$(PUB_IMAGE)
	chmod 777 $(OSDRV_DIR)/pub/$(PUB_IMAGE)/mkboot.sh
	pushd $(OSDRV_DIR)/pub/$(PUB_IMAGE);./mkboot.sh $(UBOOT_REG_BIN) $(UBOOT);popd
	rm $(OSDRV_DIR)/pub/$(PUB_IMAGE)/u-boot.bin
	rm $(OSDRV_DIR)/pub/$(PUB_IMAGE)/mkboot.sh
	rm $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(UBOOT_REG_BIN)

hiboot_clean:
	rm $(OSDRV_DIR)/opensource/uboot/u-boot-2010.06 -rf

##########################################################################################
#task [2]	build kernel
##########################################################################################
hikernel: prepare
	@echo "---------task [2] build kernel"
	rm $(OSDRV_DIR)/opensource/kernel/linux-3.4.y -rf
	tar xzf $(OSDRV_DIR)/opensource/kernel/linux-3.4.y.tgz -C $(OSDRV_DIR)/opensource/kernel/
	find $(OSDRV_DIR)/opensource/kernel/linux-3.4.y | xargs touch
	make -C $(OSDRV_DIR)/opensource/kernel/linux-3.4.y ARCH=arm CROSS_COMPILE=$(OSDRV_CROSS)- $(KERNEL_CFG)
	pushd $(OSDRV_DIR)/opensource/kernel/linux-3.4.y;make ARCH=arm CROSS_COMPILE=$(OSDRV_CROSS)- -j 20 >/dev/null;popd
	pushd $(OSDRV_DIR)/opensource/kernel/linux-3.4.y;\
		make ARCH=arm CROSS_COMPILE=$(OSDRV_CROSS)- uImage -j 20 >/dev/null;popd
	cp $(OSDRV_DIR)/opensource/kernel/linux-3.4.y/arch/arm/boot/uImage $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(UIMAGE)

hikernel_clean:
	rm $(OSDRV_DIR)/opensource/kernel/linux-3.4.y -rf

##########################################################################################
#task [3]	prepare rootfs 
##########################################################################################
hirootfs_prepare: prepare
	@echo "---------task [3] prepare rootfs "
	rm $(OSDRV_DIR)/pub/$(PUB_ROOTFS) -rf
	tar xzf $(OSDRV_DIR)/rootfs_scripts/rootfs.tgz -C $(OSDRV_DIR)/pub
	mv $(OSDRV_DIR)/pub/rootfs $(OSDRV_DIR)/pub/$(PUB_ROOTFS)
	#add
	cp -R $(OSDRV_DIR)/rootfs_scripts/ko/* $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/dev/
	cp $(OSDRV_DIR)/rootfs_scripts/etc/init.d/* $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/etc/init.d/
	cp $(OSDRV_DIR)/rootfs_scripts/etc/fstab $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/etc/fatab
	tar xzf $(OSDRV_DIR)/opensource/toolchain/$(TOOLCHAIN_DIR)/runtime_lib/$(TOOLCHAIN_RUNTIME_LIB)/$(TOOLCHAIN_RUNTIME_LIB_C) -C $(OSDRV_DIR)/pub/$(PUB_ROOTFS)

##########################################################################################
#task [4]	build busybox 
##########################################################################################
hibusybox: prepare
	@echo "---------task [4] build busybox "
	#rm $(OSDRV_DIR)/opensource/busybox/$(BUSYBOX_VER) -rf
	#tar xzf $(OSDRV_DIR)/opensource/busybox/$(BUSYBOX_VER).tgz -C $(OSDRV_DIR)/opensource/busybox
	#find $(OSDRV_DIR)/opensource/busybox/$(BUSYBOX_VER) | xargs touch
	#cp $(OSDRV_DIR)/opensource/busybox/$(BUSYBOX_VER)/$(BUSYBOX_CFG) \
		$(OSDRV_DIR)/opensource/busybox/$(BUSYBOX_VER)/.config
	#pushd $(OSDRV_DIR)/opensource/busybox/$(BUSYBOX_VER)/;make -j 20 >/dev/null;popd
	make -C $(OSDRV_DIR)/opensource/busybox/$(BUSYBOX_VER) install
	cp -af $(OSDRV_DIR)/opensource/busybox/$(BUSYBOX_VER)/_install/* $(OSDRV_DIR)/pub/$(PUB_ROOTFS)

hibusybox_clean:
	rm $(OSDRV_DIR)/opensource/busybox/$(BUSYBOX_VER) -rf

##########################################################################################
#task [5]	build pc tools
##########################################################################################
hipctools: prepare
	@echo "---------task [5] build tools which run on pc"
	make -C $(OSDRV_DIR)/tools/pc/mkyaffs2image/
	cp $(OSDRV_DIR)/tools/pc/mkyaffs2image/bin/$(YAFFS_TOOL) $(OSDRV_DIR)/pub/bin/pc
	make -C $(OSDRV_DIR)/tools/pc/jffs2_tool/
	cp $(OSDRV_DIR)/tools/pc/jffs2_tool/mkfs.jffs2 $(OSDRV_DIR)/pub/bin/pc
	cp $(OSDRV_DIR)/tools/pc/jffs2_tool/mkfs.ubifs $(OSDRV_DIR)/pub/bin/pc
	cp $(OSDRV_DIR)/tools/pc/jffs2_tool/ubinize $(OSDRV_DIR)/pub/bin/pc
	cp $(OSDRV_DIR)/tools/pc/jffs2_tool/ubi.cfg $(OSDRV_DIR)/pub/bin/pc
	make -C $(OSDRV_DIR)/tools/pc/cramfs_tool/
	cp $(OSDRV_DIR)/tools/pc/cramfs_tool/mkfs.cramfs $(OSDRV_DIR)/pub/bin/pc
	make -C $(OSDRV_DIR)/tools/pc/mkimage_tool/
	cp $(OSDRV_DIR)/tools/pc/mkimage_tool/mkimage $(OSDRV_DIR)/pub/bin/pc
	make -C $(OSDRV_DIR)/tools/pc/squashfs4.2/
	cp $(OSDRV_DIR)/tools/pc/squashfs4.2/mksquashfs $(OSDRV_DIR)/pub/bin/pc
	make -C $(OSDRV_DIR)/tools/pc/lzma_tool/
	cp $(OSDRV_DIR)/tools/pc/lzma_tool/lzma $(OSDRV_DIR)/pub/bin/pc
	make -C $(OSDRV_DIR)/tools/pc/ext4_utils/
	cp $(OSDRV_DIR)/tools/pc/ext4_utils/bin/$(EXT4_TOOL) $(OSDRV_DIR)/pub/bin/pc
	
hirootfs_pack:hirootfs_prepare hibusybox hirootfs_notools_build

hipctools_clean:
	make -C $(OSDRV_DIR)/tools/pc/mkyaffs2image clean
	make -C $(OSDRV_DIR)/tools/pc/jffs2_tool distclean
	make -C $(OSDRV_DIR)/tools/pc/cramfs_tool distclean
	make -C $(OSDRV_DIR)/tools/pc/mkimage_tool distclean
	make -C $(OSDRV_DIR)/tools/pc/squashfs4.2 distclean
	make -C $(OSDRV_DIR)/tools/pc/lzma_tool distclean
	make -C $(OSDRV_DIR)/tools/pc/ext4_utils clean


##########################################################################################
#task [6]	build board tools
##########################################################################################
hiboardtools: hirootfs_prepare hiboardtools_clean
	@echo "---------task [6] build tools which run on board "
	make -C $(OSDRV_DIR)/tools/board/e2fsprogs
	cp -af $(OSDRV_DIR)/tools/board/e2fsprogs/bin/* $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/bin
	make -C $(OSDRV_DIR)/tools/board/reg-tools-1.0.0
	cp -af $(OSDRV_DIR)/tools/board/reg-tools-1.0.0/bin/* $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/bin
	chmod 777 $(OSDRV_DIR)/tools/board/hifat/$(LIB_TYPE)/static/himount
	cp -af $(OSDRV_DIR)/tools/board/hifat/$(LIB_TYPE)/static/himount $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/bin
	cp -af $(OSDRV_DIR)/tools/board/hifat/$(LIB_TYPE)/static/libhimount_api.a $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/lib
	make -C $(OSDRV_DIR)/tools/board/udev-164
	cp -af $(OSDRV_DIR)/tools/board/udev-164/rootfs/. $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/
	make -C $(OSDRV_DIR)/tools/board/gdb
	cp $(OSDRV_DIR)/tools/board/gdb/gdb-$(OSDRV_CROSS) $(OSDRV_DIR)/pub/bin/$(PUB_BOARD)
	make -C $(OSDRV_DIR)/tools/board/mkdosfs
	cp -af $(OSDRV_DIR)/tools/board/mkdosfs/bin/* $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/sbin
	cp $(OSDRV_DIR)/tools/board/hifat/$(LIB_TYPE)/* $(OSDRV_DIR)/pub/bin/$(PUB_BOARD)/hifat/ -af
	make -C $(OSDRV_DIR)/tools/board/mtd-utils/
	cp $(OSDRV_DIR)/tools/board/mtd-utils/bin/* $(OSDRV_DIR)/pub/bin/$(PUB_BOARD)
	make -C $(OSDRV_DIR)/tools/board/ethtools/
	cp $(OSDRV_DIR)/tools/board/ethtools/bin/sbin/* $(OSDRV_DIR)/pub/bin/$(PUB_BOARD)

hiboardtools_clean:
	make -C $(OSDRV_DIR)/tools/board/e2fsprogs distclean
	make -C $(OSDRV_DIR)/tools/board/reg-tools-1.0.0 clean
	make -C $(OSDRV_DIR)/tools/board/udev-164 clean
	make -C $(OSDRV_DIR)/tools/board/gdb distclean
	make -C $(OSDRV_DIR)/tools/board/mkdosfs distclean
	make -C $(OSDRV_DIR)/tools/board/mtd-utils distclean
	make -C $(OSDRV_DIR)/tools/board/ethtools distclean

##########################################################################################
#task [7]	build rootfs
##########################################################################################
hirootfs_build: hibusybox hiboardtools hirootfs_prepare hipctools hirootfs_notools_build
hirootfs_notools_build:
	@echo "---------task [7] build rootfs"
	chmod 777 $(OSDRV_DIR)/pub/bin/$(PUB_BOARD)/*
	chmod 777 $(OSDRV_DIR)/pub/bin/pc/*
	rm $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/dev/* -rf
	pushd $(OSDRV_DIR)/pub/$(PUB_ROOTFS);ln -s sbin/init init;popd
ifeq ($(OSDRV_CROSS),arm-hisiv400-linux) 
	pushd $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/lib;ln -s ../lib ./armv5te_arm9_soft;popd
	pushd $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/usr/lib;ln -s ../lib ./armv5te_arm9_soft;popd
endif

	pushd $(OSDRV_DIR)/pub/bin/pc;./mkfs.jffs2 -d $(OSDRV_DIR)/pub/$(PUB_ROOTFS) -l -e 0x40000 -o $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(JFFS2_IMAGE_BIN_256K);popd
	pushd $(OSDRV_DIR)/pub/bin/pc;./mkfs.jffs2 -d $(OSDRV_DIR)/pub/$(PUB_ROOTFS) -l -e 0x20000 -o $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(JFFS2_IMAGE_BIN_128K);popd
	pushd $(OSDRV_DIR)/pub/bin/pc;./mkfs.jffs2 -d $(OSDRV_DIR)/pub/$(PUB_ROOTFS) -l -e 0x10000 -o $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(JFFS2_IMAGE_BIN_64K);popd
	pushd $(OSDRV_DIR)/pub/bin/pc;./mkfs.ubifs -d $(OSDRV_DIR)/pub/$(PUB_ROOTFS) -m 2KiB -e 126976 -o ./$(UBIFS_UBI_IMAGE_BIN) -c 256 -F;popd
	pushd $(OSDRV_DIR)/pub/bin/pc;./ubinize -m 2KiB -p 128KiB -o $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(UBIFS_IMAGE_BIN) ubi.cfg;popd
	pushd $(OSDRV_DIR)/pub/bin/pc;rm ./$(UBIFS_UBI_IMAGE_BIN) ubi.cfg -rf;popd

	pushd $(OSDRV_DIR)/pub/bin/pc;./$(YAFFS_TOOL) $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/ $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(YAFFS2_IMAGE_BIN) 1 2;popd
	chmod 644 $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(YAFFS2_IMAGE_BIN)
	pushd $(OSDRV_DIR)/pub/bin/pc;./$(EXT4_TOOL) -l 32M -s $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(EXT4_IMAGE_BIN) $(OSDRV_DIR)/pub/$(PUB_ROOTFS);popd
	find $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/ -name '*svn' | xargs rm -rf
	pushd $(OSDRV_DIR)/pub;tar czf $(PUB_ROOTFS).tgz $(PUB_ROOTFS);rm $(PUB_ROOTFS) -rf;popd
	@echo "---------finish osdrv work"

hirootfs_clean:
	rm $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/ -rf

##########################################################################################
#task [8]	clean pub
##########################################################################################
pub_clean:
	rm $(OSDRV_DIR)/pub/$(PUB_IMAGE)/ -rf
	rm $(OSDRV_DIR)/pub/bin/ -rf
	rm $(OSDRV_DIR)/pub/$(PUB_ROOTFS).tgz -rf
	rm $(OSDRV_DIR)/kernel/linux-3.4.y -rf
