# Notably this uses gcc-3.4, as later versions try and use symbols we don't have on some of our machines...
#build jagsock in 32 and 64 bit flavours
gcc-3.4 -m32 -I"/usr/lib/jvm/java-6-sun/include" -I"/usr/lib/jvm/java-6-sun/include/linux" -shared -O3 -fno-builtin -o"libjagsock.so" native_linux.c
gcc-3.4 -m64 -I"/usr/lib/jvm/java-6-sun/include" -I"/usr/lib/jvm/java-6-sun/include/linux" -shared -O3 -fPIC -fno-builtin -o"libjagsock64.so" native_linux.c

#copy to the corresponding run directory
cp libjagsock.so ../../../../../../run
cp libjagsock64.so ../../../../../../run

#clean any files
rm *.so
