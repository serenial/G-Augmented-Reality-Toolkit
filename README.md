# G-Augmented-Reality-Toolkit (Work in Progress)

An Augmented-Reality Toolkit for LabVIEW

## Requirements:
* LabVIEW 2020
* C++ Development Tools for Windows
* vcpkg
* CMake 3.27

## To Do:
- [x] Basic LV-C++ Interoperability Library with an EDVR based image type
- [ ] Basic Windows Media Framework Camera Driver Integration
- [ ] OpenCV basic image manipulation
- [ ] OpenCV 3dCalib routines and basic image operations
- [ ] Apriltag Integration
- [ ] OpenCV to LabVIEW 3D Control transformations
- [ ] Example Code
- [ ] Documentation
- [ ] Windows Media Framework Camera Property Control
- [ ] Build-System Cross Platform Support

## Contributions
Welcome - please open an issue if you would like to contribute

## Notes
* CLFN Callbacks (like a shift register for each CLFN instance) https://lavag.org/topic/22215-call-function-library-node-callbacks/
* EDVR https://knowledge.ni.com/KnowledgeArticleDetails?id=kA00Z0000015AcdSAE&l=en-GB

### Webcam Capture
* https://github.com/sipsorcery/mediafoundationsamples (examples and docs)
* https://github.com/openpnp/openpnp-capture (inspiritaion for cross-platform camera driver)
* https://elcharolin.wordpress.com/2017/08/28/webcam-capture-with-the-media-foundation-sdk/
* https://github.com/roman380/MediaFoundationVideoCapture
* https://learn.microsoft.com/en-us/windows/win32/medfound/colorconverter
* https://stackoverflow.com/a/44791394/5609762 (COM Init advice)
* https://en.cppreference.com/w/cpp/thread/condition_variable
* https://stackoverflow.com/questions/9111362/media-foundation-mftransform-to-convert-mfsample-from-mjpg-to-yuy2-or-rgb24
* https://www.codeproject.com/Articles/776058/Capturing-Live-video-from-Web-camera-on-Windows-an (another example project)
* https://social.msdn.microsoft.com/Forums/windowsdesktop/en-US/9d6a8704-764f-46df-a41c-8e9d84f7f0f3/mjpg-encoded-media-type-is-not-available-for-usbuvc-webcameras-after-windows-10-version-1607-os?forum=mediafoundationdevelopment (Win 10 automatically makes NV12 streams for any MJPEG stream - use that instead)
* https://github.com/microsoft/Windows-classic-samples

### Conversion
RGB24 -> RGB32 (DSP)
YUY2 -> RGB32 (DSP)
NV12 -> RGB32 (DSP)
H264 -> NV12 -> RGB32 (decoder and DSP)
