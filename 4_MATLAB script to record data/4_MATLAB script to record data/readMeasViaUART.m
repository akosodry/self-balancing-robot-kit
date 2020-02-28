%Description: todo
clear all
clc

s = serial('COM10', 'BaudRate', 115200);

fopen(s);

in = NaN(1000,2);

for i=1:1000
    in(i,:) = fscanf(s, '%d,%d,%d,%d');
end

fclose(s)


pwmDuty = in(:,1);
motorSpeed = in(:,2);
pwmDuty2 = in(:,3);
motorSpeed2 = in(:,4);

ut = 12.1;
EncX = 1;
ts = 20e-3;