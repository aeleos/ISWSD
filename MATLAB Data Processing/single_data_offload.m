clc;
clear workspace;

path = '';    % path to where the data files should be saved
port = '/dev/tty.usbmodem1434201'; % serial port
REFRESH = 5; % data points before updating plot


% SETUP

if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end

serial_device = serialport(port, 115200);

fopen(serial_device);

START_TIME = now;
START_TIME_STRING = datestr(datetime(START_TIME,'ConvertFrom','datenum'));
START_FILE_NAME = [path, START_TIME_STRING, '.csv'];

data_file = fopen(START_FILE_NAME,'wt');
fprintf(data_file,'Point,Pressure(hPa),Temp(C)\n');

pressure = zeros(REFRESH,1);
temperature = zeros(REFRESH,1);
points = zeros(REFRESH,1);

fig = figure;
tiledlayout(2,1);
set(fig,'KeyPressFcn',@endloop);

p = nexttile;
hold(p,'on');
plot(p,points,pressure);
title('Pressure');
ylabel('hPa');

t = nexttile;
hold(t,'on');
plot(points,temperature);
title('Temperature');
ylabel('C');

global loop;
loop = 1;
loop_count = 0;

% LOOP

fprintf(serial_device,"1");

while(loop)
    
    for i=1:REFRESH
        pressure( i ) = fscanf(serial_device,'%f');
        temperature( i ) = fscanf(serial_device,'%f');
        points( i ) = i+loop_count;
        fprintf(data_file,'%d,%f,%f\n', points(i),pressure(i),temperature(i));
    end
    loop_count = loop_count + REFRESH;
    
    plot(p,points,pressure,'r');
    plot(t,points,temperature,'b');
end


% CLOSING

fprintf(serial_device,"0");

fclose(serial_device);
fclose(data_file);

END_TIME = now;
END_TIME_STRING = datestr(datetime(END_TIME,'ConvertFrom','datenum'));
END_FILE_NAME = [path, START_TIME_STRING, ' - ', END_TIME_STRING, '.csv'];


movefile(START_FILE_NAME,END_FILE_NAME);

function endloop(~,event)
    if (event.Key == 10 || event.Key == 13)
        global loop;
        loop = 0;
    end
end