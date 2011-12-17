#!/usr/bin/perl
	use lib '/System/Library/Perl/Extras/5.8.9/darwin-thread-multi-2level' ;

  	use Mac::Processes;

           while ( ($psn, $psi) = each(%Process) ) {
               print "$psn\t",
                      $psi->processName,       " ",
                      $psi->processNumber,     " ",
                      $psi->processType,       " ",
                      $psi->processSignature,  " ",
                      $psi->processSize,       " ",
                      $psi->processMode,       " ",
                      $psi->processLocation,   " ",
                      $psi->processLauncher,   " ",
                      $psi->processLaunchDate, " ",
                      $psi->processActiveTime, " ",
                      $psi->processAppSpec,    "\n";
           }

