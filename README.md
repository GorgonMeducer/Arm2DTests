# README

To build on Mac

```shell
mkdir build.tmp
cd build.tmp
cmake -DHOST=YES \
-DCMSISDSP=/ArmSoftware/CMSIS-DSP \
-DARM2D=/ArmSoftware/Arm-2D ..
make
./arm2d_demo 
```

To build for Pico Rp2040:

```shell
mkdir build.tmp
cd build.tmp
cmake -DARM2D=/ArmSoftware/Arm-2D \
 -DCMSISCORE=/ArmSoftware/CMSIS_5/CMSIS/Core \
 -DCMSISDSP=/ArmSoftware/CMSIS-DSP \
 -DARM2D_RP2040=/ArmSoftware/Arm2D_RP2040/RP2040 \
 -DHOST=NO \
 ..
make
```

