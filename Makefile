-include out/config.mk

ARCHE 				?= x64
OS						?= `uname`
BUILDTYPE			?= Release
V							?= 0
CXX						?= g++
LINK					?= g++
ENV						?=
NODE					?= node
SUFFIX				?= $(ARCHE)
TOOLS          = ./tools/node_modules/avocado-tools
GYP						 = $(TOOLS)/gyp/gyp

# make-linux
PTOJECTS = make xcode msvs
GYPFILES = Makefile avocado.gyp tools/common.gypi \
					 out/config.gypi tools.gyp tools/tools.gypi
GYP_ARGS = --generator-output="out" -Goutput_dir="out" \
-Iout/var.gypi -Iout/config.gypi -Itools/common.gypi \
-Dcomponent=shared_library -Dlibrary=shared_library \
-S.$(OS).$(SUFFIX) --depth=.

ifeq ($(V), 1)
V_ARG = "V=1"
endif

ifeq ($(OS), android)
ANDROID_JNI_DIR ?= x86
define copy_android_test_so
	@mkdir -p test/android/app/src/main/jniLibs/$(ANDROID_JNI_DIR)
	@cp out/$(OS).$(SUFFIX).$(BUILDTYPE)/lib.target/*.so test/android/app/src/main/jniLibs/$(ANDROID_JNI_DIR)
	@cd test/android/app/src/main/jniLibs/$(ANDROID_JNI_DIR); rm -rf libavocado-util.so libavocado-gui.so
endef
endif

define make_compile
	@$(ENV) $(MAKE) -C "out" -f Makefile.$(OS).$(SUFFIX) \
CXX="$(CXX)" LINK="$(LINK)" $(V_ARG) BUILDTYPE=$(BUILDTYPE) \
builddir="$(shell pwd)/out/$(OS).$(SUFFIX).$(BUILDTYPE)"
endef

.PHONY: $(PTOJECTS) tools install install_d \
	help h all clear clear-all build server

.SECONDEXPANSION:

######################Build######################

all: build

build: make out/Makefile.$(OS).$(SUFFIX)
	$(make_compile)

out/config.gypi: configure
	./configure

# GYP file generation targets.
$(PTOJECTS): $(GYPFILES)
	@echo "{'variables':{'project':'$@'}}" > out/var.gypi;
	GYP_GENERATORS=$@ $(GYP) -f $@ avocado.gyp $(GYP_ARGS)

tools: $(GYPFILES)
	@echo "{'variables':{'project':'make'}}" > out/var.gypi;
	GYP_GENERATORS=make $(GYP) -f make tools.gyp $(GYP_ARGS)
	$(make_compile)
	@mkdir -p $(TOOLS)/bin/$(OS)
	@cp out/$(OS).$(SUFFIX).$(BUILDTYPE)/jsa-shell $(TOOLS)/bin/$(OS)/jsa-shell

#################################################

copy_android_test_so:
	$(copy_android_test_so)

# build ios product
ios:
	@sh ./tools/build-ios.sh

# install avocado command
install: ios
	@./configure --ffmpeg=0
	@$(MAKE) tools
	@$(NODE) ./tools/build-product.js
	@./out/product/install

# debug install avocado command
install_d:
	@./configure --ffmpeg=0
	@$(MAKE) tools
	@./tools/node_modules/install

server:
	@$(NODE) tools/server.js

clear:
	@rm -rfv out/$(OS).*
	@rm -rfv out/product/avocado-tools/product/$(OS)

clear-all:
	@rm -rfv out

help h:
	@echo
	@echo exec \"make\" or \"make build\" start compile
	@echo exec \"make xcode\" output xcode project file
	@echo You must first call before calling make \"./configure\"
	@echo
