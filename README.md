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

## 🎉 Pre-Release Version 1.0.0 🎉

A pre-release version is available in the releases section.

This contains the current state of the project (at tag 1.0.0-pre) with built binaries for 32 and 64 bit windows.

These have been provided to enable community experimentation and as a basis for feedback.

If you do have any feedback please open an issue to discuss.

Unfortunately documentation is currently very sparese. Some information on interoperability is provided in [this document](docs/Interop.md).

## LabVIEW Developer Setup
* Windows 10/11
* LabVIEW 2020
* VIPM
* LUnit (for tests - install via VIPM)

## C++ Developer Setup 

Setup `vcpkg` buildtool in the vcpkg submodule with 
```bash
git submodule init
git submodule update
```
Then run the `vcpkg` bootstrap script in the `vcpkg` directory for your platform (`.bat` for Windows or `.sh` for Linux)

### on Windows
If you wish to modify the C++ code you will require the following for building the binaries

* C++ Development Tools for Windows (VS 2022)
* Ninja build tool
* (Recommended) VSCode with C++ and CMake Tool Extensions installed

### on Linux (Ubuntu) - Still Under Development
Install the dependencies using `apt`

```bash
sudo apt install build-essential g++ python3 bison autoconf automake libtool pkg-config python3-jinja2 nasm libx11-dev libxft-dev libxext-dev libxi-dev  libxtst-dev python3-distutils ninja-build gperf libdbus-1-dev libgl1-mesa-dev libgles2-mesa-dev libglu1-mesa-dev libudev-dev libx11-xcb-dev  libxcursor-dev  libxdamage-dev  libxinerama-dev libxrandr-dev
``` 

### C++ Setup

* Install dependencies

_If using VSCode_

> **Note**
> Windows users should launch _VSCode_ or the build batchfile/script from the `x86 Native Tools Command Prompt ...` or `x64 Native Tools Command Prompt ...` (depending on desired bitness) to ensure the build tools can be located
> _VSCode_ can be launced by simply running the `code` command

* Use the `.vscode-example` directory as a template for a project `.vscode` directory and configure the *cmake/vcpkg* location
* Use the VSCode CMake intergration tools to choose the desored configuration (release/debug)
* build the "install" target which should configure, build and install the binaries to `LabVIEW/bin`
  
_If not using VSCode_
* Use the provided `<platform>-win-build.bat-example` batch files as a starting point. Adjust variables in the batch file and modify other values to suit your preferences. 
* Run your modified batch file to build and install the .dll into the `LabVIEW/bin` directory

>![NOTE] 
> When building code you will have to close the LabVIEW project and potentially exit LabVIEW to avoid file locking of the binaries.

## To Do:
- [x] Basic LV-C++ Interoperability with an EDVR based image type
- [x] OpenCV basic image manipulation
- [x] Basic Windows Media Framework Camera Driver Integration
- [ ] Linux Webcam Driver and Linux Build Tooling (Work-in-progress)
- [ ] OpenCV 3dCalib routines
- [ ] Apriltag Integration
- [ ] OpenCV to LabVIEW 3D Control transformations
- [ ] Example Code
- [ ] Documentation
- [ ] Windows Media Framework Camera Property Control


## Contributions
Welcome - please open an issue if you would like to contribute

## C++ 3rd-Party Libraries
| Library | Licence |
|---------|---------|
| OpenCV 4 | Apache License 2.0 |
| [ros_msft_camera](https://github.com/ms-iot/ros_msft_camera) (on Windows) | MIT |
| [openpnp-capture](https://github.com/openpnp/openpnp-capture.git) (on Linux) | MIT |
| [v4l2-list-devices](https://github.com/improvess/v4l2-list-devices) (on Linux) | Apache License 2.0 |
| [tinyobjectloader](https://github.com/tinyobjloader/tinyobjloader) (not yet integrated) | MIT |
| [Apriltag](https://github.com/AprilRobotics/apriltag) (not yet integrated) | BSD 2-Clause License |

