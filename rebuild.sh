#!/usr/bin/env bash

set -ex

if ! diff -u debian/changelog debian.qcom-x1e/changelog
then
    fakeroot debian/rules clean
fi

dh_clean

rm -rf debian/build/build-qcom-x1e/_____________________________________dkms/

export LLVM=1
export RUST_LIB_SRC=/usr/src/rustc-$(rustc-1.80 --version | cut -d' ' -f2)/library

time dpkg-buildpackage -d -b -aarm64 -k'jens.glathe@oldschoolsolutions.biz'

#time debuild --no-lintian -k'jens.glathe@oldschoolsolutions.biz'
