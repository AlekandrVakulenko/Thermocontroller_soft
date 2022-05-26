
clc
addpath('include/')

Log_file_adr = '../LabView Side/Null_Log.txt';
[Temp, Time, Voltage, Counter, Flag] = open_log_file(Log_file_adr);
clearvars Log_file_adr

Time_step = 0.1; %manual input time step
% time = Time.seconds;
time = [1:numel(Time.seconds)]*Time_step;


figure; hold on
plot(time, Temp.actual, '-r', 'linewidth', 1);
plot(time, Temp.setpoint, '--b', 'linewidth', 1);
plot(time, Temp.target, '--g', 'linewidth', 1);
title('Temperature')
xlabel('time, s')
ylabel('Temp, K')
legend({'actual', 'setpoint', 'target'})
% smartlim


figure
subplot(3, 1, 1) %Temp
plot(time, Temp.actual, '-r', 'linewidth', 1);
title('Temperature')
xlabel('time, s')
ylabel('Temp, K')
subplot(3, 1, 2) %Power
plot(time, Voltage.vout, '-r', 'linewidth', 1);
title('Power')
xlabel('time, s')
ylabel('Vout, V')
subplot(3, 1, 3) %Run
plot(time, Counter.run, '-r', 'linewidth', 1);
title('Run counter')
xlabel('time, s')
ylabel('Run N')











