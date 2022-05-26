
clc
addpath('include/')

Output_file_adr = '../LabView Side/input_T_list.txt';

% Temp_list = [20:2:120 118:-2:20] + 273.15;
Temp_list = [25:5:40 35:-5:25] + 273.15;


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
for i = 1:numel(Temp_list)
    Line = num2str(Temp_list(i), '%6.2f');
    fwrite(fid, Line);
    if i ~= numel(Temp_list)
        fwrite(fid, newline);
    end
end
fclose(fid);
end









