#!/usr/bin/perl
# copy all $ext files in the current dir tree in one dest 
# dir specified by $destdir
# handy to look at all the icons available

$ext = "png";
$destdir ="tmp/";

@toto = `find . -name "*.$ext"`;

foreach my $file (@toto) {
  chop $file;
#  print "- $file -\n";
  @fields = split (/\//, $file);
  for my $element (@fields)
  {
    $element =~s/\.png//;
    $element =~s/\.//;
    $new_name = "$element\-$new_name";
  }
  $new_name = "$new_name.$ext";
  print "cp $file $destdir$new_name\n";
  `cp $file $destdir$new_name`;
  $new_name = "";
}
