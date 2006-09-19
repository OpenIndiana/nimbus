#!/usr/bin/perl

$ext = "png";

@toto = `find . -name "*.$ext"`;

foreach my $file (@toto) {
  chop $file;
#  print "- $file -\n";
  @fields = split (/\//, $file);
  for my $element (@fields)
  {
    $element =~s/\.png//;
    $element =~s/\.//;
    $new_name = "$element#$new_name";
  }
  $new_name = "$new_name.$ext";
  print "cp $file $new_name\n";
  `cp $file $new_name`;
  $new_name = "";
}
