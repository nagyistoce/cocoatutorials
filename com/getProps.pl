#!/usr/bin/perl

use Win32::OLE qw(in EVENTS);
use Win32::OLE::Variant;
use Time::HiRes qw(gettimeofday tv_interval);
use Data::Dumper;



my $filename = "c:\Users\rmills\Pictures\TripHome\GettingHome\IMG_1953.jpg";


my $dso = Win32::OLE->new('DSOFile.OleDocumentProperties');
   $dso->Open($filename,1,0) ;
   
print $dso

# Dim dso As      DSOFile.OleDocumentProperties
# dso       = New DSOFile.OleDocumentProperties
# dso.Open(filename.Trim, True,      DSOFile.dsoFileOpenOptions.dsoOptionOpenReadOnlyIfNoWriteAccess)
# Console.WriteLine(dso.SummaryProperties.Author)
# Console.WriteLine(dso.SummaryProperties.ByteCount)
# Console.WriteLine(dso.SummaryProperties.CharacterCount)
# Console.WriteLine(dso.SummaryProperties.CharacterCountWithSpaces)
# Console.WriteLine(dso.SummaryProperties.Comments)
# Console.WriteLine(dso.SummaryProperties.Company)
# Console.WriteLine(dso.SummaryProperties.DateCreated)
# Console.WriteLine(dso.SummaryProperties.DateLastSaved)
# Console.WriteLine(dso.SummaryProperties.LastSavedBy)
# Console.WriteLine(dso.SummaryProperties.LineCount)
# Console.WriteLine(dso.SummaryProperties.PageCount)
# Console.WriteLine(dso.SummaryProperties.ParagraphCount)
# Console.WriteLine(dso.SummaryProperties.RevisionNumber)
# Console.WriteLine(dso.SummaryProperties.Subject)
# Console.WriteLine(dso.SummaryProperties.Title)
# Console.WriteLine(dso.SummaryProperties.WordCount)


