% used to run the project and load .csv file autumatically
% currently, this script is only for 1 resource 1 request at a time version

%these are parameters used to config the newwork
%injection rate could be floatd

clc;                       %clear all
COL=8
ROW=8
NUM_NODES=64
NUM_DIRECTIONS=5
CHANNEL_FANOUTS=20;
TOTAL_NUM_CHANNELS=20;
RESOURCE_CHANNEL_NUM=4;
CHANNEL_PER_DIRECTION=4;

higwayDepth = 2;
higwayOfEachDir = 1;

nodes=COL*ROW;
%vector=0:1:0;
%vector2=2:1:2;
%vector3=0:1:0;

throughput =1;
slotDepthVector=[16]
highwayDepthVector=[1]
highwayOfEachDirVector=[1,2]
for slot_index = 1: length (slotDepthVector) 
slot_depth= slotDepthVector(slot_index)

for highwayDepth_index = 1: length (highwayDepthVector)
higwayDepth = highwayDepthVector(highwayDepth_index)

for highwayNum_index = 1 : length (highwayOfEachDirVector)
higwayOfEachDir = highwayOfEachDirVector (highwayNum_index)

for CHANNEL_PER_DIRECTION=2.^vector
	%NUM_SUB_NETWORKS = 4/CHANNEL_PER_DIRECTION
	NUM_SUB_NETWORKS = 1

	CHANNEL_FANOUTS=5*CHANNEL_PER_DIRECTION;
	TOTAL_NUM_CHANNELS=5*CHANNEL_PER_DIRECTION;
	RESOURCE_CHANNEL_NUM=CHANNEL_PER_DIRECTION*NUM_SUB_NETWORKS;

		if CHANNEL_PER_DIRECTION ==1
			inj_factor=1;
		
		elseif CHANNEL_PER_DIRECTION ==2
			inj_factor=1.5;


		elseif CHANNEL_PER_DIRECTION ==4
			inj_factor=1;

		else 
			inj_factor=1;
		end

	inj_factor=1;

max_time=2500000


%display('current channel per direction is')
%display(CHANNEL_PER_DIRECTION);
perl('alter_configuration.pl',int2str(COL),int2str(ROW),int2str(NUM_NODES),int2str(NUM_DIRECTIONS),int2str(CHANNEL_FANOUTS),int2str(TOTAL_NUM_CHANNELS),int2str(RESOURCE_CHANNEL_NUM),int2str(CHANNEL_PER_DIRECTION),int2str(NUM_SUB_NETWORKS),int2str(max_time),int2str(slot_depth),int2str(higwayDepth),int2str(higwayOfEachDir));
cd ../build;
system('make');
cd ../test_bench;
%trafficVector={'tornado','shuffle'}
trafficVector={'uniform'}
for tr=1: length(trafficVector);
traffic= char (trafficVector(tr)); %cell to string

lifeTimeVector=[400]
%lifeTimeVector=[16]

for m=1 : length (lifeTimeVector)
	lifeTime=lifeTimeVector(m);
	
result_path=strcat('results_ppb_adp_highway_buffer1_scale_up/','sub_',int2str(NUM_SUB_NETWORKS),'/',int2str(COL),int2str(ROW),'/','ch',int2str(CHANNEL_PER_DIRECTION),'/','th_',int2str(throughput),'/',int2str(slot_depth),'/',traffic,'/',int2str(higwayDepth),'/',int2str(higwayOfEachDir),'/');

	result_file_req=strcat(result_path,int2str(lifeTime),'_req_precise');
	result_file_trans=strcat(result_path,int2str(lifeTime),'_trans');
	mkdir(result_path);

	if exist(strcat(result_file_req,'.csv'),'file')~=0
	delete (strcat(result_file_req,'.csv'));
	end
	if exist(strcat(result_file_req,'.txt'),'file')~=0
	delete (strcat(result_file_req,'.txt'));
	end
	
	if exist(strcat(result_file_trans,'.csv'),'file')~=0
	delete (strcat(result_file_trans,'.csv'));
	end
injection_vector = [0.2,1.0,2.0,3.0,3.1,3.2,3.3,3.4,3.5,3.6,3.7,3.8,3.9,4.0,4.1,4.2] 
		for injection_base=1:length (injection_vector)
			injection_rate= injection_vector(injection_base) * inj_factor
			%transaction_gen(COL,ROW,lifeTime,5000,injection_rate);
			
			transaction_gen(COL,ROW,lifeTime,max_time,injection_rate,throughput,traffic);
			prob_index_integer=floor(injection_rate)
			
    			prob_index_float= floor (10*(injection_rate-prob_index_integer)+0.1);

			filename = sprintf('./ppb_cpp ./inp_test_files_ran/%s/th_%d/%d/%d_%d/%d %d > temp ' , traffic, throughput,lifeTime,prob_index_integer,prob_index_float,nodes,lifeTime);
			display (filename);
			system(filename)
			startTime=max_time*0.05;
			endTime=max_time;

			%startTime_trans=5000000*injection_rate/10/10/lifeTime;
			%endTime_trans=5000000*injection_rate/10/10*9/lifeTime;
			perl('./Data_processor_1.4.pl',result_file_req,int2str(startTime),int2str(endTime),int2str(lifeTime));
			%perl('./Data_processor_1.3_trans.pl',result_file_trans,int2str(startTime),int2str(endTime));
			clear filename;
		csv=csvread(strcat(result_file_req,'.csv'));
			if (size(csv,1)>1 && csv(size(csv,1)-1,size(csv,2)-3)>0)
				th1=csv(size(csv,1),size(csv,2)-3)/csv(size(csv,1),size(csv,2)-2);
				th2=csv(size(csv,1)-1,size(csv,2)-3)/csv(size(csv,1)-1,size(csv,2)-2);
				if (abs(th1 - th2)/th2 < 0.01)
					display('saturation');
					csv(size(csv,1),size(csv,2)-3)
					csv(size(csv,1)-1,size(csv,2)-3)
				
					break;
				end
			end
		end
	
	x=0:0.1*lifeTime:0.1*(size(csv,1))*lifeTime;
	k=zeros(1,size(csv,2));
	csv=[k;csv];
	
	figure_name=strcat(result_file_req,'.fig');
	%exportfig(gcf,figure_name,'psc2');
	scrsz = get(0,'ScreenSize');
 	figure1=figure('Position',[1 scrsz(4)/4 scrsz(3)/4 scrsz(4)/4])
 
	plot(x,csv(:,1));
	print ('-deps', '-tiff','-r300', result_file_req);	
	saveas(gcf,figure_name,'m');
	out_csv_file=strcat(result_file_req,'_output.csv');
	temp=[x' csv(:,1:end)];
%$avDelay,$avSetupDelay,$avWaitingDelay,$maxDelay,$maxSetupDelay,$maxWaitingDelay,$avAmount,$num_satisfied,$totalAmount,$endtime-$starttime
	table_tilte=[cellstr('offered_load'),cellstr('avSetupDelay'),cellstr('avTransDelay'),cellstr('avWaitingDelay'),cellstr('maxSetupDelay'),cellstr('maxWaitingDelay'),cellstr('avAmount'),cellstr('avgRetryTimes'), cellstr('maxRetryTime'),cellstr('Totalsuccess'),cellstr('count'),cellstr('generated'),cellstr('totalAmount'),cellstr('Total_time'),cellstr('QueuePercent'),cellstr('avgQueueEffort')];
	table=[table_tilte;num2cell(temp)];
	cell2csv(out_csv_file,table);
	end
end

end

end

end

end %throughput

%  for offered_load=0.1:0.1:1
%  
%  transaction_gen(16,16,200,100000,2);
%  
%  result = csvread('1.csv'); 
