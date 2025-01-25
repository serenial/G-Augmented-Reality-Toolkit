<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="20008000">
	<Property Name="NI.LV.All.SaveVersion" Type="Str">20.0</Property>
	<Property Name="NI.LV.All.SourceOnly" Type="Bool">true</Property>
	<Item Name="My Computer" Type="My Computer">
		<Property Name="server.app.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.control.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.tcp.enabled" Type="Bool">false</Property>
		<Property Name="server.tcp.port" Type="Int">0</Property>
		<Property Name="server.tcp.serviceName" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.tcp.serviceName.default" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.vi.callsEnabled" Type="Bool">true</Property>
		<Property Name="server.vi.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="specify.custom.address" Type="Bool">false</Property>
		<Item Name="Docs-Images" Type="Folder" URL="../Docs-Images">
			<Property Name="NI.DISK" Type="Bool">true</Property>
		</Item>
		<Item Name="IMAQ" Type="Folder">
			<Item Name="Copy From IMAQ.vi" Type="VI" URL="../IMAQ-Examples/Copy From IMAQ.vi"/>
			<Item Name="Copy To IMAQ.vi" Type="VI" URL="../IMAQ-Examples/Copy To IMAQ.vi"/>
		</Item>
		<Item Name="Python" Type="Folder">
			<Item Name="example.py" Type="Document" URL="../Python/example.py"/>
			<Item Name="Python Interoperation Example.vi" Type="VI" URL="../Python/Python Interoperation Example.vi"/>
			<Item Name="requirements.txt" Type="Document" URL="../Python/requirements.txt"/>
		</Item>
		<Item Name="Tutorial" Type="Folder" URL="../Tutorial">
			<Property Name="NI.DISK" Type="Bool">true</Property>
		</Item>
		<Item Name="Dependencies" Type="Dependencies">
			<Item Name="vi.lib" Type="Folder">
				<Item Name="Empty Picture" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/Empty Picture"/>
				<Item Name="imagedata.ctl" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/imagedata.ctl"/>
			</Item>
		</Item>
		<Item Name="Build Specifications" Type="Build"/>
	</Item>
</Project>
