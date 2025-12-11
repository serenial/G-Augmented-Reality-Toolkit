# FFmpeg Port Modification

-> This port adds in the `enable-lto` option into the line 32 of the portfile.cmake file.

```CMake
set(OPTIONS "--enable-pic --disable-doc --enable-debug --enable-runtime-cpudetect --disable-autodetect --enable-lto")
```