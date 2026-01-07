G Augmented-Reality Toolkit is a Webcam based Augmented-Reality Toolkit built with LabVIEW 2020.

This toolkit is a complete solution for building Augmented Reality functionality with LabVIEW encompassing image capture, processing and AR marker detection with 3D rendering using LabVIEW’s native 3D picture.

This toolkit could also be for you if you are looking for any of the following

* A system-native camera driver built with Windows Media Foundation/Video4Linux system APIs

* Image manipulation and file I/O

* Video file handling in many common formats via FFmpeg

* Routines for detection of image-features including contours/shapes, text, barcode/QR codes and neural network driven classification thanks to integration of OpenCV with its DNN module, Tesseract OCR and ZXing

* Augmented Reality functionality such as camera calibration, image rectification, homography, pose estimation and mapping and localization as well as support for Apriltags

* 3D rendering utilities which integrate with the LabVIEW 3D picture control to provide GPU acceleration

* Interoperability with NI's IMAQ Image-Type and LabVIEW's Python node without any dependency on either

This toolkit is released with ❤️ with the LabVIEW code under the permissive BSD Zero Clause License and the dynamically linked binary code is released under the LGPL-2.1.

Users of this toolkit in any larger software must ensure that the dependencies of this toolkit are appropriately acknowledged.

## Supported Platforms

Windows (x86 and x64), Linux Desktop (x64) and NI Linux RT (x64 and armv7a*)

\* the armv7a binaries only support a subset of the full features but include the camera driver, openCV calibration and pose estimation and AprilTag detection

## Getting the Toolkit
The toolkit can be [installed as a VIPM package](https://www.vipm.io/package/serenial_io_g_augmented_reality_toolkit/) or as a zipped release for usage directly from the releases tab.

Installation via VIPM will download and install the relevant dependency for your platform.

`.zip` users can run `Download and Install Shared Binaries.vi` which will install the relevant binaries from https://serenial.gitlab.io/g-augmented-reality-toolkit-binary-distribution/ or they can build them from source.

## Installing Binaries on NI-Linux-RT Targets
 [`.ipk` files can be downloaded from here](https://serenial.gitlab.io/g-augmented-reality-toolkit-binary-distribution/?filter=.ipk) or you can add `https://serenial.gitlab.io/g-augmented-reality-toolkit-binary-distribution/downloads/` as an `opkg` feed.

## Getting Started and Docs
A Getting Started Tutorial is provided alongside the [full documentation for the toolkit](https://serenial.gitlab.io/g-augmented-reality-toolkit-docs)

## Contributions
Contributions to this toolkit are welcome. Please open an issue to discuss any bug/features or to share any interesting projects you have used this toolkit for.

## Working with the Source Code (LabVIEW and C++)
If you are interested in working with either the LabVIEW or C++ source code then please follow the guidance in the documentation at [here](https://serenial.gitlab.io/g-augmented-reality-toolkit-docs/v1/toolkit/development.html).