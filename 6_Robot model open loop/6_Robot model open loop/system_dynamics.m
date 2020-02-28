function [sys,x0,str,ts] = system_dynamics(t,x,u,flag)
switch flag,
  case 0
    [sys,x0,str,ts]=mdlInitializeSizes; 
  case 1
    sys = mdlDerivatives(t,x,u); 
  case 3
    sys = mdlOutputs(t,x,u); 
  case { 2, 4, 9 } 
    sys = [];
  otherwise
    error(['Unhandled flag = ',num2str(flag)]); 
end

function [sys,x0,str,ts] = mdlInitializeSizes
sizes = simsizes;
sizes.NumContStates  = 8;
sizes.NumDiscStates  = 0;
sizes.NumOutputs     = 8;
sizes.NumInputs      = 2;
sizes.DirFeedthrough = 0;     
sizes.NumSampleTimes = 1;
sys = simsizes(sizes);

x0 = [-2.7237 -4.7138 0.68*pi/180 -2 0.5 -40*pi/180 0 0]';

str = [];
ts = [0 0];

function sys = mdlDerivatives(t,x,u)
g = 9.81;

mw = 33.8e-3;           
r = 32e-3;       
d = 205e-3; 
fw1 = 0.54e-3;
fw2 = fw1;     

mb = 802.2e-3;
l = 20.7e-3;
JB = 0.002518;      
JA = 0.001350;

k = 43.8;
JM = 2.74e-6;
kM = 5.44e-3;
kE = 5.44e-3;
R = 4.01;
L = 2.222e-5;
fM1 = 7.27e-6; 
fM2 = fM1;     

sys(1) = x(4);                      
sys(2) = x(5);                      
sys(3) = x(6);                      

m11 = (3/2)*mw*r^2 + (1/4)*mb*r^2 + k^2*JM + (1/d^2)*l^2*r^2*mb*sin(x(3))*sin(x(3)) + JB*r^2/d^2;
m12 = (1/4)*mb*r^2 - (1/d^2)*l^2*r^2*mb*sin(x(3))*sin(x(3)) - JB*r^2/d^2;
m13 = (1/2)*mb*l*r*cos(x(3)) - k^2*JM;
m21 = m12;
m22 = m11;
m23 = m13;
m31 = m13;
m32 = m13;
m33 = mb*l^2 + JA + 2*k^2*JM;

v1 = 2*(1/d^2)*l^2*r^2*mb*sin(x(3))*cos(x(3))*x(6)*(x(4)-x(5)) - (1/2)*mb*l*r*sin(x(3))*x(6)*x(6);
v2 = 2*(1/d^2)*l^2*r^2*mb*sin(x(3))*cos(x(3))*x(6)*(x(5)-x(4)) - (1/2)*mb*l*r*sin(x(3))*x(6)*x(6);
v3 =  -(1/d^2)*l^2*r^2*mb*sin(x(3))*cos(x(3))*(x(4)-x(5))^2 - mb*g*l*sin(x(3));

M = [m11 m12 m13; m21 m22 m23; m31 m32 m33];
V = [v1; v2; v3];

dtheta_g1 = x(4)-x(6);
dtheta_g2 = x(5)-x(6);

tau1 = k*kM*x(7) - fM1*dtheta_g1 - fw1*x(4);
tau2 = k*kM*x(8) - fM2*dtheta_g2 - fw2*x(5);
tau3 = -(k*kM*x(7) + k*kM*x(8) - fM1*dtheta_g1 - fM2*dtheta_g2);

tau = [tau1; tau2; tau3];

dxx = M\(tau - V);

sys(4) = dxx(1);
sys(5) = dxx(2);
sys(6) = dxx(3);

sys(7) = (1/L)*u(1) + (1/L)*kE*k*x(6) - (1/L)*kE*k*x(4) - (1/L)*R*x(7);
sys(8) = (1/L)*u(2) + (1/L)*kE*k*x(6) - (1/L)*kE*k*x(5) - (1/L)*R*x(8);

function sys = mdlOutputs(t,x,u)
r = 32e-3;       
d = 205e-3; 
c1 = [r/2 r/2 0 0 0 0 0 0];
c2 = [0 0 0 r/2 r/2 0 0 0];
c3 = [0 0 1 0 0 0 0 0];
c4 = [0 0 0 0 0 1 0 0];
c5 = [-r/d r/d 0 0 0 0 0 0];
c6 = [0 0 0 -r/d r/d 0 0 0];
c7 = [0 0 0 0 0 0 1 0];
c8 = [0 0 0 0 0 0 0 1];
C = [c1;c2;c3;c4;c5;c6;c7;c8];
sys = C*x;