% used to run the project and load .csv file autumatically
% currently, this script is only for 1 resource 1 request at a time version

%these are parameters used to config the newwork
clc;                       %clear all
COL=8
ROW=8
NUM_NODES=64  
NUM_DIRECTIONS=5
CHANNEL_FANOUTS=20
TOTAL_NUM_CHANNELS=20
RESOURCE_CHANNEL_NUM=8
CHANNEL_PER_DIRECTION=4
NUM_SUB_NETWORKS =2
%  system('rm -rf ./inp_test_files')
%  for lifeTime=200:200:400
%    	for injection_rate=1:10
%    		transaction_gen(COL,ROW,lifeTime,5000000,injection_rate);
%    	end
%  end

%BELOW is parameters for generatin test files
vector=0:1:1;

for CHANNEL_PER_DIRECTION=2.^vector
%  for CHANNEL_PER_DIRECTION=1:1
CHANNEL_FANOUTS=5*CHANNEL_PER_DIRECTION;
TOTAL_NUM_CHANNELS=5*CHANNEL_PER_DIRECTION;
RESOURCE_CHANNEL_NUM=CHANNEL_PER_DIRECTION*NUM_SUB_NETWORKS;

display('current channel per direction is')
display(CHANNEL_PER_DIRECTION);
perl('alter_configuration.pl',int2str(COL),int2str(ROW),int2str(NUM_NODES),int2str(NUM_DIRECTIONS),int2str(CHANNEL_FANOUTS),int2str(TOTAL_NUM_CHANNELS),int2str(RESOURCE_CHANNEL_NUM),int2str(CHANNEL_PER_DIRECTION),int2str(NUM_SUB_NETWORKS));
cd ../build;
system('make');
cd ../test_bench;
for lifeTime=200:200:400
	result_path=strcat('results_subnetwork/','sub_',int2str(NUM_SUB_NETWORKS),'/',int2str(COL),int2str(ROW),'/','ch',int2str(CHANNEL_PER_DIRECTION),'/');
	result_file=strcat(result_path,int2str(lifeTime));
	
	mkdir(result_path);

	if exist(strcat(result_file,'.csv'),'file')~=0
	delete (strcat(result_file,'.csv'));
	end
		for injection_rate=1:10
			%transaction_gen(COL,ROW,lifeTime,5000,injection_rate);
			filename = sprintf('./ppb_cpp ./inp_test_files/%d/%d %d > temp ' , lifeTime,injection_rate, lifeTime);
			display (filename);
			system(filename)
			startTime=5000000*injection_rate/10/10/lifeTime;
			endTime=5000000*injection_rate/10/10*9/lifeTime;
			perl('/nobackup/liu2/ppb_cpp/ppb_cpp/test_bench/Data_processor_1.2.pl',result_file,int2str(startTime),int2str(endTime));
			clear filename;
		end
	csv=csvread(strcat(result_file,'.csv'));
	x=0:0.1:1;
	k=zeros(1,size(csv,2));
	csv=[k;csv];
	
	figure_name=strcat(result_file,'.fig');
	%exportfig(gcf,figure_name,'psc2');
	scrsz = get(0,'ScreenSize');
 	figure1=figure('Position',[1 scrsz(4)/4 scrsz(3)/4 scrsz(4)/4])
 
	plot(x,csv(:,1));
	print ('-deps', '-tiff','-r300', result_file);	
	saveas(gcf,figure_name,'m');
	out_csv_file=strcat(result_file,'_output.csv');
	temp=[x' csv(:,1:end)];
	table_tilte=[cellstr('offered_load'),cellstr('avDelay'),cellstr('avSetupDelay'),cellstr('avWaitingDelay'),cellstr('maxDelay'),cellstr('maxSetupDelay'),cellstr('maxWaitingDelay'),cellstr('avRetryTimes'),cellstr('totalsetups'),cellstr('totaltime')];
	table=[table_tilte;num2cell(temp)];
	cell2csv(out_csv_file,table);
	end
end


%  for offered_load=0.1:0.1:1
%  
%  transaction_gen(16,16,200,100000,2);
%  
%  result = csvread('1.csv'); 
