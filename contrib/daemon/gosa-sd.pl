#!/usr/bin/perl 
#===============================================================================
#
#         FILE:  gosa-support-daemon.pl
#
#        USAGE:  ./.gosa-support-daemon.pl
#
#  DESCRIPTION:  
#
#      OPTIONS:  ---
# REQUIREMENTS:  ---
#         BUGS:  ---
#        NOTES:  ---
#       AUTHOR:   Andreas Rettenberger, <rettenberger@gonicus.de>
#      COMPANY:  Gonicus GmbH, Arnsberg
#      VERSION:  1.0
#      CREATED:  21.08.2007 15:13:51 CEST
#     REVISION:  ---
#===============================================================================

use strict;
use warnings;
use Getopt::Long;
use Config::IniFiles;
use POSIX;
use Fcntl;
use Net::LDAP;
use Net::LDAP::LDIF;
use Net::LDAP::Entry;
use Switch;
use MIME::Base64;



my ($verbose, $cfg_file, $log_file, $pid_file, $foreground); 
my ($timeout, $mailto, $mailfrom, $user, $group);
my ($procid, $pid, $loglevel);
my ($fifo_path, $max_process_timeout, $max_process );
my %daemon_children;
my ($ldap, $bind_phrase, $password, $ldap_base) ;

$procid = -1 ;
$foreground = 0 ;
$verbose = 0 ;
$max_process = 1 ;
$max_process_timeout = 1 ;
$ldap_base = "dc=gonicus,dc=de" ;
#$ldap_path = "/var/run/gosa-support-daemon.socket";
#$log_path = "/var/log/gosa-support-daemon.log";
#$pid_path = "/var/run/gosa-support-daemon/gosa-support-daemon.pid";

#---------------------------------------------------------------------------
#  parse commandline options
#---------------------------------------------------------------------------
Getopt::Long::Configure( "bundling" );
GetOptions( "v|verbose+" => \$verbose,
        "c|config=s" => \$cfg_file,
        "h|help" => \&usage,
        "l|logfile=s" => \$log_file,
        "p|pid=s" => \$pid_file,
        "f|foreground" => \$foreground);

#---------------------------------------------------------------------------
#  read and set config parameters
#---------------------------------------------------------------------------
my %cfg_defaults =
("Allgemein" =>
 {"timeout"  => [ \$timeout, 1000 ],
 "mailto"   => [ \$mailto, 'root@localhost' ],
 "mailfrom" => [ \$mailfrom, 'sps-daemon@localhost' ],
 "user"     => [ \$user, "nobody" ],
 "group"    => [ \$group, "nogroup" ],
 "fifo_path" => [ \$fifo_path, "/home/rettenbe/gonicus/gosa-support/tmp/fifo" ],
 "log_file"  => [ \$log_file, "/home/rettenbe/gonicus/gosa-support/tmp/gosa-support.log" ],
 "pid_file"  => [ \$pid_file, "/home/rettenbe/gonicus/gosa-support/tmp/gosa-support.pid" ],
 "loglevel"     => [ \$loglevel, 1]
 },
"LDAP"  =>
    {"bind" => [ \$bind_phrase, "cn=ldapadmin,dc=gonicus,dc=de" ],
     "password" => [ \$password, "tester" ],
    }
 );
&read_configfile;


#===  FUNCTION  ================================================================
#         NAME:  check_cmdline_param
#      PURPOSE:  checks all commandline parameters to validity
#   PARAMETERS:  none
#      RETURNS:  none
#  DESCRIPTION:  ????
#       THROWS:  no exceptions
#     COMMENTS:  none
#     SEE ALSO:  n/a
#===============================================================================
sub check_cmdline_param () {
    my $err_config;
    my $err_log;
    my $err_pid;
    my $err_counter = 0;
    if( not defined( $cfg_file)) {
        $err_config = "please specify a config file";
        $err_counter += 1;
    }
    if( not defined( $log_file)) {
        $err_log = "please specify a log file";
        $err_counter += 1;
    }
    if( not defined( $pid_file)) {
        $err_pid = "please specify a pid file";
        $err_counter += 1;
    }
    if( $err_counter > 0 ) {
        &usage( "", 1 );
        if( defined( $err_config)) { print STDERR "$err_config\n"}
        if( defined( $err_log)) { print STDERR "$err_log\n" }
        if( defined( $err_pid)) { print STDERR "$err_pid\n"}
        print STDERR "\n";
        exit( -1 );
    }
}

#===  FUNCTION  ================================================================
#         NAME:  check_pid
#      PURPOSE:  
#   PARAMETERS:  none
#      RETURNS:  none
#  DESCRIPTION:  ????
#       THROWS:  no exceptions
#     COMMENTS:  none
#     SEE ALSO:  n/a
#===============================================================================
sub check_pid { 
    if( open( LOCK_FILE, "<$pid_file") ) {
        $procid = <LOCK_FILE>;
        if( defined $procid ) {
            chomp( $procid );
            if( -f "/proc/$procid/stat" ) {
                my($stat) = `cat /proc/$procid/stat` =~ m/$procid \((.+)\).*/;
                print "\t".$stat."\n";
                if( "sps-daemon.pl" eq $stat ) {
                    close( LOCK_FILE );
                    exit -1;
                }
            }
        }
        close( LOCK_FILE );
        unlink( $pid_file );
    }

    # Try to open PID file
    if (!sysopen(LOCK_FILE, $pid_file, O_WRONLY|O_CREAT|O_EXCL, 0644)) {
        my($msg) = "Couldn't obtain lockfile '$pid_file' ";
        if (open(LOCK_FILE, "<", $pid_file) && ($pid = <LOCK_FILE>)) {
            chomp($pid);
            $msg .= "(PID $pid)\n";
        } else {
            $msg .= "(unable to read PID)\n";
        }
        if ( ! $foreground ) {
            daemon_log( $msg."\n");
        } else {
            print( STDERR " $msg " );
        }
        exit( -1 );
    }
}

#===  FUNCTION  ================================================================
#         NAME:  read_configfile
#      PURPOSE:  read the configuration file and provide the programm with 
#                parameters
#   PARAMETERS:  none
#      RETURNS:  none
#  DESCRIPTION:  ????
#       THROWS:  no exceptions
#     COMMENTS:  none
#     SEE ALSO:  n/a
#===============================================================================
sub read_configfile {
    my $log_time = localtime(time);
    my $cfg;
    if( defined( $cfg_file) && ( length($cfg_file) > 0 )) {
        if( -r $cfg_file ) { 
            $cfg = Config::IniFiles->new( -file => $cfg_file ); 
        } else { usage( "Couldn't read config file: $cfg_file \n" ); }
    } else { $cfg = Config::IniFiles->new() ; }

    foreach my $section (keys %cfg_defaults) {      # "Parse" config into values
        foreach my $param (keys %{$cfg_defaults{ $section }}) {
            my $pinfo = $cfg_defaults{ $section }{ $param };
            ${@$pinfo[ 0 ]} = $cfg->val( $section, $param, @$pinfo[ 1 ] );
        }
    }

    if(-e $log_file ) { unlink $log_file }
    daemon_log("$log_time: config file read\n");
}

#===  FUNCTION  ================================================================
#         NAME:  daemon_log
#      PURPOSE:  log messages to specified logfile
#   PARAMETERS:  $msg, $level
#      RETURNS:  ????
#  DESCRIPTION:  Takes a message ($msg) and append it to the logfile. The 
#                standard log-level ($level) is 1. Messages whith higher level
#                than the verbosity-level (defined by commandline) are printed 
#                out to commandline. Messages with log-level lower than 2 are 
#                not logged to logfile!
#       THROWS:  no exceptions
#     COMMENTS:  none
#     SEE ALSO:  n/a
#===============================================================================
sub daemon_log {
    my( $msg, $level ) = @_;
    if(not defined $msg) { return } 
    if(not defined $level) { $level = 1 }
    open(LOG_HANDLE, ">>$log_file");
    if(not defined open( LOG_HANDLE, ">>$log_file" ) ) { return }
    chomp($msg);
    if( $verbose >= $level ) { print "$msg"."\n" }
    if( $level <= 1 ) { print LOG_HANDLE $msg."\n"  }
    close( LOG_HANDLE );
    }

#===  FUNCTION  ================================================================
#         NAME:  signal handler
#      PURPOSE:  catches signals from the programm and do diffrent things 
#                than default
#   PARAMETERS:  none
#      RETURNS:  none
#  DESCRIPTION:  sighandler
#       THROWS:  no exceptions
#     COMMENTS:  none
#     SEE ALSO:  n/a
#===============================================================================
sub sigINT {
    my $log_time = localtime(time);
    print "INT\n";
    if( -p $fifo_path ) {
        close FIFO  ;
        unlink($fifo_path) ;
        daemon_log( "$log_time: FIFO closed after signal INT!\n") ;
        }
    if(defined($ldap)) {
        $ldap->unbind;
    }
    $SIG{INT} = "DEFAULT" ;
    kill INT => $$ ;
}
$SIG{INT} = \&sigINT ;

#===  FUNCTION  ================================================================
#         NAME:  usage
#      PURPOSE:  
#   PARAMETERS:  none
#      RETURNS:  none
#  DESCRIPTION:  print out the usage of the program
#       THROWS:  no exceptions
#     COMMENTS:  none
#     SEE ALSO:  n/a
#===============================================================================
sub usage {
        my( $text, $help ) = @_;
        $text = undef if( "h" eq $text );
        (defined $text) && print STDERR "\n$text\n";
        if( (defined $help && $help) || (!defined $help && !defined $text) ) {
                print STDERR << "EOF" ;
usage: $0 [-hvf] [-c config, -l logfile, -p pidfile]

    -h        : this (help) message
    -c <file> : config file
    -l <file> : log file (example: /var/log/sps/sps.log)
    -p <file> : pid file (example: /var/run/sps/sps.pid)
    -f        : foreground (don"t fork)
    -v        : be verbose (multiple to increase verbosity)
EOF
        }
        print "\n" ;
}


#===  FUNCTION  ================================================================
#         NAME:  open_fifo
#      PURPOSE:  
#   PARAMETERS:  $fifo_path
#      RETURNS:  0: FIFO couldn"t be setup, 1: FIFO setup correctly
#  DESCRIPTION:  creates a FIFO at $fifo_path
#       THROWS:  no exceptions
#     COMMENTS:  none
#     SEE ALSO:  n/a
#===============================================================================
sub open_fifo {
    my ($fifo_path) = @_ ;
    my $log_time = localtime( time );
    if( -p $fifo_path ) {
        daemon_log("$log_time: FIFO at $fifo_path already exists\n");
        return 0;
        } 
    POSIX::mkfifo($fifo_path, 0666) or die "can't mkfifo $fifo_path: $!";
    daemon_log( "$log_time: FIFO started at $fifo_path\n" ) ;
    return 1;
    }


#===  FUNCTION  ================================================================
#         NAME:  add_ldap_entry
#      PURPOSE:  adds an element to ldap-tree
#   PARAMETERS:  
#      RETURNS:  none
#  DESCRIPTION:  ????
#       THROWS:  no exceptions
#     COMMENTS:  none
#     SEE ALSO:  n/a
#===============================================================================
sub add_ldap_entry {
    my( $mac, $gotoSysStatus, $ip, $interface, $desc ) = @_;
    #my $dn = encode_base64("cn=$mac,ou=incoming,$ldap_base", "");
    my $dn = "cn=$mac,ou=incoming,$ldap_base";

    # create LDAP entry 
    my $entry = Net::LDAP::Entry->new( $dn );
    $entry->dn($dn);
    $entry->add("objectClass" => "goHard");
    $entry->add("cn" => $mac);
    if(defined $gotoSysStatus) {$entry->add("gotoSysStatus" => $gotoSysStatus)}
    if(defined $ip) {$entry->add("ipHostNumber" => $ip) }
    #if(defined $interface) { }
    if(defined $desc) {$entry->add("description" => $desc) }
    
    # submit entry to LDAP
    my $result = $entry->update ( $ldap ); 
        
    # for $result->code constants please look at Net::LDAP::Constant
    my $log_time = localtime( time );
    if($result->code == 68) {   # entry already exists 
        daemon_log("WARNING: $log_time: $dn ".$result->error, 3);
    } elsif($result->code == 0) {   # everything went fine
        daemon_log("$log_time: add entry $dn to ldap", 1);
    } else {  # if any other error occur
        daemon_log("ERROR: $log_time: $dn, ".$result->code.", ".$result->error, 1);
    }
    return;
}


#===  FUNCTION  ================================================================
#         NAME:  change_ldap_entry
#      PURPOSE:  ????
#   PARAMETERS:  ????
#      RETURNS:  ????
#  DESCRIPTION:  ????
#       THROWS:  no exceptions
#     COMMENTS:  none
#     SEE ALSO:  n/a
#===============================================================================
sub change_ldap_entry {
    my ($mac, $gotoSysStatus ) = @_;
    #my $dn = encode_base64("cn=$mac,ou=incoming,$ldap_base", "");
    my $dn = "cn=$mac,ou=incoming,$ldap_base";
    my $result = $ldap->modify( $dn, replace => {'gotoSysStatus' => $gotoSysStatus } );

    # for $result->code constants please look at Net::LDAP::Constant
    my $log_time = localtime( time );
    if($result->code == 32) {   # entry doesnt exists 
        &add_ldap_entry($mac, $gotoSysStatus);
    } elsif($result->code == 0) {   # everything went fine
        daemon_log("$log_time: entry $dn changed successful", 1);
    } else {  # if any other error occur
        daemon_log("ERROR: $log_time: $dn, ".$result->code.", ".$result->error, 1);
    }

    return;
}
# check wether entry exists
    # if multiple time
    #   raise error
    # if true
    #   change attr of entry
    # if false
    #   add entry to ldap (invoke add_ldap_entry)
    #


#========= MAIN = main ========================================================
daemon_log( "####### START DAEMON ######\n", 1 );
#&check_cmdline_param ;
&check_pid;
&open_fifo($fifo_path);

# Just fork, if we"re not in foreground mode
if( ! $foreground ) { $pid = fork(); }
else { $pid = $$; }

# Do something useful - put our PID into the pid_file
if( 0 != $pid ) {
    open( LOCK_FILE, ">$pid_file" );
    print LOCK_FILE "$pid\n";
    close( LOCK_FILE );
    if( !$foreground ) { exit( 0 ) };
}


if( not -p $fifo_path ) { die "fifo file disappeared\n" }
sysopen(FIFO, $fifo_path, O_RDONLY) or die "can't read from $fifo_path: $!" ;

while( 1 ) {
    # wenn FIFO nicht offen, dann öffne ihn

    # checke alle prozesse im hash daemon_children ob sie noch aktiv sind, wenn
    # nicht, dann entferne prozess aus hash
    while( (my $key, my $val) = each( %daemon_children) ) {
        my $status = waitpid( $key, &WNOHANG) ;
        if( $status == -1 ) { 
            delete $daemon_children{$key} ; 
            daemon_log("childprocess finished: $key", 3) ;
        }
    }

    # ist die max_process anzahl von prozesskindern erreicht, dann warte und 
    # prüfe erneut, ob in der zwischenzeit prozesse fertig geworden sind
    if( keys( %daemon_children ) >= $max_process ) { 
        sleep($max_process_timeout) ;
        next ;
    }

    my $msg = <FIFO>;
    if( not defined( $msg )) { next ; }
    
    chomp( $msg );
    if( length( $msg ) == 0 ) { next ; }

    my $forked_pid = fork();
#=== PARENT = parent ==========================================================
    if ( $forked_pid != 0 ) { 
        daemon_log("childprocess forked: $forked_pid", 3) ;
        $daemon_children{$forked_pid} = 0 ;
    }
#=== CHILD = child ============================================================
    else {
        # parse the incoming message from arp, split the message and return 
        # the values in an array. not defined values are set to "none" 
        #my ($mac, $ip, $interface, $arp_sig, $desc) = &parse_input( $msg ) ;
        daemon_log( "childprocess read from arp: $fifo_path\nline: $msg", 3);
        my ($mac, $ip, $interface, $arp_sig, $desc) = split('\s', $msg, 5);

        # create connection to LDAP
        $ldap = Net::LDAP->new( "localhost" ) or die "$@";
        $ldap->bind($bind_phrase,
                    password => $password,
                    ) ;
        
        switch($arp_sig) {
            case 0 {&change_ldap_entry($mac, "ip-changed")} 
            case 1 {&change_ldap_entry($mac, "mac-not-whitelisted")}
            case 2 {&change_ldap_entry($mac, "mac-in-blacklist")}
            case 3 {&add_ldap_entry($mac, "new-mac-address", $ip, $interface,
                                    $desc )}
            case 4 {&change_ldap_entry($mac, "unauthorized-arp-request")}
            case 5 {&change_ldap_entry($mac, "abusive-number-of-arp-requests")}
            case 6 {&change_ldap_entry($mac, "ether-and-arp-mac-differs")}
            case 7 {&change_ldap_entry($mac, "flood-detected")}
            case 8 {&add_ldap_entry($mac, $ip, "new-system")}
            case 9 {&change_ldap_entry($mac, "mac-changed")}
        }


        # ldap search
#        my $base_phrase = "dc=gonicus,dc=de";
#        my $filter_phrase = "cn=keinesorge";
#        my $attrs_phrase = "cn macAdress";
#        my $msg_search = $ldap->search( base   => $base_phrase,
#                                        filter => $filter_phrase,
#                                        attrs => $attrs_phrase,
#                                        );
#        $msg_search->code && die $msg_search->error;
#        
#        my @entries = $msg_search->entries;
#        my $max = $msg_search->count;
#        print "anzahl der entries: $max\n";
#        my $i;
#        for ( $i = 0 ; $i < $max ; $i++ ) {
#            my $entry = $msg_search->entry ( $i );
#            foreach my $attr ( $entry->attributes ) {
#                if( not $attr eq "cn") {
#                    next;
#                }
#                print join( "\n ", $attr, $entry->get_value( $attr ) ), "\n\n";
#            }
#        }

        # ldap add
       
        
        sleep( 1 ) ;
        $ldap->unbind;
        exit;
    }

}


