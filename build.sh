#!/bin/sh
ARCHOPT="arm"

if [ "X${1}" != "X" ];then
    ARCHOPT=${1}
fi

#===============================================================================
#       functions
#===============================================================================
build_for_arm()
{
    export PATH=/opt/onyx/arm/bin/:/opt/freescale/usr/local/gcc-4.4.4-glibc-2.11.1-multilib-1.0/arm-fsl-linux-gnueabi/bin/:${PATH}

    CLFS_TARGET=arm-linux
    export CC="${CLFS_TARGET}-gcc"
    export CXX="${CLFS_TARGET}-g++"
    export AR="${CLFS_TARGET}-ar"
    export AS="${CLFS_TARGET}-as"
    export RANLIB="${CLFS_TARGET}-ranlib"
    export LD="${CLFS_TARGET}-ld"
    export STRIP="${CLFS_TARGET}-strip"

    export PKG_CONFIG_PATH=${ONYXDIR}/mwo/lib/pkgconfig:/opt/onyx/arm/lib/pkgconfig/ 
    export PKG_CONFIG_LIBDIR=${PKG_CONFIG_PATH}

    export QMAKESPEC=/opt/onyx/arm/mkspecs/qws/linux-arm-g++/
    #ONYX_SDK_ROOT='/opt/onyx/arm'

    install -d build/arm
    cd build/arm && cmake ../.. -DBUILD_FOR_ARM:BOOL=ON
    make -j2
}

build_for_x86()
{
    install -d build/x86
    cd build/x86 && cmake ../.. -DBUILD_FOR_X86:BOOL=ON
    make -j2
}
#===============================================================================
#       end of functions
#===============================================================================

case ${ARCHOPT} in
    "arm")
        echo "Building for ${ARCHOPT} ..."
        build_for_arm
        exit 0
        ;;
    "x86")
        echo "Building for ${ARCHOPT} ..."
        build_for_x86
        exit 0
        ;;
    *)
        echo "ARCH option ${ARCHOPT} is wrong or not supported yet."
        echo "Build aborted. Exit."
        exit 1
        ;;
esac

