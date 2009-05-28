@echo off
:::: Script for running Xilinx synthesizer, by Serge Vakulenko <serge@vak.ru>.
set     FPGA=xc3s500e-pq208-4
set     PINS=sram_to_arm.ucf
set XSTFLAGS=-top sram_to_arm -opt_mode Speed -opt_level 1 -glob_opt allclocknets
set  BGFLAGS=-g TdoPin:PullUp -g DriveDone:Yes -g CRC:enable -g StartUpClk:CCLK
set PRMFLAGS=-p mcs -w -c FF -s 512 -spi

if not exist build md build

:::: Create input file for XST.
echo verilog work ../sram_to_arm.v    > build/project.src

:::: Create project file for XST.
echo run -p %FPGA%     > build/project.xst
echo -ifn project.src >> build/project.xst
echo -ifmt mixed      >> build/project.xst
echo -ofn project.ngc >> build/project.xst
echo -ofmt NGC        >> build/project.xst
echo %XSTFLAGS%       >> build/project.xst

:::: Delete old log files.
if exist *.log del *.log

:::: Run synthesizer.
cd build
xst -ifn project.xst -ofn project.log -intstyle silent
cd ..
if errorlevel 1 exit
copy build\project.log 1-compile.log > NUL

:::: Run NGD builder.
cd build
ngdbuild -p %FPGA% project.ngc -uc ../%PINS%
cd ..
if errorlevel 1 exit
copy build\project.bld 2-build.log > NUL

:::: Run mapper.
cd build
map -pr b -p %FPGA% project
cd ..
if errorlevel 1 exit
copy build\project.mrp 3-map.log > NUL

:::: Run router.
cd build
par -w project project_r.ncd
cd ..
if errorlevel 1 exit
copy build\project_r.par 4-route.log > NUL

:::: Run tracer.
cd build
trce -v 25 project_r.ncd project.pcf
cd ..
if errorlevel 1 exit
copy build\project_r.twr 5-trace.log > NUL

:::: Generate firmware file.
cd build
bitgen project_r.ncd -l -w %BGFLAGS%
cd ..
if errorlevel 1 exit
copy build\project_r.bit firmware.bit > NUL
copy build\project_r.bgn 6-bitgen.log > NUL

:::: Generate PROM file.
promgen %PRMFLAGS% -u 0 firmware.bit
if exist 7-promgen.log del 7-promgen.log
ren firmware.prm 7-promgen.log
