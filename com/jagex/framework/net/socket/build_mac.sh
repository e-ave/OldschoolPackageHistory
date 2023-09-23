
#build jagsock in 32 and 64 bit flavours
gcc -m32 -I"/System/Library/Frameworks/JavaVM.framework/Versions/1.6.0/Headers/" -I"/System/Library/Frameworks/JavaVM.framework/Versions/1.6.0/Headers//mac" -shared -O3 -fPIC -fno-builtin -o "libjagsock.jnilib" native_linux.c
gcc -m64 -I"/System/Library/Frameworks/JavaVM.framework/Versions/1.6.0/Headers/" -I"/System/Library/Frameworks/JavaVM.framework/Versions/1.6.0/Headers//mac" -shared -O3 -fPIC -fno-builtin -o "libjagsock64.jnilib" native_linux.c

#copy to the corresponding run directory
cp libjagsock.jnilib ../../../../../../run
cp libjagsock64.jnilib ../../../../../../run

#clean any files
rm *.jnilib

