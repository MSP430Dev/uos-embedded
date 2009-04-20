@echo off
rem Run iMPACT to program the Xilinx device.
impact -batch progflash.impact
if exist _impact.cmd del _impact.cmd
