my $line;
my $first;
my $second;

while(($line = <STDIN>)) {
    $line =~ /^(.*)\",\"(.*)$/g;
    $first = $1;
    $second = $2;
    $first =~ s/,/;/g;
    $line = $first.'","'.$second."\n";
    #$line =~ s/(.*?),(.*)\",\"/$1;$2\",\"/g;
    print $line;
}
