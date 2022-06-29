# ArduPilot SIL Connector for Simulink

Simulink C++ S-function for software-in-the-loop simulation with ArduPilot.

[![View ardupilot-sil-connector on File Exchange](https://www.mathworks.com/matlabcentral/images/matlab-file-exchange.svg)]()

Requirements
- MATLAB & Simulink (MATLAB R2022a or earlier)
- MinGW-w64 or MSVC C/C++ Compiler
- MissionPlanner

Files

[ardupilot_sil_connector.cpp](https://github.com/aviumtechnologies/ardupilot-sil-connector/blob/master/ardupilot_sil_connector.cpp)
<div style="height:1px; background-color:rgba(0,0,0,0.12);"></div>

[make.m](https://github.com/aviumtechnologies/ardupilot-sil-connector/blob/master/make.m)
<div style="height:1px; background-color:rgba(0,0,0,0.12);"></div>

[includes.zip](https://github.com/aviumtechnologies/ardupilot-sil-connector/blob/master/includes.zip) (contains the Asio C++ library)
<div style="height:1px; background-color:rgba(0,0,0,0.12);"></div>

Build instructions

-  Install MATLAB-supported compiler  
https://mathworks.com/support/requirements/supported-compilers.html.
-  Download the "ardupilot_sil_connector.cpp" and "make.m" files and the "includes.zip" archive.
-  Unzip the "includes.zip archive".
-  Run "make.m" to create a "ardupilot_sil_connector.mexw64" (Windows), "ardupilot_sil_connector.mexa64" (Linux), "ardupilot_sil_connector.mexmaci64" (macOS) file.

Use instructions

- Download and install MissionPlanner  [https://ardupilot.org/planner/docs/mission-planner-installation.html](https://ardupilot.org/planner/docs/mission-planner-installation.html).
- Open MissionPlanner, select the "Simulation" tab and click on the "Plane" icon to download the ArduPlane SIL simulator.
- After downloading the ArduPlance SIL simulator, open a command prompt and navigate to "C:\Users\\Documents\Mission Planner\sitl".
- Execute the following command in the command prompt:  
<pre>ArduPlane.exe --home 42.841448,24.770727,0.5,0 --model json</pre>  
- Click the connect icon in Mission Planner. Use the default host and port options.
- Open and run "ardupilot_sitl_connector_example.slx".

[![Example use of the ArduPilot SIL connector](https://i.ytimg.com/vi/kRq-3Ux5j3Q/maxresdefault.jpg)](https://youtu.be/kRq-3Ux5j3Q)

<p align="center">Example use of the ArduPilot SIL connector</p>

![ArduPilot SIL connector example](https://github.com/aviumtechnologies/ardupilot-sil-connector/blob/master/ardupilot_sil_connector_example.png)

<p align="center">ArduPilot SIL connector example</p>

Additional information available at

https://fst.aviumtechnologies.com/ardupilot-sil-connector