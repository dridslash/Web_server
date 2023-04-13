#!/usr/bin/perl

use CGI;

my $cgi = CGI->new;

print $cgi->header("text/html");

if ($cgi->request_method eq "GET") {
  print "Name: " . $cgi->param("name") . "<br>";
  print "Email: " . $cgi->param("email") . "<br>";
}
elsif ($cgi->request_method eq "POST") {
  print "Name: " . $cgi->param("name") . "<br>";
  print "Email: " . $cgi->param("email") . "<br>";
}
else {
  print "Invalid request method";
}