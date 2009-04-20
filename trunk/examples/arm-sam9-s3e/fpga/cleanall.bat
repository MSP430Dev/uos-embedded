@echo off
if exist *~               del *~
if exist _impact.*        del _impact.*
if exist _impactbatch.log del _impactbatch.log
if exist build            del /s build
if exist firmware.bit     del firmware.bit
if exist firmware.mcs     del firmware.mcs
if exist *.log            del *.log
