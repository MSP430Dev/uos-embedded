#
# Local options and paths.
#
PATH		:= $(PATH):/opt/Xilinx/10.1/ISE/bin/lin
IMPACT		:= export LD_PRELOAD=/usr/local/lib/libusb-driver.so && impact

#
# Target
#
FPGA		= xc3s500e-pq208-4
XSTFLAGS	= -top sram_to_arm -opt_mode Speed -opt_level 1 -glob_opt allclocknets
BGFLAGS		= -g TdoPin:PullUp -g DriveDone:Yes -g CRC:enable -g StartUpClk:CCLK
PROMGENFLAGS	= -p mcs -w -c FF -s 512 -spi

#
# Source files
#
SRCV		= sram_to_arm.v
SRCVHD		=
PINS		= sam9_s3e.ucf

all:		firmware.bit firmware.mcs

load:		firmware.bit
		$(IMPACT) -batch loadfpga.impact

prog:		firmware.mcs
		$(IMPACT) -batch progflash.impact

clean:
		rm -f *~ _impact.* _impactbatch.log

cleanall:	clean
		rm -rf build *.mcs

#
# Synthesis
#
build/project.src:
		@[ -d build ] || mkdir build
		@rm -f $@
		for i in $(SRCVHD); do echo vhdl work ../$$i >> $@; done
		for i in $(SRCV); do echo verilog work ../$$i >> $@; done

build/project.xst: build/project.src
		echo "run" > $@
		echo "-p $(FPGA)" >> $@
		echo "-ifn project.src" >> $@
		echo "-ifmt mixed" >> $@
		echo "-ofn project.ngc" >> $@
		echo "-ofmt NGC" >> $@
		echo "$(XSTFLAGS)" >> $@

build/project.ngc: build/project.xst $(SRCV) $(SRCVHD)
		@rm -f *.log
		cd build && xst -ifn project.xst -ofn project.log -intstyle silent
		@cp -f build/project.log 1-compile.log

build/project.ngd: build/project.ngc $(PINS)
		cd build && ngdbuild -p $(FPGA) project.ngc -uc ../$(PINS)
		@cp -f build/project.bld 2-build.log

build/project.ncd: build/project.ngd
		cd build && map -pr b -p $(FPGA) project
		@cp -f build/project.mrp 3-map.log

build/project_r.ncd: build/project.ncd
		cd build && par -ol high -w project project_r.ncd
		@cp -f build/project_r.par 4-route.log

build/project_r.twr: build/project_r.ncd
		cd build && trce -v 25 project_r.ncd project.pcf
		@cp -f build/project_r.twr 5-trace.log

firmware.bit:	build/project_r.ncd build/project_r.twr
		cd build && bitgen project_r.ncd -l -w $(BGFLAGS)
		@mv -f build/project_r.bit $@
		@cp -f build/project_r.bgn 6-bitgen.log

firmware.mcs:	firmware.bit
		promgen $(PROMGENFLAGS) -u 0 firmware.bit
		@mv -f firmware.prm 7-promgen.log
