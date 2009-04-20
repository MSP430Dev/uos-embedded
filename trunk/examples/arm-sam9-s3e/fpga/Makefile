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
#  		  -iuc NO
#		  -lso sram_to_arm.lso
#		  -keep_hierarchy NO
#		  -rtlview Yes
#		  -read_cores YES
#		  -write_timing_constraints NO
#		  -cross_clock_analysis NO
#		  -hierarchy_separator /
#		  -bus_delimiter <>
#		  -case maintain
#		  -slice_utilization_ratio 100
#		  -verilog2001 YES
#		  -fsm_extract YES -fsm_encoding Auto
#		  -safe_implementation No
#		  -fsm_style lut
#		  -ram_extract Yes
#		  -ram_style Auto
#		  -rom_extract Yes
#		  -mux_style Auto
#		  -decoder_extract YES
#		  -priority_extract YES
#		  -shreg_extract YES
#		  -shift_extract YES
#		  -xor_collapse YES
#		  -rom_style Auto
#		  -mux_extract YES
#		  -resource_sharing YES
#		  -mult_style auto
#		  -iobuf YES
#		  -max_fanout 500
#		  -bufg 8
#		  -register_duplication YES
#		  -register_balancing No
#		  -slice_packing YES
#		  -optimize_primitives NO
#		  -use_clock_enable Yes
#		  -use_sync_set Yes
#		  -use_sync_reset Yes
#		  -iob auto
#		  -equivalent_register_removal YES
#		  -slice_utilization_ratio_maxmargin 5

BGFLAGS		= -g DebugBitstream:No \
		  -g Binary:no \
		  -g CRC:Enable \
		  -g ConfigRate:1 \
		  -g CclkPin:PullUp \
		  -g M0Pin:PullUp \
		  -g M1Pin:PullUp \
		  -g M2Pin:PullUp \
		  -g ProgPin:PullUp \
		  -g DonePin:PullUp \
		  -g TckPin:PullUp \
		  -g TdiPin:PullUp \
		  -g TdoPin:PullUp \
		  -g TmsPin:PullUp \
		  -g UnusedPin:PullDown \
		  -g UserID:0xFFFFFFFF \
		  -g DCMShutdown:Disable \
		  -g DCIUpdateMode:AsRequired \
		  -g StartUpClk:CClk \
		  -g DONE_cycle:4 \
		  -g GTS_cycle:5 \
		  -g GWE_cycle:6 \
		  -g LCK_cycle:NoWait \
		  -g Security:None \
		  -m \
		  -g ReadBack \
		  -g DonePipe:No \
		  -g DriveDone:Yes

PROMGENFLAGS	= -p mcs -w -c FF -s 512 -spi

#
# Source files
#
SRCV		= sram_to_arm.v
SRCVHD		=
PINS		= sram_to_arm.ucf

all:		firmware.bit firmware.mcs

load:
		@[ -f firmware.mcs ] || $(MAKE) firmware.mcs
		$(IMPACT) -batch loadfpga.impact

prog:
		@[ -f firmware.mcs ] || $(MAKE) firmware.mcs
		$(IMPACT) -batch progflash.impact
		@mv -f _impactbatch.log 8-download.log

clean:
		rm -f *~ _impact.* _impactbatch.log

cleanall:	clean
		rm -rf build firmware.bit firmware.mcs *.log

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
