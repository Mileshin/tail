#!/usr/bin/perl
use strict;
use warnings;
use v5.010;
use Getopt::Std;

# If set to true, exit script after processing --help or --version flags
$Getopt::Std::STANDARD_HELP_VERSION = 1;
our $VERSION = "0.1";
my $opt_string ='b:c:n:qv';
my $usage = 'usage: tail [-n # | -c # | -b #] [-q | -v] [file ...]';
my $pattern = '^(-|\+)?(\d+)$';
my $patternp = '^\+(\d+)$';

# Called by Getopt::Std when supplying --help option
sub HELP_MESSAGE {
  say $usage;
}

# Called by Getopt::Std when supplying --version or --help option
sub VERSION_MESSAGE {
  say "Version $VERSION";
}

my %opts;
my $num_of_keys = 0;
my $count = 10;
getopts($opt_string, \%opts) or die $usage;
# Check for the number of options
die $usage if 1 < grep { $_ =~ /^b|c|n$/ } keys %opts;
die $usage if ($#ARGV < 0);

for my $i (0 .. $#ARGV) {
   my @lines;
   my $file;
   if (($ARGV[$i] eq '-') and ($#ARGV eq 0)) {
      $file =  *STDIN;
   } else {
      open ($file,"<", $ARGV[$i]) 
         or print STDERR "can't open $ARGV[$i]:  $! \n" and next;
   }
   print "\n==> $ARGV[$i] <==\n" if (!(defined $opts{q}) and ($#ARGV > 0));   
### bytes ###
   if ((defined $opts{c}) or (defined $opts{b}) ) {
      my ($data, $end, $offset);
      $count = 0;
      $count =  abs $opts{c} if ((defined $opts{c}) 
                                 and ($opts{c} =~ /$pattern/));
      $count =  abs $opts{b} * 512 if ((defined $opts{b}) 
                                 and ($opts{b} =~ /$pattern/));
      die $usage  if !($count);
      seek $file, 0, 2;
      $end = tell $file;
      $end < $count ? $offset = 0 : $offset = $end - $count;
      seek $file, $offset, 0;
      read $file, $data, $count;
      print $data;
   }
### lines ###   
   elsif (defined $opts{n}) {
      die $usage unless $opts{n} =~ /$pattern/;
      $count = abs $opts{n};
      if ($opts{n} =~ /$patternp/) {
         print while defined($_ = <$file>) and $. <= $count;
      } else {
         goto DEFAULT;
      }
   } else {
DEFAULT:
      while (<$file>) {
         push @lines, $_;
         shift @lines if ($. > $count); 
      }
      print @lines;
   }
   close ($file);
}
