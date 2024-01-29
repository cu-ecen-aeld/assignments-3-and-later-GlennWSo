#!/usr/bin/env bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

OUTDIR="$(realpath out)" #/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.1.10
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
SYSROOT="$(aarch64-none-linux-gnu-gcc -print-sysroot)"
CROSS_COMPILE=aarch64-none-linux-gnu-

if [ $# -lt 1 ]
then
	echo "Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi

mkdir -p ${OUTDIR}

cd "$OUTDIR"
if [ ! -d "linux-stable" ]; then
    #Clone only if the repository does not exist.
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi
if [ ! -e $OUTDIR/Image ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

    # TODO: Add your kernel build steps here
    echo starting kernel build
    echo deep clean, removing the .config file with any existing configs
    make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE mrproper

    echo creating default config
    make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE defconfig # create a default .config

    echo building kernal image
    make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE -j4 all # make kernal image

    echo skipping building modules
    # make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE modules

    echo building device tree
    make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE dtbs

    echo "Adding the Image in outdir"
    mv arch/${ARCH}/boot/Image ../Image
    echo linux kernel build fin
fi


echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs
fi

echo Create necessary base directories
mkdir rootfs
cd rootfs
ROOTFS=$PWD
mkdir bin dev etc home lib lib64 proc sbin sys tmp
mkdir -p usr/bin usr/lib usr/sbin var/log


cd "$OUTDIR"
if [ ! -d "busybox" ]
then
  git clone git://busybox.net/busybox.git
  cd busybox
  git checkout ${BUSYBOX_VERSION}
  echo Configuring busybox
  make distclean
  make defconfig
else
    cd busybox
fi

echo Make and install busybox
make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE
make CONFIG_PREFIX=$ROOTFS ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE install

echo "Library dependencies"
${CROSS_COMPILE}readelf -a busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a busybox | grep "Shared library"

# req interpreter: /lib/ld-linux-aarch64.so.1 needs to in /lib
# req shared libs: libm.so.6 libresolv.so.2 libc.so.6 into /lib64
echo cp buzybox interpreter libs into rootfs
buzy_interp="$(find $SYSROOT -name ld-linux-aarch64.so.1)"
cp $buzy_interp $ROOTFS/lib
libm="$(find $SYSROOT -name libm.so.6)"
libresolv="$(find $SYSROOT -name libresolv.so.2)"
libc="$(find $SYSROOT -name libc.so.6)"
cp $libm $libresolv $libc $ROOTFS/lib64


echo creating null device
cd $ROOTFS
sudo mknod -m 666 dev/null c 1 3
sudo mknod -m 600 dev/console c 5 1


echo Clean and build the writer utility
cd $FINDER_APP_DIR
make clean
make CROSS_COMPILE=$CROSS_COMPILE writer

# these commands reveal that the deps of writer.
# aarch64-none-linux-gnu-readelf -a writer | grep "Shared libr"
# file writer
# but these are less then set of deps required by busybox so
# write_interp="$(find $SYSROOT -name ld-linux-aarch64.so.1)"
# libc="$(find $SYSROOT -name libc.so.6)"
cp writer $ROOTFS/bin

echo prepare intitramfs
cd $OUTDIR
# TODO: Chown the root directory
sudo chown root $ROOTFS
cd $ROOTFS
find . | cpio -H newc -ov --owner root:root > ${OUTDIR}/initramfs.cpio
cd ..
gzip -f initramfs.cpio



