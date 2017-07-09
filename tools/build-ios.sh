#!/bin/sh

CWD=`pwd`
PRODUCT_DIR="out/product/avocado-tools/product"

build() {
	make build
  cd out/ios.$1.Release
  
	ar rc libavocado.a `find obj.target/avocado-gui \
	 												obj.target/avocado-util \
	 												obj.target/avocado-v8 \
	 												obj.target/zlib \
	 												obj.target/ft2 \
	 												obj.target/http_parser \
	 												obj.target/openssl \
	 												obj.target/reachability \
	 												obj.target/tess2 obj.target/libuv -name *.o|xargs`
	ar rc libv8.a `find obj.target/v8_base \
									obj.target/v8_libbase \
									obj.target/v8_libplatform \
									obj.target/v8_libsampler obj.target/v8_nosnapshot -name *.o|xargs`

	ranlib libavocado.a
	ranlib libv8.a
	cd $CWD
}

./configure --os=ios --arch=arm
build armv7
./configure --os=ios --arch=arm64
build arm64
./configure --os=ios --arch=x64
build x64

LIBS_AVOCADO="out/ios.armv7.Release/libavocado.a 
							out/ios.arm64.Release/libavocado.a 
							out/ios.x64.Release/libavocado.a"
LIBS_V8="out/ios.armv7.Release/libv8.a 
				out/ios.arm64.Release/libv8.a 
				out/ios.x64.Release/libv8.a"
LIBS_FFMPEG="out/ios.armv7.Release/libffmpeg.a 
						out/ios.arm64.Release/libffmpeg.a
						out/ios.x64.Release/libffmpeg.a"

if [ "$1" = 1 ]; then
	./configure --os=ios --arch=arm --armv7s
	build armv7s
	./configure --os=ios --arch=x86
	build x86
	
	LIBS_AVOCADO="$OUT_LIBS_AVOCADO
								out/ios.armv7s.Release/libavocado.a 
								out/ios.x86.Release/libavocado.a"
	LIBS_V8="$LIBS_V8 
					out/ios.armv7s.Release/libv8.a 
					out/ios.x86.Release/libv8.a"
	LIBS_FFMPEG="$LIBS_FFMPEG 
							out/ios.armv7s.Release/libffmpeg.a 
							out/ios.x86.Release/libffmpeg.a"
fi

mkdir -p ${PRODUCT_DIR}/ios
rm -rf ${PRODUCT_DIR}/ios/*.a

lipo $LIBS_AVOCADO -create -output ${PRODUCT_DIR}/ios/libavocado.a
lipo $LIBS_V8 -create -output ${PRODUCT_DIR}/ios/libv8.a
lipo $LIBS_FFMPEG -create -output ${PRODUCT_DIR}/ios/libffmpeg.a
