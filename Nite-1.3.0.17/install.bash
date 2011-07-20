#!/bin/bash -e

cp Bin/libXnVNite.so /usr/lib
mkdir -p /usr/include/nite
cp Include/* /usr/include/nite
cp XnVFeatures/Bin/lib*.so /usr/lib
mkdir -p /usr/etc/primesense/XnVFeatures
cp XnVFeatures/Data/* /usr/etc/primesense/XnVFeatures
niReg /usr/lib/libXnVFeatures.so /usr/etc/primesense/XnVFeatures
cp XnVHandGenerator/Bin/lib*.so /usr/lib
mkdir -p /usr/etc/primesense/XnVHandGenerator
cp XnVHandGenerator/Data/* /usr/etc/primesense/XnVHandGenerator
niReg /usr/lib/libXnVHandGenerator.so /usr/etc/primesense/XnVHandGenerator

LIC_KEY=""
ASK_LIC="1"
while (( "$#" )); do
	case "$1" in
	-l=*)
		ASK_LIC="0"
		LIC_KEY=${1:3}
		;;
	esac
	shift
done

if [ "$ASK_LIC" == "1" ]; then
	printf "Please enter your PrimeSense license key: "
	read LIC_KEY
fi

if [ -z "$LIC_KEY" ]; then
	echo
	echo "*** WARNING: *****************************************************"
	echo "** No license key provided. Note that you can always install    **"
	echo "** new license keys by running:                                 **"
	echo "**                                                              **"
	echo "**    niLicense PrimeSense <key>                                **"
	echo "**                                                              **"
	echo "******************************************************************"
else
        niLicense PrimeSense $LIC_KEY
fi

make

#/usr/lib
#	libXnVNite.so
#	libXnVFeatures.so
#	libXnVHandGenerator.so
#/usr/include/nite
#	XnV*.h NITE header files
#/usr/etc/primesense/XnVFeatures
#	FeatureExtraction.ini
#	h.dat
#	s.dat
#/usr/etc/primesense/XnVHandGenerator
#	Nite.ini

