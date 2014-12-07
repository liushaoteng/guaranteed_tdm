#!/usr/bin/perl
#in this version, we also count the total success numbers
########################################################
# This file is for statistical analysis of all parameters
########################################################
use Storable;
use Text::CSV_XS;
use Storable qw(nstore store_fd nstore_fd freeze fd_retrieve);


$networksize = 8;

#$ratelow = 400;
#$ratehigh=500;
$ratestep=1;
$sample = 8;

$sleeplow=0;
$sleephigh=0;
$sleepstep=1;

$duriationlow=200;
$duriationhigh=200;
$duriationstep=100;

#$networksize =2;
$generated=0;

$starttime =$ARGV[1];
$endtime =$ARGV[2];
$lifetime =$ARGV[3];
#############################statisticac book initialize##########################################



#  新建一个样式	  


$scol=0;


#######################################################################
#begin to read in data
###############################################################
$srow=1;
$scol=1;



#for ($r=$duriation/2; $r<=$sample*$ratestep+$duriation/2; $r=$r+$ratestep)
#for ($r=1; $r<=10; $r=$r+$ratestep)
#{

#    for($s=$sleeplow; $s <=$sleephigh; $s=$s+$sleepstep)
#{
    $processfile = "./temp";
    
    print "start execute $processfile, $lifetime \n";

    open(INFILE,$processfile)||die ("can not open $!");
    my $csv = Text::CSV_XS->new ({ binary => 1, eol => $/ });
    open $OUTFILE,">>$ARGV[0].csv"||die ("can not open $!");
    open $OUTFILE2,">>$ARGV[0].txt"||die ("can not open $!"); 	
    $starts=0; 
    print $OUTFILE2 "start execute $processfile\n";
    while (defined($a=<INFILE>)){
########################Get the infromation of transaction start ########################################
# #13 ns top.node_(5,7)generate trans 0557 number 1
# 	$n=$a=~m/\s*(\d*) ns top.(node_\(.*\))generate trans (.*) number (\d*)/;
# #	$n=$a=~m/^(\d*) ns.*generate trans (.*) number (\d*)/;
# #	$n=$a=~m/.*generate.*/;
# 	if($n)
# 	{
# # 	    print "$1 $2 gen $3 number: $4 \n";	
# 	    $rec=@{$Data{$2}{start}};
# 	    $Data{$2}{start}[$rec]=$1;
# #$Record{$2}{3}{start}=$1;
# 	    #if ($3 >= 9 && $3<=11)
# 	    #{$Data{$2}{type}[$rec] = 0;} #  burst read
# 	    #elsif ($3 >= 12 && $3<=14)
# 	    #{ $Data{$2}{type}[$rec] = 1;} #burst write
# 	    #elsif ($3>=3 && $3<=5)
# 	    #{ $Data{$2}{type}[$rec] = 2;} #single read
# 	    #elsif ($3>=6 && $3<=8) 
# 	    #{$Data{$2}{type}[$rec] = 3;}  #single write
# 	    
# 	}
# 
# 
# ##############################Get the information of transaction send########################################
# 	$n=$a=~m/\s*(\d*) ns top\.(node_\(.*\))send new trans (.*)/;
# 	if($n && ($1>$starts))
# 	{
# # 	    print "$1 $2 send $3 \n";	
# 	    $rec=@{$Data{$2}{sent}};
# 	    $Data{$2}{sent}[$rec]=$1;
# 	    
# 	   
# 	    
# 	}
# 
# ##############################Get the information of transaction end########################################   
# #100249 ns top.node_(2,6)request response is succeed trans 2426 retrytimes 0
# 1820124 ns top.node_(1,12) request  succeed trans ca1c retrytimes 13 wait 264491 setup 1925 total 266416
# 41343 ns top.node_(3,0) [request] internal_Cancel trans 11 num_succs  1 num_req 4 amount 800 setup 1267 total 4170
# 61213 ns top.node_(3,1) [request] internal_Cancel trans 52 num_succs  1 num_req 4 amount 800 setup 880 total 880
	#$n=$a=~m/\s*(\d*) ns top\.(node_\(.*\)) [request] internal_Cancel trans (.*) num_succs  (\d*) num_req (\d*) amount (\d*) setup (\d*) total (\d*)/;
	#$n=$a=~m/\s*(\d*) ns top\.(node_\(.*\)) \[request\] internal_Cancel trans (.*) num_succs  (\d*) num_req (\d*) amount (\d*) setup (\d*) total (\d*)/;
#61215 ns top.node_(1,5) [request] internal_Cancel trans 14 id 115 num_succs 1 num_req 1 amount 100 setup 12 transfer 409 total 428 retry_times 0

#[request] internal_Cancel trans 7422 num_succs 1 num_req 1 amount 400 setup 4fe transfer 192 total 2efd retry_times df


$n=$a=~m/\s*(\d*) ns top\.(node_\(.*\)) \[request\] internal_Cancel trans (.*) num_succs (\d*) num_req (\d*) amount (\d*) setup (\d*) transfer (\d*) total (\d*) retry_times (\d*) has queue (\d*) highway_effort (\d*)/;
	if($n)
	{
 	   # print "$1 $2 finishes $3 num_succs $4 num_req $5 amount $6 setup $7 total $8\n ";
	    $rec=@{$Data{$2}{end}};
	    $Data{$2}{end}[$rec]=$1;

	    $rec=@{$Data{$2}{num_succs}};
	    $Data{$2}{num_succs}[$rec]=$4;

	    $rec=@{$Data{$2}{num_req}};
	    $Data{$2}{num_req}[$rec] = $5;

	     $rec=@{$Data{$2}{amount}};
	    $Data{$2}{amount}[$rec] = $6;	

	     $rec=@{$Data{$2}{setup}};
	    $Data{$2}{setup}[$rec] = $7;

	    $rec=@{$Data{$2}{transfer}};
	    $Data{$2}{transfer}[$rec] = $8;

	     $rec=@{$Data{$2}{total}};
	    $Data{$2}{total}[$rec] = $9;

	   $rec=@{$Data{$2}{retry}};
	    $Data{$2}{retry}[$rec] = $10;
            		
             $rec=@{$Data{$2}{success}};
	    $Data{$2}{success}[$rec] = 1;
	    
	    $rec=@{$Data{$2}{hasQueue}};
	     $Data{$2}{hasQueue}[$rec] = $11;

	     $rec=@{$Data{$2}{queueEffort}};
	     $Data{$2}{queueEffort}[$rec] = $12;

	    	#print "$1 $2 succ $3 number: numscc: $4, req $5, setup $7, transfer $8, total $9, retry ,$10\n";
#	print "$1 $2 succeed $3 number: $4 \n";	
	  #  print "endtime  $Data{$2}{end}[$rec] \n"
	}
#61237 ns top.node_(6,3) [failed] cancelDueContention trans 71 id 130 num_succs 0 num_req 1 amount 100 setup 147 transfer 0 total 150 retry_times 6
#43993 ns top.node_(0,6) [request] internal_Cancel trans 60 id 83 num_succs 1 num_req 1 amount 100 setup 31 transfer 405 total 438 retry_times 0
$n=$a=~m/\s*(\d*) ns top\.(node_\(.*\)) \[failed\] cancelDueContention trans (.*) num_succs (\d*) num_req (\d*) amount (\d*) setup (\d*) transfer (\d*) total (\d*) retry_times (\d*)/;
#############################Get the information of
	if($n)
	{
 	   # print "$1 $2 finishes $3 num_succs $4 num_req $5 amount $6 setup $7 total $8\n ";
	    $rec=@{$Data{$2}{end}};
	    $Data{$2}{end}[$rec]=$1;

	    $rec=@{$Data{$2}{num_succs}};
	    $Data{$2}{num_succs}[$rec]=$4;

	    $rec=@{$Data{$2}{num_req}};
	    $Data{$2}{num_req}[$rec] = $5;

	     $rec=@{$Data{$2}{amount}};
	    $Data{$2}{amount}[$rec] = $6;	

	     $rec=@{$Data{$2}{setup}};
	    $Data{$2}{setup}[$rec] = $7;

	    $rec=@{$Data{$2}{transfer}};
	    $Data{$2}{transfer}[$rec] = $8;

	     $rec=@{$Data{$2}{total}};
	    $Data{$2}{total}[$rec] = $9;

		$rec=@{$Data{$2}{retry}};
	    $Data{$2}{retry}[$rec] = $10;
            		
             $rec=@{$Data{$2}{success}};
	    $Data{$2}{success}[$rec] = 0;
#	print "$1 $2 failed $3 number: numscc: $4, req $5, setup $7, transfer $8, total $9, retry ,$10\n";	
	  #  print "endtime  $Data{$2}{end}[$rec] \n"
	}
#50134 ns top.node_(4,1) [gen] 4741 number 96 lf 100 th 1
$n=$a=~m/\s*(\d*) ns top\.(node_\(.*\)) \[gen\].*/;

      if($n)
      {
	 if ($1 > $starttime && $1 <$endtime)
	    {
		$generated = $generated+1;
	#	 print "gen";
	    }

	#print "$a \n";
#	 print "$1 $2";
      }

    }
# store the Hash Data for future usage ######################################### 

    #$k="$ARGV[0]$ARGV[0]";
    #open (STD,">$k")||die("$!");
    #print $k;
    #store_fd (\%Data, *STD)||die("$!");
    #close(STD);
    #open (STD,"$k")||die("$!");
    #$Data_r = fd_retrieve(*STD);
	$Data_r= \%Data;
    #close (STD);
    #undef (%Data);
##########stroage the Hash Data in an excel######################################
#print out for check ########################################################
#     foreach $node (keys %$Data_r)
# {
#     print "$node :\n";
#     $index_end=-1;
#     for ($index=0; $index< @{$$Data_r{$node}{start}}; $index++)
#     {
# 	# if ($$Data_r{$node}{type}[$index] == 0)
# 	# {   
# 	#     $index_end=$index_end+$ARGV[0];  ############################one bust read correspond to 8 reponse #############################
# 	# }
# 	# if ($$Data_r{$node}{type}[$index] == 2)
# 	# {$index_end=$index_end+1;}          
# 	#############################for private  memory single access, only data read has reponcse####
# 	$index_end=$index_end+1;
# 	#{$Timeconsummed =$$Data_r{$node}{end}[$index]-$$Data_r{$node}{start}[$index];}
# 	$Timeconsummed =$$Data_r{$node}{end}[$index_end]-$$Data_r{$node}{start}[$index];
# 	#if (($$Data_r{$node}{type}[$index] == 0)||($$Data_r{$node}{type}[$index] == 2))
# 	#{
# 	#print "time consumption $$Data_r{$node}{end}[$index_end]-$$Data_r{$node}{start}[$index] $index_end \n";
# 	#}
# 	
#     }
# }
# ##################exel write#################################
# 
# 
# 
# # 添加一个工作表	  
# 
# 
# #$worksheet = $workbook -> add_worksheet ("$duriation $r $s");
# 
# #  新建2个样式	  
# #$format2 = $workbook->add_format(); # Add a format2   
# #$format2->set_bold(0);#设置字体为细体   
# #$format2->set_align('center');#设置单元格居中  
# #$format2->set_size(12);
# #$format2->set_text_justlast();
# $row=0;
# $col=0;
# 
# 
#用于统计的变量############################################################################
$avDelay=0;
$maxDelay = 0;
$avSetupDelay=0;
$maxSetupDelay=0;
$avWaitingDelay=0;
$maxWaitingDelay=0;

$avThrougpht=0;


$avAmount=0;
$totalAmount=0;

$num_transfers=0;
$num_satisfied=0;

$avgRetryTimes=0;
$maxRetryTimes=0;
$avgTransferDelay=0;

$hasQueue_total =0;

$queueEffort_total =0;

#$totalRequest=0;
$totalSuccess =0;





print "start number is $startnumber, end number is $endnumber \n";
$count=1;
# $starttime=-1;
#     $endtime=0;	
foreach $node (keys %$Data_r)  { 
    #$worksheet->write($row, $col, "$node", $format);   
    #$row=1; 
    #$worksheet->write($row, $col, "start_time", $format);
    #$worksheet->write($row, $col+1, "end_time", $format);   
    #$worksheet->write($row, $col+2, "consumed_time", $format);   
    $index_end =-1;
    $row=2;

#     print "$node :\n";
$node_xx=0;
$node_11=0;

$avDelay_node_xx=0;
$avSetupDelay_node_xx=0;
$avDelay_node_11=0;
    $index_end=-1;

#     if ($starttime== -1)
#     {$starttime =$$Data_r{$node}{end}[$startnumber];}
#     elsif ($$Data_r{$node}{end}[$startnumber]<$starttime)
#     {
# 	$starttime =$$Data_r{$node}{end}[$startnumber];
#     }
	
    
    	
    for ($index=0; $index< @{$$Data_r{$node}{end}}; $index++)
    {
	
	$index_end=$index_end+1;

	$Timeconsummed =$$Data_r{$node}{total}[$index];
	
	$SetupDelay   = $$Data_r{$node}{setup}[$index];

	$WaitingDelay = $$Data_r{$node}{total}[$index]-$$Data_r{$node}{transfer}[$index];
	
	$transfer = $$Data_r{$node}{transfer}[$index];
	$amount = $$Data_r{$node}{amount}[$index];
	$retry  =  $$Data_r{$node}{retry}[$index];

	$success =  $$Data_r{$node}{success}[$index];
	$hasQueue = $$Data_r{$node}{hasQueue}[$index];
	$queueEffort = $$Data_r{$node}{queueEffort}[$index];
	
	
# 	if ($Timeconsummed < 0)
# 	 {
#		print "$node num $index result: $Timeconsummed, $SetupDelay,$WaitingDelay,$RetryTimes,$ContendTimes \n";
# 	}
	    
	#if (($$Data_r{$node}{type}[$index] == 0)||($$Data_r{$node}{type}[$index] == 2))
	#{
	   # $worksheet->write($row, $col, $$Data_r{$node}{start}[$index], $format2);
	   # $worksheet->write($row, $col+1,  $$Data_r{$node}{end}[$index_end], $format2); 
	   # $worksheet->write($row, $col+2, $Timeconsummed, $format2);
	    $row=$row+1;
		if ($$Data_r{$node}{end}[$index] > $starttime && $$Data_r{$node}{end}[$index] < $endtime)
	  	  { 
# 			if ($$Data_r{$node}{end}[$index]>$endtime)
# 			{
# 			 $endtime = $$Data_r{$node}{end}[$index];
# 			}
		       
			$node_xx = $node_xx+1;
			$count = $count+1;
			$avDelay = $Timeconsummed +$avDelay;
			$avDelay_node_xx = $Timeconsummed +$avDelay_node_xx;
			$hasQueue_total = $hasQueue_total+$hasQueue;
			$queueEffort_total = $queueEffort_total +$queueEffort;
# 	 		print "avDelay $avDelay\n";
			if ($Timeconsummed > $maxDelay)
			{$maxDelay = $Timeconsummed;}
	
			$avSetupDelay = $SetupDelay+$avSetupDelay;

			$avSetupDelay_node_xx = $SetupDelay+$avSetupDelay_node_xx;

			if ($retry > $maxRetryTimes)
			{$maxRetryTimes =$retry;}
			
			if ($SetupDelay > $maxSetupDelay)
			{$maxSetupDelay =$SetupDelay;}

			
	
			$avWaitingDelay = $WaitingDelay + $avWaitingDelay;

			if ($WaitingDelay > $maxWaitingDelay)
			{$maxWaitingDelay = $WaitingDelay;}
			
			$totalAmount=$amount+$totalAmount;
			$totalSuccess =$success+$totalSuccess;

			$avgRetryTimes = $avgRetryTimes +$retry;
			$avgTransferDelay = $avgTransferDelay +$transfer;

			if ($$Data_r{$node}{num_succs}[$index]==$$Data_r{$node}{num_req}[$index])
			{ $num_satisfied = $num_satisfied+1;}
			
			

	   	 }
	#}

    }

# $avDelay_node_xx= ($avDelay_node_xx/$node_xx);
# $avSetupDelay_node_xx = ($avSetupDelay_node_xx /$node_xx);
#    print $OUTFILE2 "$node, total num of transfer is $node_xx, average delay is  $avDelay_node_xx , setup delay is $avSetupDelay_node_xx\n";
    
    $row=0;
    $col=$col+3;
}	



$avDelay = ($avDelay /$count);
$maxDelay =$maxDelay;
$avSetupDelay = ($avSetupDelay/$count);
$maxSetupDelay =$maxSetupDelay;
$avWaitingDelay = ($avWaitingDelay/$count);
$maxWaitingDelay =$maxWaitingDelay;
$avAmount= ($totalAmount/$count);
$maxRetryTimes = $maxRetryTimes;
$avgRetryTimes = ($avgRetryTimes/$count);
$avgTransferDelay = ($avgTransferDelay/$count);

$num_transfers=$count;

$queuedPercent = $hasQueue_total/$count;
$avgQueueEffort = $queueEffort_total / ($count*$lifetime);


#print "avretrytimes $avRetryTimes\n";

#print "avDelay $avDelay\n";
#print "avSetupDelay $avSetupDelay\n";

@outputArray=($avSetupDelay,$avgTransferDelay,$avWaitingDelay,$maxSetupDelay,$maxWaitingDelay,$avAmount,$avgRetryTimes,$maxRetryTimes,$totalSuccess, $count, $generated,$totalAmount,$endtime-$starttime,$queuedPercent, $avgQueueEffort);
print '$avSetupDelay,$avgTransferDelay,$avWaitingDelay,$maxSetupDelay,$maxWaitingDelay,$avAmount,$avgRetryTimes,$maxRetryTimes,$Totalsuccess, $count, $generated,$totalAmount,$endtime-$starttime,$queuedPercent,$avgQueueEffort';
print "\n";
print "@outputArray\n";
$arrayRef=\@outputArray;
$csv->print($OUTFILE,$arrayRef);
print $ARGV[0];
close $OUTFILE or die "cannot close $!";
#  $stsheet->write ($srow, $scol, "400", $foramt);
#  $srow=$srow+1;
#$stsheet->write ($srow, $scol, $avDelay, $foramt);
#$scol=$scol+1;
#$stsheet->write ($srow, $scol, $maxDelay, $foramt);
#$scol=$scol+1;
#$stsheet->write ($srow, $scol, $avSetupDelay, $foramt);
#$scol=$scol+1;
#$scol=$scol+1;
#$stsheet->write ($srow, $scol, $maxWaitingDelay, $foramt);

#$scol=$scol+1;
#$stsheet->write ($srow, $scol, $avContendTimes, $foramt);
#$scol=$scol+1;
#$stsheet->write ($srow, $scol, $maxContendTimes, $foramt);

#$scol=$scol+1;
#$stsheet->write ($srow, $scol, $avRetryTimes, $foramt);
#$scol=$scol+1;
#$stsheet->write ($srow, $scol, $maxRetryTimes, $foramt);



$avDelay=0;
$maxDelay = 0;
$avSetupDelay=0;
$maxSetupDelay=0;
$avWaitingDelay=0;
$maxWaitingDelay=0;
$avContendTimes=0;
$maxContendTimes=0;
$avRetryTimes=0;
$maxRetryTimes=0;
$avgTransferDelay=0;
$queuedPercent = 0;
undef (%Data);

#}
$scol=1;
$srow=$srow+1;
#}

#}

 exit;
