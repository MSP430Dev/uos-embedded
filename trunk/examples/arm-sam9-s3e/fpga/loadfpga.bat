@echo off
rem Run iMPACT to program the Xilinx device.
impact -batch loadfpga.impact
if exist _impact.cmd del _impact.cmd
