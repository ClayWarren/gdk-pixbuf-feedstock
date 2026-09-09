cmake_minimum_required(VERSION 3.16)
project(gdk_pixbuf_consumer C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GDK_PIXBUF REQUIRED IMPORTED_TARGET gdk-pixbuf-2.0)
add_executable(gdk-pixbuf-consumer test-consumer.c)
target_link_libraries(gdk-pixbuf-consumer PRIVATE PkgConfig::GDK_PIXBUF)
