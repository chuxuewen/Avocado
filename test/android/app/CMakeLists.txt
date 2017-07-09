
cmake_minimum_required(VERSION 3.4.1)

include_directories(../../..)
include_directories(../../../depe/curl/include)
include_directories(../../../depe/zlib/contrib/minizip)
include_directories(../../../depe/sqlite-amalgamation)
include_directories(../../../depe/rapidjson/include)
include_directories(../../../depe/tinyxml2)
include_directories(../../../out)
include_directories(../../../autil/os)
include_directories(../../../depe/freetype2/include)
include_directories(../../../depe/tess2/Include)
include_directories(../../../depe/libwebp)
include_directories(../../../depe/libgif/lib)
include_directories(../../../depe/libjpeg-turbo)
include_directories(../../../depe/libpng)
include_directories(../../../depe/libwebp)
include_directories(../../../depe/ffmpeg)
link_directories(src/main/jniLibs/armeabi-v7a)

add_definitions(
   -DDEBUG
   -D__STDC_CONSTANT_MACROS
)

add_library( avocado-util
             SHARED
             ../../../autil/array.cc
             ../../../autil/codec.cc
             ../../../autil/error.cc
             ../../../autil/http.cc
             ../../../autil/ws.cc
             ../../../autil/fs.cc
             ../../../autil/fs-search.cc
             ../../../autil/fs-reader.cc
             ../../../autil/handle.cc
             ../../../autil/buffer.cc
             ../../../autil/json.cc
             ../../../autil/string.cc
             ../../../autil/string-builder.cc
             ../../../autil/map.cc
             ../../../autil/util.cc
             ../../../autil/zlib.cc
             ../../../autil/thread.cc
             ../../../autil/storage.cc
             ../../../autil/jsx.cc
             ../../../autil/ref.cc
             ../../../autil/sys.cc
             ../../../autil/os/android-jni.cc
             ../../../autil/os/android-log.cc
             ../../../autil/os/android-fs.cc
             ../../../autil/os/android-sys.cc
             ../../../android/avocado.cc
           )

add_library( avocado-gui
             SHARED
             ../../../out/glsl-shader.cc
             ../../../out/glsl-es2-shader.cc
             ../../../agui/action.cc
             ../../../agui/app.cc
             ../../../agui/audio-player.cc
             ../../../agui/div.cc
             ../../../agui/limit.cc
             ../../../agui/free-div.cc
             ../../../agui/gradient-div.cc
             ../../../agui/shadow-div.cc
             ../../../agui/limit-div.cc
             ../../../agui/image.cc
             ../../../agui/bezier.cc
             ../../../agui/event.cc
             ../../../agui/display-port.cc
             ../../../agui/font/font.cc
             ../../../agui/image/codec.cc
             ../../../agui/image/codec-tga.cc
             ../../../agui/image/codec-pvrtc.cc
             ../../../agui/image/codec-gif.cc
             ../../../agui/image/codec-jpeg.cc
             ../../../agui/image/codec-png.cc
             ../../../agui/image/codec-webp.cc
             ../../../agui/pre-render.cc
             ../../../agui/mathe.cc
             ../../../agui/media-codec.cc
             ../../../agui/media-codec-inl.cc
             ../../../agui/label.cc
             ../../../agui/layout.cc
             ../../../agui/box.cc
             ../../../agui/view.cc
             ../../../agui/draw.cc
             ../../../agui/ogl/ogl.cc
             ../../../agui/ogl/ogles2.cc
             ../../../agui/ogl/texture.cc
             ../../../agui/ogl/font.cc
             ../../../agui/root.cc
             ../../../agui/sprite.cc
             ../../../agui/scroll.cc
             ../../../agui/sprite.cc
             ../../../agui/span.cc
             ../../../agui/text.cc
             ../../../agui/text-rows.cc
             ../../../agui/text-font.cc
             ../../../agui/text-node.cc
             ../../../agui/texture.h
             ../../../agui/texture.cc
             ../../../agui/vector.cc
             ../../../agui/video.cc
             ../../../agui/value.cc
             ../../../agui/activity.cc
             ../../../agui/button.cc
             ../../../agui/os/android-ogl.cc
             ../../../agui/os/android-app.cc
             ../../../agui/os/android-media-codec.cc
             ../../../agui/os/android-pcm-player.cc
             ../../../agui/os/android-pcm-audio-track.cc
             ../../../agui/keyboard.cc
             ../../../agui/os/android-keyboard.cc
           )

add_definitions(
  -DCHROME_PNG_WRITE_SUPPORT
  -DPNG_USER_CONFIG
)

target_link_libraries( avocado-util atomic log android z avocado-zlib curl openssl )
target_link_libraries( avocado-gui OpenSLES GLESv3 EGL mediandk avocado-util tess2 ft2 gif jpeg-x png-x webp ffmpeg )
