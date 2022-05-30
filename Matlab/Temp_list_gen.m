
clc
addpath('include/')



% Temp_list = [20:2:120 118:-2:20] + 273.15;
% Temp_list = [25:25:100 75:-25:25] + 273.15;

% disp(Temp_list' - 273.15)

Deg_range = 120;
Deg_step = 1.5;
Exposition_time = 5;
Run_time = (Deg_range/Deg_step * Exposition_time)/60 * 1.10 %min


Temp_list_start = [60:5:75 80:2.5:92.5];
Temp_list_end = [92.5:-2.5:80 75:-5:60];

Tmin = 95;
Tmax = 126;
Tstep = 1;
Temp_list_mid = [Tmin:Tstep:Tmax Tmax-Tstep:-Tstep:Tmin];

Temp_list_All = [Temp_list_start Temp_list_mid Temp_list_end];

plot(Temp_list_All, '-x');

Temp_list_full = zeros(1,2*numel(Temp_list_All));
Temp_list_full(1:2:end-1) = Temp_list_All;
Temp_list_full(2:2:end) = Temp_list_All;
Temp_list_full_K = Temp_list_full + 273.15;

Number_of_runs = numel(Temp_list_full)

Exp_full_time = Run_time*Number_of_runs;


disp(Temp_list_full')
disp(['Number of runs: ' num2str(Number_of_runs/2) '*2=' num2str(Number_of_runs)])
disp(['Run time: ' num2str(Run_time) ' min'])
disp(['Full time: ' num2str(Exp_full_time/60) ' hours'])



%%

Output_file_adr = '../LabView Side/T_list_all_temps.txt';
create_file(Temp_list_All, Output_file_adr);



%% -------------------
function create_file(Temp_list, Output_file_adr)
if size(Temp_list, 1) ~= 1
    error('Wrong Temp list array dimentions');
end

[Max, ind] = max(Temp_list - 273.15);
if Max > 120
    Msg = ['Temp exceed 120C limit: T = ' num2str(Max), ' at i =  ' num2str(ind)];
    error(Msg);
end

fid = fopen(Output_file_adr, 'w');
N = numel(Temp_list);
for i = 1:N
    Line = num2str(Temp_list(i), '%6.2f');
    fwrite(fid, Line);
    if i ~= N
        fwrite(fid, newline);
    end
end
fclose(fid);
disp(['file created: ' num2str(N) ' T points'])
end









