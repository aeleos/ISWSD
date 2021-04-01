clc;
clear workspace;
STEP = 1; % seconds between data measurements
PORT = '/dev/ttyUSB0'; % serial port
PATH = '';    % path to where the data files should be saved

REFRESH = 1; % data points before updating plot


% SETUP

if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end

serial_device = serial(PORT, 'BAUD', 115200);

start_time = now;
start_time_string = datestr(datetime(start_time,'ConvertFrom','datenum'));
start_file_name = strrep(strrep([PATH, start_time_string, '.csv'],' ','_'),':','-');

data_file = fopen(start_file_name,'wt');
fopen(serial_device);
fprintf(data_file,'Point,Pressure(hPa),Temp(C)\n');

pressure = zeros(1,REFRESH);
temperature = zeros(1,REFRESH);
points = zeros(1,REFRESH);

fig = figure;
tiledlayout(2,1);
set(fig,'KeyPressFcn',@endloop);

p = nexttile;
hold(p,'on');
pp=plot(p,points,pressure,'r');
title('Pressure');
ylabel('hPa');
xlim([1 inf]);

t = nexttile;
hold(t,'on');
tt=plot(t,points,temperature,'b');
title('Temperature');
ylabel('C');
xlim([1 inf]);

global loop;
loop = 1;
loop_count = 0;
pause(5);

% LOOP

fprintf(serial_device,"%d", STEP);

while(loop)
    
    for i=1:REFRESH
        pause(STEP/2);
        temperature( i )=fscanf(serial_device,'%f');
        pause(STEP/2);
        pressure( i ) = fscanf(serial_device,'%f');
        points( i ) = STEP*(i+loop_count-1);
        fprintf(data_file,'%d,%f,%f\n', points(i),pressure(i),temperature(i));
    end
    loop_count = loop_count + REFRESH;
    
    xdata = get(pp,'XData');
    pdata = get(pp,'YData');
    tdata = get(tt,'YData');
    set(pp,'XData',[xdata points]);
    set(pp, 'YData',[pdata pressure]);
    set(tt,'XData',[xdata points]);
    set(tt, 'YData',[tdata temperature]);
end


% CLOSING

fprintf(serial_device,"0");

fclose(serial_device);
delete(serial_device);
fclose(data_file);

end_time = now;
end_time_string = datestr(datetime(end_time,'ConvertFrom','datenum'));
end_file_name = strrep(strrep([PATH, start_time_string, '_', end_time_string, '_S.csv'],' ','_'),':','-');
clear workspace;
disp('Session closed.')

movefile(start_file_name,end_file_name);

function endloop(~,event)
    if (length(event.Key) == 6)
    if (event.Key == 'escape')
        global loop;
        loop = 0;
        disp('Ending logging session...');
    end
    end
end