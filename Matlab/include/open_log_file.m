
function [Temp, Time, Voltage, Counter, Flag] = open_log_file(Log_file_adr)
[Date, Time, ...
 TempK, Temp_SPK, Temp_TGK, ...
 VinV, VoutV, ...
 Heating, Ramping, ...
 FrameN, RunN, ...
 Trig, TrigTimes] = importfile(Log_file_adr);

Tmp = char(ones(numel(Date), 1));
Tmp(:) = ' ';
Datetime_str = [char(Date) Tmp char(Time)];
date_time = datetime(Datetime_str, 'InputFormat', 'dd.MM.yyyy HH:mm:ss');
clearvars Date Time Tmp Datetime_str

Time.date_time = date_time;
Base_time = date_time(1);
Time.seconds = seconds(date_time - Base_time);
clearvars date_time Base_time

Flag.heating = Heating;
Flag.ramping = Ramping;
Flag.trig = Trig;
clearvars Heating Ramping Trig

Counter.frame = FrameN;
Counter.run = RunN;
Counter.trig_time = TrigTimes;
clearvars FrameN RunN TrigTimes

Voltage.vin = VinV;
Voltage.vout = VoutV;
clearvars VinV VoutV

Temp.actual = TempK;
Temp.setpoint = Temp_SPK;
Temp.target = Temp_TGK;
clearvars TempK Temp_SPK Temp_TGK

clearvars Log_file_adr
end


function [Date, Time, TempK, Temp_SPK, Temp_TGK, VinV, VoutV, Heating, Ramping, FrameN, RunN, Trig, TrigTimes] = importfile(filename, dataLines)
dataLines = [2, Inf];

opts = delimitedTextImportOptions("NumVariables", 14);
opts.DataLines = dataLines;
opts.Delimiter = "\t";

opts.VariableNames = ["Var1", "Date", "Time", "TempK", "Temp_SPK", "Temp_TGK", "VinV", "VoutV", "Heating", "Ramping", "FrameN", "RunN", "Trig", "TrigTimes"];
opts.SelectedVariableNames = ["Date", "Time", "TempK", "Temp_SPK", "Temp_TGK", "VinV", "VoutV", "Heating", "Ramping", "FrameN", "RunN", "Trig", "TrigTimes"];
opts.VariableTypes = ["string", "string", "string", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double", "double"];

opts.ExtraColumnsRule = "ignore";
opts.EmptyLineRule = "read";

opts = setvaropts(opts, ["Var1", "Date", "Time"], "WhitespaceRule", "preserve");
opts = setvaropts(opts, ["Var1", "Date", "Time"], "EmptyFieldRule", "auto");

tbl = readtable(filename, opts);

Date = tbl.Date;
Time = tbl.Time;
TempK = tbl.TempK;
Temp_SPK = tbl.Temp_SPK;
Temp_TGK = tbl.Temp_TGK;
VinV = tbl.VinV;
VoutV = tbl.VoutV;
Heating = tbl.Heating;
Ramping = tbl.Ramping;
FrameN = tbl.FrameN;
RunN = tbl.RunN;
Trig = tbl.Trig;
TrigTimes = tbl.TrigTimes;
end