#!/usr/bin/perl

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
    
    print "start execute $processfile\n";

    open(INFILE,$processfile)||die ("can not open $!");
    my $csv = Text::CSV_XS->new ({ binary => 1, eol => $/ });
    open $OUTFILE,">>$ARGV[0].csv"||die ("can not open $!");
    $starts=0; 
    while (defined($a=<INFILE>)){
########################Get the infromation of transaction start ########################################
#13 ns top.node_(5,7)generate trans 0557 number 1
	$n=$a=~m/\s*(\d*) ns top.(node_\(.*\))generate trans (.*) number (\d*)/;
#	$n=$a=~m/^(\d*) ns.*generate trans (.*) number (\d*)/;
#	$n=$a=~m/.*generate.*/;
	if($n)
	{
# 	    print "$1 $2 gen $3 number: $4 \n";	
	    $rec=@{$Data{$2}{start}};
	    $Data{$2}{start}[$rec]=$1;
#$Record{$2}{3}{start}=$1;
	    #if ($3 >= 9 && $3<=11)
	    #{$Data{$2}{type}[$rec] = 0;} #  burst read
	    #elsif ($3 >= 12 && $3<=14)
	    #{ $Data{$2}{type}[$rec] = 1;} #burst write
	    #elsif ($3>=3 && $3<=5)
	    #{ $Data{$2}{type}[$rec] = 2;} #single read
	    #elsif ($3>=6 && $3<=8) 
	    #{$Data{$2}{type}[$rec] = 3;}  #single write
	    
	}


##############################Get the information of transaction send########################################
	$n=$a=~m/\s*(\d*) ns top\.(node_\(.*\))send new trans (.*)/;
	if($n && ($1>$starts))
	{
# 	    print "$1 $2 send $3 \n";	
	    $rec=@{$Data{$2}{sent}};
	    $Data{$2}{sent}[$rec]=$1;
	    
	   
	    
	}

##############################Get the information of transaction end########################################   
#100249 ns top.node_(2,6)request response is succeed trans 2426 retrytimes 0
	$n=$a=~m/\s*(\d*) ns top\.(node_\(.*\))request response is succeed trans (.*) retrytimes (\d*)/;
	if($n && ($1>$starts))
	{
# 	    print "$1 $2 succeed to $3 retrytime $4 ";
	    $rec=@{$Data{$2}{end}};
	    $Data{$2}{end}[$rec]=$1;

	    $rec=@{$Data{$2}{contend}};
	    $Data{$2}{contend}[$rec]=$4;

	    $rec=@{$Data{$2}{try}};
	    $Data{$2}{try}[$rec] = $4;
# 	    print "retrytime  $Data{$2}{try}[$rec] \n"
	}
#############################Get the information of
	
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
$avContendTimes=0;
$maxContendTimes=0;
$avRetryTimes=0;
$maxRetryTimes=0;


$startnumber =$ARGV[1];
$endnumber =$ARGV[2];
$count=1;

foreach $node (keys %$Data_r)  { 
    #$worksheet->write($row, $col, "$node", $format);   
    #$row=1; 
    #$worksheet->write($row, $col, "start_time", $format);
    #$worksheet->write($row, $col+1, "end_time", $format);   
    #$worksheet->write($row, $col+2, "consumed_time", $format);   
    $index_end =-1;
    $row=2;

#     print "$node :\n";
    $index_end=-1;
    for ($index=0; $index< @{$$Data_r{$node}{start}}; $index++)
    {
	
	$index_end=$index_end+1;

	$Timeconsummed =$$Data_r{$node}{end}[$index_end]-$$Data_r{$node}{start}[$index];
	
	$Setupdelay   = $$Data_r{$node}{end}[$index_end]-$$Data_r{$node}{sent}[$index];
	$Waitingdelay = $$Data_r{$node}{sent}[$index_end]-$$Data_r{$node}{start}[$index];
	$RetryTimes  = $$Data_r{$node}{try}[$index_end];
	$ContendTimes =  $$Data_r{$node}{contend}[$index_end];
# 	if ($Timeconsummed < 0)
# 	 {
# 		print "$node num $index result: $Timeconsummed, $Setupdelay,$Waitingdelay,$RetryTimes,$ContendTimes \n";
# 	}
	    
	#if (($$Data_r{$node}{type}[$index] == 0)||($$Data_r{$node}{type}[$index] == 2))
	#{
	   # $worksheet->write($row, $col, $$Data_r{$node}{start}[$index], $format2);
	   # $worksheet->write($row, $col+1,  $$Data_r{$node}{end}[$index_end], $format2); 
	   # $worksheet->write($row, $col+2, $Timeconsummed, $format2);
	    $row=$row+1;
if (($row > $startnumber)&& ($row < $endnumber))
	    { 
		$count = $count+1;
		$avDelay = $Timeconsummed +$avDelay;
# 		print "avDelay $avDelay\n";
		if ($Timeconsummed > $maxDelay)
		{$maxDelay = $Timeconsummed;}

		$avSetupDelay = $Setupdelay+$avSetupDelay;
		
		if ($Setupdelay > $maxSetupDelay)
		{$maxSetupDelay =$Setupdelay;}

		$avWaitingDelay = $Waitingdelay + $avWaitingDelay;
		if ($Waitingdelay > $maxWaitingDelay)
		{$maxWaitingDelay = $Waitingdelay;}

		$avContendTimes = $ContendTimes +$avContendTimes;
		if ($ContendTimes > $maxContendTimes)
		{$maxContendTimes = $ContendTimes;}
		
		$avRetryTimes = $RetryTimes +$avRetryTimes;

		if ($RetryTimes > $maxRetryTimes)
		    {$maxRetryTimes = $RetryTimes;}

	    }
	#}

    }

    
    
    $row=0;
    $col=$col+3;
}	



$avDelay = ($avDelay /$count);
$maxDelay =$maxDelay;
$avSetupDelay = ($avSetupDelay/$count);
$maxSetupDelay =$maxSetupDelay;
$avWaitingDelay = ($avWaitingDelay/$count);
$maxWaitingDelay =$maxWaitingDelay;
$avContendTimes =$avContendTimes/$count;

$avRetryTimes =$avRetryTimes/$count;

print "avretrytimes $avRetryTimes\n";

print "avDelay $avDelay\n";
print "avSetupDelay $avSetupDelay\n";

@outputArray=($avDelay,$avSetupDelay,$avWaitingDelay,$maxDelay,$maxSetupDelay,$maxWaitingDelay,$avRetryTimes);
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
#$stsheet->write ($srow, $scol, $maxSetupDelay, $foramt);
#$scol=$scol+1;
#$stsheet->write ($srow, $scol, $avWaitingDelay, $foramt);
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
undef (%Data);

#}
$scol=1;
$srow=$srow+1;
#}

#}

 exit;
