<div style="text-align: center; font-size:18px;">
	<div style="display: inline-block; text-align: left; color:#1d1d1d; font-family: Tahoma;">
	<h1 align="center" style="color:#1d1d1d; font-family: Tahoma;">Advance Topic in Malware</h1>
	<h2 align="center" style="color:#1d1d1d; font-family: Tahoma;">API Hooking</h2>
	<p align="center">
		<img src="https://i.ibb.co/Q6PVL4m/774484-20690520-3650113-cb369453-image.jpg" width="250" alt="timing_attack"/>
	<h3 align="center" style="color:#1d1d1d; font-family: Tahoma;">By Yarden Curiel & Gal Azaria</h3>
	<h3 align="center" style="color:#1d1d1d; font-family: Tahoma;">Built in CPP<br></h3>
	</p>
	<p align="center">
  <a href="https://docs.microsoft.com/en-us/cpp/cpp/?view=msvc-160" target="_blank"> <img src="https://raw.githubusercontent.com/leungwensen/svg-icon/b84b3f3a3da329b7c1d02346865f8e98beb05413/dist/svg/logos/cpp.svg" alt="cpp" width="40" height="40"/> </a> 
	</p>
	<br>
	<p>
	API Hooking is a technique that we can abuse by manipulating a process data structure, IAT, to point at a custom implementation of the function that can easily cause a malicious behavior.
	<ul>
		<li>Import Address Table (IAT) - is a structure, exists in Windows PE file, which contains pointers to information that is critical for an executable as: list of necessary DLL files and a list of function names and their addresses from those DLLs</li>
	</ul>  
	<br><br>
	</p>
	<p align="center">
	In our implementation, we choose to hook API function called 'NtQuerySystemInformation' that exists in 
	'Ntdll.dll'<br> which retrieve system information. The main purpose of our hooked function is to skip on the 'CMD.exe' process.<br>
		<img src="https://i.ibb.co/fnSwvyF/Picture1.png"/>
	  <br><br>
	  API Hooking example; Visualization of API Hooking steps:<br>
		<img src="https://i.ibb.co/vcqwWxr/Image-Export-Descriptor.png"/>
	</center>
	<br><br>
	</p>
</div>
</div>