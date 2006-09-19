#!/usr/bin/perl


($filename) = @ARGV;

open (IN, $filename) || die "cannot open $filename for reading : $!";


while (<IN>)
{
  chop $_;
  $Directories = "$Directories$_,";
  $size = $_;
  $size =~ s/x.*//;
  $context = $_;
  # chop
  $context =~ s/.*\///;
  # uppercase 1st letter
  $context = "\u$context";

  $section = "[$_]\nSize=$size\nContext=$context\nType=Threshold\n\n";
  $ThemeBuffer = "$ThemeBuffer$section";

  
#  print "* $_ *";
}

$Directories="Directories=$Directories";

$ThemeBuffer="[Icon Theme]\nName=nimbus\nInherits=blueprint\n$Directories\n$ThemeBuffer";

print "$ThemeBuffer";

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
