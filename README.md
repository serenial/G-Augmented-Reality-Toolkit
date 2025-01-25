# G-Augmented-Reality-Toolkit

A Webcam based Augmented-Reality Toolkit for LabVIEW

This toolkit is a complete solution for building Augmented Reality functionality with LabVIEW encompassing image capture, processing and AR marker detection with 3D rendering using LabVIEW's native 3D rendering.

Whilst the intended application is around AR this toolkit also provides
* A system-native open-source webcam/capture-card driver for LabVIEW
* Integration of OpenCV routines for basic image manipulation
* Access to OpenCV camera calibration and image rectification
* Utilites for fitting 3D points to planes and combining 3D point-maps

## Supported Platforms
The LabVIEW code is written in LabVIEW 2020 SP1 and provides binaries for the following platforms:

| Platform | Supported |
|----------|-----------|
| Windows x86 | ✅ |
| Windows x64 | ✅ |
| Linux x64 | ✅ |
| NI-LinuxRT x64 | Planned |
| NI-LinuxRT ARM | Not Planned |
| MacOS (x86-64 or Apple Silicon) | Not Supported |
 
## Installation
This library is distributed on VIPM #TODO:link or the zipped source can be downloaded from the release section for use on a per-project basis.

## Getting Started and Docs
A Getting Started Tutorial is provided alongside the full documentation for the toolkit #TODO:Link

## Licence
This source is distributed under Zero Clause BSD to make it as easy to use and integrate as possible.

Whilst no attribution is required for this code, it depends on the following open source components which will have different licencincg terms which you will have to meet to comply with copyright law.

Full SBOM provision is in progress but is [waiting on support from the C++ package manger used by this toolkit](https://github.com/microsoft/vcpkg-tool/pull/1514)

### 3rd-Party Libraries
| Library | Licence |
|---------|---------|
| [OpenCV 4](https://github.com/opencv/opencv) | Apache License 2.0 |
| [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader) | MIT |
| [Apriltag](https://github.com/AprilRobotics/apriltag) | BSD 2-Clause License |
| [Apriltag Generation](https://github.com/AprilRobotics/apriltag-generation) (if usage includes AprilTag Generation VIs) | BSD 2-Clause Licence |
| [whereami](https://github.com/gpakosz/whereami.git) | MIT |
| [ros_msft_camera](https://github.com/ms-iot/ros_msft_camera) (on Windows) | MIT |
| [v4l2-list-devices](https://github.com/improvess/v4l2-list-devices) (on Linux) | Apache License 2.0 |
| [WeChat QR/Barcode Reader Models](https://github.com/WeChatCV/opencv_3rdparty/tree/wechat_qrcode) | Apache License 2.0 |
| [eigen](https://gitlab.com/libeigen/eigen) | Mozilla Public License 2.0 |
| [Averaging Quaternions](https://github.com/tolgabirdal/averaging_quaternions) | MIT |

### Other Items

The following are distrubuted with the source for use with examples or testing etc

*Models*

| Model | Licence | Source |
|-------|---------|--------|
| Spot  | https://creativecommons.org/publicdomain/zero/1.0/ | https://www.cs.cmu.edu/~kmcrane/Projects/ModelRepository/ |

*Test Images*

| Image | Licence | Source |
|-------|---------|--------|
| tag36h11-mosaic.png  | BSD 2-Clause Licence | https://github.com/AprilRobotics/apriltag-imgs |
| exampled_coins.jpg | MIT | https://github.com/JamesMc86/Python-and-LabVIEW-Presentation |
| barcode_book.jpg | Apache License 2.0 | https://docs.opencv.org/4.10.0/barcode_book.jpg |
| Qr-code-ver-10.png|  Creative Commons CC0 1.0 Universal Public Domain Dedication | https://commons.wikimedia.org/wiki/File:Qr-code-ver-10.png |

## Contributions
Contributions to this toolkit are welcome. Please open an issue to discuss any bug/features or to share any interesting projects you have used this toolkit for.

## Developer Tooling
The tooling for LabVIEW development of this toolkit can be installed by the included `.vipc` file.

A `.lvversion` file is included to maintain the source version for users of LV2024Q3 onwards but building of packeges will require LabVIEW 2020 SP1.

Those interested in modifying and building the `C++` code will need to setup their system as described in the documentation #TODO:link

Those wanting to modify and build the docs locally will need to follow the instructions here #TODO:link

## Related Toolkits and Code
[G-Image](https://github.com/dataflowg/g-image) - A cross-platform LabVIEW library for loading, saving, and processing images.
[3DEngine](https://github.com/neilpate/3DEngine) - A LabVIEW native 3D engine
[G<sup>2</sup>CPU](https://www.g2cpu.com/) - A high performance computing toolkit (community version is avaliable).
[Haro3D by HaroTek](https://www.vipm.io/package/harotek_lib_haro3d/) - A toolkit with support for AR type hardware
