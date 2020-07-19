my $line;
my $first;
my $second;
my @first_list;
my $elem;

while(($line = <STDIN>)) {
    $line =~ /^\"(.*)\",\"(.*)$/;
    $first = $1;    
    $second = $2;
    @first_list = split /;/, $first;
    foreach $elem (@first_list) {
	$elem =~ s/^\W(.*)$/$1/;
	print '"'.$elem.'","'.$second."\n";
    }
}
