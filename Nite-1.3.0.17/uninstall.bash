#!/bin/bash 

niReg -u /usr/lib/libXnVFeatures.so
niReg -u /usr/lib/libXnVHandGenerator.so
rm /usr/lib/libXnVNite.so
rm /usr/lib/libXnVFeatures.so
rm /usr/lib/libXnVHandGenerator.so
rm -rf /usr/etc/primesense/XnVFeatures
rm -rf /usr/etc/primesense/XnVHandGenerator
rm -rf /usr/include/nite/

