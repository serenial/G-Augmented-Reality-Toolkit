# G-Augmented-Reality-Toolkit (Work in Progress)

An Augmented-Reality Toolkit for LabVIEW.

This toolkit ~~is~~ will eventually be a single solution to building Augmented Reality functionality with LabVIEW encompassing webcam capture, image processing and AR marker detection and 3D rendering using LabVIEW's built-in 3D picture functionality.

Whilst the intended application is around AR this toolkit also provides
* A system-native open-source webcam driver for LabVIEW
* Integration of OpenCV routines for basic image manipulation
* Access to OpenCV camera calibration and image rectification
* Utilites for fitting 3D points to planes and combining 3D point-maps

## What is Augmented Reality?

If you would like to learn more about AR in LabVIEW and the motivations behind this library then [checkout this talk from gdev-con (eur) 2023](https://youtu.be/N7MVxLI1WsQ?si=qfJQf3qi_rYwaQ_8)

And how can this look in LabVIEW?
This is a simple example based on proof-of-concept code which is being polished and integrated into the toolkit.

![Am animated-gif of a AR cow floating in-front of a chessboard in LabVIEW](docs/demo.gif "an example of a simple AR application in LabVIEW (built using proof-of-concept code not in this toolkit yet)")

## Developer Requirements:
Whilst this library is a work in progress, the following tools are required to build the binaries and develop the code in LabVIEW
* Windows 10/11
* LabVIEW 2020
* LUnit (via VIPM)
* C++ Development Tools for Windows (VS 2022)
* vcpkg
* (Recommended) VSCode with C++ and CMake Tool Extensions installed

## Developmer Setup
The build binary files are not currently provided so must be build from source. To create the binaries for your system (with the bitness matching your LabVIEW bitness), complete the following:

* Install dependencies
_If using VSCode_
* Use the `.vscode-example` directory as a template for a project `.vscode` directory and configure the *cmake/vcpkg* location
* Use the VSCode CMake intergration tools to choose the build-kit, release type to match your system
* build the "install" target which should configure, build and install the binaries to `LabVIEW/bin`
_If not using VSCode_
* Use the provided `<platform>-win-build.bat-example` batch files as a starting point. Set the `VCKPGROOT` variable in the batch file and modify other values to suit your preferences. 
* Run your modified batch file to build and install the .dll into the `LabVIEW/bin` directory

>![NOTE] 
> When building code you will have to close the LabVIEW project and potentially exit LabVIEW to avoid file locking of the binaries.

## IMAQ Interoperability
Whilst this toolkit is designed to have no IMAQ or NI-Vision dependencies, users may wish to integrate it alongside IMAQ/NI-Vision functionality.

Methods of copying to/from IMAQ images into the g-ar-tookit image format can be accomplished using the snippets below:

![Copy to IMAQ](docs/snippets/copy-from-imaq.png "Copy from IMAQ")

![Copy to IMAQ](docs/snippets/copy-to-imaq.png "Copy to IMAQ")

> ![NOTE]
> Only basic IMAQ functions are required when using the `Get Pixel Ptr` IMAQ vi. Other operations using the IMAQ to Array functions require NI-Vision

## To Do:
- [x] Basic LV-C++ Interoperability with an EDVR based image type
- [x] OpenCV basic image manipulation
- [x] Basic Windows Media Framework Camera Driver Integration
- [ ] OpenCV 3dCalib routines
- [ ] Apriltag Integration
- [ ] OpenCV to LabVIEW 3D Control transformations
- [ ] Example Code
- [ ] Documentation
- [ ] Windows Media Framework Camera Property Control
- [ ] Linux Webcam Driver and Linux Build Tooling

## Contributions
Welcome - please open an issue if you would like to contribute

## C++ 3rd-Party Libraries
| Library | Licence |
|---------|---------|
| OpenCV 4 | Apache License 2.0 |
| tinyobjectloader | MIT |
| Apriltag | BSD-2 |
| ros_msft_camera | MIT |