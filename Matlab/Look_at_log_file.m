
clc
addpath('include/')

Log_file_adr = '../LabView Side/Null_Log.txt';
% Log_file_adr = 'F:/_OneDrive/YandexDisk/YandexDisk/Компьютер MAINHOMEPC/OneDrive - Personal/_Diffraction/LOG_60-125.txt';

[Temp, Time, Voltage, Counter, Flag] = open_log_file(Log_file_adr);
clearvars Log_file_adr

Time_step = 0.100; %s (manual input time step)
% time = Time.seconds;
time = [1:numel(Time.seconds)]*Time_step;
Time.time = time';


figure; hold on
plot(time, Temp.actual, '-r', 'linewidth', 2);
plot(time, Temp.setpoint, '--b', 'linewidth', 1);
plot(time, Temp.target, '--g', 'linewidth', 1);
title('Temperature')
xlabel('time, s')
ylabel('Temp, K')
% legend({'actual', 'setpoint', 'target'})
% smartlim


% figure
% subplot(3, 1, 1) %Temp
% plot(time, Temp.actual, '-r', 'linewidth', 1);
% title('Temperature')
% xlabel('time, s')
% ylabel('Temp, K')
% subplot(3, 1, 2) %Power
% plot(time, Voltage.vout, '-r', 'linewidth', 1);
% title('Power')
% xlabel('time, s')
% ylabel('Vout, V')
% subplot(3, 1, 3) %Run
% plot(time, Counter.run, '-r', 'linewidth', 1);
% title('Run counter')
% xlabel('time, s')
% ylabel('Run N')


%%






























