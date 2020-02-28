# self-balancing-robot-kit
The supplementary online material for the article: FUZZY CONTROL OF SELF‐BALANCING ROBOTS: A CONTROL LABORATORY PROJECT

**Cite this work as:**

Odry Á, Fullér R, Rudas IJ, Odry P. **Fuzzy control of self‐balancing robots: A control laboratory project**. Comput Appl Eng Educ. 2020;1–24. https://doi.org/10.1002/cae.22219

A **short video demonstration** of the closed loop behavior is available **here**:
https://youtu.be/y2i62vh2vE4


The paper systematically describes the construction, development and fuzzy control of the proposed lab setup for teaching control system design in laboratories. All the information, including the STL files for 3D printing, computer-aided design (CAD) models, MATLAB/Simulink files, and MCU software have been made publicly available here to help other lab teams in designing similar experiments.


The description of the files is summarized as follows.

**1_3D printer files** This file contains the STL files that can be used in 3D printing software such as Ultimaker Cura.

**2_Solidworks models** 	This file contains the CAD model of the lab setup. It was created by a mechatronic engineering student at the University of Dunaújváros.

**3_Minimal working environment**	This file contains the Arduino code that initializes the MCU peripherals of the lab setup, drives the motors in both directions, and reads and sends the instantaneous IMU (acceleration and angular rate) and encoder (motor shaft position) measurements to the PC.

**4_MATLAB script to record data**	This file contains the MATLAB code used to collect the measurement results via the serial port. The file also contains the recorded measurement results.

**5_DC motor model**	This file contains both the Simulink model of the DC motor and the Simulink Design Optimization session file.

**6_Robot model open loop**	This file contains the S-function-based Simulink model of the robot.

**7_Robot model closed loop**	This file contains the Simulink model of the fuzzy control strategy.

**8_LUT generation**	This file contains the MATLAB functions that generate the lookup-table approximation of the fuzzy logic controllers.

**9_Arduino closed loop**	This file contains the Arduino code that applies the fuzzy control strategy to stabilize the robot.
