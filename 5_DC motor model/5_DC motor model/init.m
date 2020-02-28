%description: todo

clear all
clc

load('meas_20ms_1XDEC_01m1');
%%
motorSpeed = motorSpeed(21:470);
pwmDuty = pwmDuty(21:470);

%%

fs = 1/ts;
measTime = [0:length(motorSpeed)-1]'*ts; %in sec

% initial motor parameters
k = 43.8;
Jm = 8e-7;
R = 5;
L = 1e-5;
kE = 1e-2;
kM = kE;
fm = 2e-5;

bs = 5;
ks = 1000;
Jg = 5e-7;
fg = 2e-4;

tau = 0.0001;
Imax = 10;
wmin = 0;

% encoder parameters
encRes = 16;

motorSpeedIn1PS = (motorSpeed/ts)*(2*pi)/(k*encRes*EncX);
motorSpeedInRPM = motorSpeedIn1PS/(2*pi)*60;
terminalV = pwmDuty*ut/255;

%open('dcmotor_model')

%% optimized motor parameters
 Jm = 2.7438e-06;
 L = 2.2223e-05;
 R = 4.0191;
 fm = 7.2702e-06;
 kM = 0.0054436;

open('dcmotor_model_opt')




