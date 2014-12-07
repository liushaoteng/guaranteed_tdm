The purpose of this project "guaranteed_TDM" is to simulate a TDM based communication network for on-chip usage.  This TDM communication network is designed by myself. It can set up TDM connections at runtime to offer guaranteed communication service.  Its hardware behaviour is described by using SystemC, which is cycle-accurate. 

This project contains three folders: src/, testbench/, build/. I use Cmake to auto-generate the Makefile of this project. The src/ contains all the c++ source files of this project. The testbench/ contains matlab and perl scripts to generate testbenches and analyzes simulation results. The build/ holds the intermediate files generated during the compilation process. 

In order to build and run this project: 

1. Install Cmake and SystemC 2.3.x
http://www.cmake.org/install/
http://www.accellera.org/downloads/standards/systemc

2. Modify the src/CMakeLists.txt, make sure the SystemC and SystemC TLM library path is set correctly.

3. Check whether the packages required by testbench/Data_processor_1.4.pl are installed

4. Go to testbench/. Use matlab to run the script auto_process_float.m. This script will automatically change the simulator settings, generate testbenches, compile the source code,  run the simulation and analyze data. The simulation results will be put into testbench/results_*/


Good luck
Shaoteng Liu





 
