#!/opt/local/bin/perl -w

use Mac::Finder::DSStore qw( writeDSDBEntries makeEntries );

&writeDSDBEntries("Robin",
    &makeEntries(".VolumeIcon.icns", Iloc_xy => [ 286, 320, 0, 9, 65535, 31296 ]),
    &makeEntries(".background.png", Iloc_xy => [ 183, 295, 0, 8, 65535, 31296 ]),
    &makeEntries("Applications", Iloc_xy => [ 308, 149, 0, 5 ]),
    &makeEntries("Phatch.app", Iloc_xy => [ 109, 153, 0, 4 ]),
    &makeEntries("background.png", Iloc_xy => [ 351, 60 ])
);

