function [r1min, r2min, res1, res2, LUT] = generateLUT(file, n1, n2)

% Load FLC from file
sys = readfis(file);

% Determine the input ranges
r1min = sys.input(1,1).range(1);
r1max = sys.input(1,1).range(2);
r2min = sys.input(1,2).range(1);
r2max = sys.input(1,2).range(2);

% Resolutions between the points
res1 = (abs(r1min) + abs(r1max))/(n1-1);
res2 = (abs(r2min) + abs(r2max))/(n2-1);

% Generate the input-output vectors
in1 = r1min:res1:r1max;
in2 = r2min:res2:r2max;
LUT = NaN(length(in1),length(in2));

% Registering the output by raking through the input ranges
for i = 1:length(in1)
    for j = 1:length(in2)
        LUT(i,j) = evalfis([in1(i); in2(j)], sys);
    end
end

% Convert the float voltage to PWM duty cycle
LUT = int16(LUT * 255/12);

% Create the LUT header file
name = file(1:length(file)-4);
fid = fopen([name '.h'],'wt');

fprintf(fid, ['#define NROW_' upper(name) ' ' num2str(n1) '\n']);
fprintf(fid, ['#define NCOL_' upper(name) ' ' num2str(n2) '\n\n']);
fprintf(fid, ['float r1min_' upper(name) '=' num2str(r1min,'%2.5e') ';\n']);
fprintf(fid, ['float r2min_' upper(name) '=' num2str(r2min,'%2.5e') ';\n']);
fprintf(fid, ['float res1_'   upper(name) '=' num2str(res1,'%2.5e')   ';\n']);
fprintf(fid, ['float res2_'   upper(name) '=' num2str(res2,'%2.5e')   ';\n\n']);

fprintf(fid, ['int16_t' ' ' name '[NROW_' upper(name) '][NCOL_' upper(name) '] = { \n']);

for i = 1:n1
    fprintf(fid,'{ ');
    for j=1:n2
        fprintf(fid, [num2str(LUT(i,j),'%d') ',' '\t']);
    end
    fprintf(fid,' },\n');
end

fprintf(fid,'};\n');

fclose(fid);
end

