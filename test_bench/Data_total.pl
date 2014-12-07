#!/usr/bin/perl

########################################################
# This file is for statistical analysis of all parameters
########################################################
use Storable;
use Spreadsheet::WriteExcel;
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
my $workbook = Spreadsheet::WriteExcel->new("1616duri200_0.5_retrysuccess.xls");
my $statisticbook = Spreadsheet::WriteExcel->new("1616duri200_0.5_retrysuccessstatistics.xls");

#############################statisticac book initialize##########################################



#  新建一个样式	  
$format = $workbook->add_format(); # Add a format   
$format->set_bold();#设置字体为粗体   
$format->set_align('center');#设置单元格居中   
$format->set_size(10);
$format->set_text_wrap(1);
$format->set_text_justlast(1);

for ($duriation=$duriationlow; $duriation<=$duriationhigh; $duriation = $duriation+$duriationstep)

{
    $scol=0;
    $srow=0;
    $stsheet = $statisticbook -> add_worksheet ("duriation $duriation ");

    $stsheet->write ($srow, $scol, 'injectionrate\sleeptime ', $foramt);

#     for ($scol=1; $scol<=($sleephigh-$sleeplow)/$sleepstep+1;$scol=$scol+1)
# {
    
#    # $size=$scol+1;
#     $sleep= $sleepstep*($scol-1)+$sleeplow;
#     $stsheet->write ($srow, $scol, "$sleep" , $foramt);
# }

    {
	$scol=1;
        $stsheet->write ($srow, $scol, 'average delay', $foramt);
	$scol=2;
        $stsheet->write ($srow, $scol, 'max delay', $foramt);

	$scol=3;
        $stsheet->write ($srow, $scol, 'average setup delay', $foramt);
	$scol=4;	
        $stsheet->write ($srow, $scol, 'max setup delay', $foramt);

	$scol=5;
        $stsheet->write ($srow, $scol, 'average waiting time', $foramt);
	$scol=6;
        $stsheet->write ($srow, $scol, 'max waiting time', $foramt);

	$scol=7;
        $stsheet->write ($srow, $scol, 'average contend times', $foramt);
	$scol=8;
        $stsheet->write ($srow, $scol, 'max contend times', $foramt);

	$scol=9;
        $stsheet->write ($srow, $scol, 'average retry times', $foramt);
	$scol=10;
        $stsheet->write ($srow, $scol, 'max retry times', $foramt);

    }


$scol=0;

for ($srow=1; $srow<=$sample+1;$srow=$srow+1)
{
    
  #  $injrate= $ratestep * ($srow-1) +$duriation/2;
    $injrate= $ratestep * ($srow-1) +200;
    $temp=$injrate*2;                                            #actual rate is twoo times higher than nominal rate
    $stsheet->write ($srow, $scol,"1/ $temp" , $foramt);
}

#######################################################################
#begin to read in data
###############################################################
$srow=1;
$scol=1;



#for ($r=$duriation/2; $r<=$sample*$ratestep+$duriation/2; $r=$r+$ratestep)
for ($r=1; $r<=10; $r=$r+$ratestep)
{

    for($s=$sleeplow; $s <=$sleephigh; $s=$s+$sleepstep)
{
    $processfile = "./data_noretry/ncsim"."$duriation"."$r"."$s".".log";
    
    print "start execute $processfile\n";

    open(INFILE,$processfile)||die ("can not open $!");
    open(OUTFILE,">>evaluatetest.log")||die ("can not open $!");
    $starts=0; 
    while (defined($a=<INFILE>)){
########################Get the infromation of transaction start ########################################
	$n=$a=~m/\s*(\d*) ns (node \(.*\)) add \(.*\) to queue, number:(\d*)/;
	if($n && ($1>$starts))
	{
# 	    print "$1 $2 add $3 number: $4 \n";	
	    $rec=@{$Data{$2}{start}};
	    $Data{$2}{start}[$rec]=$1;
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
	$n=$a=~m/\s*(\d*) ns (node \(.*\)) send probe .*, to (\(.*\))/;
	if($n && ($1>$starts))
	{
# 	    print "$1 $2 send to $3 \n";	
	    $rec=@{$Data{$2}{sent}};
	    $Data{$2}{sent}[$rec]=$1;
	   
	    
	}

##############################Get the information of transaction end########################################   

	$n=$a=~m/\s*(\d*) ns (node \(.*\)) succeed probe .*, to (\(.*\)), retrytimes is (\d*), totaltimes is (\d*)/;
	if($n && ($1>$starts))
	{
# 	    print "$1 $2 succeed to $3 contendtime $4 ";
	    $rec=@{$Data{$2}{end}};
	    $Data{$2}{end}[$rec]=$1;

	    $rec=@{$Data{$2}{contend}};
	    $Data{$2}{contend}[$rec]=$4;

	    $rec=@{$Data{$2}{try}};
	    $Data{$2}{try}[$rec] = $5;
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
# #     print "$node :\n";
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
}
##################exel write#################################



# 添加一个工作表	  


$worksheet = $workbook -> add_worksheet ("$duriation $r $s");

#  新建2个样式	  
$format2 = $workbook->add_format(); # Add a format2   
$format2->set_bold(0);#设置字体为细体   
$format2->set_align('center');#设置单元格居中  
$format2->set_size(12);
#$format2->set_text_justlast();
$row=0;
$col=0;


#用于统计的变量
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


$startnumber =1000;
$endnumber =600;
$count=1;

foreach $node (keys %$Data_r)  { 
    $worksheet->write($row, $col, "$node", $format);   
    $row=1; 
    $worksheet->write($row, $col, "start_time", $format);
    $worksheet->write($row, $col+1, "end_time", $format);   
    $worksheet->write($row, $col+2, "consumed_time", $format);   
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
	
	    
	#if (($$Data_r{$node}{type}[$index] == 0)||($$Data_r{$node}{type}[$index] == 2))
	#{
	    $worksheet->write($row, $col, $$Data_r{$node}{start}[$index], $format2);
	    $worksheet->write($row, $col+1,  $$Data_r{$node}{end}[$index_end], $format2); 
	    $worksheet->write($row, $col+2, $Timeconsummed, $format2);
	    $row=$row+1;
	    if (($row > $startnumber)&& ($row < $endnumber))
	    { 
		$count = $count+1;
		$avDelay = $Timeconsummed +$avDelay;
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

print "avretrytimes $avRetryTimes\n";

$avDelay = ($avDelay /$count)/10;
$maxDelay =$maxDelay/10;
$avSetupDelay = ($avSetupDelay/$count)/10;
$maxSetupDelay =$maxSetupDelay/10;
$avWaitingDelay = ($avWaitingDelay/$count)/10;
$maxWaitingDelay =$maxWaitingDelay/10;
$avContendTimes =$avContendTimes/$count;

$avRetryTimes =$avRetryTimes/$count;

print "avretrytimes $avRetryTimes\n";
#  $stsheet->write ($srow, $scol, "400", $foramt);
#  $srow=$srow+1;
$stsheet->write ($srow, $scol, $avDelay, $foramt);
$scol=$scol+1;
$stsheet->write ($srow, $scol, $maxDelay, $foramt);
$scol=$scol+1;
$stsheet->write ($srow, $scol, $avSetupDelay, $foramt);
$scol=$scol+1;
$stsheet->write ($srow, $scol, $maxSetupDelay, $foramt);
$scol=$scol+1;
$stsheet->write ($srow, $scol, $avWaitingDelay, $foramt);
$scol=$scol+1;
$stsheet->write ($srow, $scol, $maxWaitingDelay, $foramt);

$scol=$scol+1;
$stsheet->write ($srow, $scol, $avContendTimes, $foramt);
$scol=$scol+1;
$stsheet->write ($srow, $scol, $maxContendTimes, $foramt);

$scol=$scol+1;
$stsheet->write ($srow, $scol, $avRetryTimes, $foramt);
$scol=$scol+1;
$stsheet->write ($srow, $scol, $maxRetryTimes, $foramt);



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

}
$scol=1;
$srow=$srow+1;
}

}

exit;
