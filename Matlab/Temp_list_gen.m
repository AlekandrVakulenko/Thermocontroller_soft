
clc
addpath('include/')

Output_file_adr = '../LabView Side/input_T_list_2.txt';

% Temp_list = [20:2:120 118:-2:20] + 273.15;
Temp_list = [25:25:100 75:-25:25] + 273.15;

disp(Temp_list' - 273.15)

create_file(Temp_list, Output_file_adr);








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









