<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="20008000">
	<Property Name="NI.LV.All.SaveVersion" Type="Str">20.0</Property>
	<Property Name="NI.LV.All.SourceOnly" Type="Bool">true</Property>
	<Property Name="NI.Project.Description" Type="Str"></Property>
	<Item Name="My Computer" Type="My Computer">
		<Property Name="NI.SortType" Type="Int">3</Property>
		<Property Name="server.app.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.control.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.tcp.enabled" Type="Bool">false</Property>
		<Property Name="server.tcp.port" Type="Int">0</Property>
		<Property Name="server.tcp.serviceName" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.tcp.serviceName.default" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.vi.callsEnabled" Type="Bool">true</Property>
		<Property Name="server.vi.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="specify.custom.address" Type="Bool">false</Property>
		<Item Name="VIPM" Type="Folder">
			<Item Name="Toolkit Binaries Pre-Uninstall Custom Action.vi" Type="VI" URL="../VIPM/Toolkit Binaries Pre-Uninstall Custom Action.vi"/>
			<Item Name="Toolkit Post-Install Custom Action.vi" Type="VI" URL="../VIPM/Toolkit Post-Install Custom Action.vi"/>
			<Item Name="Toolkit Binaries Post-Install Custom Action.vi" Type="VI" URL="../VIPM/Toolkit Binaries Post-Install Custom Action.vi"/>
		</Item>
		<Item Name="test-images" Type="Folder" URL="../test-images">
			<Property Name="NI.DISK" Type="Bool">true</Property>
		</Item>
		<Item Name="models" Type="Folder" URL="../models">
			<Property Name="NI.DISK" Type="Bool">true</Property>
		</Item>
		<Item Name="utility" Type="Folder" URL="../utility">
			<Property Name="NI.DISK" Type="Bool">true</Property>
		</Item>
		<Item Name="build-tools" Type="Folder" URL="../build-tools">
			<Property Name="NI.DISK" Type="Bool">true</Property>
		</Item>
		<Item Name="bin" Type="Folder" URL="../common/bin">
			<Property Name="NI.DISK" Type="Bool">true</Property>
		</Item>
		<Item Name="wechat_models" Type="Folder" URL="../test/FeatureDetection/wechat_models">
			<Property Name="NI.DISK" Type="Bool">true</Property>
		</Item>
		<Item Name=".lvversion" Type="Document" URL="../.lvversion"/>
		<Item Name="common.lvlib" Type="Library" URL="../common/common.lvlib"/>
		<Item Name="image.lvlib" Type="Library" URL="../image/image.lvlib"/>
		<Item Name="video_io.lvlib" Type="Library" URL="../video_io/video_io.lvlib"/>
		<Item Name="feature_detection.lvlib" Type="Library" URL="../feature_detection/feature_detection.lvlib"/>
		<Item Name="capture.lvlib" Type="Library" URL="../capture/capture.lvlib"/>
		<Item Name="ar.lvlib" Type="Library" URL="../ar/ar.lvlib"/>
		<Item Name="shape.lvlib" Type="Library" URL="../shape/shape.lvlib"/>
		<Item Name="examples.lvlib" Type="Library" URL="../examples/examples.lvlib"/>
		<Item Name="binary_installation_tools.lvlib" Type="Library" URL="../binary_installation_tools/binary_installation_tools.lvlib"/>
		<Item Name="test.lvlib" Type="Library" URL="../test/test.lvlib"/>
		<Item Name="antidoc.config" Type="Document" URL="../antidoc.config"/>
		<Item Name="LICENSE" Type="Document" URL="../../LICENSE"/>
		<Item Name="Download and Install Shared Binaries.vi" Type="VI" URL="../Download and Install Shared Binaries.vi"/>
		<Item Name="Dependencies" Type="Dependencies">
			<Item Name="vi.lib" Type="Folder">
				<Item Name="Error Cluster From Error Code.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Error Cluster From Error Code.vi"/>
				<Item Name="NI_Matrix.lvlib" Type="Library" URL="/&lt;vilib&gt;/Analysis/Matrix/NI_Matrix.lvlib"/>
				<Item Name="NI_AALBase.lvlib" Type="Library" URL="/&lt;vilib&gt;/Analysis/NI_AALBase.lvlib"/>
				<Item Name="imagedata.ctl" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/imagedata.ctl"/>
				<Item Name="Flatten Pixmap.vi" Type="VI" URL="/&lt;vilib&gt;/picture/pixmap.llb/Flatten Pixmap.vi"/>
				<Item Name="Draw Flattened Pixmap.vi" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/Draw Flattened Pixmap.vi"/>
				<Item Name="FixBadRect.vi" Type="VI" URL="/&lt;vilib&gt;/picture/pictutil.llb/FixBadRect.vi"/>
				<Item Name="Picture to Pixmap.vi" Type="VI" URL="/&lt;vilib&gt;/picture/pictutil.llb/Picture to Pixmap.vi"/>
				<Item Name="Unflatten Pixmap.vi" Type="VI" URL="/&lt;vilib&gt;/picture/pixmap.llb/Unflatten Pixmap.vi"/>
				<Item Name="Empty Picture" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/Empty Picture"/>
				<Item Name="Draw Oval.vi" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/Draw Oval.vi"/>
				<Item Name="Set Pen State.vi" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/Set Pen State.vi"/>
				<Item Name="Draw Rectangle.vi" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/Draw Rectangle.vi"/>
				<Item Name="Search Unsorted 1D Array.vim" Type="VI" URL="/&lt;vilib&gt;/Array/Search Unsorted 1D Array.vim"/>
				<Item Name="Equal Functor.lvclass" Type="LVClass" URL="/&lt;vilib&gt;/Comparison/Equal/Equal Functor/Equal Functor.lvclass"/>
				<Item Name="Equal Comparable.lvclass" Type="LVClass" URL="/&lt;vilib&gt;/Comparison/Equal/Equal Comparable/Equal Comparable.lvclass"/>
				<Item Name="Search Unsorted 1D Array Core.vim" Type="VI" URL="/&lt;vilib&gt;/Array/Helpers/Search Unsorted 1D Array Core.vim"/>
				<Item Name="Equals.vim" Type="VI" URL="/&lt;vilib&gt;/Comparison/Equals.vim"/>
				<Item Name="NI_AALPro.lvlib" Type="Library" URL="/&lt;vilib&gt;/Analysis/NI_AALPro.lvlib"/>
				<Item Name="NI_Gmath.lvlib" Type="Library" URL="/&lt;vilib&gt;/gmath/NI_Gmath.lvlib"/>
				<Item Name="Test Case.lvclass" Type="LVClass" URL="/&lt;vilib&gt;/Astemes/LUnit/Test Case.lvclass"/>
				<Item Name="Clear Errors.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Clear Errors.vi"/>
				<Item Name="1D String Array to Delimited String.vi" Type="VI" URL="/&lt;vilib&gt;/AdvancedString/1D String Array to Delimited String.vi"/>
				<Item Name="Get LV Class Path.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/LVClass/Get LV Class Path.vi"/>
				<Item Name="LV3DPointTypeDef.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/miscctls.llb/LV3DPointTypeDef.ctl"/>
				<Item Name="LVRGBAColorTypeDef.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/miscctls.llb/LVRGBAColorTypeDef.ctl"/>
				<Item Name="NI_3D Picture Control.lvlib" Type="Library" URL="/&lt;vilib&gt;/picture/3D Picture Control/NI_3D Picture Control.lvlib"/>
				<Item Name="Get File Extension.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/libraryn.llb/Get File Extension.vi"/>
				<Item Name="Read JPEG File.vi" Type="VI" URL="/&lt;vilib&gt;/picture/jpeg.llb/Read JPEG File.vi"/>
				<Item Name="Check Path.vi" Type="VI" URL="/&lt;vilib&gt;/picture/jpeg.llb/Check Path.vi"/>
				<Item Name="Directory of Top Level VI.vi" Type="VI" URL="/&lt;vilib&gt;/picture/jpeg.llb/Directory of Top Level VI.vi"/>
				<Item Name="Read PNG File.vi" Type="VI" URL="/&lt;vilib&gt;/picture/png.llb/Read PNG File.vi"/>
				<Item Name="Create Mask By Alpha.vi" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/Create Mask By Alpha.vi"/>
				<Item Name="Bit-array To Byte-array.vi" Type="VI" URL="/&lt;vilib&gt;/picture/pictutil.llb/Bit-array To Byte-array.vi"/>
				<Item Name="Read BMP File.vi" Type="VI" URL="/&lt;vilib&gt;/picture/bmp.llb/Read BMP File.vi"/>
				<Item Name="Read BMP File Data.vi" Type="VI" URL="/&lt;vilib&gt;/picture/bmp.llb/Read BMP File Data.vi"/>
				<Item Name="Read BMP Header Info.vi" Type="VI" URL="/&lt;vilib&gt;/picture/bmp.llb/Read BMP Header Info.vi"/>
				<Item Name="Calc Long Word Padded Width.vi" Type="VI" URL="/&lt;vilib&gt;/picture/bmp.llb/Calc Long Word Padded Width.vi"/>
				<Item Name="Flip and Pad for Picture Control.vi" Type="VI" URL="/&lt;vilib&gt;/picture/bmp.llb/Flip and Pad for Picture Control.vi"/>
				<Item Name="LVBoundsTypeDef.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/miscctls.llb/LVBoundsTypeDef.ctl"/>
				<Item Name="Create NI GUID.vi" Type="VI" URL="/&lt;vilib&gt;/string/Create NI GUID.vi"/>
				<Item Name="Enum to Array of Enums.vim" Type="VI" URL="/&lt;vilib&gt;/numeric/Enum to Array of Enums.vim"/>
				<Item Name="Color to RGB.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/colorconv.llb/Color to RGB.vi"/>
				<Item Name="Create Mask.vi" Type="VI" URL="/&lt;vilib&gt;/picture/pictutil.llb/Create Mask.vi"/>
				<Item Name="Application Directory.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Application Directory.vi"/>
				<Item Name="NI_FileType.lvlib" Type="Library" URL="/&lt;vilib&gt;/Utility/lvfile.llb/NI_FileType.lvlib"/>
				<Item Name="Recursive File List.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/libraryn.llb/Recursive File List.vi"/>
				<Item Name="List Directory and LLBs.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/libraryn.llb/List Directory and LLBs.vi"/>
				<Item Name="Get LV Class Default Value.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/LVClass/Get LV Class Default Value.vi"/>
				<Item Name="Edit LVLibs.lvlib" Type="Library" URL="/&lt;vilib&gt;/Utility/EditLVLibs/Edit LVLibs.lvlib"/>
				<Item Name="Simple Error Handler.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Simple Error Handler.vi"/>
				<Item Name="DialogType.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/DialogType.ctl"/>
				<Item Name="General Error Handler.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/General Error Handler.vi"/>
				<Item Name="DialogTypeEnum.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/DialogTypeEnum.ctl"/>
				<Item Name="General Error Handler Core CORE.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/General Error Handler Core CORE.vi"/>
				<Item Name="whitespace.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/whitespace.ctl"/>
				<Item Name="Check Special Tags.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Check Special Tags.vi"/>
				<Item Name="TagReturnType.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/TagReturnType.ctl"/>
				<Item Name="Set String Value.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Set String Value.vi"/>
				<Item Name="GetRTHostConnectedProp.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/GetRTHostConnectedProp.vi"/>
				<Item Name="Error Code Database.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Error Code Database.vi"/>
				<Item Name="Trim Whitespace.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Trim Whitespace.vi"/>
				<Item Name="Format Message String.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Format Message String.vi"/>
				<Item Name="Find Tag.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Find Tag.vi"/>
				<Item Name="Search and Replace Pattern.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Search and Replace Pattern.vi"/>
				<Item Name="Set Bold Text.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Set Bold Text.vi"/>
				<Item Name="Details Display Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Details Display Dialog.vi"/>
				<Item Name="ErrWarn.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/ErrWarn.ctl"/>
				<Item Name="eventvkey.ctl" Type="VI" URL="/&lt;vilib&gt;/event_ctls.llb/eventvkey.ctl"/>
				<Item Name="Not Found Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Not Found Dialog.vi"/>
				<Item Name="Three Button Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Three Button Dialog.vi"/>
				<Item Name="Three Button Dialog CORE.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Three Button Dialog CORE.vi"/>
				<Item Name="LVRectTypeDef.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/miscctls.llb/LVRectTypeDef.ctl"/>
				<Item Name="Longest Line Length in Pixels.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Longest Line Length in Pixels.vi"/>
				<Item Name="Convert property node font to graphics font.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Convert property node font to graphics font.vi"/>
				<Item Name="Get Text Rect.vi" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/Get Text Rect.vi"/>
				<Item Name="Get String Text Bounds.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Get String Text Bounds.vi"/>
				<Item Name="BuildHelpPath.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/BuildHelpPath.vi"/>
				<Item Name="GetHelpDir.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/GetHelpDir.vi"/>
				<Item Name="LVMapReplaceAction.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/miscctls.llb/LVMapReplaceAction.ctl"/>
				<Item Name="TRef TravTarget.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/traverseref.llb/TRef TravTarget.ctl"/>
				<Item Name="Stall Data Flow.vim" Type="VI" URL="/&lt;vilib&gt;/Utility/Stall Data Flow.vim"/>
				<Item Name="VI Scripting - Traverse.lvlib" Type="Library" URL="/&lt;vilib&gt;/Utility/traverseref.llb/VI Scripting - Traverse.lvlib"/>
				<Item Name="TRef Traverse.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/traverseref.llb/TRef Traverse.vi"/>
				<Item Name="LVSceneTextAlignment.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/miscctls.llb/LVSceneTextAlignment.ctl"/>
				<Item Name="New Zip File.vi" Type="VI" URL="/&lt;vilib&gt;/zip/New Zip File.vi"/>
				<Item Name="Path To Command Line String.vi" Type="VI" URL="/&lt;vilib&gt;/AdvancedString/Path To Command Line String.vi"/>
				<Item Name="PathToUNIXPathString.vi" Type="VI" URL="/&lt;vilib&gt;/Platform/CFURL.llb/PathToUNIXPathString.vi"/>
				<Item Name="Compare Two Paths.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/libraryn.llb/Compare Two Paths.vi"/>
				<Item Name="Add File to Zip.vi" Type="VI" URL="/&lt;vilib&gt;/zip/Add File to Zip.vi"/>
				<Item Name="Relative Path To Platform Independent String.vi" Type="VI" URL="/&lt;vilib&gt;/AdvancedString/Relative Path To Platform Independent String.vi"/>
				<Item Name="Close Zip File.vi" Type="VI" URL="/&lt;vilib&gt;/zip/Close Zip File.vi"/>
				<Item Name="Parameterized Test Case.lvclass" Type="LVClass" URL="/&lt;vilib&gt;/Astemes/LUnit Parameterized/Parameterized Test Case/Parameterized Test Case.lvclass"/>
				<Item Name="Create Directory Recursive.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/libraryn.llb/Create Directory Recursive.vi"/>
				<Item Name="Generate Temporary File Path.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/libraryn.llb/Generate Temporary File Path.vi"/>
				<Item Name="LVPoint32TypeDef.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/miscctls.llb/LVPoint32TypeDef.ctl"/>
				<Item Name="ex_CorrectErrorChain.vi" Type="VI" URL="/&lt;vilib&gt;/express/express shared/ex_CorrectErrorChain.vi"/>
				<Item Name="subFile Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/express/express input/FileDialogBlock.llb/subFile Dialog.vi"/>
				<Item Name="System Directory Type.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/sysdir.llb/System Directory Type.ctl"/>
				<Item Name="Get System Directory.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/sysdir.llb/Get System Directory.vi"/>
				<Item Name="sizeaction.ctl" Type="VI" URL="/&lt;vilib&gt;/event_ctls.llb/sizeaction.ctl"/>
				<Item Name="Remove Duplicates From 1D Array.vim" Type="VI" URL="/&lt;vilib&gt;/Array/Remove Duplicates From 1D Array.vim"/>
				<Item Name="Find First Error.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Find First Error.vi"/>
				<Item Name="Close File+.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Close File+.vi"/>
				<Item Name="compatReadText.vi" Type="VI" URL="/&lt;vilib&gt;/_oldvers/_oldvers.llb/compatReadText.vi"/>
				<Item Name="Read File+ (string).vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Read File+ (string).vi"/>
				<Item Name="Open File+.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Open File+.vi"/>
				<Item Name="Read Lines From File (with error IO).vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Read Lines From File (with error IO).vi"/>
				<Item Name="Read Delimited Spreadsheet (string).vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Read Delimited Spreadsheet (string).vi"/>
				<Item Name="Read Delimited Spreadsheet (I64).vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Read Delimited Spreadsheet (I64).vi"/>
				<Item Name="Read Delimited Spreadsheet (DBL).vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Read Delimited Spreadsheet (DBL).vi"/>
				<Item Name="Read Delimited Spreadsheet.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Read Delimited Spreadsheet.vi"/>
				<Item Name="NI_Data Type.lvlib" Type="Library" URL="/&lt;vilib&gt;/Utility/Data Type/NI_Data Type.lvlib"/>
				<Item Name="Get LV Class Name.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/LVClass/Get LV Class Name.vi"/>
				<Item Name="Test Suite.lvclass" Type="LVClass" URL="/&lt;vilib&gt;/Astemes/LUnit/Test Suite.lvclass"/>
				<Item Name="LUnit Runnable.lvclass" Type="LVClass" URL="/&lt;vilib&gt;/Astemes/LUnit/LUnit Runnable.lvclass"/>
				<Item Name="Space Constant.vi" Type="VI" URL="/&lt;vilib&gt;/dlg_ctls.llb/Space Constant.vi"/>
				<Item Name="Parameterized Test Suite.lvclass" Type="LVClass" URL="/&lt;vilib&gt;/Astemes/LUnit Parameterized/Parameterized Test Suite/Parameterized Test Suite.lvclass"/>
				<Item Name="Delimited String to 1D String Array.vi" Type="VI" URL="/&lt;vilib&gt;/AdvancedString/Delimited String to 1D String Array.vi"/>
				<Item Name="Parameter.lvclass" Type="LVClass" URL="/&lt;vilib&gt;/Astemes/LUnit Parameterized/Parameter/Parameter.lvclass"/>
				<Item Name="NI_Unzip.lvlib" Type="Library" URL="/&lt;vilib&gt;/zip/NI_Unzip.lvlib"/>
				<Item Name="System Exec.vi" Type="VI" URL="/&lt;vilib&gt;/Platform/system.llb/System Exec.vi"/>
				<Item Name="Check if File or Folder Exists.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/libraryn.llb/Check if File or Folder Exists.vi"/>
				<Item Name="NI_PackedLibraryUtility.lvlib" Type="Library" URL="/&lt;vilib&gt;/Utility/LVLibp/NI_PackedLibraryUtility.lvlib"/>
				<Item Name="LabVIEWHTTPClient.lvlib" Type="Library" URL="/&lt;vilib&gt;/httpClient/LabVIEWHTTPClient.lvlib"/>
				<Item Name="NI_Message Digest API.lvlib" Type="Library" URL="/&lt;vilib&gt;/security/Message Digest/API/NI_Message Digest API.lvlib"/>
				<Item Name="NI_SHA-256.lvclass" Type="LVClass" URL="/&lt;vilib&gt;/security/Message Digest/SHA-2/SHA-256/NI_SHA-256.lvclass"/>
				<Item Name="NI_Bit Manipulation.lvlib" Type="Library" URL="/&lt;vilib&gt;/Bit Manipulation/NI_Bit Manipulation.lvlib"/>
				<Item Name="NI_SHA-512.lvclass" Type="LVClass" URL="/&lt;vilib&gt;/security/Message Digest/SHA-2/SHA-512/NI_SHA-512.lvclass"/>
				<Item Name="NI_SHA-3.lvclass" Type="LVClass" URL="/&lt;vilib&gt;/security/Message Digest/SHA-3/NI_SHA-3.lvclass"/>
				<Item Name="NI_Keccak.lvlib" Type="Library" URL="/&lt;vilib&gt;/security/Message Digest/Keccak/NI_Keccak.lvlib"/>
				<Item Name="Open URL in Default Browser core.vi" Type="VI" URL="/&lt;vilib&gt;/Platform/browser.llb/Open URL in Default Browser core.vi"/>
				<Item Name="Escape Characters for HTTP.vi" Type="VI" URL="/&lt;vilib&gt;/printing/PathToURL.llb/Escape Characters for HTTP.vi"/>
				<Item Name="Path to URL inner.vi" Type="VI" URL="/&lt;vilib&gt;/printing/PathToURL.llb/Path to URL inner.vi"/>
				<Item Name="Path to URL.vi" Type="VI" URL="/&lt;vilib&gt;/printing/PathToURL.llb/Path to URL.vi"/>
				<Item Name="Open URL in Default Browser (path).vi" Type="VI" URL="/&lt;vilib&gt;/Platform/browser.llb/Open URL in Default Browser (path).vi"/>
				<Item Name="Open URL in Default Browser (string).vi" Type="VI" URL="/&lt;vilib&gt;/Platform/browser.llb/Open URL in Default Browser (string).vi"/>
				<Item Name="Open URL in Default Browser.vi" Type="VI" URL="/&lt;vilib&gt;/Platform/browser.llb/Open URL in Default Browser.vi"/>
				<Item Name="VIPM API_vipm_api.lvlib" Type="Library" URL="/&lt;vilib&gt;/JKI/VIPM API/VIPM API_vipm_api.lvlib"/>
			</Item>
			<Item Name="lvanlys.dll" Type="Document" URL="/&lt;resource&gt;/lvanlys.dll"/>
			<Item Name="gartoolkit_fd_64-1.so" Type="Document" URL="../common/bin/gartoolkit_fd_64-1.so"/>
			<Item Name="gartoolkit_64-1.so" Type="Document" URL="../common/bin/gartoolkit_64-1.so"/>
			<Item Name="NextCapture.vim" Type="VI" URL="../capture/Stream/NextCapture.vim"/>
		</Item>
		<Item Name="Build Specifications" Type="Build">
			<Item Name="Installer Source Distribution" Type="Source Distribution">
				<Property Name="Bld_autoIncrement" Type="Bool">true</Property>
				<Property Name="Bld_buildCacheID" Type="Str">{80C320E8-24C6-4243-99FF-15221D3F3FBA}</Property>
				<Property Name="Bld_buildSpecName" Type="Str">Installer Source Distribution</Property>
				<Property Name="Bld_excludedDirectory[0]" Type="Path">vi.lib</Property>
				<Property Name="Bld_excludedDirectory[0].pathType" Type="Str">relativeToAppDir</Property>
				<Property Name="Bld_excludedDirectory[1]" Type="Path">instr.lib</Property>
				<Property Name="Bld_excludedDirectory[1].pathType" Type="Str">relativeToAppDir</Property>
				<Property Name="Bld_excludedDirectory[2]" Type="Path">user.lib</Property>
				<Property Name="Bld_excludedDirectory[2].pathType" Type="Str">relativeToAppDir</Property>
				<Property Name="Bld_excludedDirectory[3]" Type="Path">resource/objmgr</Property>
				<Property Name="Bld_excludedDirectory[3].pathType" Type="Str">relativeToAppDir</Property>
				<Property Name="Bld_excludedDirectory[4]" Type="Path">/C/ProgramData/National Instruments/InstCache/20.0</Property>
				<Property Name="Bld_excludedDirectory[5]" Type="Path">/C/Users/john/Documents/LabVIEW Data/2020(32-bit)/ExtraVILib</Property>
				<Property Name="Bld_excludedDirectoryCount" Type="Int">6</Property>
				<Property Name="Bld_localDestDir" Type="Path">../build/installer-source-distribution</Property>
				<Property Name="Bld_localDestDirType" Type="Str">relativeToCommon</Property>
				<Property Name="Bld_previewCacheID" Type="Str">{E2A0ADA6-962A-4E89-87A1-2C05F03CA305}</Property>
				<Property Name="Bld_version.build" Type="Int">19</Property>
				<Property Name="Bld_version.major" Type="Int">1</Property>
				<Property Name="Destination[0].destName" Type="Str">Destination Directory</Property>
				<Property Name="Destination[0].path" Type="Path">../build/installer-source-distribution</Property>
				<Property Name="Destination[1].destName" Type="Str">Support Directory</Property>
				<Property Name="Destination[1].path" Type="Path">../build/installer-source-distribution/data</Property>
				<Property Name="Destination[2].destName" Type="Str">common.bin</Property>
				<Property Name="Destination[2].path" Type="Path">../build/installer-source-distribution/common/bin</Property>
				<Property Name="DestinationCount" Type="Int">3</Property>
				<Property Name="Source[0].itemID" Type="Str">{654DF4CE-1D38-4C0D-8822-D4A4D8826739}</Property>
				<Property Name="Source[0].type" Type="Str">Container</Property>
				<Property Name="Source[1].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[1].itemID" Type="Ref">/My Computer/binary_installation_tools.lvlib</Property>
				<Property Name="Source[1].Library.allowMissingMembers" Type="Bool">true</Property>
				<Property Name="Source[1].newName" Type="Str">g_ar_toolkit.bit.lvlib</Property>
				<Property Name="Source[1].type" Type="Str">Library</Property>
				<Property Name="Source[2].destinationIndex" Type="Int">2</Property>
				<Property Name="Source[2].itemID" Type="Ref">/My Computer/bin/COPYING</Property>
				<Property Name="Source[2].sourceInclusion" Type="Str">Include</Property>
				<Property Name="Source[3].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[3].itemID" Type="Ref">/My Computer/VIPM/Toolkit Binaries Pre-Uninstall Custom Action.vi</Property>
				<Property Name="Source[3].newName" Type="Str">__Toolkit Binaries Pre-Uninstall Custom Action__.vi</Property>
				<Property Name="Source[3].sourceInclusion" Type="Str">Include</Property>
				<Property Name="Source[3].type" Type="Str">VI</Property>
				<Property Name="Source[4].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[4].itemID" Type="Ref">/My Computer/VIPM/Toolkit Binaries Post-Install Custom Action.vi</Property>
				<Property Name="Source[4].newName" Type="Str">__Toolkit Binaries Post-Install Custom Action__.vi</Property>
				<Property Name="Source[4].sourceInclusion" Type="Str">Include</Property>
				<Property Name="Source[4].type" Type="Str">VI</Property>
				<Property Name="Source[5].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[5].itemID" Type="Ref">/My Computer/VIPM/Toolkit Post-Install Custom Action.vi</Property>
				<Property Name="Source[5].type" Type="Str">VI</Property>
				<Property Name="SourceCount" Type="Int">6</Property>
			</Item>
			<Item Name="Source Distribution" Type="Source Distribution">
				<Property Name="Bld_buildCacheID" Type="Str">{2D2DC497-4EC7-44A6-9CE9-936A5A75204D}</Property>
				<Property Name="Bld_buildSpecName" Type="Str">Source Distribution</Property>
				<Property Name="Bld_excludedDirectory[0]" Type="Path">vi.lib</Property>
				<Property Name="Bld_excludedDirectory[0].pathType" Type="Str">relativeToAppDir</Property>
				<Property Name="Bld_excludedDirectory[1]" Type="Path">resource/objmgr</Property>
				<Property Name="Bld_excludedDirectory[1].pathType" Type="Str">relativeToAppDir</Property>
				<Property Name="Bld_excludedDirectory[2]" Type="Path">/C/ProgramData/National Instruments/InstCache/20.0</Property>
				<Property Name="Bld_excludedDirectory[3]" Type="Path">/C/Users/john/Documents/LabVIEW Data/2020(32-bit)/ExtraVILib</Property>
				<Property Name="Bld_excludedDirectory[4]" Type="Path">instr.lib</Property>
				<Property Name="Bld_excludedDirectory[4].pathType" Type="Str">relativeToAppDir</Property>
				<Property Name="Bld_excludedDirectory[5]" Type="Path">user.lib</Property>
				<Property Name="Bld_excludedDirectory[5].pathType" Type="Str">relativeToAppDir</Property>
				<Property Name="Bld_excludedDirectoryCount" Type="Int">6</Property>
				<Property Name="Bld_excludeDependentDLLs" Type="Bool">true</Property>
				<Property Name="Bld_excludeDependentPPLs" Type="Bool">true</Property>
				<Property Name="Bld_localDestDir" Type="Path">../build/source-distribution</Property>
				<Property Name="Bld_localDestDirType" Type="Str">relativeToCommon</Property>
				<Property Name="Bld_previewCacheID" Type="Str">{335EEF3B-14B7-4A77-85E2-594933016307}</Property>
				<Property Name="Bld_removeVIObj" Type="Int">1</Property>
				<Property Name="Bld_version.major" Type="Int">1</Property>
				<Property Name="Bld_version.minor" Type="Int">1</Property>
				<Property Name="Destination[0].destName" Type="Str">Destination Directory</Property>
				<Property Name="Destination[0].path" Type="Path">../build/source-distribution</Property>
				<Property Name="Destination[0].preserveHierarchy" Type="Bool">true</Property>
				<Property Name="Destination[1].destName" Type="Str">Support Directory</Property>
				<Property Name="Destination[1].path" Type="Path">../build/source-distribution/data</Property>
				<Property Name="Destination[10].destName" Type="Str">examples</Property>
				<Property Name="Destination[10].path" Type="Path">../build/source-distribution/g_ar_toolkit.examples</Property>
				<Property Name="Destination[11].destName" Type="Str">video_io</Property>
				<Property Name="Destination[11].path" Type="Path">../build/source-distribution/g_ar_toolkit.video_io</Property>
				<Property Name="Destination[11].preserveHierarchy" Type="Bool">true</Property>
				<Property Name="Destination[12].destName" Type="Str">binary-installation-tools</Property>
				<Property Name="Destination[12].path" Type="Path">../build/source-distribution/g_ar_toolkit.binary-installation-tools</Property>
				<Property Name="Destination[12].preserveHierarchy" Type="Bool">true</Property>
				<Property Name="Destination[2].destName" Type="Str">common</Property>
				<Property Name="Destination[2].path" Type="Path">../build/source-distribution/g_ar_toolkit.common</Property>
				<Property Name="Destination[2].preserveHierarchy" Type="Bool">true</Property>
				<Property Name="Destination[3].destName" Type="Str">image</Property>
				<Property Name="Destination[3].path" Type="Path">../build/source-distribution/g_ar_toolkit.image</Property>
				<Property Name="Destination[3].preserveHierarchy" Type="Bool">true</Property>
				<Property Name="Destination[4].destName" Type="Str">ar</Property>
				<Property Name="Destination[4].path" Type="Path">../build/source-distribution/g_ar_toolkit.ar</Property>
				<Property Name="Destination[4].preserveHierarchy" Type="Bool">true</Property>
				<Property Name="Destination[5].destName" Type="Str">feature_detection</Property>
				<Property Name="Destination[5].path" Type="Path">../build/source-distribution/g_ar_toolkit.feature_detection</Property>
				<Property Name="Destination[5].preserveHierarchy" Type="Bool">true</Property>
				<Property Name="Destination[6].destName" Type="Str">capture</Property>
				<Property Name="Destination[6].path" Type="Path">../build/source-distribution/g_ar_toolkit.capture</Property>
				<Property Name="Destination[6].preserveHierarchy" Type="Bool">true</Property>
				<Property Name="Destination[7].destName" Type="Str">shape</Property>
				<Property Name="Destination[7].path" Type="Path">../build/source-distribution/g_ar_toolkit.shape</Property>
				<Property Name="Destination[7].preserveHierarchy" Type="Bool">true</Property>
				<Property Name="Destination[8].destName" Type="Str">bin</Property>
				<Property Name="Destination[8].path" Type="Path">../build/source-distribution/g_ar_toolkit.common/bin</Property>
				<Property Name="Destination[9].destName" Type="Str">root</Property>
				<Property Name="Destination[9].path" Type="Path">../build/source-distribution</Property>
				<Property Name="DestinationCount" Type="Int">13</Property>
				<Property Name="Source[0].itemID" Type="Str">{24A02B5B-4FFB-4987-A109-7CE4D12013D5}</Property>
				<Property Name="Source[0].type" Type="Str">Container</Property>
				<Property Name="Source[1].Container.applyDestination" Type="Bool">true</Property>
				<Property Name="Source[1].Container.applyInclusion" Type="Bool">true</Property>
				<Property Name="Source[1].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[1].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[1].itemID" Type="Ref">/My Computer/test-images</Property>
				<Property Name="Source[1].sourceInclusion" Type="Str">Include</Property>
				<Property Name="Source[1].type" Type="Str">Container</Property>
				<Property Name="Source[10].Container.applyDestination" Type="Bool">true</Property>
				<Property Name="Source[10].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[10].destinationIndex" Type="Int">2</Property>
				<Property Name="Source[10].itemID" Type="Ref">/My Computer/common.lvlib/Types</Property>
				<Property Name="Source[10].type" Type="Str">Container</Property>
				<Property Name="Source[11].destinationIndex" Type="Int">2</Property>
				<Property Name="Source[11].itemID" Type="Ref">/My Computer/common.lvlib/Utility/NI-Linux-RT Shared Library Path (Core).vi</Property>
				<Property Name="Source[11].type" Type="Str">VI</Property>
				<Property Name="Source[12].Container.applyDestination" Type="Bool">true</Property>
				<Property Name="Source[12].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[12].destinationIndex" Type="Int">3</Property>
				<Property Name="Source[12].itemID" Type="Ref">/My Computer/image.lvlib/Private</Property>
				<Property Name="Source[12].type" Type="Str">Container</Property>
				<Property Name="Source[13].Container.applyDestination" Type="Bool">true</Property>
				<Property Name="Source[13].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[13].destinationIndex" Type="Int">3</Property>
				<Property Name="Source[13].itemID" Type="Ref">/My Computer/image.lvlib/Polymorphic Implementations</Property>
				<Property Name="Source[13].type" Type="Str">Container</Property>
				<Property Name="Source[14].Container.applyDestination" Type="Bool">true</Property>
				<Property Name="Source[14].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[14].destinationIndex" Type="Int">3</Property>
				<Property Name="Source[14].itemID" Type="Ref">/My Computer/image.lvlib/API</Property>
				<Property Name="Source[14].type" Type="Str">Container</Property>
				<Property Name="Source[15].Container.applyDestination" Type="Bool">true</Property>
				<Property Name="Source[15].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[15].destinationIndex" Type="Int">3</Property>
				<Property Name="Source[15].itemID" Type="Ref">/My Computer/image.lvlib/Mapped Memory Access Image</Property>
				<Property Name="Source[15].type" Type="Str">Container</Property>
				<Property Name="Source[16].destinationIndex" Type="Int">6</Property>
				<Property Name="Source[16].itemID" Type="Ref">/My Computer/capture.lvlib/API/Enumerate Capture Devices.vi</Property>
				<Property Name="Source[16].type" Type="Str">VI</Property>
				<Property Name="Source[17].destinationIndex" Type="Int">6</Property>
				<Property Name="Source[17].itemID" Type="Ref">/My Computer/capture.lvlib/API/Stream.lvclass</Property>
				<Property Name="Source[17].type" Type="Str">Library</Property>
				<Property Name="Source[18].Container.applyDestination" Type="Bool">true</Property>
				<Property Name="Source[18].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[18].destinationIndex" Type="Int">5</Property>
				<Property Name="Source[18].itemID" Type="Ref">/My Computer/feature_detection.lvlib/Private</Property>
				<Property Name="Source[18].type" Type="Str">Container</Property>
				<Property Name="Source[19].Container.applyDestination" Type="Bool">true</Property>
				<Property Name="Source[19].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[19].destinationIndex" Type="Int">5</Property>
				<Property Name="Source[19].itemID" Type="Ref">/My Computer/feature_detection.lvlib/Polymorphic Implementations</Property>
				<Property Name="Source[19].type" Type="Str">Container</Property>
				<Property Name="Source[2].Container.applyDestination" Type="Bool">true</Property>
				<Property Name="Source[2].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[2].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[2].itemID" Type="Ref">/My Computer/models</Property>
				<Property Name="Source[2].type" Type="Str">Container</Property>
				<Property Name="Source[20].Container.applyDestination" Type="Bool">true</Property>
				<Property Name="Source[20].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[20].destinationIndex" Type="Int">5</Property>
				<Property Name="Source[20].itemID" Type="Ref">/My Computer/feature_detection.lvlib/Types</Property>
				<Property Name="Source[20].type" Type="Str">Container</Property>
				<Property Name="Source[21].destinationIndex" Type="Int">5</Property>
				<Property Name="Source[21].itemID" Type="Ref">/My Computer/feature_detection.lvlib/API/AprilTag Detector.lvclass</Property>
				<Property Name="Source[21].type" Type="Str">Library</Property>
				<Property Name="Source[22].destinationIndex" Type="Int">5</Property>
				<Property Name="Source[22].itemID" Type="Ref">/My Computer/feature_detection.lvlib/API/Barcode Detector.lvclass</Property>
				<Property Name="Source[22].type" Type="Str">Library</Property>
				<Property Name="Source[23].destinationIndex" Type="Int">5</Property>
				<Property Name="Source[23].itemID" Type="Ref">/My Computer/ar.lvlib/API/Find Chessboard Corners.vi</Property>
				<Property Name="Source[23].type" Type="Str">VI</Property>
				<Property Name="Source[24].destinationIndex" Type="Int">5</Property>
				<Property Name="Source[24].itemID" Type="Ref">/My Computer/feature_detection.lvlib/API/AprilTag Tag ID List.vi</Property>
				<Property Name="Source[24].type" Type="Str">VI</Property>
				<Property Name="Source[25].Container.applyDestination" Type="Bool">true</Property>
				<Property Name="Source[25].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[25].destinationIndex" Type="Int">4</Property>
				<Property Name="Source[25].itemID" Type="Ref">/My Computer/ar.lvlib/Private</Property>
				<Property Name="Source[25].type" Type="Str">Container</Property>
				<Property Name="Source[26].Container.applyDestination" Type="Bool">true</Property>
				<Property Name="Source[26].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[26].destinationIndex" Type="Int">4</Property>
				<Property Name="Source[26].itemID" Type="Ref">/My Computer/ar.lvlib/Types</Property>
				<Property Name="Source[26].type" Type="Str">Container</Property>
				<Property Name="Source[27].Container.applyDestination" Type="Bool">true</Property>
				<Property Name="Source[27].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[27].destinationIndex" Type="Int">4</Property>
				<Property Name="Source[27].itemID" Type="Ref">/My Computer/ar.lvlib/Polymorphic Implementations</Property>
				<Property Name="Source[27].type" Type="Str">Container</Property>
				<Property Name="Source[28].Container.applyDestination" Type="Bool">true</Property>
				<Property Name="Source[28].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[28].destinationIndex" Type="Int">4</Property>
				<Property Name="Source[28].itemID" Type="Ref">/My Computer/ar.lvlib/API</Property>
				<Property Name="Source[28].type" Type="Str">Container</Property>
				<Property Name="Source[29].Container.applyDestination" Type="Bool">true</Property>
				<Property Name="Source[29].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[29].destinationIndex" Type="Int">5</Property>
				<Property Name="Source[29].itemID" Type="Ref">/My Computer/feature_detection.lvlib/API</Property>
				<Property Name="Source[29].type" Type="Str">Container</Property>
				<Property Name="Source[3].destinationIndex" Type="Int">2</Property>
				<Property Name="Source[3].itemID" Type="Ref">/My Computer/common.lvlib</Property>
				<Property Name="Source[3].Library.allowMissingMembers" Type="Bool">true</Property>
				<Property Name="Source[3].newName" Type="Str">g_ar_toolkit.common.lvlib</Property>
				<Property Name="Source[3].sourceInclusion" Type="Str">Include</Property>
				<Property Name="Source[3].type" Type="Str">Library</Property>
				<Property Name="Source[30].Container.applyDestination" Type="Bool">true</Property>
				<Property Name="Source[30].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[30].destinationIndex" Type="Int">7</Property>
				<Property Name="Source[30].itemID" Type="Ref">/My Computer/shape.lvlib/API</Property>
				<Property Name="Source[30].type" Type="Str">Container</Property>
				<Property Name="Source[31].Container.applyDestination" Type="Bool">true</Property>
				<Property Name="Source[31].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[31].destinationIndex" Type="Int">7</Property>
				<Property Name="Source[31].itemID" Type="Ref">/My Computer/shape.lvlib/Polymorphic Implementations</Property>
				<Property Name="Source[31].type" Type="Str">Container</Property>
				<Property Name="Source[32].Container.applyDestination" Type="Bool">true</Property>
				<Property Name="Source[32].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[32].destinationIndex" Type="Int">6</Property>
				<Property Name="Source[32].itemID" Type="Ref">/My Computer/capture.lvlib/API</Property>
				<Property Name="Source[32].type" Type="Str">Container</Property>
				<Property Name="Source[33].destinationIndex" Type="Int">9</Property>
				<Property Name="Source[33].itemID" Type="Ref">/My Computer/build-tools/g-ar-toolkit.lvproj.xml</Property>
				<Property Name="Source[33].newName" Type="Str">g-ar-toolkit.lvproj</Property>
				<Property Name="Source[33].sourceInclusion" Type="Str">Include</Property>
				<Property Name="Source[34].Container.applyDestination" Type="Bool">true</Property>
				<Property Name="Source[34].Container.applyInclusion" Type="Bool">true</Property>
				<Property Name="Source[34].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[34].destinationIndex" Type="Int">8</Property>
				<Property Name="Source[34].itemID" Type="Ref">/My Computer/bin</Property>
				<Property Name="Source[34].newName" Type="Str">lib</Property>
				<Property Name="Source[34].sourceInclusion" Type="Str">Include</Property>
				<Property Name="Source[34].type" Type="Str">Container</Property>
				<Property Name="Source[35].Container.applyInclusion" Type="Bool">true</Property>
				<Property Name="Source[35].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[35].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[35].itemID" Type="Ref">/My Computer/VIPM</Property>
				<Property Name="Source[35].sourceInclusion" Type="Str">Include</Property>
				<Property Name="Source[35].type" Type="Str">Container</Property>
				<Property Name="Source[36].destinationIndex" Type="Int">10</Property>
				<Property Name="Source[36].itemID" Type="Ref">/My Computer/examples.lvlib</Property>
				<Property Name="Source[36].Library.allowMissingMembers" Type="Bool">true</Property>
				<Property Name="Source[36].newName" Type="Str">g_ar_toolkit.examples.lvlib</Property>
				<Property Name="Source[36].sourceInclusion" Type="Str">Include</Property>
				<Property Name="Source[36].type" Type="Str">Library</Property>
				<Property Name="Source[37].Container.applyInclusion" Type="Bool">true</Property>
				<Property Name="Source[37].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[37].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[37].itemID" Type="Ref">/My Computer/models/spot</Property>
				<Property Name="Source[37].sourceInclusion" Type="Str">Include</Property>
				<Property Name="Source[37].type" Type="Str">Container</Property>
				<Property Name="Source[38].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[38].itemID" Type="Ref">/My Computer/models/models.txt</Property>
				<Property Name="Source[38].sourceInclusion" Type="Str">Include</Property>
				<Property Name="Source[39].destinationIndex" Type="Int">9</Property>
				<Property Name="Source[39].itemID" Type="Ref">/My Computer/LICENSE</Property>
				<Property Name="Source[39].sourceInclusion" Type="Str">Include</Property>
				<Property Name="Source[4].destinationIndex" Type="Int">3</Property>
				<Property Name="Source[4].itemID" Type="Ref">/My Computer/image.lvlib</Property>
				<Property Name="Source[4].Library.allowMissingMembers" Type="Bool">true</Property>
				<Property Name="Source[4].newName" Type="Str">g_ar_toolkit.image.lvlib</Property>
				<Property Name="Source[4].sourceInclusion" Type="Str">Include</Property>
				<Property Name="Source[4].type" Type="Str">Library</Property>
				<Property Name="Source[40].destinationIndex" Type="Int">11</Property>
				<Property Name="Source[40].itemID" Type="Ref">/My Computer/video_io.lvlib</Property>
				<Property Name="Source[40].Library.allowMissingMembers" Type="Bool">true</Property>
				<Property Name="Source[40].newName" Type="Str">g_ar_toolkit.video_io.lvlib</Property>
				<Property Name="Source[40].sourceInclusion" Type="Str">Include</Property>
				<Property Name="Source[40].type" Type="Str">Library</Property>
				<Property Name="Source[41].destinationIndex" Type="Int">11</Property>
				<Property Name="Source[41].itemID" Type="Ref">/My Computer/video_io.lvlib/API/Reader.lvclass</Property>
				<Property Name="Source[41].type" Type="Str">Library</Property>
				<Property Name="Source[42].Container.applyDestination" Type="Bool">true</Property>
				<Property Name="Source[42].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[42].destinationIndex" Type="Int">11</Property>
				<Property Name="Source[42].itemID" Type="Ref">/My Computer/video_io.lvlib/API</Property>
				<Property Name="Source[42].type" Type="Str">Container</Property>
				<Property Name="Source[43].Container.applyDestination" Type="Bool">true</Property>
				<Property Name="Source[43].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[43].destinationIndex" Type="Int">2</Property>
				<Property Name="Source[43].itemID" Type="Ref">/My Computer/common.lvlib/Private</Property>
				<Property Name="Source[43].type" Type="Str">Container</Property>
				<Property Name="Source[44].Container.applyDestination" Type="Bool">true</Property>
				<Property Name="Source[44].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[44].destinationIndex" Type="Int">2</Property>
				<Property Name="Source[44].itemID" Type="Ref">/My Computer/common.lvlib/Utility</Property>
				<Property Name="Source[44].type" Type="Str">Container</Property>
				<Property Name="Source[45].Container.applyDestination" Type="Bool">true</Property>
				<Property Name="Source[45].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[45].destinationIndex" Type="Int">11</Property>
				<Property Name="Source[45].itemID" Type="Ref">/My Computer/video_io.lvlib/Private</Property>
				<Property Name="Source[45].type" Type="Str">Container</Property>
				<Property Name="Source[46].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[46].itemID" Type="Ref">/My Computer/Download and Install Shared Binaries.vi</Property>
				<Property Name="Source[46].sourceInclusion" Type="Str">Include</Property>
				<Property Name="Source[46].type" Type="Str">VI</Property>
				<Property Name="Source[47].destinationIndex" Type="Int">12</Property>
				<Property Name="Source[47].itemID" Type="Ref">/My Computer/binary_installation_tools.lvlib</Property>
				<Property Name="Source[47].Library.allowMissingMembers" Type="Bool">true</Property>
				<Property Name="Source[47].newName" Type="Str">g_ar_toolkit.binary_installation_tools.lvlib</Property>
				<Property Name="Source[47].sourceInclusion" Type="Str">Include</Property>
				<Property Name="Source[47].type" Type="Str">Library</Property>
				<Property Name="Source[48].Container.applyDestination" Type="Bool">true</Property>
				<Property Name="Source[48].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[48].destinationIndex" Type="Int">12</Property>
				<Property Name="Source[48].itemID" Type="Ref">/My Computer/binary_installation_tools.lvlib/Private</Property>
				<Property Name="Source[48].type" Type="Str">Container</Property>
				<Property Name="Source[49].Container.applyDestination" Type="Bool">true</Property>
				<Property Name="Source[49].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[49].destinationIndex" Type="Int">12</Property>
				<Property Name="Source[49].itemID" Type="Ref">/My Computer/binary_installation_tools.lvlib/API</Property>
				<Property Name="Source[49].type" Type="Str">Container</Property>
				<Property Name="Source[5].destinationIndex" Type="Int">6</Property>
				<Property Name="Source[5].itemID" Type="Ref">/My Computer/capture.lvlib</Property>
				<Property Name="Source[5].Library.allowMissingMembers" Type="Bool">true</Property>
				<Property Name="Source[5].newName" Type="Str">g_ar_toolkit.capture.lvlib</Property>
				<Property Name="Source[5].sourceInclusion" Type="Str">Include</Property>
				<Property Name="Source[5].type" Type="Str">Library</Property>
				<Property Name="Source[6].destinationIndex" Type="Int">5</Property>
				<Property Name="Source[6].itemID" Type="Ref">/My Computer/feature_detection.lvlib</Property>
				<Property Name="Source[6].Library.allowMissingMembers" Type="Bool">true</Property>
				<Property Name="Source[6].newName" Type="Str">g_ar_toolkit.feature_detection.lvlib</Property>
				<Property Name="Source[6].sourceInclusion" Type="Str">Include</Property>
				<Property Name="Source[6].type" Type="Str">Library</Property>
				<Property Name="Source[7].destinationIndex" Type="Int">4</Property>
				<Property Name="Source[7].itemID" Type="Ref">/My Computer/ar.lvlib</Property>
				<Property Name="Source[7].Library.allowMissingMembers" Type="Bool">true</Property>
				<Property Name="Source[7].newName" Type="Str">g_ar_toolkit.ar.lvlib</Property>
				<Property Name="Source[7].sourceInclusion" Type="Str">Include</Property>
				<Property Name="Source[7].type" Type="Str">Library</Property>
				<Property Name="Source[8].destinationIndex" Type="Int">7</Property>
				<Property Name="Source[8].itemID" Type="Ref">/My Computer/shape.lvlib</Property>
				<Property Name="Source[8].Library.allowMissingMembers" Type="Bool">true</Property>
				<Property Name="Source[8].newName" Type="Str">g_ar_toolkit.shape.lvlib</Property>
				<Property Name="Source[8].sourceInclusion" Type="Str">Include</Property>
				<Property Name="Source[8].type" Type="Str">Library</Property>
				<Property Name="Source[9].Container.applyDestination" Type="Bool">true</Property>
				<Property Name="Source[9].Container.depDestIndex" Type="Int">0</Property>
				<Property Name="Source[9].destinationIndex" Type="Int">2</Property>
				<Property Name="Source[9].itemID" Type="Ref">/My Computer/common.lvlib/Mathematical</Property>
				<Property Name="Source[9].type" Type="Str">Container</Property>
				<Property Name="SourceCount" Type="Int">50</Property>
			</Item>
		</Item>
	</Item>
</Project>
