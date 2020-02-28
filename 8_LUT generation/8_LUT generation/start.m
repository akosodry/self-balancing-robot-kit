clear all
clc

n1 = 50;
n2 = 50;

th3 = linspace(-15,15,n1);
om3 = linspace(-180,180,n2);

sys = readfis('FLC2.fis');

for i = 1:length(th3)
    for j = 1:length(om3)
        OUT(j, i) = evalfis([th3(i); om3(j)], sys);
    end
end

figure
h = surf(th3, om3, OUT);
xlim([-15 15]);
ylim([-180 180]);

colormap(jet)
shading interp
%%
[r1min, r2min, res1, res2, lut] = generateLUT('FLC2.fis', n1, n2);

figure
h2 = surf(th3, om3, lut');
xlim([-15 15]);
ylim([-180 180]);
zlim([-300 300]);

colormap(jet)

%%
for i = 1:length(th3)
    for j = 1:length(om3)
        OUT2(j, i) = FLCapprox(th3(i), om3(j), r1min, r2min, res1, res2, lut, n1, n2);
    end
end

figure
h3 = surf(th3, om3, OUT-OUT2);

xlim([-15 15]);
ylim([-180 180]);

colormap(jet)


