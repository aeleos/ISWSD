clc;
clear workspace;

data = readmatrix(uigetfile('*_D.csv'),'RANGE',[2 1]);

points = data(:,1);
pressure1 = data(:,2);
temperature1 = data(:,3);
pressure2 = data(:,4);
temperature2 = data(:,5);
height1 = 44330 * (1-(pressure1/1013.25).^.1903);
height1 = height1 - height1(1);
height2 = 44330 * (1-(pressure2/1013.25).^.1903);
height2 = height2 -height2(2);
pressure1 = pressure1 - pressure1(1);
pressure2 = pressure2 - pressure2(1);

fig = figure;
tiledlayout(4,1);

p = nexttile; hold on;
plot(p,points,pressure1,'r','LineWidth',2);
plot(p,points,pressure2,'b--','LineWidth',2);
title('Pressure');
ylabel('hPa');

h = nexttile; hold on;
plot(h,points,height1,'r','LineWidth',2);
plot(h,points,height2,'b--','LineWidth',2);
title('Altitude');
ylabel('m');

t = nexttile; hold on;
plot(points,temperature1,'r','LineWidth',2);
plot(points,temperature2,'b--','LineWidth',2);
title('Temperature');
ylabel('C');
