cmake_minimum_required(VERSION 3.5)

set(PACKAGE "LINEDRAWER")
project(${PACKAGE})

IF (WIN32)
  SET(CMAKE_CXX_COMPILER "x86_64-w64-mingw32-g++")
  link_directories($ENV{VIPS_INCLUDE}/lib)
ELSE()
  find_package(PkgConfig)
  pkg_check_modules(VIPS vips-cpp REQUIRED)
ENDIF()

include_directories(include)

file(GLOB SOURCE_FILES lib/drawLines.c++ lib/generatePoints.c++ lib/progress_bar.c++)

set( EXECUTABLES RunLineDraw )

set (CMAKE_CXX_STANDARD 11)
set(VIPS_LDFLAGS pthread ${VIPS_LDFLAGS})

foreach( EXECUTABLE ${EXECUTABLES} )
    add_executable( ${EXECUTABLE} lib/${EXECUTABLE}.c++ ${SOURCE_FILES} )
    IF (WIN32)
      set(VIPS_LDFLAGS vips-cpp vips gsf-1 z jpeg xml2 fftw3 m lcms2 pangoft2-1.0 tiff png16 exif pango-1.0 freetype fontconfig gobject-2.0 gmodule-2.0 gthread-2.0 glib-2.0 intl)
      include_directories($ENV{VIPS_INCLUDE}/include $ENV{VIPS_INCLUDE}/include/glib-2.0 $ENV{VIPS_INCLUDE}/lib/glib-2.0/include)
      target_link_libraries(${EXECUTABLE} ${VIPS_LDFLAGS})
    ELSE()
      target_link_libraries(${EXECUTABLE} ${VIPS_LIBRARIES})
      target_link_libraries(${EXECUTABLE} ${VIPS_LDFLAGS})
      add_definitions(${VIPS_CFLAGS} ${VIPS_CFLAGS_OTHER})
    ENDIF()
endforeach( EXECUTABLE ${EXECUTABLES} )
