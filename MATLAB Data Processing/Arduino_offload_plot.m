clear all;
clc;
clear workspace;

path = '';

% Based on Matlab Script Provided for Laboratory 10 ECE 216
% Prof. Thomas Howard

% SETUP

if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end

serial_device = serialport('/dev/tty.usbmodem1434201', 'BaudRate', 230400);

fopen(serial_device);

START_TIME = now;
START_TIME_STRING = datetime(START_TIME,'ConvertFrom','datenum');
START_FILE_NAME = path + START_TIME_STRING + '.csv';

data_file = fopen(START_FILE_NAME,'wt');
fprintf(data_file,'Point,Pressure(hPa),Temp(C)');

fig = figure, hold on;
set(fig,'KeyPressFcn',@endloop);
plot(point,pressure,point,temperature);
title('Pressure and Temperature vs Time');
xlabel('data point');
ylabel('hPa, C');
legend('Pressure','Temp');

pressure = zeros(100,1);
temperature = zeros(100,1);
points = [1-100:100-100];

global loop;
loop = 1;

% LOOP

while (loop)
    
    for i=1:100
        pressure( i ) = fscanf(serial_device,'%f');
        temperature( i ) = fscanf(serial_device,'%f');
        points( i ) = loop_points( i ) + 100;
        fprintf(data_file,'%f,%f,%f\n', points(i),pressure(i),temperature(i));
    end
    
    plot(points,pressure,points,temperature);
end


% CLOSING

fclose(serial_device);
fclose(data_file);

END_TIME = now;
END_TIME_STRING = datetime(END_TIME,'ConvertFrom','datenum');
END_FILE_NAME = path + START_TIME_STRING + ' - ' + END_TIME_STRING + '.csv';


movefile(START_FILE_NAME,END_FILE_NAME);

function endloop(src,event)
    if (event.Key == 10 || event.Key == 13)
        global loop;
        loop = 0;
    end
end