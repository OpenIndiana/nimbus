#!/usr/bin/perl
# usage : in icons dir :  ./utils/create-makefiles.pl ./utils/dir.list.icon-name-mapping

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
    chdir ("$pwd/$dir/$_");

    
    $filename="$pwd/$dir/$_/Makefile.am";

    printf "creating $filename\n";

    $Makefile = "size = $dir\n";
    $Makefile .= "context = $_\n";

    $Makefile .= "iconsdir = \$\(themedir)/\$\(size)/\$\(context)\n";

    $Makefile .= "icons_DATA = \t\t\t\t";
    $list=`ls *.png *.icon`;
    @listoffiles=split (/\n/, $list);
    foreach my $file (@listoffiles) {
      $Makefile .= " \t\t\\\n\t\t$file";
    }
    $Makefile .= "\n";

    $Makefile .= "EXTRA_DIST = \$\(icons_DATA)\n\n";

    $Makefile .= "install-data-local: install-iconsDATA\n";

    $Makefile .= "\t(cd \$\(DESTDIR)\$\(themedir)/\$\(size) && \$\(ICONMAP) -c \$\(context) -e \$\(iconsrcdir)/icons/my-mapping.xml)\n";

    $Makefile .= "\nuninstall-local:\n";
    $Makefile .= "\trm -Rf \$\(DESTDIR)\$\(themedir)/\$\(size)/\$\(context)\n";


    open (FILE,">$filename") || warn "couldn't open $filename\n";
    print FILE "$Makefile";
    close (FILE);
    printf $Makefile;
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
