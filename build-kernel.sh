#!/bin/sh

set -eE
export ARCH=arm
# ================================================================================================
#export COMPILE_PATH="/opt/data/rv1126/sdk/rv1126_rv1109_sdk"
export COMPILE_PATH=".."
export CROSS_COMPILE="${COMPILE_PATH}/toolchain/bin/arm-rockchip830-linux-uclibcgnueabihf-"
export DEFCONFIG=rv1106_defconfig
export KERNEL_DTS=rv1106-atom
# ================================================================================================
#export COMPILE_PATH="/opt/data/rv1126/sdk/rv1126_rv1109_sdk"
export DTB_PATH=arch/$ARCH/boot/dts/$KERNEL_DTS.dtb
export ROOTFS_PATH=rootfs.cpio.gz

echo "CONFIG_BLK_DEV_INITRD=y" > arch/$ARCH/configs/rv1106-temporary.config
make $DEFCONFIG rv1106-evb.config rv1106-pm.config rv1106-atom.config rv1106-temporary.config
make $KERNEL_DTS.img BOOT_ITS=boot.its -j24
rm -f arch/$ARCH/configs/rv1106-temporary.config

rm -rf /tmp/lib/
make modules_install INSTALL_MOD_PATH=/tmp
cd /tmp/
tar czvf /tmp/lib_modules.tar.gz lib
cd -
cp /tmp/lib_modules.tar.gz ./

sed -i "/fdt {/ {n; s|data = .*|data = /incbin/(\"$DTB_PATH\");|}" boot4recovery.its
sed -i "/ramdisk {/ {n; s|data = .*|data = /incbin/(\"$ROOTFS_PATH\");|}" boot4recovery.its
./mk-fitimage.sh boot4recovery.its $ROOTFS_PATH arch/arm/boot/zImage $DTB_PATH resource.img boot.img arm $DTB_PATH
