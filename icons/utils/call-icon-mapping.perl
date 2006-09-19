#!/usr/bin/perl


($filename) = @ARGV;

open (IN, $filename) || die "cannot open $filename for reading : $!";

$pwd = `pwd`;

chop $pwd;

while (<IN>)
{
  chop $_;
  if ( $_ =~ /x/)
  {
    $dir = $_;
  }
  else
  {
    chdir ("$pwd/$dir");
    print "in dir $pwd/$dir running : \n";
    $command = "/home/erwannc/code/Nimbus/icons/icon-name-mapping -c $_ -e /home/erwannc/code/Nimbus/icons/my-mapping.xml\n";
    print "$command\n";
    `$command`;

#    print "/home/erwannc/code/Nimbus/icons/-icon-name-mapping -c $_ -e /home/erwannc/code/Nimbus/icons/old-legacy-icon-mapping.xml\n";
    #print "/usr/libexec/icon-name-mapping -c $_\n";
#    `/usr/libexec/icon-name-mapping -c $_`;
  }
    
  

  
}
close (IN);

#$num_digits = length (@toto);
#print  "num digits $num_digits\n";
#
#$i=1;
#foreach my $file (@toto) {
#  chop $file;
# $new_file = "$i";
#  $count = length ($i);
#  for ($j= $num_digits; $j > $count; $j--)
#  { 
#    $new_file = "0$new_file";
#  }
# print "mv $file $new_file.jpg\n";
# `mv $file $new_file.jpg`;
#  $i++;
#}
