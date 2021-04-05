clc;
clear workspace;

data = readmatrix(uigetfile('*_D.csv'),'RANGE',[2 1]);

points = data(:,1);
pressure1 = data(:,2);
temperature1 = data(:,3);
pressure2 = data(:,4);
temperature2 = data(:,5);
height1 = 44330 * (1-(pressure1/1013.25).^.1903);
height2 = 44330 * (1-(pressure2/1013.25).^.1903);

START_POINT = 1;
END_POINT = 121;

pressure1 = pressure1(START_POINT:END_POINT);
pressure2 = pressure2(START_POINT:END_POINT);
height1 = height1(START_POINT:END_POINT);
height2 = height2(START_POINT:END_POINT);
temperature1 = temperature1(START_POINT:END_POINT);
temperature2 = temperature2(START_POINT:END_POINT);
points = points(START_POINT:END_POINT);

height1 = height1 - height1(1);
height2 = height2 -height2(1);
pressure1 = pressure1 - pressure1(1);
pressure2 = pressure2 - pressure2(1);

difference = abs(height1-height2)*100;

fig = figure;
tiledlayout(3,1);

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
plot(points,difference,'g','LineWidth',2);
title('Height difference');
ylabel('cm');
xlabel('time (s)');

fprintf('Max drift difference: %f cm\n',max(difference));
fprintf('Mean drift difference: %f cm\n',mean(difference));
fprintf('Median drift difference: %f cm\n',median(difference));
