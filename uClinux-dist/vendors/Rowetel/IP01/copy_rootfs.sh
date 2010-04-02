#!/bin/sh
# Copies from ext2 fs to yaffs
# Used to initialise the yaffs root filesystem on the IP04

if mount | grep -q /mnt; then
echo "NAND flash already mounted, please wait..."
else
mount /dev/mtdblock2 /mnt
fi

# Don't overwite /etc if it exists, that way we keep our * configs
# This is a bit primitive - any ideas for improvement?

if [ ! -d /mnt/etc ] ; then
  cp -a /etc /mnt
fi

# rm all old stuff so we get a clean install

cd /mnt; rm -Rf bin lib tmp home root usr sbin var dev mnt proc sys

# now copy rootfs across

cp -a /bin /mnt
cp -a /lib /mnt
cp -a /tmp /mnt
cp -a /home /mnt
cp -a /root /mnt
cp -a /usr /mnt
cp -a /sbin /mnt
cp -a /var /mnt
cp -a /dev /mnt
mkdir /mnt/mnt
mkdir /mnt/proc
mkdir /mnt/sys

sync
cd /
umount /mnt
