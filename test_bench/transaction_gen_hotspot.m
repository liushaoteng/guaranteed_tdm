function transaction_gen_hotspot(col,row,lifetime,Tmax,inj_rate, throughput)

% initialize rand to a different state each time
% To make sure we use different rand state for generating the transactions for every node
% the arrival time is according to poisson distribution
% the injection_rate is tricky, it actually used to denote offer load, ex, 10 means 0.1 offered load
%load('./poisson_process.m')  


% rmdir('./inp_test_files','s');

disp ('inputs should be col, row, lifetime, Tmax, inj_rate, throughput'); 
%rmdir(strcat('./inp_test_files'+'/lifetime','s');
%clear all;
clc;

%parent_dir = 'inp_test_files/';
file_ext = '.txt';

% -------------------------Parameters ---------------------------------------------
%n                  = 4;  % mesh size nxn
% rows             = n;        % number of rows in the selected mesh network
% cols             = n;     % number of columns in the selected mesh network
sim_cycles       = Tmax;    % Number of cycles for the simulation period
% probability      = 1/50;    % probility of generating the transaction in every cycle
% probability      = 1./(10:10:200);    % probility of generating the transaction in every cycle from 1/10, 1/20, 1/30, ... , 1/200
%probability = 1/10;
%probability = 0.1 : 0.1 : 1 ;


%address_width_1D = 5;        % address width of destination or resource
% ---------------------------------------------------------------------------------
%sim_cycle_str_length = 5;    % for upto cycles in the range of 5 digit integers

%cd inp_test_files 

    prob_index_integer=floor(inj_rate);
    prob_index_float= floor (10*(inj_rate-prob_index_integer)+0.1);
   

mkdir(strcat('./inp_test_files/','th_',int2str(throughput),'/',int2str(lifetime),'/',int2str(prob_index_integer), '_', int2str(prob_index_float)));
  % mkdir(strcat('inp_test_files/', int2str(prob_index)));
    for row_index = 0: row-1
        for col_index = 0 : col-1
	
            file_name = sprintf('inp_test_files/th_%d/%d/%d_%d/%02x%02x.txt' ,throughput,lifetime,prob_index_integer, prob_index_float,row_index,col_index);
		if exist(file_name,'file')~=0
			%delete (file_name);
			break;
		end
            fid = fopen(file_name, 'wt');
   %         fprintf(fid,'hello')
            
          %  random_transaction_array = find(rand(sim_cycles,1)<probability(prob_index));
           
		
	   % if ((row_index == 0 && col_index==0) || (row_index==1 && col_index==1))
		if ( ((col_index==0) || (row_index==0 )) && col_index ~= row_index )
		random_transaction_array=poisson_process(lifetime*10/inj_rate, sim_cycles);
		%random_transaction_array = 0:lifetime*10/inj_rate:sim_cycles;
		temp_random_row_address = 0;
                temp_random_col_address = 0;
		for m = 1 : length(random_transaction_array)
                %random_row_address = dec2bin(temp_random_row_address,address_width_1D);
                %random_col_address = dec2bin(temp_random_col_address,address_width_1D);
                %random_address = strcat(num2str(random_row_address),num2str(random_col_address));
                %c_random_transaction_array = strcat('00000',num2str(random_transaction_array(m)));
               
                %fprintf(fid,'\n%s %s', c_random_transaction_array((length(c_random_transaction_array)-(sim_cycle_str_length-1)):length(c_random_transaction_array)), random_address);
                fprintf(fid, '%d %d %d %d %f\n', random_transaction_array(m), temp_random_row_address, temp_random_col_address, lifetime, throughput);
		end
           end
            fclose(fid);
	    clear file_name;
	
        end
    end
    disp(row*col);
    disp('files were generated for Injection rate');
  %  disp(probability(prob_index));
%end
%cd ..
disp('Transaction Generation Complete');
    
% random_transaction_array
% dec2bin(random_address_array(1:m),address_width)
