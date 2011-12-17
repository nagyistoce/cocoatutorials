#!/opt/local/bin/perl -w

use Mac::Finder::DSStore qw( writeDSDBEntries makeEntries );
use Mac::Memory qw( );
use Mac::Files qw( NewAliasMinimal );

&writeDSDBEntries("/Volumes/Firefox/.DS_Store",
    &makeEntries(" ", Iloc_xy => [ 320, 152 ]),
    &makeEntries(".",
        BKGD_alias => NewAliasMinimal("/Volumes/Firefox/.background/background.png"),
        ICVO => 1,
        fwi0_flds => [ 95, 362, 390, 777, "icnv", 0, 0 ],
        fwsw => 229,
        fwvh => 295,
        icgo => "\0\0\0\0\0\0\0\4",
        icvo => pack('A4 n A4 A4 n*', "icv4", 128, "none", "botm", 0, 0, 0, 0, 4, 0),
        icvt => 12
    ),
    &makeEntries("Firefox.app", Iloc_xy => [ 95, 152 ])
);

