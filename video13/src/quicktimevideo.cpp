// ***************************************************************** -*- C++ -*-
/*
 * Copyright (C) 2004-2013 Andreas Huggel <ahuggel@gmx.net>
 *
 * This program is part of the Exiv2 distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301 USA.
 */
/*
  File:      quicktimevideo.cpp
  Version:   $Rev$
  Author(s): Abhinav Badola for GSoC 2012 (AB) <mail.abu.to@gmail.com>
             Mahesh Hegde for GSoC 2013 <maheshmhegade at gmail dot com>
  History:   28-Jun-12, AB: created
  Credits:   See header file
 */
// *****************************************************************************
#include "rcsid_int.hpp"
EXIV2_RCSID("@(#) $Id$")

// *****************************************************************************
// included header files
#include "quicktimevideo.hpp"
#include "futils.hpp"
#include "basicio.hpp"
#include "tags.hpp"
// + standard includes
#include <cmath>
#include <iostream>
using namespace std;
// *****************************************************************************
// class member definitions
namespace Exiv2
{
namespace Internal
{

extern const TagVocabulary qTimeFileType[] =
{
    {   "3g2a", "3GPP2 Media (.3G2) compliant with 3GPP2 C.S0050-0 V1.0" },
    {   "3g2b", "3GPP2 Media (.3G2) compliant with 3GPP2 C.S0050-A V1.0.0" },
    {   "3g2c", "3GPP2 Media (.3G2) compliant with 3GPP2 C.S0050-B v1.0" },
    {   "3ge6", "3GPP (.3GP) Release 6 MBMS Extended Presentations" },
    {   "3ge7", "3GPP (.3GP) Release 7 MBMS Extended Presentations" },
    {   "3gg6", "3GPP Release 6 General Profile" },
    {   "3gp1", "3GPP Media (.3GP) Release 1 (probably non-existent)" },
    {   "3gp2", "3GPP Media (.3GP) Release 2 (probably non-existent)" },
    {   "3gp3", "3GPP Media (.3GP) Release 3 (probably non-existent)" },
    {   "3gp4", "3GPP Media (.3GP) Release 4" },
    {   "3gp5", "3GPP Media (.3GP) Release 5" },
    {   "3gp6", "3GPP Media (.3GP) Release 6 Streaming Servers" },
    {   "3gs7", "3GPP Media (.3GP) Release 7 Streaming Servers" },
    {   "CAEP", "Canon Digital Camera" },
    {   "CDes", "Convergent Design" },
    {   "F4A ", "Audio for Adobe Flash Player 9+ (.F4A)" },
    {   "F4B ", "Audio Book for Adobe Flash Player 9+ (.F4B)" },
    {   "F4P ", "Protected Video for Adobe Flash Player 9+ (.F4P)" },
    {   "F4V ", "Video for Adobe Flash Player 9+ (.F4V)" },
    {   "JP2 ", "JPEG 2000 Image (.JP2) [ISO 15444-1 ?]" },
    {   "JP20", "Unknown, from GPAC samples (prob non-existent)" },
    {   "KDDI", "3GPP2 EZmovie for KDDI 3G cellphones" },
    {   "M4A ", "Apple iTunes AAC-LC (.M4A) Audio" },
    {   "M4B ", "Apple iTunes AAC-LC (.M4B) Audio Book" },
    {   "M4P ", "Apple iTunes AAC-LC (.M4P) AES Protected Audio" },
    {   "M4V ", "Apple iTunes Video (.M4V) Video" },
    {   "M4VH", "Apple TV (.M4V)" },
    {   "M4VP", "Apple iPhone (.M4V)" },
    {   "MPPI", "Photo Player, MAF [ISO/IEC 23000-3]" },
    {   "MSNV", "MPEG-4 (.MP4) for SonyPSP" },
    {   "NDAS", "MP4 v2 [ISO 14496-14] Nero Digital AAC Audio" },
    {   "NDSC", "MPEG-4 (.MP4) Nero Cinema Profile" },
    {   "NDSH", "MPEG-4 (.MP4) Nero HDTV Profile" },
    {   "NDSM", "MPEG-4 (.MP4) Nero Mobile Profile" },
    {   "NDSP", "MPEG-4 (.MP4) Nero Portable Profile" },
    {   "NDSS", "MPEG-4 (.MP4) Nero Standard Profile" },
    {   "NDXC", "H.264/MPEG-4 AVC (.MP4) Nero Cinema Profile" },
    {   "NDXH", "H.264/MPEG-4 AVC (.MP4) Nero HDTV Profile" },
    {   "NDXM", "H.264/MPEG-4 AVC (.MP4) Nero Mobile Profile" },
    {   "NDXP", "H.264/MPEG-4 AVC (.MP4) Nero Portable Profile" },
    {   "NDXS", "H.264/MPEG-4 AVC (.MP4) Nero Standard Profile" },
    {   "NIKO", "Nikon" },
    {   "ROSS", "Ross Video" },
    {   "avc1", "MP4 Base w/ AVC ext [ISO 14496-12:2005]" },
    {   "caqv", "Casio Digital Camera" },
    {   "da0a", "DMB MAF w/ MPEG Layer II aud, MOT slides, DLS, JPG/PNG/MNG images" },
    {   "da0b", "DMB MAF, extending DA0A, with 3GPP timed text, DID, TVA, REL, IPMP" },
    {   "da1a", "DMB MAF audio with ER-BSAC audio, JPG/PNG/MNG images" },
    {   "da1b", "DMB MAF, extending da1a, with 3GPP timed text, DID, TVA, REL, IPMP" },
    {   "da2a", "DMB MAF aud w/ HE-AAC v2 aud, MOT slides, DLS, JPG/PNG/MNG images" },
    {   "da2b", "DMB MAF, extending da2a, with 3GPP timed text, DID, TVA, REL, IPMP" },
    {   "da3a", "DMB MAF aud with HE-AAC aud, JPG/PNG/MNG images" },
    {   "da3b", "DMB MAF, extending da3a w/ BIFS, 3GPP timed text, DID, TVA, REL, IPMP" },
    {   "dmb1", "DMB MAF supporting all the components defined in the specification" },
    {   "dmpf", "Digital Media Project" },
    {   "drc1", "Dirac (wavelet compression), encapsulated in ISO base media (MP4)" },
    {   "dv1a", "DMB MAF vid w/ AVC vid, ER-BSAC aud, BIFS, JPG/PNG/MNG images, TS" },
    {   "dv1b", "DMB MAF, extending dv1a, with 3GPP timed text, DID, TVA, REL, IPMP" },
    {   "dv2a", "DMB MAF vid w/ AVC vid, HE-AAC v2 aud, BIFS, JPG/PNG/MNG images, TS" },
    {   "dv2b", "DMB MAF, extending dv2a, with 3GPP timed text, DID, TVA, REL, IPMP" },
    {   "dv3a", "DMB MAF vid w/ AVC vid, HE-AAC aud, BIFS, JPG/PNG/MNG images, TS" },
    {   "dv3b", "DMB MAF, extending dv3a, with 3GPP timed text, DID, TVA, REL, IPMP" },
    {   "dvr1", "DVB (.DVB) over RTP" },
    {   "dvt1", "DVB (.DVB) over MPEG-2 Transport Stream" },
    {   "isc2", "ISMACryp 2.0 Encrypted File" },
    {   "iso2", "MP4 Base Media v2 [ISO 14496-12:2005]" },
    {   "isom", "MP4 Base Media v1 [IS0 14496-12:2003]" },
    {   "jpm ", "JPEG 2000 Compound Image (.JPM) [ISO 15444-6]" },
    {   "jpx ", "JPEG 2000 with extensions (.JPX) [ISO 15444-2]" },
    {   "mj2s", "Motion JPEG 2000 [ISO 15444-3] Simple Profile" },
    {   "mjp2", "Motion JPEG 2000 [ISO 15444-3] General Profile" },
    {   "mmp4", "MPEG-4/3GPP Mobile Profile (.MP4/3GP) (for NTT)" },
    {   "mp21", "MPEG-21 [ISO/IEC 21000-9]" },
    {   "mp41", "MP4 v1 [ISO 14496-1:ch13]" },
    {   "mp42", "MP4 v2 [ISO 14496-14]" },
    {   "mp71", "MP4 w/ MPEG-7 Metadata [per ISO 14496-12]" },
    {   "mqt ", "Sony / Mobile QuickTime (.MQV) US Patent 7,477,830 (Sony Corp)" },
    {   "niko", "Nikon" },
    {   "odcf", "OMA DCF DRM Format 2.0 (OMA-TS-DRM-DCF-V2_0-20060303-A)" },
    {   "opf2", "OMA PDCF DRM Format 2.1 (OMA-TS-DRM-DCF-V2_1-20070724-C)" },
    {   "opx2", "OMA PDCF DRM + XBS extensions (OMA-TS-DRM_XBS-V1_0-20070529-C)" },
    {   "pana", "Panasonic Digital Camera" },
    {   "qt  ", "Apple QuickTime (.MOV/QT)" },
    {   "sdv ", "SD Memory Card Video" },
    {   "ssc1", "Samsung stereoscopic, single stream" },
    {   "ssc2", "Samsung stereoscopic, dual stream" },
};

extern const TagVocabulary handlerClassTags[] =
{
    {   "dhlr", "Data Handler" },
    {   "mhlr", "Media Handler" }
};

extern const TagVocabulary handlerTypeTags[] =
{
    {   "alis", "Alias Data" },
    {   "crsm", "Clock Reference" },
    {   "hint", "Hint Track" },
    {   "ipsm", "IPMP" },
    {   "m7sm", "MPEG-7 Stream" },
    {   "mdir", "Metadata" },
    {   "mdta", "Metadata Tags" },
    {   "mjsm", "MPEG-J" },
    {   "ocsm", "Object Content" },
    {   "odsm", "Object Descriptor" },
    {   "sdsm", "Scene Description" },
    {   "soun", "Audio Track" },
    {   "text", "Text" },
    {   "tmcd", "Time Code" },
    {   "url ", "URL" },
    {   "vide", "Video Track" }
};

extern const TagVocabulary vendorIDTags[] =
{
    {   "FFMP", "FFmpeg" },
    {   "appl", "Apple" },
    {   "olym", "Olympus" },
    {   "GIC ", "General Imaging Co." },
    {   "fe20", "Olympus (fe20)" },
    {   "pana", "Panasonic" },
    {   "KMPI", "Konica-Minolta" },
    {   "kdak", "Kodak" },
    {   "pent", "Pentax" },
    {   "NIKO", "Nikon" },
    {   "leic", "Leica" },
    {   "pr01", "Olympus (pr01)" },
    {   "SMI ", "Sorenson Media Inc." },
    {   "mino", "Minolta" },
    {   "sany", "Sanyo" },
    {   "ZORA", "Zoran Corporation" },
    {   "niko", "Nikon" }
};

extern const TagVocabulary cameraByteOrderTags[] =
{
    {   "II", "Little-endian (Intel, II)" },
    {   "MM", "Big-endian (Motorola, MM)" }
};

extern const TagDetails graphicsModetags[] =
{
    {   0x0, "srcCopy" },
    {   0x1, "srcOr" },
    {   0x2, "srcXor" },
    {   0x3, "srcBic" },
    {   0x4, "notSrcCopy" },
    {   0x5, "notSrcOr" },
    {   0x6, "notSrcXor" },
    {   0x7, "notSrcBic" },
    {   0x8, "patCopy" },
    {   0x9, "patOr" },
    {   0xa, "patXor" },
    {   0xb, "patBic" },
    {   0xc, "notPatCopy" },
    {   0xd, "notPatOr" },
    {   0xe, "notPatXor" },
    {   0xf, "notPatBic" },
    {   0x20, "blend" },
    {   0x21, "addPin" },
    {   0x22, "addOver" },
    {   0x23, "subPin" },
    {   0x24, "transparent" },
    {   0x25, "addMax" },
    {   0x26, "subOver" },
    {   0x27, "addMin" },
    {   0x31, "grayishTextOr" },
    {   0x32, "hilite" },
    {   0x40, "ditherCopy" },
    {   0x100, "Alpha" },
    {   0x101, "White Alpha" },
    {   0x102, "Pre-multiplied Black Alpha" },
    {   0x110, "Component Alpha" }
};

extern const TagDetails mediaLanguageCode[] =
{
    { 0x0, "English" },
    { 0x1, "French" },
    { 0x2, "German" },
    { 0x3, "Italian" },
    { 0x4, "Dutch" },
    { 0x5, "Swedish" },
    { 0x6, "Spanish" },
    { 0x7, "Danish" },
    { 0x8, "Portuguese" },
    { 0x9, "Norwegian" },
    { 0xa, "Hebrew" },
    { 0xb, "Japanese" },
    { 0xc, "Arabic" },
    { 0xd, "Finnish" },
    { 0xe, "Greek" },
    { 0xf, "Icelandic" },
    { 0x10, "Maltese" },
    { 0x11, "Turkish" },
    { 0x12, "Croatian" },
    { 0x13, "Traditional Chinese" },
    { 0x14, "Urdu" },
    { 0x15, "Hindi" },
    { 0x16, "Thai" },
    { 0x17, "Korean" },
    { 0x18, "Lithuanian" },
    { 0x19, "Polish" },
    { 0x1a, "Hungarian" },
    { 0x1b, "Estonian" },
    { 0x1c, "Lettish" },
    { 0x1c, "Latvian" },
    { 0x1d, "Saami" },
    { 0x1d, "Sami" },
    { 0x1e, "Faroese" },
    { 0x1f, "Farsi" },
    { 0x20, "Russian" },
    { 0x21, "Simplified Chinese" },
    { 0x22, "Flemish" },
    { 0x23, "Irish" },
    { 0x24, "Albanian" },
    { 0x25, "Romanian" },
    { 0x26, "Czech" },
    { 0x27, "Slovak" },
    { 0x28, "Slovenian" },
    { 0x29, "Yiddish" },
    { 0x2a, "Serbian" },
    { 0x2b, "Macedonian" },
    { 0x2c, "Bulgarian" },
    { 0x2d, "Ukrainian" },
    { 0x2e, "Belarusian" },
    { 0x2f, "Uzbek" },
    { 0x30, "Kazakh" },
    { 0x31, "Azerbaijani" },
    { 0x32, "AzerbaijanAr" },
    { 0x33, "Armenian" },
    { 0x34, "Georgian" },
    { 0x35, "Moldavian" },
    { 0x36, "Kirghiz" },
    { 0x37, "Tajiki" },
    { 0x38, "Turkmen" },
    { 0x39, "Mongolian" },
    { 0x3a, "MongolianCyr" },
    { 0x3b, "Pashto" },
    { 0x3c, "Kurdish" },
    { 0x3d, "Kashmiri" },
    { 0x3e, "Sindhi" },
    { 0x3f, "Tibetan" },
    { 0x40, "Nepali" },
    { 0x41, "Sanskrit" },
    { 0x42, "Marathi" },
    { 0x43, "Bengali" },
    { 0x44, "Assamese" },
    { 0x45, "Gujarati" },
    { 0x46, "Punjabi" },
    { 0x47, "Oriya" },
    { 0x48, "Malayalam" },
    { 0x49, "Kannada" },
    { 0x4a, "Tamil" },
    { 0x4b, "Telugu" },
    { 0x4c, "Sinhala" },
    { 0x4d, "Burmese" },
    { 0x4e, "Khmer" },
    { 0x4f, "Lao" },
    { 0x50, "Vietnamese" },
    { 0x51, "Indonesian" },
    { 0x52, "Tagalog" },
    { 0x53, "MalayRoman" },
    { 0x54, "MalayArabic" },
    { 0x55, "Amharic" },
    { 0x56, "Galla" },
    { 0x57, "Oromo" },
    { 0x58, "Somali" },
    { 0x59, "Swahili" },
    { 0x5a, "Kinyarwanda" },
    { 0x5b, "Rundi" },
    { 0x5c, "Nyanja" },
    { 0x5d, "Malagasy" },
    { 0x5e, "Esperanto" },
    { 0x80, "Welsh" },
    { 0x81, "Basque" },
    { 0x82, "Catalan" },
    { 0x83, "Latin" },
    { 0x84, "Quechua" },
    { 0x85, "Guarani" },
    { 0x86, "Aymara" },
    { 0x87, "Tatar" },
    { 0x88, "Uighur" },
    { 0x89, "Dzongkha" },
    { 0x8a, "JavaneseRom" }
};
extern const TagVocabulary userDatatags[] =
{
    {   "AllF", "PlayAllFrames" },
    {   "CNCV", "CompressorVersion" },
    {   "CNFV", "FirmwareVersion" },
    {   "CNMN", "Model" },
    {   "CNTH", "CanonCNTH" },
    {   "DcMD", "DcMD" },
    {   "FFMV", "FujiFilmFFMV" },
    {   "INFO", "SamsungINFO" },
    {   "LOOP", "LoopStyle" },
    {   "MMA0", "MinoltaMMA0" },
    {   "MMA1", "MinoltaMMA1" },
    {   "MVTG", "FujiFilmMVTG" },
    {   "NCDT", "NikonNCDT" },
    {   "PANA", "PanasonicPANA" },
    {   "PENT", "PentaxPENT" },
    {   "PXMN", "MakerNotePentax5b" },
    {   "PXTH", "PentaxPreview" },
    {   "QVMI", "CasioQVMI" },
    {   "SDLN", "PlayMode" },
    {   "SelO", "PlaySelection" },
    {   "TAGS", "KodakTags/KonicaMinoltaTags/MinoltaTags/NikonTags/OlympusTags/PentaxTags/SamsungTags/SanyoMOV/SanyoMP4" },
    {   "WLOC", "WindowLocation" },
    {   "XMP_", "XMP" },
    {   "Xtra", "Xtra" },
    {   "hinf", "HintTrackInfo" },
    {   "hinv", "HintVersion" },
    {   "hnti", "Hint" },
    {   "meta", "Meta" },
    {   "name", "Name" },
    {   "ptv ", "PrintToVideo" },
    {   "scrn", "OlympusPreview" },
    {   "thmb", "MakerNotePentax5a/OlympusThumbnail" },
};

extern const TagVocabulary userDataReferencetags[] =
{
    {   "CNCV", "Xmp.video.CompressorVersion" },
    {   "CNFV", "Xmp.video.FirmwareVersion" },
    {   "CNMN", "Xmp.video.Model" },
    {   "NCHD", "Xmp.video.MakerNoteType" },
    {   "WLOC", "Xmp.video.WindowLocation" },
    {   "SDLN", "Xmp.video.PlayMode" },
    {   "FFMV", "Xmp.video.StreamName" },
    {   "SelO", "Xmp.video.PlaySelection" },
    {   "name", "Xmp.video.Name" },
    {   "vndr", "Xmp.video.Vendor" },
    {   " ART", "Xmp.video.Artist" },
    {   " alb", "Xmp.video.Album" },
    {   " arg", "Xmp.video.Arranger" },
    {   " ark", "Xmp.video.ArrangerKeywords" },
    {   " cmt", "Xmp.video.Comment" },
    {   " cok", "Xmp.video.ComposerKeywords" },
    {   " com", "Xmp.video.Composer" },
    {   " cpy", "Xmp.video.Copyright" },
    {   " day", "Xmp.video.CreateDate" },
    {   " dir", "Xmp.video.Director" },
    {   " ed1", "Xmp.video.Edit1" },
    {   " ed2", "Xmp.video.Edit2" },
    {   " ed3", "Xmp.video.Edit3" },
    {   " ed4", "Xmp.video.Edit4" },
    {   " ed5", "Xmp.video.Edit5" },
    {   " ed6", "Xmp.video.Edit6" },
    {   " ed7", "Xmp.video.Edit7" },
    {   " ed8", "Xmp.video.Edit8" },
    {   " ed9", "Xmp.video.Edit9" },
    {   " enc", "Xmp.video.Encoder" },
    {   " fmt", "Xmp.video.Format" },
    {   " gen", "Xmp.video.Genre" },
    {   " grp", "Xmp.video.Grouping" },
    {   " inf", "Xmp.video.Information" },
    {   " isr", "Xmp.video.ISRCCode" },
    {   " lab", "Xmp.video.RecordLabelName" },
    {   " lal", "Xmp.video.RecordLabelURL" },
    {   " lyr", "Xmp.video.Lyrics" },
    {   " mak", "Xmp.video.Make" },
    {   " mal", "Xmp.video.MakerURL" },
    {   " mod", "Xmp.video.Model" },
    {   " nam", "Xmp.video.Title" },
    {   " pdk", "Xmp.video.ProducerKeywords" },
    {   " phg", "Xmp.video.RecordingCopyright" },
    {   " prd", "Xmp.video.Producer" },
    {   " prf", "Xmp.video.Performers" },
    {   " prk", "Xmp.video.PerformerKeywords" },
    {   " prl", "Xmp.video.PerformerURL" },
    {   " req", "Xmp.video.Requirements" },
    {   " snk", "Xmp.video.SubtitleKeywords" },
    {   " snm", "Xmp.video.Subtitle" },
    {   " src", "Xmp.video.SourceCredits" },
    {   " swf", "Xmp.video.SongWriter" },
    {   " swk", "Xmp.video.SongWriterKeywords" },
    {   " swr", "Xmp.video.SoftwareVersion" },
    {   " too", "Xmp.video.Encoder" },
    {   " trk", "Xmp.video.Track" },
    {   " wrt", "Xmp.video.Composer" },
    {   " xyz", "Xmp.video.GPSCoordinates" },
    {   "CMbo", "Xmp.video.CameraByteOrder" },
    {   "Cmbo", "Xmp.video.CameraByteOrder" },
};

extern const TagDetails NikonNCTGTags[] =
{
    {       0x0001, "Xmp.video.Make" },
    {       0x0002, "Xmp.video.Model" },
    {       0x0003, "Xmp.video.Software" },
    {       0x0011, "Xmp.video.CreationDate" },
    {       0x0012, "Xmp.video.DateTimeOriginal" },
    {       0x0013, "Xmp.video.FrameCount" },
    {       0x0016, "Xmp.video.FrameRate" },
    {       0x0022, "Xmp.video.FrameWidth" },
    {       0x0023, "Xmp.video.FrameHeight" },
    {       0x0032, "Xmp.audio.channelType" },
    {       0x0033, "Xmp.audio.BitsPerSample" },
    {       0x0034, "Xmp.audio.sampleRate" },
    {    0x1108822, "Xmp.video.ExposureProgram" },
    {    0x1109204, "Xmp.video.ExposureCompensation" },
    {    0x1109207, "Xmp.video.MeteringMode" },
    {    0x110a434, "Xmp.video.LensModel" },
    {    0x1200000, "Xmp.video.GPSVersionID" },
    {    0x1200001, "Xmp.video.GPSLatitudeRef" },
    {    0x1200002, "Xmp.video.GPSLatitude" },
    {    0x1200003, "Xmp.video.GPSLongitudeRef" },
    {    0x1200004, "Xmp.video.GPSLongitude" },
    {    0x1200005, "Xmp.video.GPSAltitudeRef" },
    {    0x1200006, "Xmp.video.GPSAltitude" },
    {    0x1200007, "Xmp.video.GPSTimeStamp" },
    {    0x1200008, "Xmp.video.GPSSatellites" },
    {    0x1200010, "Xmp.video.GPSImgDirectionRef" },
    {    0x1200011, "Xmp.video.GPSImgDirection" },
    {    0x1200012, "Xmp.video.GPSMapDatum" },
    {    0x120001d, "Xmp.video.GPSDateStamp" },
    {    0x2000001, "Xmp.video.MakerNoteVersion" },
    {    0x2000005, "Xmp.video.WhiteBalance" },
    {    0x200000b, "Xmp.video.WhiteBalanceFineTune" },
    {    0x200001e, "Xmp.video.ColorSpace" },
    {    0x2000023, "Xmp.video.PictureControlData" },
    {    0x2000024, "Xmp.video.WorldTime" },
    {    0x200002c, "Xmp.video.UnknownInfo" },
    {    0x2000032, "Xmp.video.UnknownInfo2" },
    {    0x2000039, "Xmp.video.LocationInfo" },
    {    0x2000083, "Xmp.video.LensType" },
    {    0x2000084, "Xmp.video.LensModel" },
    {    0x20000ab, "Xmp.video.VariProgram" },
};

extern const TagDetails NikonColorSpace[] =
{
    {       1, "sRGB" },
    {       2, "Adobe RGB" },
};

extern const TagVocabulary NikonGPS_Latitude_Longitude_ImgDirection_Reference[] =
{
    {   "N", "North" },
    {   "S", "South" },
    {   "E", "East" },
    {   "W", "West" },
    {   "M", "Magnetic North" },
    {   "T", "True North" },
};

extern const TagDetails NikonGPSAltitudeRef[] =
{
    {   0, "Above Sea Level" },
    {   1, "Below Sea Level" },
};

extern const TagDetails NikonExposureProgram[] =
{
    {   0, "Not Defined" },
    {   1, "Manual" },
    {   2, "Program AE" },
    {   3, "Aperture-priority AE" },
    {   4, "Shutter speed priority AE" },
    {   5, "Creative (Slow speed)" },
    {   6, "Action (High speed)" },
    {   7, "Portrait" },
    {   8, "Landscape" },
};

extern const TagDetails NikonMeteringMode[] =
{
    {   0, "Unknown" },
    {   1, "Average" },
    {   2, "Center-weighted average" },
    {   3, "Spot" },
    {   4, "Multi-spot" },
    {   5, "Multi-segment" },
    {   6, "Partial" },
    { 255, "Other" },
};

extern const TagDetails PictureControlAdjust[] =
{
    {   0, "Default Settings" },
    {   1, "Quick Adjust" },
    {   2, "Full Control" },
};

//! Contrast and Sharpness
extern const TagDetails NormalSoftHard[] =
{
    {   0, "Normal" },
    {   1, "Soft"   },
    {   2, "Hard"   }
};

//! Saturation
extern const TagDetails Saturation[] =
{
    {   0, "Normal" },
    {   1, "Low"    },
    {   2, "High"   }
};

//! YesNo, used for DaylightSavings
extern const TagDetails YesNo[] =
{
    {   0, "No"    },
    {   1, "Yes"   }
};

//! DateDisplayFormat
extern const TagDetails DateDisplayFormat[] =
{
    {   0, "Y/M/D" },
    {   1, "M/D/Y" },
    {   2, "D/M/Y" }
};

extern const TagDetails FilterEffect[] =
{
    {   0x80, "Off" },
    {   0x81, "Yellow"    },
    {   0x82, "Orange"   },
    {   0x83, "Red"   },
    {   0x84, "Green"   },
    {   0xff, "n/a"   },
};

extern const TagDetails ToningEffect[] =
{
    {   0x80, "B&W" },
    {   0x81, "Sepia" },
    {   0x82, "Cyanotype" },
    {   0x83, "Red" },
    {   0x84, "Yellow" },
    {   0x85, "Green" },
    {   0x86, "Blue-green" },
    {   0x87, "Blue" },
    {   0x88, "Purple-blue" },
    {   0x89, "Red-purple" },
    {   0xff, "n/a" },
};

extern const TagDetails whiteBalance[] =
{
    {   0, "Auto" },
    {   1, "Daylight" },
    {   2, "Shade" },
    {   3, "Fluorescent" },
    {   4, "Tungsten" },
    {   5, "Manual" },
};

enum movieHeaderTags
{
    MovieHeaderVersion, CreateDate, ModifyDate, TimeScale, Duration, PreferredRate, PreferredVolume,
    PreviewTime = 18, PreviewDuration,PosterTime, SelectionTime, SelectionDuration, CurrentTime, NextTrackID
};
enum trackHeaderTags
{
    TrackHeaderVersion, TrackCreateDate, TrackModifyDate, TrackID, TrackDuration = 5, TrackLayer = 8,
    TrackVolume, ImageWidth = 19, ImageHeight
};
enum mediaHeaderTags
{
    MediaHeaderVersion, MediaCreateDate, MediaModifyDate, MediaTimeScale, MediaDuration, MediaLanguageCode
};
enum handlerTags
{
    HandlerClass = 1, HandlerType, HandlerVendorID
};
enum videoHeaderTags
{
    GraphicsMode = 2, OpColor
};
enum stream
{
    Video, Audio, Hint, Null, GenMediaHeader
};
enum imageDescTags
{
    codec, VendorID = 4, SourceImageWidth_Height = 7,  XResolution,
    YResolution, CompressorName = 10, BitDepth
};
enum audioDescTags
{
    AudioFormat, AudioVendorID = 4, AudioChannels, AudioSampleRate = 7, MOV_AudioFormat = 13
};

/*!
      @brief Function used to check equality of a Tags with a
          particular string (ignores case while comparing).
      @param buf Data buffer that will contain Tag to compare
      @param str char* Pointer to string
      @return Returns true if the buffer value is equal to string.
     */
bool equalsQTimeTag(Exiv2::DataBuf& buf ,const char* str)
{
    int32_t i;
    for(i = 0; i < 4; ++i)
        if(tolower(buf.pData_[i]) != tolower(str[i]))
            return false;
    return true;
}

bool equalsQTimeTag(Exiv2::DataBuf& buf,const char arr[][5],int32_t arraysize)
{
    int32_t i,j;
    for (i=0; i< arraysize; i++)
    {
        bool matched = true;
        for(j = 0; j < 4; j++ )
        {
            if(tolower(buf.pData_[j]) != arr[i][j])
            {
                matched = false;
                break;
            }
        }
        if(matched)
        {
            return true;
        }
    }
}

/*!
      @brief Function used to ignore Tags and values stored in them,
          since they are not necessary as metadata information
      @param buf Data buffer that will contain Tag to compare
      @return Returns true, if Tag is found in the ignoreList[]
     */
bool ignoreList (Exiv2::DataBuf& buf)
{
    const char ignoreList[13][5] =
    {
        "mdat", "edts", "junk", "iods", "alis", "stsc", "stsz", "stco", "ctts", "stss",
        "skip", "wide", "cmvd",
    };

    int32_t i;
    for(i = 0 ; i < 13 ; ++i)
        if(equalsQTimeTag(buf, ignoreList[i]))
            return true;

    return false;
}

/*!
      @brief Function used to ignore Tags, basically Tags which
          contain other tags inside them, since they are not necessary
          as metadata information
      @param buf Data buffer that will contain Tag to compare
      @return Returns true, if Tag is found in the ignoreList[]
     */
bool dataIgnoreList (Exiv2::DataBuf& buf)
{
    const char ignoreList[8][5] =
    {
        "moov", "mdia", "minf", "dinf", "alis", "stbl", "cmov",
        "meta",
    };

    int32_t i;
    for(i = 0 ; i < 8 ; ++i)
        if(equalsQTimeTag(buf, ignoreList[i]))
            return true;

    return false;
}

/*!
 * \brief reverseTagVocabulary: reverse attributed of array elements.To perform reverse
 *        operation of getting keys from description
 * \param inputTagVocabulary
 * \param outputTagVocabulary
 * \param size
 */
void reverseTagVocabulary(const TagVocabulary inputTagVocabulary[],TagVocabulary  outputTagVocabulary[] ,int size)
{
    int32_t i;
    for (i=0; i<size ;i++)
    {
        outputTagVocabulary[i].label_ = inputTagVocabulary[i].voc_;
        outputTagVocabulary[i].voc_ = inputTagVocabulary[i].label_;
    }
}

/*!
 * \brief getNumberFromString
 * \param stringData: stringdata which is formed by concatinating two distinct
 *                    numerical values
 * \param seperator
 * \return
 */
const std::vector<uint16_t> getNumberFromString(const std::string stringData,char seperator )
{
    int32_t counter = (stringData.size() - 1);
    vector<uint16_t> shortValues;
    int32_t i;
    for(i=0; i<2; i++)
    {
        int32_t tmpValueIndex = 3;
        byte tmpValue[4] = {};
        while((counter >= 0) && (stringData[counter] != seperator) && (tmpValueIndex >=0))
        {
            tmpValue[tmpValueIndex] = stringData[counter];
            counter--;tmpValueIndex--;
        }
        counter--;
        shortValues.push_back((int16_t)Exiv2::getShort(tmpValue, bigEndian));
    }
    return shortValues;
}

/*!
 * \brief getLongFromString functionality is same as above method,with return type
 *        long vector
 * \param stringData: stringdata which is formed by concatinating two distinct
 *                    numerical values
 * \param seperator: seperation character like . , etc
 * \return
 */
const std::vector<int32_t> getLongFromString(const std::string stringData,char seperator )
{
    int32_t counter = (stringData.size() - 1);
    vector<int32_t> longValues;
    for(int32_t i=0; i<2; i++)
    {
        int32_t tmpValueIndex = 3;
        byte tmpValue[4]={};
        while((counter >= 0) && (stringData[counter] != seperator) && (tmpValueIndex >=0))
        {
            tmpValue[tmpValueIndex] = stringData[counter];
            counter--;tmpValueIndex--;
        }
        counter--;
        longValues.push_back((int32_t)Exiv2::getLong(tmpValue, bigEndian));
    }
    return longValues;
}
/*!
      @brief Function used to convert buffer data into 64-bit
          signed integer, information stored in Big Endian format
      @param buf Data buffer that will contain data to be converted
      @return Returns a signed 64-bit integer
     */
int64_t returnBufValue(Exiv2::DataBuf& buf, int32_t n = 4)
{
    int64_t temp = 0;
    int32_t i;
    for(i = n - 1; i >= 0; i--)
#ifdef _MSC_VER
        temp = temp + static_cast<int64_t>(buf.pData_[i]*(pow(static_cast<float>(256), n-i-1)));
#else
        temp = temp + buf.pData_[i]*(pow((float)256,n-i-1));
#endif

    return temp;
}

/*!
 * \brief returnBuf: converts integer value to byte value
 *                   which can be written to the file.
 * \param intValue
 * \param n
 * \return
 */
DataBuf returnBuf(int64_t intValue,int32_t n=4)
{
    DataBuf buf((uint32_t)(n+1));
    buf.pData_[n] = '\0';
    int32_t i;
    for(i = n - 1; i >= 0; i--)
    {
#ifdef _MSC_VER
        buf.pData_[i] = static_cast<byte> static_cast<uint16_t>
                static_cast<int32_t> uint16_t (intValue / (pow(static_cast<int32_t>(256), n-i-1))) % 256);
#else
        buf.pData_[i] = (byte)(int16_t) ((int32_t)(intValue / (int32_t)pow(256.0,n-i-1)) % 256);
#endif
    }
    return buf;
}
/*!
      @brief Function used to convert buffer data into 64-bit
          unsigned integer, information stored in Big Endian format
      @param buf Data buffer that will contain data to be converted
      @return Returns an unsigned 64-bit integer
     */
uint64_t returnUnsignedBufValue(Exiv2::DataBuf& buf, int32_t n = 4)
{
    uint64_t temp = 0;
    int32_t i;
    for( i = n-1; i >= 0; i--)
#if _MSC_VER
        temp = temp + static_cast<uint64_t>(buf.pData_[i]*(pow(static_cast<float>(256), n-i-1)));
#else
        temp = temp + buf.pData_[i]*(pow((float)256,n-i-1));
#endif

    return temp;
}

DataBuf returnUnsignedBuf(uint64_t intValue,int32_t n=4)
{
    DataBuf buf((uint32_t)(n+1));
    buf.pData_[n] = '\0';
    int32_t i;
    for(int32_t i = n - 1; i >= 0; i--)
    {
#ifdef _MSC_VER
        buf.pData_[i] = static_cast<byte> static_cast<uint16_t>
                static_cast<int32_t> uint16_t (intValue / (pow(static_cast<int32_t>(256), n-i-1))) % 256);
#else
        buf.pData_[i] = (byte)(uint16_t) ((int32_t)(intValue / (int32_t)pow(256.0,n-i-1)) % 256);
#endif
    }
    return buf;
}
/*!
      @brief Function used to quicktime files, by checking the
          the tags at the start of the file. If the Tag is any one
          of the tags listed below, then it is of Quicktime Type.
      @param a, b, c, d - characters used to compare
      @return Returns true, if Tag is found in the list qTimeTags
     */
bool isQuickTimeType (char a, char b, char c, char d)
{
    char qTimeTags[][5] =
    {
        "PICT", "free", "ftyp", "junk", "mdat",
        "moov", "pict", "pnot", "skip",  "uuid", "wide"
    };

    int32_t i;
    for( i = 0; i <= 10; i++)
        if(a == qTimeTags[i][0] && b == qTimeTags[i][1] && c == qTimeTags[i][2] && d == qTimeTags[i][3])
            return true;
    return false;
}

}

}                                      // namespace Internal, Exiv2

namespace Exiv2
{

using namespace Exiv2::Internal;

QuickTimeVideo::QuickTimeVideo(BasicIo::AutoPtr io)
    : Image(ImageType::qtime, mdNone, io)
{
    m_decodeMetadata = true;
    m_modifyMetadata = false;
} // QuickTimeVideo::QuickTimeVideo

/*!
 * \brief QuickTimeVideo::findAtomPositions:
 *                        This method can be called only after read operation,it will return
 *                        list atom(multiple atoms with same atom Id)
 *                        location and atom size,for a given atomId
 * \param atomId
 * \return
 */
std::vector< pair<uint32_t,uint32_t> > QuickTimeVideo::findAtomPositions(const char* atomId)
{
    DataBuf hdrId((uint32_t)5);
    hdrId.pData_[4] = '\0';

    std::vector< pair<uint32_t,uint32_t> > atomsDetails;
    for (uint64_t i=0; i < m_QuickSkeleton.size();i++)
    {
        io_->seek((m_QuickSkeleton[i]->m_AtomLocation - 4),BasicIo::beg);
        io_->read(hdrId.pData_,4);
        if(equalsQTimeTag(hdrId,atomId))
        {
            atomsDetails.push_back(make_pair((uint32_t)io_->tell(),
                                             (uint32_t)(m_QuickSkeleton[i]->m_AtomSize)));
        }
    }
    return atomsDetails;
}

std::string QuickTimeVideo::mimeType() const
{
    return "video/quicktime";
}

void QuickTimeVideo::writeMetadata()
{
    if (io_->open() != 0)
    {
        throw Error(9, io_->path(), strError());
    }
    IoCloser closer(*io_);
    doWriteMetadata();

    io_->close();
    return;
}

/*!
 * \brief QuickTimeVideo::doWriteMetadata
 *        Search for all the tags(effectively all supported tags in Exiv2)
 *        present in a file(in m_riffskeleton) and call corresponding handler
 *        method to write modified metadata.
 */
void QuickTimeVideo::doWriteMetadata()
{
    if (!io_->isopen())
    {
        throw Error(20);
    }
    // Ensure that this is the correct image type
    if (!isQTimeType(*io_, false))
    {
        if (io_->error() || io_->eof()) throw Error(14);
        throw Error(3, "QuickTime");
    }

    m_decodeMetadata = false;
    m_modifyMetadata = true;

    std::vector< pair<uint32_t,uint32_t> > ftypPositions = findAtomPositions("ftyp");
    uint64_t i;
    for(i=0; i<ftypPositions.size(); i++)
    {
        io_->seek(ftypPositions[i].first,BasicIo::beg);
        fileTypeDecoder(ftypPositions[i].second);
    }

    std::vector< pair<uint32_t,uint32_t> > mvhdPositions = findAtomPositions("mvhd");
    for(i=0; i<mvhdPositions.size(); i++)
    {
        io_->seek(mvhdPositions[i].first,BasicIo::beg);
        movieHeaderDecoder(mvhdPositions[i].second);
    }

    std::vector< pair<uint32_t,uint32_t> > trakPositions = findAtomPositions("trak");
    std::vector< pair<uint32_t,uint32_t> > tkhdPositions = findAtomPositions("tkhd");

    for(i=0; i<tkhdPositions.size(); i++)
    {
        io_->seek(trakPositions[i].first,BasicIo::beg);
        setMediaStream();
        io_->seek(tkhdPositions[i].first,BasicIo::beg);
        trackHeaderDecoder(tkhdPositions[i].second);
    }

    std::vector< pair<uint32_t,uint32_t> > mdhdPositions = findAtomPositions("mdhd");
    for(i=0; i<mdhdPositions.size(); i++)
    {
        io_->seek(trakPositions[i].first,BasicIo::beg);
        setMediaStream();
        io_->seek(mdhdPositions[i].first,BasicIo::beg);
        mediaHeaderDecoder(mdhdPositions[i].second);
    }

    std::vector< pair<uint32_t,uint32_t> > hdlrPositions = findAtomPositions("hdlr");
    for(i=0; i<hdlrPositions.size(); i++)
    {
        io_->seek(trakPositions[i].first-4,BasicIo::beg);
        setMediaStream();
        io_->seek(hdlrPositions[i].first,BasicIo::beg);
        //        Fixme:Editing dandler data is known to corrupt file data
        //        handlerDecoder(hdlrPositions[i].second);
    }

    std::vector< pair<uint32_t,uint32_t> > vmhdPositions = findAtomPositions("vmhd");
    for(i=0; i<vmhdPositions.size(); i++)
    {
        io_->seek(vmhdPositions[i].first,BasicIo::beg);
        videoHeaderDecoder(vmhdPositions[i].second);
    }

    std::vector< pair<uint32_t,uint32_t> > udtaPositions = findAtomPositions("udta");
    for(i=0; i<udtaPositions.size(); i++)
    {
        io_->seek(udtaPositions[i].first,BasicIo::beg);
        userDataDecoder(udtaPositions[i].second);
    }

    std::vector< pair<uint32_t,uint32_t> > pnotPositions = findAtomPositions("pnot");
    for(i=0; i<pnotPositions.size(); i++)
    {
        io_->seek(pnotPositions[i].first,BasicIo::beg);
        previewTagDecoder(pnotPositions[i].second);
    }

    std::vector< pair<uint32_t,uint32_t> > taptPositions = findAtomPositions("tapt");
    for(i=0; i<taptPositions.size(); i++)
    {
        io_->seek(taptPositions[i].first,BasicIo::beg);
        trackApertureTagDecoder(taptPositions[i].second);
    }

    std::vector< pair<uint32_t,uint32_t> > keysPositions = findAtomPositions("keys");
    for(i=0; i<keysPositions.size(); i++)
    {
        io_->seek(keysPositions[i].first,BasicIo::beg);
        keysTagDecoder(keysPositions[i].second);
    }

    std::vector< pair<uint32_t,uint32_t> > stsdPositions = findAtomPositions("stsd");
    for(i=0; i<stsdPositions.size(); i++)
    {
        io_->seek(trakPositions[i].first,BasicIo::beg);
        setMediaStream();
        io_->seek(stsdPositions[i].first,BasicIo::beg);
        sampleDesc(stsdPositions[i].second);
    }
    return;
}

void QuickTimeVideo::readMetadata()
{
    if (io_->open() != 0) throw Error(9, io_->path(), strError());

    // Ensure that this is the correct image type
    if (!isQTimeType(*io_, false))
    {
        if (io_->error() || io_->eof()) throw Error(14);
        throw Error(3, "QuickTime");
    }

    IoCloser closer(*io_);
    clearMetadata();
    continueTraversing_ = true;
    height_ = width_ = 1;

    xmpData_["Xmp.video.FileSize"] = (double)io_->size()/(double)1048576;
    xmpData_["Xmp.video.FileName"] = io_->path();
    xmpData_["Xmp.video.MimeType"] = mimeType();

    while (continueTraversing_)
    {
        decodeBlock();
    }
    aspectRatio();
    return;
} // QuickTimeVideo::readMetadata

void QuickTimeVideo::decodeBlock()
{
    const int32_t bufMinSize = 4;
    DataBuf buf(bufMinSize+1);
    uint32_t size = 0;
    buf.pData_[4] = '\0' ;

    std::memset(buf.pData_, 0x0, buf.size_);

    io_->read(buf.pData_, 4);
    if(io_->eof())
    {
        continueTraversing_ = false;
        return;
    }

    size = Exiv2::getULong(buf.pData_, bigEndian);
    io_->read(buf.pData_, 4);
    if(size < 8)
        return;

    //      std::cerr<<"\nTag=>"<<buf.pData_<<"     size=>"<<size-8;
    tagDecoder(buf,size-8);
    return;
} // QuickTimeVideo::decodeBlock

void QuickTimeVideo::tagDecoder(Exiv2::DataBuf &buf, uint32_t size)
{
    const char allAtomFlags[][5]={"ftyp","mvhd","trak","tkhd","mdhd","hdlr","vmhd","udta","dref","stsd","stts","pnot",
                                  "tapt","keys","url ","urn ","dcom","smhd"};
    if(equalsQTimeTag(buf,allAtomFlags,(int)(sizeof(allAtomFlags)/5)))
    {
        QuickAtom *tmpAtom = new QuickAtom();
        tmpAtom->m_AtomLocation = io_->tell();
        tmpAtom->m_AtomSize = size;
        memcpy((byte *)tmpAtom->m_AtomId,(const byte*)buf.pData_,5);
        m_QuickSkeleton.push_back(tmpAtom);
    }

    uint64_t j;

    if (ignoreList(buf))
        discard(size);

    else if (dataIgnoreList(buf))
        decodeBlock();

    else if (equalsQTimeTag(buf, "ftyp"))
        fileTypeDecoder(size);

    else if (equalsQTimeTag(buf, "trak"))
        setMediaStream();

    else if (equalsQTimeTag(buf, "mvhd"))
        movieHeaderDecoder(size);

    else if (equalsQTimeTag(buf, "tkhd"))
        trackHeaderDecoder(size);

    else if (equalsQTimeTag(buf, "mdhd"))
        mediaHeaderDecoder(size);

    else if (equalsQTimeTag(buf, "hdlr"))
        handlerDecoder(size);

    else if (equalsQTimeTag(buf, "vmhd"))
        videoHeaderDecoder(size);

    else if (equalsQTimeTag(buf, "udta"))
        userDataDecoder(size);

    else if (equalsQTimeTag(buf, "dref"))
        multipleEntriesDecoder();

    else if (equalsQTimeTag(buf, "stsd"))
        sampleDesc(size);

    else if (equalsQTimeTag(buf, "stts"))
        timeToSampleDecoder();

    else if (equalsQTimeTag(buf, "pnot"))
        previewTagDecoder(size);

    else if (equalsQTimeTag(buf, "tapt"))
        trackApertureTagDecoder(size);

    else if (equalsQTimeTag(buf, "keys"))
        keysTagDecoder(size);

    else if (equalsQTimeTag(buf, "url "))
    {
        if(!m_modifyMetadata)
        {
            io_->read(buf.pData_, size);
            if (currentStream_ == Video)
                xmpData_["Xmp.video.URL"] = Exiv2::toString(buf.pData_);
            else if (currentStream_ == Audio)
                xmpData_["Xmp.audio.URL"] = Exiv2::toString(buf.pData_);
        }
        else
        {
            if ((currentStream_ == Video) && (xmpData_["Xmp.video.URL"].count() >0))
            {
                byte rawURL[size];
                const std::string URL = xmpData_["Xmp.video.URL"].toString();
                for(j=0; j<size; j++)
                {
                    rawURL[j] = (byte)URL[j];
                }
                io_->write(rawURL,size);
            }
            else if ((currentStream_ == Audio) && (xmpData_["Xmp.audio.URL"].count() >0))
            {
                byte rawURL[size];
                const std::string URL = xmpData_["Xmp.audio.URL"].toString();
                for(j=0; j<size; j++)
                {
                    rawURL[j] = (byte)URL[j];
                }
                io_->write(rawURL,size);
            }
            else
            {
                io_->seek(size,BasicIo::cur);
            }
        }
    }

    else if (equalsQTimeTag(buf, "urn "))
    {
        if(!m_modifyMetadata)
        {
            io_->read(buf.pData_, size);
            if (currentStream_ == Video)
                xmpData_["Xmp.video.URN"] = Exiv2::toString(buf.pData_);
            else if (currentStream_ == Audio)
                xmpData_["Xmp.audio.URN"] = Exiv2::toString(buf.pData_);
        }
        else
        {
            if ((currentStream_ == Video) && (xmpData_["Xmp.video.URN"].count() >0))
            {
                byte rawURN[size];
                const std::string URN = xmpData_["Xmp.video.URN"].toString();
                for(j=0; j<size; j++)
                {
                    rawURN[j] = (byte)URN[j];
                }
                io_->write(rawURN,size);
            }
            else if ((currentStream_ == Audio) && (xmpData_["Xmp.audio.URN"].count() >0))
            {
                byte rawURN[size];
                const std::string URN = xmpData_["Xmp.audio.URN"].toString();
                for(j=0; j<size; j++)
                {
                    rawURN[j] = (byte)URN[j];
                }
                io_->write(rawURN,size);
            }
            else
            {
                io_->seek(size,BasicIo::cur);
            }
        }
    }

    else if (equalsQTimeTag(buf, "dcom"))
    {
        if(!m_modifyMetadata)
        {
            io_->read(buf.pData_, size);
            xmpData_["Xmp.video.Compressor"] = Exiv2::toString(buf.pData_);
        }
        else
        {
            if(xmpData_["Xmp.video.Compressor"].count() >0)
            {
                byte rawCompressor[size];
                const std::string compressor = xmpData_["Xmp.video.Compressor"].toString();
                for(j=0; j<size; j++)
                {
                    rawCompressor[j] = (byte)compressor[j];
                }
                io_->write(rawCompressor,size);
            }
            else
            {
                io_->seek(size,BasicIo::cur);
            }
        }
    }

    else if (equalsQTimeTag(buf, "smhd"))
    {
        io_->seek(4,BasicIo::cur);
        if(!m_modifyMetadata)
        {
            io_->read(buf.pData_, 4);
            xmpData_["Xmp.audio.Balance"] = returnBufValue(buf, 2);
        }
        else
        {
            if(xmpData_["Xmp.audio.Balance"].count() >0)
            {
                byte rawBalance[4];
                const std::string balance = xmpData_["Xmp.audio.Balance"].toString();
                for(j=0; j<4; j++)
                {
                    rawBalance[j] = (byte)balance[j];
                }
                io_->write(rawBalance,4);
            }
            else
            {
                io_->seek(4,BasicIo::cur);
            }
        }
    }

    else
    {
        discard(size);
    }
    return;
} // QuickTimeVideo::tagDecoder

void QuickTimeVideo::discard(uint32_t size)
{
    uint64_t cur_pos = io_->tell();
    io_->seek(cur_pos + size, BasicIo::beg);
    return;
} // QuickTimeVideo::discard

void QuickTimeVideo::previewTagDecoder(uint32_t size)
{
    DataBuf buf(4);
    uint64_t cur_pos = io_->tell();
    if(!m_modifyMetadata)
    {
        io_->read(buf.pData_, 4);
        xmpData_["Xmp.video.PreviewDate"] = getULong(buf.pData_, bigEndian);
        io_->read(buf.pData_, 2);
        xmpData_["Xmp.video.PreviewVersion"] = getShort(buf.pData_, bigEndian);

        io_->read(buf.pData_, 4);
        if(equalsQTimeTag(buf, "PICT"))
            xmpData_["Xmp.video.PreviewAtomType"] = "QuickDraw Picture";
        else
            xmpData_["Xmp.video.PreviewAtomType"] = Exiv2::toString(buf.pData_);

        io_->seek(cur_pos + size, BasicIo::beg);
    }
    else
    {
        if(xmpData_["Xmp.video.PreviewDate"].count() >0)
        {
            const int32_t previewDate = xmpData_["Xmp.video.PreviewDate"].toLong();
            io_->write((byte*)&previewDate,4);
        }
        else
        {
            io_->seek(4,BasicIo::cur);
        }
        if(xmpData_["Xmp.video.PreviewVersion"].count() >0)
        {
            const uint16_t previewVersion = (uint16_t) xmpData_["Xmp.video.PreviewVersion"].toLong();
            io_->write((byte*)&previewVersion,2);
        }
        else
        {
            io_->seek(2,BasicIo::cur);
        }
        if(xmpData_["Xmp.video.PreviewAtomType"].count() >0)
        {
            DataBuf buf((uint32_t)5);
            byte rawPreviewAtomType[4] = {'P','I','C','T'};
            const std::string previewAtomType = xmpData_["Xmp.video.PreviewAtomType"].toString();
            if(!equalsQTimeTag(buf, "PICT"))
            {
                int32_t j;
                for( j=0; j<4; j++)
                {
                    buf.pData_[j] = previewAtomType[j];
                }
                io_->write(buf.pData_,4);
            }
            else
            {
                io_->write(rawPreviewAtomType,4);
            }
        }
        else
        {
            io_->seek(4,BasicIo::cur);
        }
        io_->seek(cur_pos + size, BasicIo::beg);
    }
    return;
} // QuickTimeVideo::previewTagDecoder

void QuickTimeVideo::keysTagDecoder(uint32_t size)
{
    DataBuf buf(4);
    uint64_t cur_pos = io_->tell();

    if(!m_modifyMetadata)
    {
        io_->read(buf.pData_, 4);
        xmpData_["Xmp.video.PreviewDate"] = getULong(buf.pData_, bigEndian);
        io_->read(buf.pData_, 2);
        xmpData_["Xmp.video.PreviewVersion"] = getShort(buf.pData_, bigEndian);

        io_->read(buf.pData_, 4);
        if(equalsQTimeTag(buf, "PICT"))
            xmpData_["Xmp.video.PreviewAtomType"] = "QuickDraw Picture";
        else
            xmpData_["Xmp.video.PreviewAtomType"] = Exiv2::toString(buf.pData_);

        io_->seek(cur_pos + size, BasicIo::beg);
    }
    else
    {
        if(xmpData_["Xmp.video.PreviewDate"].count() >0)
        {
            const int32_t previewDate = xmpData_["Xmp.video.PreviewDate"].toLong();
            io_->write((byte*)&previewDate,4);
        }
        else
        {
            io_->seek(4,BasicIo::cur);
        }
        if(xmpData_["Xmp.video.PreviewVersion"].count() >0)
        {
            const uint16_t previewVersion = (uint16_t) xmpData_["Xmp.video.PreviewVersion"].toLong();
            io_->write((byte*)&previewVersion,2);
        }
        else
        {
            io_->seek(2,BasicIo::cur);
        }
        if(xmpData_["Xmp.video.PreviewAtomType"].count() >0)
        {
            DataBuf buf((uint32_t)5);
            byte rawPreviewAtomType[4] = {'P','I','C','T'};
            const std::string previewAtomType = xmpData_["Xmp.video.PreviewAtomType"].toString();
            int32_t j;
            for( j=0; j<4; j++)
            {
                buf.pData_[j] = previewAtomType[j];
            }
            if(!equalsQTimeTag(buf, "PICT"))
            {
                io_->write(buf.pData_,4);
            }
            else
            {
                io_->write(rawPreviewAtomType,4);
            }
        }
        else
        {
            io_->seek(4,BasicIo::cur);
        }
        io_->seek(cur_pos + size, BasicIo::beg);
    }
    return;
} // QuickTimeVideo::keysTagDecoder

void QuickTimeVideo::trackApertureTagDecoder(uint32_t size)
{
    DataBuf buf(4), buf2(2);
    uint64_t cur_pos = io_->tell();
    if(!m_modifyMetadata)
    {
        byte n = 3;
        while(n--)
        {
            io_->seek(static_cast<int32_t>(4), BasicIo::cur); io_->read(buf.pData_, 4);

            if(equalsQTimeTag(buf, "clef"))
            {
                io_->seek(static_cast<int32_t>(4), BasicIo::cur);
                io_->read(buf.pData_, 2); io_->read(buf2.pData_, 2);
                xmpData_["Xmp.video.CleanApertureWidth"]    =   Exiv2::toString(getUShort(buf.pData_, bigEndian))
                        + "." + Exiv2::toString(getUShort(buf2.pData_, bigEndian));
                io_->read(buf.pData_, 2); io_->read(buf2.pData_, 2);
                xmpData_["Xmp.video.CleanApertureHeight"]   =   Exiv2::toString(getUShort(buf.pData_, bigEndian))
                        + "." + Exiv2::toString(getUShort(buf2.pData_, bigEndian));
            }

            else if(equalsQTimeTag(buf, "prof"))
            {
                io_->seek(static_cast<int32_t>(4), BasicIo::cur);
                io_->read(buf.pData_, 2); io_->read(buf2.pData_, 2);
                xmpData_["Xmp.video.ProductionApertureWidth"]    =   Exiv2::toString(getUShort(buf.pData_, bigEndian))
                        + "." + Exiv2::toString(getUShort(buf2.pData_, bigEndian));
                io_->read(buf.pData_, 2); io_->read(buf2.pData_, 2);
                xmpData_["Xmp.video.ProductionApertureHeight"]   =   Exiv2::toString(getUShort(buf.pData_, bigEndian))
                        + "." + Exiv2::toString(getUShort(buf2.pData_, bigEndian));
            }

            else if(equalsQTimeTag(buf, "enof"))
            {
                io_->seek(static_cast<int32_t>(4), BasicIo::cur);
                io_->read(buf.pData_, 2); io_->read(buf2.pData_, 2);
                xmpData_["Xmp.video.EncodedPixelsWidth"]    =   Exiv2::toString(getUShort(buf.pData_, bigEndian))
                        + "." + Exiv2::toString(getUShort(buf2.pData_, bigEndian));
                io_->read(buf.pData_, 2); io_->read(buf2.pData_, 2);
                xmpData_["Xmp.video.EncodedPixelsHeight"]   =   Exiv2::toString(getUShort(buf.pData_, bigEndian))
                        + "." + Exiv2::toString(getUShort(buf2.pData_, bigEndian));
            }
        }
        io_->seek(static_cast<int32_t>(cur_pos + size), BasicIo::beg);
    }
    else
    {
        byte n = 3;
        while(n--)
        {
            io_->seek(static_cast<int32_t>(4), BasicIo::cur); io_->read(buf.pData_, 4);

            if(equalsQTimeTag(buf, "clef"))
            {
                io_->seek(static_cast<int32_t>(4), BasicIo::cur);
                if(xmpData_["Xmp.video.CleanApertureWidth"].count() >0)
                {
                    byte rawCleanApertureWidth[4];
                    byte rawCleanApertureWidthTmp[2];
                    const std::string cleanApertureWidth =  xmpData_["Xmp.video.CleanApertureWidth"].toString();
                    vector<uint16_t> cleanApertureWidthValue = getNumberFromString(cleanApertureWidth,'.');
                    memcpy(rawCleanApertureWidthTmp,&cleanApertureWidthValue.at(0),2);
                    rawCleanApertureWidth[0] = rawCleanApertureWidthTmp[0];
                    rawCleanApertureWidth[1] = rawCleanApertureWidthTmp[1];
                    memcpy(rawCleanApertureWidthTmp,&cleanApertureWidthValue.at(1),2);
                    rawCleanApertureWidth[2] = rawCleanApertureWidthTmp[0];
                    rawCleanApertureWidth[3] = rawCleanApertureWidthTmp[1];
                    io_->write(rawCleanApertureWidth,4);
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }

                if(xmpData_["Xmp.video.CleanApertureHeight"].count() >0)
                {
                    byte rawCleanApertureHeight[4]; byte rawCleanApertureHeightTmp[2];
                    const std::string cleanApertureHeight =  xmpData_["Xmp.video.CleanApertureHeight"].toString();
                    vector<uint16_t> cleanApertureHeightValue = getNumberFromString(cleanApertureHeight,'.');
                    memcpy(rawCleanApertureHeightTmp,&cleanApertureHeightValue.at(0),2);
                    rawCleanApertureHeight[0] = rawCleanApertureHeightTmp[0];
                    rawCleanApertureHeight[1] = rawCleanApertureHeightTmp[1];
                    memcpy(rawCleanApertureHeightTmp,&cleanApertureHeightValue.at(1),2);
                    rawCleanApertureHeight[2] = rawCleanApertureHeightTmp[0];
                    rawCleanApertureHeight[3] = rawCleanApertureHeightTmp[1];
                    io_->write(rawCleanApertureHeight,4);
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
            }

            else if(equalsQTimeTag(buf, "prof"))
            {
                io_->seek(static_cast<int32_t>(4), BasicIo::cur);
                if(xmpData_["Xmp.video.ProductionApertureWidth"].count() >0)
                {
                    byte rawProductionApertureWidth[4]; byte rawProductionApertureWidthTmp[2];
                    const std::string productionApertureWidth =  xmpData_["Xmp.video.ProductionApertureWidth"].toString();
                    vector<uint16_t> productionApertureWidthValue = getNumberFromString(productionApertureWidth,'.');
                    memcpy(rawProductionApertureWidthTmp,&productionApertureWidthValue.at(0),2);
                    rawProductionApertureWidth[0] = rawProductionApertureWidthTmp[0];
                    rawProductionApertureWidth[1] = rawProductionApertureWidthTmp[1];
                    memcpy(rawProductionApertureWidthTmp,&productionApertureWidthValue.at(1),2);
                    rawProductionApertureWidth[2] = rawProductionApertureWidthTmp[0];
                    rawProductionApertureWidth[3] = rawProductionApertureWidthTmp[1];
                    io_->write(rawProductionApertureWidth,4);
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }

                if(xmpData_["Xmp.video.ProductionApertureHeight"].count() >0)
                {
                    byte rawProductionApertureHeight[4]; byte rawProductionApertureHeightTmp[2];
                    const std::string productionApertureHeight =  xmpData_["Xmp.video.ProductionApertureHeight"].toString();
                    vector<uint16_t> productionApertureHeightValue = getNumberFromString(productionApertureHeight,'.');
                    memcpy(rawProductionApertureHeightTmp,&productionApertureHeightValue.at(0),2);
                    rawProductionApertureHeight[0] = rawProductionApertureHeightTmp[0];
                    rawProductionApertureHeight[1] = rawProductionApertureHeightTmp[1];
                    memcpy(rawProductionApertureHeightTmp,&productionApertureHeightValue.at(1),2);
                    rawProductionApertureHeight[2] = rawProductionApertureHeightTmp[0];
                    rawProductionApertureHeight[3] = rawProductionApertureHeightTmp[1];
                    io_->write(rawProductionApertureHeight,4);
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
            }
            else if(equalsQTimeTag(buf, "enof"))
            {
                io_->seek(static_cast<int32_t>(4), BasicIo::cur);
                if(xmpData_["Xmp.video.EncodedPixelsWidth"].count() >0)
                {
                    byte rawEncodedPixelsWidth[4]; byte rawEncodedPixelsWidthTmp[2];
                    const std::string encodedPixelsWidth =  xmpData_["Xmp.video.EncodedPixelsWidth"].toString();
                    vector<uint16_t> encodedPixelsWidthValue = getNumberFromString(encodedPixelsWidth,'.');
                    memcpy(rawEncodedPixelsWidthTmp,&encodedPixelsWidthValue.at(0),2);
                    rawEncodedPixelsWidth[0] = rawEncodedPixelsWidthTmp[0];
                    rawEncodedPixelsWidth[1] = rawEncodedPixelsWidthTmp[1];

                    memcpy(rawEncodedPixelsWidthTmp,&encodedPixelsWidthValue.at(1),2);
                    rawEncodedPixelsWidth[2] = rawEncodedPixelsWidthTmp[0];
                    rawEncodedPixelsWidth[3] = rawEncodedPixelsWidthTmp[1];
                    io_->write(rawEncodedPixelsWidth,4);
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }

                if(xmpData_["Xmp.video.EncodedPixelsHeight"].count() >0)
                {
                    byte rawEncodedPixelsHeight[4]; byte rawEncodedPixelsHeightTmp[2];
                    const std::string encodedPixelsHeight =  xmpData_["Xmp.video.EncodedPixelsHeight"].toString();
                    vector<uint16_t> encodedPixelsHeightValue = getNumberFromString(encodedPixelsHeight,'.');
                    memcpy(rawEncodedPixelsHeightTmp,&encodedPixelsHeightValue.at(0),2);
                    rawEncodedPixelsHeight[0] = rawEncodedPixelsHeightTmp[0];
                    rawEncodedPixelsHeight[1] = rawEncodedPixelsHeightTmp[1];

                    memcpy(rawEncodedPixelsHeightTmp,&encodedPixelsHeightValue.at(1),2);
                    rawEncodedPixelsHeight[2] = rawEncodedPixelsHeightTmp[0];
                    rawEncodedPixelsHeight[3] = rawEncodedPixelsHeightTmp[1];
                    io_->write(rawEncodedPixelsHeight,4);
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
            }
        }
        io_->seek(static_cast<int32_t>(cur_pos + size), BasicIo::beg);
    }
    return;
} // QuickTimeVideo::trackApertureTagDecoder

void QuickTimeVideo::CameraTagsDecoder(uint32_t size_external)
{
    uint64_t cur_pos = io_->tell();
    DataBuf buf(50), buf2(4);
    const TagDetails* td;
    const RevTagDetails* rtd;
    io_->read(buf.pData_, 4);
    if(equalsQTimeTag(buf, "NIKO"))
    {
        io_->seek(cur_pos, BasicIo::beg);
        if(!m_modifyMetadata)
        {
            io_->read(buf.pData_, 24);
            xmpData_["Xmp.video.Make"] = Exiv2::toString(buf.pData_);
            io_->read(buf.pData_, 14);
            xmpData_["Xmp.video.Model"] = Exiv2::toString(buf.pData_);
            io_->read(buf.pData_, 4);
            xmpData_["Xmp.video.ExposureTime"] = "1/" + Exiv2::toString( ceil( getULong(buf.pData_, littleEndian) / (double)10));
            io_->read(buf.pData_, 4); io_->read(buf2.pData_, 4);
            xmpData_["Xmp.video.FNumber"] =  getULong(buf.pData_, littleEndian) / (double)getULong(buf2.pData_, littleEndian) ;
            io_->read(buf.pData_, 4); io_->read(buf2.pData_, 4);
            xmpData_["Xmp.video.ExposureCompensation"] =  getULong(buf.pData_, littleEndian) / (double)getULong(buf2.pData_, littleEndian) ;
            io_->read(buf.pData_, 10); io_->read(buf.pData_, 4);
            td = find(whiteBalance, getULong(buf.pData_, littleEndian));
            if (td)
                xmpData_["Xmp.video.WhiteBalance"] = exvGettext(td->label_);
            io_->read(buf.pData_, 4); io_->read(buf2.pData_, 4);
            xmpData_["Xmp.video.FocalLength"] =  getULong(buf.pData_, littleEndian) / (double)getULong(buf2.pData_, littleEndian) ;
            io_->seek(static_cast<int32_t>(95), BasicIo::cur);
            io_->read(buf.pData_, 48);
            xmpData_["Xmp.video.Software"] = Exiv2::toString(buf.pData_);
            io_->read(buf.pData_, 4);
            xmpData_["Xmp.video.ISO"] = getULong(buf.pData_, littleEndian);
            io_->seek(cur_pos + size_external, BasicIo::beg);
        }
        else
        {
            if(equalsQTimeTag(buf, "NIKO"))
            {
                io_->seek(cur_pos, BasicIo::beg);
                RevTagDetails revTagDetails[(sizeof(whiteBalance)/sizeof(whiteBalance[0]))];
                reverseTagDetails(whiteBalance,revTagDetails,((sizeof(whiteBalance)/sizeof(whiteBalance[0]))));

                if(xmpData_["Xmp.video.Make"].count() >0)
                {
                    byte rawMake[24];
                    const std::string make = xmpData_["Xmp.video.Make"].toString();
                    int32_t j;
                    for( j=0; j<24; j++)
                    {
                        rawMake[j] = make[j];
                    }
                    io_->write(rawMake,24);
                }
                else
                {
                    io_->seek(24,BasicIo::cur);
                }
                if(xmpData_["Xmp.video.Model"].count() >0)
                {
                    byte rawModel[14];
                    const std::string model = xmpData_["Xmp.video.Model"].toString();
                    int32_t j;
                    for( j=0; j<14; j++)
                    {
                        rawModel[j] = model[j];
                    }
                    io_->write(rawModel,14);
                }
                else
                {
                    io_->seek(14,BasicIo::cur);
                }
                if(xmpData_["Xmp.video.ExposureTime"].count() >0)
                {
                    const std::vector<int32_t> d_exposureTime =
                            getLongFromString(xmpData_["Xmp.video.ExposureTime"].toString(),'/');
                    const int32_t tmpValue = (d_exposureTime.at(1)*10);
                    io_->write((byte*)&tmpValue,4);
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                if(xmpData_["Xmp.video.FNumber"].count() >0)
                {
                    io_->seek(4,BasicIo::cur);
                    io_->read(buf2.pData_,4);
                    io_->seek(-8,BasicIo::cur);
                    const int32_t fNumber =(int32_t)
                            ((double)xmpData_["Xmp.video.FNumber"].toLong()
                            *(double)getULong(buf2.pData_, littleEndian));
                    io_->write((byte*)&fNumber,4);
                    io_->seek(4,BasicIo::cur);
                }
                else
                {
                    io_->seek(8,BasicIo::cur);
                }
                if(xmpData_["Xmp.video.ExposureCompensation"].count() >0)
                {
                    io_->seek(4,BasicIo::cur);
                    io_->read(buf2.pData_,4);
                    io_->seek(-8,BasicIo::cur);
                    const int32_t exposureCompensation =(int32_t)
                            ((double)xmpData_["Xmp.video.ExposureCompensation"].toLong()
                            *(double)getULong(buf2.pData_, littleEndian));
                    io_->write((byte*)&exposureCompensation,4);
                    io_->seek(14,BasicIo::cur);
                }
                else
                {
                    io_->seek(18,BasicIo::cur);
                }
                if(xmpData_["Xmp.video.WhiteBalance"].count() >0)
                {
                    rtd = find(revTagDetails,xmpData_["Xmp.video.WhiteBalance"].toString());
                    if(rtd)
                    {
                        const int32_t sWhiteBalance = (int32_t)rtd->val_;
                        io_->write((byte*)&sWhiteBalance,4);
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                if(xmpData_["Xmp.video.FocalLength"].count() >0)
                {
                    io_->seek(4,BasicIo::cur);
                    io_->read(buf2.pData_,4);
                    io_->seek(-8,BasicIo::cur);
                    const int32_t focalLength =(int32_t)((double)xmpData_["Xmp.video.FocalLength"].toLong()
                            *(double)getULong(buf2.pData_, littleEndian));
                    io_->write((byte*)&focalLength,4);
                    io_->seek(99,BasicIo::cur);
                }
                else
                {
                    io_->seek(103,BasicIo::cur);
                }
                if(xmpData_["Xmp.video.Software"].count() >0)
                {
                    byte rawSoftware[48];
                    const std::string software = xmpData_["Xmp.video.Software"].toString();
                    int32_t j;
                    for( j=0; j<48; j++)
                    {
                        rawSoftware[j] = software[j];
                    }
                    io_->write(rawSoftware,48);
                }
                else
                {
                    io_->seek(48,BasicIo::cur);
                }
                if(xmpData_["Xmp.video.ISO"].count() >0)
                {
                    const int32_t ISO = xmpData_["Xmp.video.ISO"].toLong();
                    io_->write((byte*)&ISO,4);
                }
            }
        }
        io_->seek(cur_pos + size_external, BasicIo::beg);
    }
    return;
} // QuickTimeVideo::CameraTagsDecoder

void QuickTimeVideo::userDataDecoder(uint32_t size_external)
{
    uint64_t      cur_pos = io_->tell();
    const TagVocabulary* td;
    const TagVocabulary* tv;
    const TagVocabulary* tv_internal;
    const int32_t        bufMinSize = 100;
    DataBuf              buf(bufMinSize);
    uint64_t             size = 0;
    uint64_t             size_internal = size_external;
    uint64_t             j;
    
    std::memset(buf.pData_, 0x0, buf.size_);

    if(!m_modifyMetadata)
    {
        while((size_internal/4 != 0) && (size_internal > 0))
        {
            buf.pData_[4] = '\0' ;
            io_->read(buf.pData_, 4);
            size = Exiv2::getULong(buf.pData_, bigEndian);
            if(size > size_internal)
                break;
            size_internal -= size;
            io_->read(buf.pData_, 4);

            if(buf.pData_[0] == 169)
                buf.pData_[0] = ' ';
            td = find(userDatatags, Exiv2::toString( buf.pData_));

            tv = find(userDataReferencetags, Exiv2::toString( buf.pData_));

            if(size == 0 || (size - 12) <= 0)
                break;

            else if(equalsQTimeTag(buf, "DcMD")  || equalsQTimeTag(buf, "NCDT"))
                userDataDecoder(size - 8);

            else if(equalsQTimeTag(buf, "NCTG"))
                NikonTagsDecoder(size - 8);

            else if(equalsQTimeTag(buf, "TAGS"))
                CameraTagsDecoder(size - 8);

            else if(equalsQTimeTag(buf, "CNCV") || equalsQTimeTag(buf, "CNFV")
                    || equalsQTimeTag(buf, "CNMN") || equalsQTimeTag(buf, "NCHD")
                    || equalsQTimeTag(buf, "FFMV"))
            {
                io_->read(buf.pData_, size - 8);
                xmpData_[exvGettext(tv->label_)] = Exiv2::toString(buf.pData_);
            }

            else if(equalsQTimeTag(buf, "CMbo") || equalsQTimeTag(buf, "Cmbo"))
            {
                io_->read(buf.pData_, 2);
                buf.pData_[2] = '\0' ;
                tv_internal = find(cameraByteOrderTags, Exiv2::toString( buf.pData_));

                if (tv_internal)
                    xmpData_[exvGettext(tv->label_)] = exvGettext(tv_internal->label_);
                else
                    xmpData_[exvGettext(tv->label_)] = Exiv2::toString(buf.pData_);
            }

            else if(tv)
            {
                io_->read(buf.pData_, size-8);
                xmpData_[exvGettext(tv->label_)] = Exiv2::toString(buf.pData_);
            }

            else if(td)
                tagDecoder(buf,size-8);
        }
        io_->seek(cur_pos + size_external, BasicIo::beg);
    }
    else
    {
        while((size_internal/4 != 0) && (size_internal > 0))
        {
            buf.pData_[4] = '\0' ;
            io_->read(buf.pData_, 4);
            size = Exiv2::getULong(buf.pData_, bigEndian);
            if(size > size_internal)
                break;
            size_internal -= size;
            io_->read(buf.pData_, 4);

            if(buf.pData_[0] == 169)
                buf.pData_[0] = ' ';
            td = find(userDatatags, Exiv2::toString( buf.pData_));

            tv = find(userDataReferencetags, Exiv2::toString( buf.pData_));

            if(size == 0 || (size - 12) <= 0)
                break;

            else if(equalsQTimeTag(buf, "DcMD")  || equalsQTimeTag(buf, "NCDT"))
                userDataDecoder(size - 8);

            else if(equalsQTimeTag(buf, "NCTG"))
                NikonTagsDecoder(size - 8);

            else if(equalsQTimeTag(buf, "TAGS"))
                CameraTagsDecoder(size - 8);

            else if(equalsQTimeTag(buf, "CNCV") || equalsQTimeTag(buf, "CNFV")
                    || equalsQTimeTag(buf, "CNMN") || equalsQTimeTag(buf, "NCHD")
                    || equalsQTimeTag(buf, "FFMV"))
            {
                if(xmpData_[exvGettext(tv->label_)].count() > 0)
                {
                    byte rawTagData[size-8];
                    const std::string tagData = xmpData_[exvGettext(tv->label_)].toString();
                    int32_t j;
                    for(j=0; j <min((int32_t)(size-8),(int32_t)xmpData_[exvGettext(tv->label_)].size()); j++)
                    {
                        rawTagData[j] = tagData[j];
                    }
                    io_->write(rawTagData,(size-8));
                }
                else
                {
                    io_->seek((size-8),BasicIo::cur);
                }
            }

            else if(equalsQTimeTag(buf, "CMbo") || equalsQTimeTag(buf, "Cmbo"))
            {
                io_->seek(2,BasicIo::cur);
            }

            else if(tv)
            {
                if(xmpData_[exvGettext(tv->label_)].count() >0)
                {
                    byte rawtagData[size-8];
                    uint64_t newXmpDataSize = xmpData_[exvGettext(tv->label_)].size();
                    const std::string tagData = xmpData_[exvGettext(tv->label_)].toString();
                    for(j=0; j<min(size-8,newXmpDataSize); j++)
                    {
                        rawtagData[j] = tagData[j];
                    }
                    if((size-8) > newXmpDataSize)
                        for(j=newXmpDataSize; j<(size-8); j++)
                        {
                            rawtagData[j] = ' ';
                        }
                    io_->write(rawtagData,(size-8));
                }
                else
                {
                    io_->seek((size-12),BasicIo::cur);
                }
            }
        }
        io_->seek(cur_pos + size_external, BasicIo::beg);
    }
    return;
} // QuickTimeVideo::userDataDecoder

void QuickTimeVideo::NikonTagsDecoder(uint32_t size_external)
{
    uint64_t   cur_pos = io_->tell();
    DataBuf    buf(200);
    DataBuf    buf2(4+1);
    uint64_t   TagID      = 0 ;
    uint16_t   dataLength = 0 ;
    uint16_t   dataType   = 2 ;
    const TagDetails*     td  ;
    const TagDetails*     td2 ;
    const RevTagDetails*  rtd2;

    if(!m_modifyMetadata)
    {
        int32_t i;
        for( i = 0 ; i < 100 ; i++)
        {
            io_->read(buf.pData_, 4);
            TagID = Exiv2::getULong(buf.pData_, bigEndian);
            td = find(NikonNCTGTags, TagID);

            io_->read(buf.pData_, 2);
            dataType = Exiv2::getUShort(buf.pData_, bigEndian);

            std::memset(buf.pData_, 0x0, buf.size_);
            io_->read(buf.pData_, 2);

            if(TagID == 0x2000023)
            {
                uint64_t local_pos = io_->tell();
                dataLength = Exiv2::getUShort(buf.pData_, bigEndian);
                std::memset(buf.pData_, 0x0, buf.size_);

                io_->read(buf.pData_, 4);   xmpData_["Xmp.video.PictureControlVersion"]  = Exiv2::toString(buf.pData_);
                io_->read(buf.pData_, 20);  xmpData_["Xmp.video.PictureControlName"]     = Exiv2::toString(buf.pData_);
                io_->read(buf.pData_, 20);  xmpData_["Xmp.video.PictureControlBase"]     = Exiv2::toString(buf.pData_);
                io_->read(buf.pData_, 4);   std::memset(buf.pData_, 0x0, buf.size_);

                io_->read(buf.pData_, 1);
                td2 = find(PictureControlAdjust, (int)buf.pData_[0] & 7 );
                if(td2)
                    xmpData_["Xmp.video.PictureControlAdjust"] = exvGettext(td2->label_);
                else
                    xmpData_["Xmp.video.PictureControlAdjust"] = (int)buf.pData_[0] & 7 ;

                io_->read(buf.pData_, 1);
                td2 = find(NormalSoftHard, (int)buf.pData_[0] & 7 );
                if(td2)
                    xmpData_["Xmp.video.PictureControlQuickAdjust"] = exvGettext(td2->label_);

                io_->read(buf.pData_, 1);
                td2 = find(NormalSoftHard, (int)buf.pData_[0] & 7 );
                if(td2)
                    xmpData_["Xmp.video.Sharpness"] = exvGettext(td2->label_);
                else
                    xmpData_["Xmp.video.Sharpness"] = (int)buf.pData_[0] & 7;

                io_->read(buf.pData_, 1);
                td2 = find(NormalSoftHard, (int)buf.pData_[0] & 7 );
                if(td2)
                    xmpData_["Xmp.video.Contrast"] = exvGettext(td2->label_);
                else
                    xmpData_["Xmp.video.Contrast"] = (int)buf.pData_[0] & 7;

                io_->read(buf.pData_, 1);
                td2 = find(NormalSoftHard, (int)buf.pData_[0] & 7 );
                if(td2)
                    xmpData_["Xmp.video.Brightness"] = exvGettext(td2->label_);
                else
                    xmpData_["Xmp.video.Brightness"] = (int)buf.pData_[0] & 7;

                io_->read(buf.pData_, 1);
                td2 = find(Saturation, (int)buf.pData_[0] & 7 );
                if(td2)
                    xmpData_["Xmp.video.Saturation"] = exvGettext(td2->label_);
                else
                    xmpData_["Xmp.video.Saturation"] = (int)buf.pData_[0] & 7;

                io_->read(buf.pData_, 1);
                xmpData_["Xmp.video.HueAdjustment"] = (int)buf.pData_[0] & 7;

                io_->read(buf.pData_, 1);
                td2 = find(FilterEffect, (int)buf.pData_[0]);
                if(td2)
                    xmpData_["Xmp.video.FilterEffect"] = exvGettext(td2->label_);
                else
                    xmpData_["Xmp.video.FilterEffect"] = (int)buf.pData_[0];

                io_->read(buf.pData_, 1);
                td2 = find(ToningEffect, (int)buf.pData_[0]);
                if(td2)
                    xmpData_["Xmp.video.ToningEffect"] = exvGettext(td2->label_);
                else
                    xmpData_["Xmp.video.ToningEffect"] = (int)buf.pData_[0];

                io_->read(buf.pData_, 1);   xmpData_["Xmp.video.ToningSaturation"] = (int)buf.pData_[0];

                io_->seek(local_pos + dataLength, BasicIo::beg);
            }

            else if(TagID == 0x2000024)
            {
                uint64_t local_pos = io_->tell();
                dataLength = Exiv2::getUShort(buf.pData_, bigEndian);
                std::memset(buf.pData_, 0x0, buf.size_);

                io_->read(buf.pData_, 2);   xmpData_["Xmp.video.TimeZone"] = Exiv2::getShort(buf.pData_, bigEndian);
                io_->read(buf.pData_, 1);
                td2 = find(YesNo, (int)buf.pData_[0]);
                if(td2)
                    xmpData_["Xmp.video.DayLightSavings"] = exvGettext(td2->label_);

                io_->read(buf.pData_, 1);
                td2 = find(DateDisplayFormat, (int)buf.pData_[0]);
                if(td2)
                    xmpData_["Xmp.video.DateDisplayFormat"] = exvGettext(td2->label_);

                io_->seek(local_pos + dataLength, BasicIo::beg);
            }

            else if(dataType == 2 || dataType == 7)
            {
                dataLength = Exiv2::getUShort(buf.pData_, bigEndian);
                std::memset(buf.pData_, 0x0, buf.size_);

                // Sanity check with an "unreasonably" large number
                if (dataLength > 200)
                {
#ifndef SUPPRESS_WARNINGS
                    EXV_ERROR << "Xmp.video Nikon Tags, dataLength was found to be larger than 200."
                              << " Entries considered invalid. Not Processed.\n";
#endif
                    io_->seek(io_->tell() + dataLength, BasicIo::beg);
                }
                else
                    io_->read(buf.pData_, dataLength);

                if(td)
                    xmpData_[exvGettext(td->label_)] = Exiv2::toString(buf.pData_);
            }
            else if(dataType == 4)
            {
                dataLength = Exiv2::getUShort(buf.pData_, bigEndian) * 4;
                std::memset(buf.pData_, 0x0, buf.size_);
                io_->read(buf.pData_, 4);
                if(td)
                    xmpData_[exvGettext(td->label_)] = Exiv2::toString(Exiv2::getULong( buf.pData_, bigEndian));

                // Sanity check with an "unreasonably" large number
                if (dataLength > 200 || dataLength < 4)
                {
#ifndef SUPPRESS_WARNINGS
                    EXV_ERROR << "Xmp.video Nikon Tags, dataLength was found to be of inapropriate size."
                              << " Entries considered invalid. Not Processed.\n";
#endif
                    io_->seek(io_->tell() + dataLength - 4, BasicIo::beg);
                }
                else
                    io_->read(buf.pData_, dataLength - 4);
            }
            else if(dataType == 3)
            {
                dataLength = Exiv2::getUShort(buf.pData_, bigEndian) * 2;
                std::memset(buf.pData_, 0x0, buf.size_);
                io_->read(buf.pData_, 2);
                if(td)
                    xmpData_[exvGettext(td->label_)] = Exiv2::toString(Exiv2::getUShort( buf.pData_, bigEndian));

                // Sanity check with an "unreasonably" large number
                if (dataLength > 200 || dataLength < 2)
                {
#ifndef SUPPRESS_WARNINGS
                    EXV_ERROR << "Xmp.video Nikon Tags, dataLength was found to be of inapropriate size."
                              << " Entries considered invalid. Not Processed.\n";
#endif
                    io_->seek(io_->tell() + dataLength - 2, BasicIo::beg);
                }
                else
                    io_->read(buf.pData_, dataLength - 2);
            }
            else if(dataType == 5)
            {
                dataLength = Exiv2::getUShort(buf.pData_, bigEndian) * 8;
                std::memset(buf.pData_, 0x0, buf.size_);
                io_->read(buf.pData_, 4);
                io_->read(buf2.pData_, 4);
                if(td)
                    xmpData_[exvGettext(td->label_)] = Exiv2::toString((double)Exiv2::getULong( buf.pData_, bigEndian)
                                                                       / (double)Exiv2::getULong( buf2.pData_, bigEndian));

                // Sanity check with an "unreasonably" large number
                if (dataLength > 200 || dataLength < 8)
                {
#ifndef SUPPRESS_WARNINGS
                    EXV_ERROR << "Xmp.video Nikon Tags, dataLength was found to be of inapropriate size."
                              << " Entries considered invalid. Not Processed.\n";
#endif
                    io_->seek(io_->tell() + dataLength - 8, BasicIo::beg);
                }
                else
                    io_->read(buf.pData_, dataLength - 8);
            }
            else if(dataType == 8)
            {
                dataLength = Exiv2::getUShort(buf.pData_, bigEndian) * 2;
                std::memset(buf.pData_, 0x0, buf.size_);
                io_->read(buf.pData_, 2);
                io_->read(buf2.pData_, 2);
                if(td)
                    xmpData_[exvGettext(td->label_)] = Exiv2::toString(Exiv2::getUShort( buf.pData_, bigEndian) )
                            + " " + Exiv2::toString(Exiv2::getUShort( buf2.pData_, bigEndian));

                // Sanity check with an "unreasonably" large number
                if (dataLength > 200 || dataLength < 4)
                {
#ifndef SUPPRESS_WARNINGS
                    EXV_ERROR << "Xmp.video Nikon Tags, dataLength was found to be of inapropriate size."
                              << " Entries considered invalid. Not Processed.\n";
#endif
                    io_->seek(io_->tell() + dataLength - 4, BasicIo::beg);
                }
                else
                    io_->read(buf.pData_, dataLength - 4);
            }
        }
        io_->seek(cur_pos + size_external, BasicIo::beg);
    }
    else
    {
        RevTagDetails revNikonNCTGTags[(sizeof(NikonNCTGTags)/sizeof(NikonNCTGTags[0]))];
        reverseTagDetails(NikonNCTGTags,revNikonNCTGTags,((sizeof(NikonNCTGTags)/sizeof(NikonNCTGTags[0]))));

        int32_t i;
        for(i = 0 ; i < 100 ; i++)
        {
            io_->read(buf.pData_, 4);
            TagID = Exiv2::getULong(buf.pData_, bigEndian);
            td = find(NikonNCTGTags, TagID);

            io_->read(buf.pData_, 2);
            dataType = Exiv2::getUShort(buf.pData_, bigEndian);

            std::memset(buf.pData_, 0x0, buf.size_);
            io_->read(buf.pData_, 2);

            if(TagID == 0x2000023)
            {
                uint64_t local_pos = io_->tell();
                dataLength = Exiv2::getUShort(buf.pData_, bigEndian);

                if(xmpData_["Xmp.video.PictureControlVersion"].count() >0)
                {
                    byte rawPictureControlVersion[4];
                    const std::string pictureControlVersion = xmpData_["Xmp.video.PictureControlVersion"].toString();
                    int32_t j;
                    for(j=0; j<4; j++)
                    {
                        rawPictureControlVersion[j] = pictureControlVersion[j];
                    }
                    io_->write(rawPictureControlVersion,4);
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                if(xmpData_["Xmp.video.PictureControlName"].count() >0)
                {
                    byte rawPictureControlName[20];
                    const std::string pictureControlName = xmpData_["Xmp.video.PictureControlName"].toString();
                    int32_t j;
                    for(j=0; j<min(20,(int)pictureControlName.size()); j++)
                    {
                        rawPictureControlName[j] = pictureControlName[j];
                    }
                    io_->write(rawPictureControlName,20);
                }
                else
                {
                    io_->seek(20,BasicIo::cur);
                }
                if(xmpData_["Xmp.video.PictureControlBase"].count() >0)
                {
                    byte rawPictureControlBase[20];
                    const std::string pictureControlBase = xmpData_["Xmp.video.PictureControlBase"].toString();
                    int32_t j;
                    for(j=0; j<min(20,(int)pictureControlBase.size()); j++)
                    {
                        rawPictureControlBase[j] = pictureControlBase[j];
                    }
                    io_->write(rawPictureControlBase,20);
                }
                else
                {
                    io_->seek(20,BasicIo::cur);
                }

                io_->seek(4,BasicIo::cur);   std::memset(buf.pData_, 0x0, buf.size_);

                if(xmpData_["Xmp.video.PictureControlAdjust"].count() >0)
                {
                    RevTagDetails revTagDetails[(sizeof(PictureControlAdjust)/sizeof(PictureControlAdjust[0]))];
                    reverseTagDetails(PictureControlAdjust,revTagDetails,((sizeof(PictureControlAdjust)/sizeof(PictureControlAdjust[0]))));

                    rtd2 = find(revTagDetails,xmpData_["Xmp.video.PictureControlAdjust"].toString());
                    if(rtd2)
                    {
                        byte rawPictureControlAdjust = (byte)rtd2->val_;
                        io_->write(&rawPictureControlAdjust,1);
                    }
                    else
                    {
                        io_->seek(1,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(1,BasicIo::cur);
                }
                if(xmpData_["Xmp.video.PictureControlQuickAdjust"].count() >0)
                {
                    RevTagDetails revTagDetails[(sizeof(NormalSoftHard)/sizeof(NormalSoftHard[0]))];
                    reverseTagDetails(NormalSoftHard,revTagDetails,((sizeof(NormalSoftHard)/sizeof(NormalSoftHard[0]))));

                    rtd2 = find(revTagDetails, xmpData_["Xmp.video.PictureControlQuickAdjust"].toString() );
                    if(rtd2)
                    {
                        byte rawPictureControlQuickAdjust = (byte)rtd2->val_;
                        io_->write(&rawPictureControlQuickAdjust,1);
                    }
                    else
                    {
                        io_->seek(1,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(1,BasicIo::cur);
                }
                if(xmpData_["Xmp.video.Sharpness"].count() >0)
                {
                    RevTagDetails revTagDetails[(sizeof(NormalSoftHard)/sizeof(NormalSoftHard[0]))];
                    reverseTagDetails(NormalSoftHard,revTagDetails,((sizeof(NormalSoftHard)/sizeof(NormalSoftHard[0]))));

                    rtd2 = find(revTagDetails, xmpData_["Xmp.video.Sharpness"].toString() );
                    if(rtd2)
                    {
                        byte rawSharpness = (byte)rtd2->val_;
                        io_->write(&rawSharpness,1);
                    }
                    else
                    {
                        io_->seek(1,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(1,BasicIo::cur);
                }
                if(xmpData_["Xmp.video.Contrast"].count() >0)
                {
                    RevTagDetails revTagDetails[(sizeof(NormalSoftHard)/sizeof(NormalSoftHard[0]))];
                    reverseTagDetails(NormalSoftHard,revTagDetails,((sizeof(NormalSoftHard)/sizeof(NormalSoftHard[0]))));

                    rtd2 = find(revTagDetails, xmpData_["Xmp.video.Contrast"].toString() );
                    if(rtd2)
                    {
                        byte rawContrast = (byte)rtd2->val_;
                        io_->write(&rawContrast,1);
                    }
                    else
                    {
                        io_->seek(1,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(1,BasicIo::cur);
                }
                if(xmpData_["Xmp.video.Brightness"].count() >0)
                {
                    RevTagDetails revTagDetails[(sizeof(NormalSoftHard)/sizeof(NormalSoftHard[0]))];
                    reverseTagDetails(NormalSoftHard,revTagDetails,((sizeof(NormalSoftHard)/sizeof(NormalSoftHard[0]))));

                    rtd2 = find(revTagDetails, xmpData_["Xmp.video.Brightness"].toString() );
                    if(rtd2)
                    {
                        byte rawBrightness = (byte)rtd2->val_;
                        io_->write(&rawBrightness,1);
                    }
                    else
                    {
                        io_->seek(1,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(1,BasicIo::cur);
                }
                if(xmpData_["Xmp.video.Saturation"].count() >0)
                {
                    RevTagDetails revTagDetails[(sizeof(Saturation)/sizeof(Saturation[0]))];
                    reverseTagDetails(Saturation,revTagDetails,((sizeof(Saturation)/sizeof(Saturation[0]))));

                    rtd2 = find(revTagDetails, xmpData_["Xmp.video.Saturation"].toString() );
                    if(rtd2)
                    {
                        byte rawSaturation = (byte)rtd2->val_;
                        io_->write(&rawSaturation,1);
                    }
                    else
                    {
                        io_->seek(1,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(1,BasicIo::cur);
                }
                if(xmpData_["Xmp.video.HueAdjustment"].count() >0)
                {
                    byte rawHueAdjustment = (byte)xmpData_["Xmp.video.HueAdjustment"].toString()[0];
                    io_->write(&rawHueAdjustment,1);
                }
                else
                {
                    io_->seek(1,BasicIo::cur);
                }
                if(xmpData_["Xmp.video.FilterEffect"].count() >0)
                {
                    RevTagDetails revTagDetails[(sizeof(FilterEffect)/sizeof(FilterEffect[0]))];
                    reverseTagDetails(FilterEffect,revTagDetails,((sizeof(FilterEffect)/sizeof(FilterEffect[0]))));

                    rtd2 = find(revTagDetails, xmpData_["Xmp.video.FilterEffect"].toString() );
                    if(rtd2)
                    {
                        byte rawFilterEffect = (byte)rtd2->val_;
                        io_->write(&rawFilterEffect,1);
                    }
                    else
                    {
                        io_->seek(1,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(1,BasicIo::cur);
                }
                if(xmpData_["Xmp.video.ToningEffect"].count() >0)
                {
                    RevTagDetails revTagDetails[(sizeof(ToningEffect)/sizeof(ToningEffect[0]))];
                    reverseTagDetails(ToningEffect,revTagDetails,((sizeof(ToningEffect)/sizeof(ToningEffect[0]))));

                    rtd2 = find(revTagDetails, xmpData_["Xmp.video.ToningEffect"].toString() );
                    if(rtd2)
                    {
                        byte rawToningEffect = (byte)rtd2->val_;
                        io_->write(&rawToningEffect,1);
                    }
                    else
                    {
                        io_->seek(1,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(1,BasicIo::cur);
                }
                if(xmpData_["Xmp.video.ToningSaturation"].count() >0)
                {
                    byte rawToningSaturation = (byte)xmpData_["Xmp.video.ToningSaturation"].toString()[0];
                    io_->write(&rawToningSaturation,1);
                }
                io_->seek(local_pos + dataLength, BasicIo::beg);
            }

            else if(TagID == 0x2000024)
            {
                uint64_t local_pos = io_->tell();
                dataLength = Exiv2::getUShort(buf.pData_, bigEndian);
                std::memset(buf.pData_, 0x0, buf.size_);

                if(xmpData_["Xmp.video.TimeZone"].count() >0)
                {
                    uint16_t timeZone = xmpData_["Xmp.video.TimeZone"].toLong();
                    io_->write((byte*)&timeZone,2);
                }
                else
                {
                    io_->seek(2,BasicIo::cur);
                }
                if(xmpData_["Xmp.video.DayLightSavings"].count() >0)
                {
                    RevTagDetails revTagDetails[(sizeof(YesNo)/sizeof(YesNo[0]))];
                    reverseTagDetails(YesNo,revTagDetails,((sizeof(YesNo)/sizeof(YesNo[0]))));

                    rtd2 = find(revTagDetails, xmpData_["Xmp.video.DayLightSavings"].toString() );
                    if(rtd2)
                    {
                        byte rawDayLightSavings = (byte)rtd2->val_;
                        io_->write(&rawDayLightSavings,1);
                    }
                    else
                    {
                        io_->seek(1,BasicIo::cur);
                    }
                }
                if(xmpData_["Xmp.video.DateDisplayFormat"].count() >0)
                {
                    RevTagDetails revTagDetails[(sizeof(DateDisplayFormat)/sizeof(DateDisplayFormat[0]))];
                    reverseTagDetails(DateDisplayFormat,revTagDetails,((sizeof(DateDisplayFormat)/sizeof(DateDisplayFormat[0]))));

                    rtd2 = find(revTagDetails, xmpData_["Xmp.video.DateDisplayFormat"].toString() );
                    if(rtd2)
                    {
                        byte rawDateDisplayFormat = (byte)rtd2->val_;
                        io_->write(&rawDateDisplayFormat,1);
                    }
                    else
                    {
                        io_->seek(1,BasicIo::cur);
                    }
                }
                io_->seek(local_pos + dataLength, BasicIo::beg);
            }

            else if(dataType == 2 || dataType == 7)
            {
                dataLength = Exiv2::getUShort(buf.pData_, bigEndian);
                std::memset(buf.pData_, 0x0, buf.size_);

                // Sanity check with an "unreasonably" large number
                if (dataLength > 200)
                {
#ifndef SUPPRESS_WARNINGS
                    EXV_ERROR << "Xmp.video Nikon Tags, dataLength was found to be larger than 200."
                              << " Entries considered invalid. Not Processed.\n";
#endif
                    io_->seek(io_->tell() + dataLength, BasicIo::beg);
                }
                else
                {
                    if(td && (xmpData_[exvGettext(td->label_)].count() >0))
                    {
                        byte rawTagData[dataLength];
                        const std::string m_strData = xmpData_[exvGettext(td->label_)].toString();
                        int32_t j;
                        for(j=0; j<min(dataLength,(uint16_t)m_strData.size()); j++)
                        {
                            rawTagData[j] = m_strData[j];
                        }
                        io_->write(rawTagData,dataLength);
                    }
                    else
                    {
                        io_->seek(dataLength,BasicIo::cur);
                    }
                }
            }
            else if(dataType == 4)
            {
                dataLength = Exiv2::getUShort(buf.pData_, bigEndian) * 4;
                if(td && (xmpData_[exvGettext(td->label_)].count() >0) && (dataLength < 200))
                {
                    byte rawTagData[4];
                    const int32_t tagData = xmpData_[exvGettext(td->label_)].toLong();
                    io_->write((byte*)&rawTagData,4);
                    io_->seek((dataLength-4),BasicIo::cur);
                }
                else
                {
                    io_->seek(dataLength,BasicIo::cur);
                }
            }
            else if(dataType == 3)
            {
                dataLength = Exiv2::getUShort(buf.pData_, bigEndian) * 2;
                std::memset(buf.pData_, 0x0, buf.size_);
                io_->read(buf.pData_, 2);
                if(td &&(xmpData_[exvGettext(td->label_)].count() >0) && (dataLength < 200))
                {
                    const uint16_t tagData = (uint16_t)xmpData_[exvGettext(td->label_)].toLong();
                    io_->write((byte*)&tagData,2);
                    io_->seek((dataLength-2),BasicIo::cur);
                }
                else
                {
                    io_->seek(dataLength,BasicIo::cur);
                }
            }
            else if(dataType == 5)
            {
                dataLength = Exiv2::getUShort(buf.pData_, bigEndian) * 8;
                std::memset(buf.pData_, 0x0, buf.size_);
                io_->read(buf.pData_, 4);
                io_->read(buf2.pData_, 4);
                if(td && (xmpData_[exvGettext(td->label_)].count() >0) &&(dataLength < 200))
                {
                    io_->seek(-8,BasicIo::cur);
                    const int32_t tagData = (int32_t)((double)xmpData_[exvGettext(td->label_)].toLong()*
                            (double)Exiv2::getULong( buf2.pData_, bigEndian));
                    io_->write((byte*)&tagData,4);
                    io_->seek((dataLength-4),BasicIo::cur);
                }
                else
                {
                    io_->seek(dataLength,BasicIo::cur);
                }
            }
            else if(dataType == 8)
            {
                dataLength = Exiv2::getUShort(buf.pData_, bigEndian) * 2;
                std::memset(buf.pData_, 0x0, buf.size_);
                if(td && (xmpData_[exvGettext(td->label_)].count() >0) &&(dataLength < 200))
                {
                    vector<uint16_t> dataEight = getNumberFromString(xmpData_[exvGettext(td->label_)].toString(),' ');
                    io_->write((byte*)&dataEight[0],2);
                    io_->write((byte*)&dataEight[1],2);
                    io_->seek(dataLength - 4,BasicIo::cur);
                }
                else
                {
                    io_->seek(dataLength - 4,BasicIo::cur);
                }
            }
        }
        io_->seek(cur_pos + size_external, BasicIo::beg);
    }
    return;
} // QuickTimeVideo::NikonTagsDecoder

void QuickTimeVideo::setMediaStream()
{
    uint64_t current_position = io_->tell();
    DataBuf buf(4+1);

    while(!io_->eof())
    {
        io_->read(buf.pData_, 4);
        if (equalsQTimeTag(buf, "hdlr"))
        {
            io_->seek(8,BasicIo::cur);
            io_->read(buf.pData_, 4);

            if (equalsQTimeTag(buf, "vide"))
                currentStream_ = Video;
            else if(equalsQTimeTag(buf, "soun"))
                currentStream_ = Audio;
            else if (equalsQTimeTag(buf, "hint"))
                currentStream_ = Hint;
            else
                currentStream_ = GenMediaHeader;
            break;
        }
    }
    io_->seek(current_position, BasicIo::beg);
} // QuickTimeVideo::setMediaStream

void QuickTimeVideo::timeToSampleDecoder()
{
    DataBuf buf(4+1);
    io_->read(buf.pData_, 4);
    io_->read(buf.pData_, 4);
    uint64_t noOfEntries, totalframes = 0, timeOfFrames = 0;
    noOfEntries = returnUnsignedBufValue(buf);
    uint64_t temp;
    uint32_t i;
    for( i=1; i <= noOfEntries; i++)
    {
        io_->read(buf.pData_, 4);
        temp = returnBufValue(buf);
        totalframes += temp;
        io_->read(buf.pData_, 4);
        timeOfFrames += temp * returnBufValue(buf);
    }
    if(!m_modifyMetadata)
    {
        if (currentStream_ == Video)
            xmpData_["Xmp.video.FrameRate"] = (double)totalframes * (double)timeScale_ / (double)timeOfFrames;
    }
    else
    {
        if(xmpData_["Xmp.video.FrameRate"].count() >0)
        {//TODO:implement write
        }
    }
    return;
} // QuickTimeVideo::timeToSampleDecoder

void QuickTimeVideo::sampleDesc(uint32_t  size)
{
    DataBuf buf(100);
    uint64_t cur_pos = io_->tell();
    io_->read(buf.pData_, 4);
    io_->read(buf.pData_, 4);
    uint64_t noOfEntries;
    noOfEntries = returnUnsignedBufValue(buf);
    uint32_t i;
    for(i = 1; i <= noOfEntries; i++)
    {
        if (currentStream_ == Video)
            imageDescDecoder();
        else if (currentStream_ == Audio)
            audioDescDecoder();
    }
    io_->seek(cur_pos + size, BasicIo::beg);
    return;
} // QuickTimeVideo::sampleDesc

void QuickTimeVideo::audioDescDecoder()
{
    DataBuf buf(40);
    std::memset(buf.pData_, 0x0, buf.size_);
    buf.pData_[4] = '\0';
    io_->read(buf.pData_, 4);
    uint64_t size = 82;

    const TagVocabulary* td;

    if(!m_modifyMetadata)
    {
        int32_t i;
        for (i = 0; size/4 != 0 ; size -= 4, i++)
        {
            io_->read(buf.pData_, 4);
            switch(i)
            {
            case AudioFormat:
                td = find(qTimeFileType, Exiv2::toString( buf.pData_));
                if(td)
                    xmpData_["Xmp.audio.Compressor"] = exvGettext(td->label_);
                else
                    xmpData_["Xmp.audio.Compressor"] = Exiv2::toString( buf.pData_);
                break;
            case AudioVendorID:
                td = find(vendorIDTags, Exiv2::toString( buf.pData_));
                if(td)
                    xmpData_["Xmp.audio.VendorID"] = exvGettext(td->label_);
                break;
            case AudioChannels:
                xmpData_["Xmp.audio.ChannelType"] = returnBufValue(buf, 2);
                xmpData_["Xmp.audio.BitsPerSample"] = (buf.pData_[2] * 256 + buf.pData_[3]);
                break;
            case AudioSampleRate:
                xmpData_["Xmp.audio.SampleRate"] = returnBufValue(buf, 2) + ((buf.pData_[2] * 256 + buf.pData_[3]) * 0.01);
                break;
            default:
                break;
            }
        }
        io_->read(buf.pData_, static_cast<int32_t>(size % 4));	//cause size is so small, this cast should be right.
    }
    else
    {
        TagVocabulary revTagVocabulary[(sizeof(qTimeFileType)/sizeof(qTimeFileType[0]))];
        reverseTagVocabulary(qTimeFileType,revTagVocabulary,((sizeof(qTimeFileType)/sizeof(qTimeFileType[0]))));

        TagVocabulary revVendorIDTags[(sizeof(vendorIDTags)/sizeof(vendorIDTags[0]))];
        reverseTagVocabulary(vendorIDTags,revVendorIDTags,((sizeof(vendorIDTags)/sizeof(vendorIDTags[0]))));

        int32_t i;
        for (i = 0; size/4 != 0 ; size -= 4, i++)
        {
            switch(i)
            {
            case AudioFormat:
                if(xmpData_["Xmp.audio.Compressor"].count() > 0)
                {
                    byte rawCompressor[4];
                    td = find(revTagVocabulary,xmpData_["Xmp.audio.Compressor"].toString());
                    if(td)
                    {
                        const char* compressor = td->label_;
                        int32_t j;
                        for(j=0; j<4; j++)
                        {
                            rawCompressor[j] = compressor[j];
                        }
                        io_->write(rawCompressor,4);
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case AudioVendorID:
                if(xmpData_["Xmp.audio.VendorID"].count() >0)
                {
                    byte rawVendorID[4];
                    td = find(revVendorIDTags,xmpData_["Xmp.audio.VendorID"].toString());
                    if(td)
                    {
                        const char* vendorID = td->label_;
                        int32_t j;
                        for(j=0; j<4; j++)
                        {
                            rawVendorID[j] = vendorID[j];
                        }
                        io_->write(rawVendorID,4);
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case AudioChannels:
                if(xmpData_["Xmp.audio.ChannelType"].count() >0)
                {
                    uint16_t channelType =(uint16_t) xmpData_["Xmp.audio.ChannelType"].toLong();
                    io_->write((byte*)&channelType,2);
                }
                else
                {
                    io_->seek(2,BasicIo::cur);
                }
                if(xmpData_["Xmp.audio.BitsPerSample"].count() >0)
                {
                    uint16_t bitsPerSample =(uint16_t) (xmpData_["Xmp.audio.BitsPerSample"].toLong()/256);
                    io_->write((byte*)&bitsPerSample,2);
                }
                else
                {
                    io_->seek(2,BasicIo::cur);
                }
                break;
            case AudioSampleRate:
                if(xmpData_["Xmp.audio.SampleRate"].count() >0)
                {
                    DataBuf rawSampleRate(5);
                    rawSampleRate.pData_[4] = '\0';
                    const int64_t sampleRate = (int64_t)xmpData_["Xmp.audio.SampleRate"].toLong();
                    rawSampleRate = returnBuf(sampleRate,4);
                    io_->write(rawSampleRate.pData_,4);
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            default:
                io_->seek(4,BasicIo::cur);
                break;
            }
        }
        io_->seek(static_cast<int32_t>(size % 4),BasicIo::cur);
    }
    return;
} // QuickTimeVideo::audioDescDecoder

void QuickTimeVideo::imageDescDecoder()
{
    DataBuf buf(40);
    std::memset(buf.pData_, 0x0, buf.size_);
    buf.pData_[4] = '\0';
    io_->read(buf.pData_, 4);
    uint64_t size = 82;

    const TagVocabulary* td;

    if(!m_modifyMetadata)
    {
        int32_t i;
        for (i = 0; size/4 != 0 ; size -= 4, i++)
        {
            io_->read(buf.pData_, 4);

            switch(i)
            {
            case codec:
                td = find(qTimeFileType, Exiv2::toString( buf.pData_));
                if(td)
                    xmpData_["Xmp.video.Codec"] = exvGettext(td->label_);
                else
                    xmpData_["Xmp.video.Codec"] = Exiv2::toString( buf.pData_);
                break;
            case VendorID:
                td = find(vendorIDTags, Exiv2::toString( buf.pData_));
                if(td)
                    xmpData_["Xmp.video.VendorID"] = exvGettext(td->label_);
                break;
            case SourceImageWidth_Height:
                xmpData_["Xmp.video.SourceImageWidth"] = returnBufValue(buf, 2);
                xmpData_["Xmp.video.SourceImageHeight"] = (buf.pData_[2] * 256 + buf.pData_[3]);
                break;
            case XResolution:
                xmpData_["Xmp.video.XResolution"] = returnBufValue(buf, 2) + ((buf.pData_[2] * 256 + buf.pData_[3]) * 0.01);
                break;
            case YResolution:
                xmpData_["Xmp.video.YResolution"] = returnBufValue(buf, 2) + ((buf.pData_[2] * 256 + buf.pData_[3]) * 0.01);
                io_->read(buf.pData_, 3); size -= 3;
                break;
            case CompressorName:
                io_->read(buf.pData_, 32); size -= 32;
                xmpData_["Xmp.video.Compressor"] = Exiv2::toString( buf.pData_);
                break;
            default:
                break;
            }
        }
        io_->read(buf.pData_, static_cast<int32_t>(size % 4));
        xmpData_["Xmp.video.BitDepth"] = returnBufValue(buf, 1);
    }
    else
    {
        TagVocabulary revTagVocabulary[(sizeof(qTimeFileType)/sizeof(qTimeFileType[0]))];
        reverseTagVocabulary(qTimeFileType,revTagVocabulary,((sizeof(qTimeFileType)/sizeof(qTimeFileType[0]))));

        TagVocabulary revVendorIDTags[(sizeof(vendorIDTags)/sizeof(vendorIDTags[0]))];
        reverseTagVocabulary(vendorIDTags,revVendorIDTags,((sizeof(vendorIDTags)/sizeof(vendorIDTags[0]))));

        int32_t i;
        for (i = 0; size/4 != 0 ; size -= 4, i++)
        {
            switch(i)
            {
            case codec:
                if(xmpData_["Xmp.video.Codec"].count() >0)
                {
                    std::string codecName = xmpData_["Xmp.video.Codec"].toString();
                    td = find(revTagVocabulary, codecName);
                    byte rawCodec[4];
                    if(td)
                    {
                        const char *codecNameTag = td->label_;
                        int32_t j;
                        for(j=0; j<4 ;j++)
                        {
                            rawCodec[j] = (byte)codecNameTag[j];
                        }
                    }
                    else
                    {
                        int32_t j;
                        for(j=0; j<4 ;j++)
                        {
                            rawCodec[j] = (byte)codecName[j];
                        }
                    }
                    io_->write(rawCodec,4);
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case VendorID:
                if(xmpData_["Xmp.video.VendorID"].count() >0)
                {
                    td = find(revVendorIDTags, xmpData_["Xmp.video.VendorID"].toString());
                    if(td)
                    {
                        byte rawVendorID[4];
                        const char *vendorID = td->label_;
                        int32_t j;
                        for(j=0; j<4 ;j++)
                        {
                            rawVendorID[j] = vendorID[j];
                        }
                        io_->write(rawVendorID,4);
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case SourceImageWidth_Height:
                if(xmpData_["Xmp.video.SourceImageWidth"].count() >0)
                {
                    DataBuf imageWidth = returnBuf((int64_t)xmpData_["Xmp.video.SourceImageWidth"].toLong(),2);
                    io_->write(imageWidth.pData_,2);
                }
                else
                {
                    io_->seek(2,BasicIo::cur);
                }
                if(xmpData_["Xmp.video.SourceImageHeight"].count() >0)
                {
                    DataBuf imageHeight = returnBuf(((int64_t)(xmpData_["Xmp.video.SourceImageHeight"].toLong() -
                                                     xmpData_["Xmp.video.SourceImageHeight"].toLong()/65536)*65536));
                    io_->write(imageHeight.pData_,2);
                }
                else
                {
                    io_->seek(2,BasicIo::cur);
                }
                break;
            case XResolution:
                //This field should not be edited
                io_->seek(4,BasicIo::cur);
                break;
            case YResolution:
                io_->seek(7,BasicIo::cur);
                size -= 3;
                break;
            case CompressorName:
                if(xmpData_["Xmp.video.Compressor"].count() >0)
                {
                    byte rawCompressor[32] = {};
                    const std::string compressor = xmpData_["Xmp.video.Compressor"].toString();
                    int32_t j;
                    for(j=0;j<min((int32_t)32,(int32_t)compressor.size()); j++)
                    {
                        rawCompressor[j] = (byte)compressor[j];
                    }
                    io_->seek(4,BasicIo::cur);
                    io_->write(rawCompressor,32);
                }
                else
                {
                    io_->seek(36,BasicIo::cur);
                }
                size -= 32;
                break;
            default:
                io_->seek(4,BasicIo::cur);
                break;
            }
        }
        if(xmpData_["Xmp.video.BitDepth"].count() > 0)
        {
            DataBuf rawBitDepth(1);
            rawBitDepth = returnBuf((int64_t)xmpData_["Xmp.video.BitDepth"].toLong(),1);
            io_->write(rawBitDepth.pData_,1);
        }
        io_->seek(static_cast<int32_t>(size % 4),BasicIo::cur);
    }
    return;
} // QuickTimeVideo::imageDescDecoder

void QuickTimeVideo::multipleEntriesDecoder()
{
    DataBuf buf(5);
    io_->read(buf.pData_, 4);
    io_->read(buf.pData_, 4);
    uint64_t noOfEntries;

    noOfEntries = returnUnsignedBufValue(buf);

    uint32_t i;
    for( i = 1; i <= noOfEntries; i++)
        decodeBlock();
    return;
} // QuickTimeVideo::multipleEntriesDecoder

void QuickTimeVideo::videoHeaderDecoder(uint32_t size)
{
    DataBuf buf(3);
    std::memset(buf.pData_, 0x0, buf.size_);
    buf.pData_[2] = '\0';
    currentStream_ = Video;

    const RevTagDetails* rtd;
    const TagDetails* td;

    if(!m_modifyMetadata)
    {
        int32_t i;
        for (i = 0; size/2 != 0 ; size -= 2, i++)
        {
            io_->read(buf.pData_, 2);

            switch(i)
            {
            case GraphicsMode:
                td = find(graphicsModetags, returnBufValue(buf,2));
                if(td)
                    xmpData_["Xmp.video.GraphicsMode"] = exvGettext(td->label_);
                break;
            case OpColor:
                xmpData_["Xmp.video.OpColor"] = returnBufValue(buf,2);
                break;
            default:
                break;
            }
        }
        io_->read(buf.pData_, size % 2);
    }
    else
    {
        RevTagDetails revTagDetails[(sizeof(graphicsModetags)/sizeof(graphicsModetags[0]))];
        reverseTagDetails(graphicsModetags,revTagDetails,((sizeof(graphicsModetags)/sizeof(graphicsModetags[0]))));

        int32_t i;
        for (i = 0; size/2 != 0 ; size -= 2, i++)
        {
            switch(i)
            {
            case GraphicsMode:
                if(xmpData_["Xmp.video.GraphicsMode"].count() >0)
                {
                    DataBuf rawGraphicsMode(2);
                    rtd = find(revTagDetails,xmpData_["Xmp.video.GraphicsMode"].toString());
                    if(rtd)
                    {
                        rawGraphicsMode = returnBuf((int64_t)rtd->val_,2);
                        io_->write(rawGraphicsMode.pData_,2);
                    }
                    else
                    {
                        io_->seek(2,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(2,BasicIo::cur);
                }
                break;
            case OpColor:
                if(xmpData_["Xmp.video.OpColor"].count() >0)
                {
                    DataBuf rawOpColor((uint32_t)3);
                    rawOpColor.pData_[2] = '\0';
                    const int64_t opColor = xmpData_["Xmp.video.OpColor"].toLong();
                    rawOpColor = returnBuf(opColor,2);
                    io_->write(rawOpColor.pData_,2);
                }
                else
                {
                    io_->seek(2,BasicIo::cur);
                }
                break;
            default:
                io_->seek(2,BasicIo::cur);
                break;
            }
        }
        io_->seek(size % 2,BasicIo::cur);
    }
    return;
} // QuickTimeVideo::videoHeaderDecoder

void QuickTimeVideo::handlerDecoder(uint32_t size)
{
    uint64_t cur_pos = io_->tell();
    DataBuf buf(100);
    std::memset(buf.pData_, 0x0, buf.size_);
    buf.pData_[4] = '\0';

    const TagVocabulary* tv;

    if(!m_modifyMetadata)
    {
        int32_t i;
        for (i = 0; i < 5 ; i++)
        {
            io_->read(buf.pData_, 4);

            switch(i)
            {
            case HandlerClass:
                tv = find(handlerClassTags, Exiv2::toString( buf.pData_));
                if(tv)
                {
                    if (currentStream_ == Video)
                        xmpData_["Xmp.video.HandlerClass"] = exvGettext(tv->label_);
                    else if (currentStream_ == Audio)
                        xmpData_["Xmp.audio.HandlerClass"] = exvGettext(tv->label_);
                }
                break;
            case HandlerType:
                tv = find(handlerTypeTags, Exiv2::toString( buf.pData_));
                if(tv)
                {
                    if (currentStream_ == Video)
                        xmpData_["Xmp.video.HandlerType"] = exvGettext(tv->label_);
                    else if (currentStream_ == Audio)
                        xmpData_["Xmp.audio.HandlerType"] = exvGettext(tv->label_);
                }
                break;
            case HandlerVendorID:
                tv = find(vendorIDTags, Exiv2::toString( buf.pData_));
                if(tv)
                {
                    if (currentStream_ == Video)
                        xmpData_["Xmp.video.HandlerVendorID"] = exvGettext(tv->label_);
                    else if (currentStream_ == Audio)
                        xmpData_["Xmp.audio.HandlerVendorID"] = exvGettext(tv->label_);
                }
                break;
            default:
                break;
            }
        }
        io_->seek(cur_pos + size, BasicIo::beg);
    }
    else
    {
        TagVocabulary revTagVocabulary[(sizeof(handlerClassTags)/sizeof(handlerClassTags[0]))];
        reverseTagVocabulary(handlerClassTags,revTagVocabulary,((sizeof(handlerClassTags)/sizeof(handlerClassTags[0]))));

        TagVocabulary revVendorIDTags[(sizeof(vendorIDTags)/sizeof(vendorIDTags[0]))];
        reverseTagVocabulary(vendorIDTags,revVendorIDTags,((sizeof(vendorIDTags)/sizeof(vendorIDTags[0]))));

        TagVocabulary revHandlerType[(sizeof(handlerTypeTags)/sizeof(handlerTypeTags[0]))];
        reverseTagVocabulary(handlerTypeTags,revHandlerType,((sizeof(handlerTypeTags)/sizeof(handlerTypeTags[0]))));

        int32_t i;
        for (i = 0; i < 5 ; i++)
        {
            switch(i)
            {
            case HandlerClass:
                if (currentStream_ == Video)
                {
                    if(xmpData_["Xmp.video.HandlerClass"].count() >0)
                    {
                        byte rawHandlerClass[4];
                        tv = find(revTagVocabulary, xmpData_["Xmp.video.HandlerClass"].toString());

                        if(tv)
                        {
                            const std::string handlerClass = tv->label_;
                            int32_t j;
                            for(j=0; j<4; j++)
                            {
                                rawHandlerClass[j] = handlerClass[j];
                            }
                            io_->write(rawHandlerClass,4);
                        }
                        else
                        {
                            io_->seek(4,BasicIo::cur);
                        }
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else if (currentStream_ == Audio)
                {
                    if(xmpData_["Xmp.audio.HandlerClass"].count() >0)
                    {
                        byte rawHandlerClass[4];
                        tv = find(revTagVocabulary, xmpData_["Xmp.audio.HandlerClass"].toString());
                        if(tv)
                        {
                            const std::string handlerClass = tv->label_;
                            int32_t j;
                            for(j=0; j<4; j++)
                            {
                                rawHandlerClass[j] = handlerClass[j];
                            }
                            io_->write(rawHandlerClass,4);
                        }
                        else
                        {
                            io_->seek(4,BasicIo::cur);
                        }
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case HandlerType:
                if (currentStream_ == Video)
                {
                    if(xmpData_["Xmp.video.HandlerType"].count() >0)
                    {
                        byte rawHandlerType[4];
                        tv = find(revHandlerType, xmpData_["Xmp.video.HandlerType"].toString());
                        if(tv)
                        {
                            const std::string handlerType = tv->label_;
                            int32_t j;
                            for(j=0; j<4; j++)
                            {
                                rawHandlerType[j] = handlerType[j];
                            }
                            io_->write(rawHandlerType,4);
                        }
                        else
                        {
                            io_->seek(4,BasicIo::cur);
                        }
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else if (currentStream_ == Audio)
                {
                    if(xmpData_["Xmp.audio.HandlerType"].count() >0)
                    {
                        byte rawHandlerType[4];
                        tv = find(revTagVocabulary, xmpData_["Xmp.audio.HandlerType"].toString());
                        if(tv)
                        {
                            const std::string handlerType = tv->label_;
                            int32_t j;
                            for(j=0; j<4; j++)
                            {
                                rawHandlerType[j] = handlerType[j];
                            }
                            io_->write(rawHandlerType,4);
                        }
                        else
                        {
                            io_->seek(4,BasicIo::cur);
                        }
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case HandlerVendorID:
                if (currentStream_ == Video)
                {
                    if(xmpData_["Xmp.video.HandlerVendorID"].count() >0)
                    {
                        byte rawHandlerVendorID[4];
                        tv = find(revVendorIDTags, xmpData_["Xmp.video.HandlerVendorID"].toString());
                        if(tv)
                        {
                            const std::string handlerVendorID = tv->label_;
                            int32_t j;
                            for(j=0; j<4; j++)
                            {
                                rawHandlerVendorID[j] = handlerVendorID[j];
                            }
                            io_->write(rawHandlerVendorID,4);
                        }
                        else
                        {
                            io_->seek(4,BasicIo::cur);
                        }
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else if (currentStream_ == Audio)
                {
                    if(xmpData_["Xmp.audio.HandlerVendorID"].count() >0)
                    {
                        byte rawHandlerVendorID[4];
                        tv = find(revTagVocabulary, xmpData_["Xmp.audio.HandlerVendorID"].toString());
                        if(tv)
                        {
                            const std::string handlerVendorID = tv->label_;
                            int32_t j;
                            for(j=0; j<4; j++)
                            {
                                rawHandlerVendorID[j] = handlerVendorID[j];
                            }
                            io_->write(rawHandlerVendorID,4);
                        }
                        else
                        {
                            io_->seek(4,BasicIo::cur);
                        }
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            default:
                io_->seek(4,BasicIo::cur);
                break;
            }
        }
        io_->seek(cur_pos + size, BasicIo::beg);
    }
    return;
} // QuickTimeVideo::handlerDecoder

void QuickTimeVideo::fileTypeDecoder(uint32_t size)
{
    DataBuf buf(5);
    const int32_t cur_pos = io_->tell();
    std::memset(buf.pData_, 0x0, buf.size_);
    buf.pData_[4] = '\0';
    Exiv2::Value::AutoPtr v = Exiv2::Value::create(Exiv2::xmpSeq);
    const TagVocabulary* td;

    if(!m_modifyMetadata)
    {
        int32_t i;
        for (i = 0; size/4 != 0; size -=4, i++)
        {
            io_->read(buf.pData_, 4);
            td = find(qTimeFileType, Exiv2::toString( buf.pData_));
            switch(i)
            {
            case 0:
                if(td)
                    xmpData_["Xmp.video.MajorBrand"] = exvGettext(td->label_);
                break;
            case 1:
                xmpData_["Xmp.video.MinorVersion"] = returnBufValue(buf);
                break;
            default:
                if(td)
                    v->read(exvGettext(td->label_));
                else
                    v->read(Exiv2::toString(buf.pData_));
                break;
            }
            xmpData_.add(Exiv2::XmpKey("Xmp.video.CompatibleBrands"), v.get());
            io_->read(buf.pData_, size%4);
        }
    }
    else
    {
        TagVocabulary revTagVocabulary[(sizeof(qTimeFileType)/sizeof(qTimeFileType[0]))];
        reverseTagVocabulary(qTimeFileType,revTagVocabulary,((sizeof(qTimeFileType)/sizeof(qTimeFileType[0]))));
        DataBuf buf;
        int32_t i;
        for (i = 0; size/4 != 0; size -=4, i++)
        {
            switch(i)
            {
            case 0:
                if(xmpData_["Xmp.video.MajorBrand"].count() > 0)
                {
                    byte rawMajorBrand[4];
                    td = find(revTagVocabulary, xmpData_["Xmp.video.MajorBrand"].toString());
                    const char *majorBrandVoc = td->label_;
                    int32_t j;
                    for(j=0; j<4 ;j++)
                    {
                        rawMajorBrand[j] = majorBrandVoc[i];
                    }
                    io_->write(rawMajorBrand,4);
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case 1:
                if(xmpData_["Xmp.video.MinorVersion"].count() >0)
                {
                    buf = returnBuf((int64_t)xmpData_["Xmp.video.MinorVersion"].toLong());
                    io_->write(buf.pData_,4);
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            default:
                if(xmpData_["Xmp.video.CompatibleBrands"].count() >0)
                {
                    byte rawCompatibleBrand[4];
                    td = find(revTagVocabulary, xmpData_["Xmp.video.CompatibleBrands"].toString());
                    if(td)
                    {
                        const char *compatibleBrandVoc = td->label_;
                        int32_t j;
                        for(j=0; j<4 ;j++)
                        {
                            rawCompatibleBrand[i]  = compatibleBrandVoc[i];
                        }
                    }
                    else
                    {
                        const std::string compatibleBrand = xmpData_["Xmp.video.CompatibleBrands"].toString();
                        int32_t j;
                        for(j=0; j<4; j++)
                        {
                            rawCompatibleBrand[i] = compatibleBrand[i];
                        }
                    }
                    io_->write(rawCompatibleBrand,4);
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            }
            io_->seek(size%4,BasicIo::cur);
        }
    }
    return;
} // QuickTimeVideo::fileTypeDecoder

void QuickTimeVideo::mediaHeaderDecoder(uint32_t size)
{
    DataBuf buf(5);
    std::memset(buf.pData_, 0x0, buf.size_);
    buf.pData_[4] = '\0';
    int64_t time_scale = 1;
    const TagDetails *td;
    const RevTagDetails *rtd;

    if(!m_modifyMetadata)
    {
        int32_t i;
        for (i = 0; size/4 != 0 ; size -=4, i++)
        {
            io_->read(buf.pData_, 4);

            switch(i)
            {
            case MediaHeaderVersion:
                if(currentStream_ == Video)
                    xmpData_["Xmp.video.MediaHeaderVersion"] = returnBufValue(buf,1);
                else if (currentStream_ == Audio)
                    xmpData_["Xmp.audio.MediaHeaderVersion"] = returnBufValue(buf,1);
                break;
            case MediaCreateDate:
                //A 32-bit integer that specifies (in seconds since midnight, January 1, 1904) when the movie atom was created.
                if(currentStream_ == Video)
                    xmpData_["Xmp.video.MediaCreateDate"] = returnUnsignedBufValue(buf);
                else if (currentStream_ == Audio)
                    xmpData_["Xmp.audio.MediaCreateDate"] = returnUnsignedBufValue(buf);
                break;
            case MediaModifyDate:
                //A 32-bit integer that specifies (in seconds since midnight, January 1, 1904) when the movie atom was created.
                if(currentStream_ == Video)
                    xmpData_["Xmp.video.MediaModifyDate"] = returnUnsignedBufValue(buf);
                else if (currentStream_ == Audio)
                    xmpData_["Xmp.audio.MediaModifyDate"] = returnUnsignedBufValue(buf);
                break;
            case MediaTimeScale:
                if(currentStream_ == Video)
                    xmpData_["Xmp.video.MediaTimeScale"] = returnBufValue(buf);
                else if (currentStream_ == Audio)
                    xmpData_["Xmp.audio.MediaTimeScale"] = returnBufValue(buf);
                time_scale = returnBufValue(buf);
                break;
            case MediaDuration:
                if(currentStream_ == Video)
                    xmpData_["Xmp.video.MediaDuration"] = returnBufValue(buf)/time_scale;
                else if (currentStream_ == Audio)
                    xmpData_["Xmp.audio.MediaDuration"] = returnBufValue(buf)/time_scale;
                break;
            case MediaLanguageCode:
                if(currentStream_ == Video)
                {
                    DataBuf tmpBuf((uint32_t)5);
                    tmpBuf.pData_[1] = buf.pData_[2];
                    tmpBuf.pData_[0] = buf.pData_[3];
                    xmpData_["Xmp.video.Quality"] = returnUnsignedBufValue(tmpBuf,2);
                    xmpData_["Xmp.video.MediaLangCode"] = returnUnsignedBufValue(buf,2);
                    td = find(mediaLanguageCode,returnUnsignedBufValue(buf,2));
                    if(td)
                    {
                        xmpData_["Xmp.video.MediaLanguage"] = Exiv2::StringValue(td->label_);
                    }
                }
                else if (currentStream_ == Audio)
                {
                    DataBuf tmpBuf((uint32_t)5);
                    tmpBuf.pData_[1] = buf.pData_[2];
                    tmpBuf.pData_[0] = buf.pData_[3];
                    xmpData_["Xmp.audio.Quality"] = returnUnsignedBufValue(tmpBuf,2);
                    xmpData_["Xmp.audio.MediaLangCode"] = returnUnsignedBufValue(buf,2);
                    td = find(mediaLanguageCode,returnUnsignedBufValue(buf,2));
                    if(td)
                    {
                        xmpData_["Xmp.audio.MediaLanguage"] = Exiv2::StringValue(td->label_);
                    }
                }
                break;

            default:
                break;
            }
        }
        io_->read(buf.pData_, size%4);
    }
    else
    {
        RevTagDetails revTagDetails[(sizeof(mediaLanguageCode)/sizeof(mediaLanguageCode[0]))];
        reverseTagDetails(mediaLanguageCode,revTagDetails,((sizeof(mediaLanguageCode)/sizeof(mediaLanguageCode[0]))));
        int32_t i;
        for (i = 0; size/4 != 0 ; size -=4, i++)
        {
            switch(i)
            {
            case MediaHeaderVersion:
                if(currentStream_ == Video)
                {
                    if(xmpData_["Xmp.video.MediaHeaderVersion"].count() > 0)
                    {
                        buf = returnBuf((int64_t)xmpData_["Xmp.video.MediaHeaderVersion"].toLong());
                        io_->write(&buf.pData_[3],1);
                        io_->seek(3,BasicIo::cur);
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else if (currentStream_ == Audio)
                {
                    if(xmpData_["Xmp.audio.MediaHeaderVersion"].count() > 0)
                    {
                        buf = returnBuf((int64_t)xmpData_["Xmp.audio.MediaHeaderVersion"].toLong());
                        io_->write(&buf.pData_[3],1);
                        io_->seek(3,BasicIo::cur);
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case MediaCreateDate:
                //A 32-bit integer that specifies (in seconds since midnight, January 1, 1904) when the movie atom was created.
                if(currentStream_ == Video)
                {
                    if(xmpData_["Xmp.video.MediaCreateDate"].count() > 0)
                    {
                        buf = returnUnsignedBuf((uint64_t)xmpData_["Xmp.video.MediaCreateDate"].toLong());
                        io_->write(buf.pData_,4);
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else if (currentStream_ == Audio)
                {
                    if(xmpData_["Xmp.audio.MediaCreateDate"].count() > 0)
                    {
                        buf = returnUnsignedBuf((uint64_t)xmpData_["Xmp.audio.MediaCreateDate"].toLong());
                        io_->write(buf.pData_,4);
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case MediaModifyDate:
                //A 32-bit integer that specifies (in seconds since midnight, January 1, 1904) when the movie atom was created.
                if(currentStream_ == Video)
                {
                    if(xmpData_["Xmp.video.MediaModifyDate"].count() > 0)
                    {
                        buf = returnUnsignedBuf((uint64_t)xmpData_["Xmp.video.MediaModifyDate"].toLong());
                        io_->write(buf.pData_,4);
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else if (currentStream_ == Audio)
                {
                    if(xmpData_["Xmp.audio.MediaModifyDate"].count() > 0)
                    {
                        buf = returnUnsignedBuf((uint64_t)xmpData_["Xmp.audio.MediaModifyDate"].toLong());
                        io_->write(buf.pData_,4);
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case MediaTimeScale:
                if(currentStream_ == Video)
                {
                    if(xmpData_["Xmp.video.MediaTimeScale"].count() > 0)
                    {
                        buf = returnBuf((int64_t)xmpData_["Xmp.video.MediaTimeScale"].toLong());
                        io_->write(buf.pData_,4);
                        io_->seek(-4,BasicIo::cur);
                    }
                }
                else if (currentStream_ == Audio)
                {
                    if(xmpData_["Xmp.audio.MediaTimeScale"].count() > 0)
                    {
                        buf = returnBuf((int64_t)xmpData_["Xmp.audio.MediaTimeScale"].toLong());
                        io_->write(buf.pData_,4);
                        io_->seek(-4,BasicIo::cur);
                    }
                }
                break;
            case MediaDuration:
                if(currentStream_ == Video)
                {
                    if(xmpData_["Xmp.video.MediaDuration"].count() > 0)
                    {
                        io_->read(buf.pData_,4);
                        time_scale = returnBufValue(buf);
                        buf = returnBuf((int64_t)xmpData_["Xmp.video.MediaDuration"].toLong()*time_scale);
                        io_->write(buf.pData_,4);
                    }
                    else
                    {
                        io_->read(buf.pData_,4);
                        time_scale = returnBufValue(buf);
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else if (currentStream_ == Audio)
                {
                    if(xmpData_["Xmp.audio.MediaDuration"].count() > 0)
                    {
                        io_->read(buf.pData_,4);
                        time_scale = returnBufValue(buf);
                        buf = returnBuf((int64_t)xmpData_["Xmp.audio.MediaDuration"].toLong()*time_scale);
                        io_->write(buf.pData_,4);
                    }
                    else
                    {
                        io_->read(buf.pData_,4);
                        time_scale = returnBufValue(buf);
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else
                {
                    io_->read(buf.pData_,4);
                    time_scale = returnBufValue(buf);
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case MediaLanguageCode:
                if(currentStream_ == Video)
                {
                    if(xmpData_["Xmp.video.MediaLanguage"].count() > 0)
                    {
                        rtd = find(revTagDetails,xmpData_["Xmp.video.MediaLanguage"].toString());
                        if(rtd)
                        {
                            buf = returnBuf(rtd->val_,2);
                            io_->write(buf.pData_,2);
                        }
                        else
                        {
                            io_->seek(2,BasicIo::cur);
                        }
                    }
                    else
                    {
                        io_->seek(2,BasicIo::cur);
                    }
                    if(xmpData_["Xmp.video.Quality"].count() >0)
                    {
                        buf = returnBuf((uint64_t)xmpData_["Xmp.video.Quality"].toLong(),2);
                        io_->write(&buf.pData_[1],1);
                        io_->write(&buf.pData_[0],1);
                    }
                    else
                    {
                        io_->seek(2,BasicIo::cur);
                    }
                }
                else if (currentStream_ == Audio)
                {
                    if(xmpData_["Xmp.audio.MediaLanguage"].count() > 0)
                    {
                        rtd = find(revTagDetails,xmpData_["Xmp.audio.MediaLanguage"].toString());
                        if(rtd)
                        {
                            buf = returnBuf(rtd->val_,2);
                            io_->write(buf.pData_,2);
                        }
                        else
                        {
                            io_->seek(2,BasicIo::cur);
                        }
                    }
                    else
                    {
                        io_->seek(2,BasicIo::cur);
                    }
                    if(xmpData_["Xmp.audio.Quality"].count() >0)
                    {
                        buf = returnBuf((uint64_t)xmpData_["Xmp.audio.Quality"].toLong(),2);
                        io_->write(&buf.pData_[1],1);
                        io_->write(&buf.pData_[0],1);
                    }
                    else
                    {
                        io_->seek(2,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            default:
                io_->seek(4,BasicIo::cur);
                break;
            }
        }
        io_->seek(size%4,BasicIo::cur);
    }
    return;
} // QuickTimeVideo::mediaHeaderDecoder

void QuickTimeVideo::trackHeaderDecoder(uint32_t size)
{
    DataBuf buf(5);
    std::memset(buf.pData_, 0x0, buf.size_);
    buf.pData_[4] = '\0';
    int64_t temp = 0;

    if(!m_modifyMetadata)
    {
        int32_t i;
        for (i = 0; size/4 != 0  ; size -=4, i++)
        {
            io_->read(buf.pData_, 4);

            switch(i)
            {
            case TrackHeaderVersion:
                if(currentStream_ == Video)
                    xmpData_["Xmp.video.TrackHeaderVersion"] = returnBufValue(buf,1);
                else if(currentStream_ == Audio)
                    xmpData_["Xmp.audio.TrackHeaderVersion"] = returnBufValue(buf,1);
                break;
            case TrackCreateDate:
                //A 32-bit integer that specifies (in seconds since midnight, January 1, 1904) when the movie atom was created.
                if(currentStream_ == Video)
                    xmpData_["Xmp.video.TrackCreateDate"] = returnUnsignedBufValue(buf);
                else if(currentStream_ == Audio)
                    xmpData_["Xmp.audio.TrackCreateDate"] = returnUnsignedBufValue(buf);
                break;
            case TrackModifyDate:
                //A 32-bit integer that specifies (in seconds since midnight, January 1, 1904) when the movie atom was created.
                if(currentStream_ == Video)
                    xmpData_["Xmp.video.TrackModifyDate"] = returnUnsignedBufValue(buf);
                else if(currentStream_ == Audio)
                    xmpData_["Xmp.audio.TrackModifyDate"] = returnUnsignedBufValue(buf);
                break;
            case TrackID:
                if(currentStream_ == Video)
                    xmpData_["Xmp.video.TrackID"] = returnBufValue(buf);
                else if(currentStream_ == Audio)
                    xmpData_["Xmp.audio.TrackID"] = returnBufValue(buf);
                break;
            case TrackDuration:
                if(currentStream_ == Video)
                    xmpData_["Xmp.video.TrackDuration"] = (int64_t)((double)returnBufValue(buf)
                                                                    *1000/(double)timeScale_);
                else if(currentStream_ == Audio)
                    xmpData_["Xmp.audio.TrackDuration"] = (int64_t)((double)returnBufValue(buf)
                                                                    *1000/(double)timeScale_);
                break;
            case TrackLayer:
                if(currentStream_ == Video)
                    xmpData_["Xmp.video.TrackLayer"] = returnBufValue(buf, 2);
                else if(currentStream_ == Audio)
                    xmpData_["Xmp.audio.TrackLayer"] = returnBufValue(buf, 2);
                break;
            case TrackVolume:
                if(currentStream_ == Video)
                    xmpData_["Xmp.video.TrackVolume"] = (returnBufValue(buf, 2)/2);//128=100%;
                else if(currentStream_ == Audio)
                    xmpData_["Xmp.audio.TrackVolume"] = (returnBufValue(buf, 2)/2);//;
                break;
            case ImageWidth:
                if(currentStream_ == Video)
                {
                    temp = (returnBufValue(buf, 4)/(256*256));
                    xmpData_["Xmp.video.Width"] = temp;
                    width_ = temp;
                }
                break;
            case ImageHeight:
                if(currentStream_ == Video)
                {
                    temp = (returnBufValue(buf, 4)/(256*256));
                    xmpData_["Xmp.video.Height"] = temp;
                    height_ = temp;
                }
                break;
            default:
                break;
            }
        }
        io_->read(buf.pData_, size%4);
    }
    else
    {
        int32_t i;
        for (i = 0; size/4 != 0  ; size -=4, i++)
        {
            switch(i)
            {
            case TrackHeaderVersion:
                if(currentStream_ == Video)
                {
                    if(xmpData_["Xmp.video.TrackHeaderVersion"].count() >0)
                    {
                        int64_t trackHeaderVersion = xmpData_["Xmp.video.TrackHeaderVersion"].toLong();
                        buf = returnBuf(trackHeaderVersion);
                        io_->write(&buf.pData_[3],1);
                        io_->seek(3,BasicIo::cur);
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else if(currentStream_ == Audio)
                {
                    if(xmpData_["Xmp.audio.TrackHeaderVersion"].count() >0)
                    {
                        int64_t trackHeaderVersion = xmpData_["Xmp.audio.TrackHeaderVersion"].toLong();
                        buf = returnBuf(trackHeaderVersion,1);
                        io_->write(buf.pData_,1);
                        io_->seek(3,BasicIo::cur);
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case TrackCreateDate:
                //A 32-bit integer that specifies (in seconds since midnight, January 1, 1904) when the movie atom was created.
                if(currentStream_ == Video)
                {
                    if(xmpData_["Xmp.video.TrackCreateDate"].count() >0)
                    {
                        int64_t trackCreateDate = xmpData_["Xmp.video.TrackCreateDate"].toLong();
                        buf = returnUnsignedBuf(trackCreateDate);
                        io_->write(buf.pData_,4);
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else if(currentStream_ == Audio)
                {
                    if(xmpData_["Xmp.audio.TrackCreateDate"].count() >0)
                    {
                        int64_t trackCreateDate = xmpData_["Xmp.audio.TrackCreateDate"].toLong();
                        buf = returnUnsignedBuf(trackCreateDate);
                        io_->write(buf.pData_,4);
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case TrackModifyDate:
                //A 32-bit integer that specifies (in seconds since midnight, January 1, 1904) when the movie atom was created.
                if(currentStream_ == Video)
                {
                    if(xmpData_["Xmp.video.TrackModifyDate"].count() >0)
                    {
                        int64_t trackModifyDate = xmpData_["Xmp.video.TrackModifyDate"].toLong();
                        buf = returnUnsignedBuf(trackModifyDate);
                        io_->write(buf.pData_,4);
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else if(currentStream_ == Audio)
                {
                    if(xmpData_["Xmp.audio.TrackModifyDate"].count() >0)
                    {
                        int64_t trackModifyDate = xmpData_["Xmp.audio.TrackModifyDate"].toLong();
                        buf = returnUnsignedBuf(trackModifyDate);
                        io_->write(buf.pData_,4);
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case TrackID:
                if(currentStream_ == Video)
                {
                    if(xmpData_["Xmp.video.TrackID"].count() >0)
                    {
                        int64_t trackID = xmpData_["Xmp.video.TrackID"].toLong();
                        buf = returnBuf(trackID);
                        io_->write(buf.pData_,4);
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else if(currentStream_ == Audio)
                {
                    if(xmpData_["Xmp.audio.TrackID"].count() >0)
                    {
                        int64_t trackID = xmpData_["Xmp.audio.TrackID"].toLong();
                        buf = returnBuf(trackID);
                        io_->write(buf.pData_,4);
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case TrackDuration:
                if(currentStream_ == Video)
                {
                    if(xmpData_["Xmp.video.TrackDuration"].count() >0)
                    {
                        int64_t trackDuration= (int64_t)((double)xmpData_["Xmp.video.TrackDuration"].
                                toLong()*(double)timeScale_/1000);
                        buf = returnBuf(trackDuration);
                        io_->write(buf.pData_,4);
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else if(currentStream_ == Audio)
                {
                    if(xmpData_["Xmp.audio.TrackDuration"].count() >0)
                    {
                        int64_t trackDuration= (int64_t)((double)xmpData_["Xmp.audio.TrackDuration"].
                                toLong()*(double)timeScale_/1000);
                        buf = returnBuf(trackDuration);
                        io_->write(buf.pData_,4);
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case TrackLayer:
                if(currentStream_ == Video)
                {
                    if(xmpData_["Xmp.video.TrackLayer"].count() >0)
                    {
                        int64_t trackLayer = xmpData_["Xmp.video.TrackLayer"].toLong();
                        buf = returnBuf(trackLayer,2);
                        io_->write(buf.pData_,2);
                        io_->seek(2,BasicIo::cur);
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else if(currentStream_ == Audio)
                {
                    if(xmpData_["Xmp.audio.TrackLayer"].count() >0)
                    {
                        int64_t trackLayer = xmpData_["Xmp.audio.TrackLayer"].toLong();
                        buf = returnBuf(trackLayer,2);
                        io_->write(buf.pData_,2);
                        io_->seek(2,BasicIo::cur);
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case TrackVolume:
                if(currentStream_ == Video)
                {
                    if(xmpData_["Xmp.video.TrackVolume"].count() >0)
                    {
                        int64_t trackVolume = (int64_t)(xmpData_["Xmp.video.TrackVolume"].toLong()*2);
                        buf = returnBuf(trackVolume,2);
                        io_->write(buf.pData_,2);
                        io_->seek(2,BasicIo::cur);
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else if(currentStream_ == Audio)
                {
                    if(xmpData_["Xmp.audio.TrackVolume"].count() >0)
                    {
                        int64_t trackVolume = (int64_t)(xmpData_["Xmp.audio.TrackVolume"].toLong()*2);
                        buf = returnBuf(trackVolume,2);
                        io_->write(buf.pData_,2);
                        io_->seek(2,BasicIo::cur);
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case ImageWidth:
                if(currentStream_ == Video)
                {
                    if(xmpData_["Xmp.video.Width"].count() >0)
                    {
                        int64_t width = xmpData_["Xmp.video.Width"].toLong()*(256*256);
                        buf = returnBuf(width);
                        io_->write(buf.pData_,4);
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case ImageHeight:
                if(currentStream_ == Video)
                {
                    if(xmpData_["Xmp.video.Height"].count() >0)
                    {
                        int64_t Height = xmpData_["Xmp.video.Height"].toLong()*(256*256);
                        buf = returnBuf(Height);
                        io_->write(buf.pData_,4);
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            default:
                io_->seek(4,BasicIo::cur);
                break;
            }
        }
        io_->read(buf.pData_, size%4);
    }
    return;
} // QuickTimeVideo::trackHeaderDecoder

void QuickTimeVideo::movieHeaderDecoder(uint32_t size)
{
    DataBuf buf(5);
    std::memset(buf.pData_, 0x0, buf.size_);
    buf.pData_[4] = '\0';

    if(!m_modifyMetadata)
    {
        int32_t i;
        for (i = 0; size/4 != 0 ; size -=4, i++)
        {
            io_->read(buf.pData_, 4);
            switch(i)
            {
            case MovieHeaderVersion:
                xmpData_["Xmp.video.MovieHeaderVersion"] = returnBufValue(buf,1); break;
            case CreateDate:
                //A 32-bit integer that specifies (in seconds since midnight, January 1, 1904) when the movie atom was created.
                xmpData_["Xmp.video.DateUTC"] = returnUnsignedBufValue(buf); break;
            case ModifyDate:
                //A 32-bit integer that specifies (in seconds since midnight, January 1, 1904) when the movie atom was created.
                xmpData_["Xmp.video.ModificationDate"] = returnUnsignedBufValue(buf); break;
            case TimeScale:
                xmpData_["Xmp.video.TimeScale"] = returnBufValue(buf);
                timeScale_ = returnBufValue(buf); break;
            case Duration:
                xmpData_["Xmp.video.Duration"] = (int64_t)((double)returnBufValue(buf)*
                                                           (double)1000/ (double)timeScale_); break;
            case PreferredRate:
                xmpData_["Xmp.video.PreferredRate"] = returnBufValue(buf, 4)/(256*256); break;
            case PreferredVolume:
                xmpData_["Xmp.video.PreferredVolume"] = (returnUnsignedBufValue(buf, 2)/2); break;
            case PreviewTime:
                xmpData_["Xmp.video.PreviewTime"] = (int64_t)((double)returnBufValue(buf)*
                                                              (double)1000/ (double)timeScale_); break;
            case PreviewDuration:
                xmpData_["Xmp.video.PreviewDuration"] = (int64_t)((double)returnBufValue(buf)
                                                                  *(double)1000/ (double)timeScale_); break;
            case PosterTime:
                xmpData_["Xmp.video.PosterTime"] = returnBufValue(buf); break;
            case SelectionTime:
                xmpData_["Xmp.video.SelectionTime"] = returnBufValue(buf); break;
            case SelectionDuration:
                xmpData_["Xmp.video.SelectionDuration"] = returnBufValue(buf)*1000/ timeScale_; break;
            case CurrentTime:
                xmpData_["Xmp.video.CurrentTime"] = returnBufValue(buf); break;
            case NextTrackID:
                xmpData_["Xmp.video.NextTrackID"] = returnBufValue(buf); break;
            default:
                break;
            }
        }
        io_->read(buf.pData_, size%4);
    }
    else
    {
        int32_t i;
        for (i = 0; size/4 != 0 ; size -=4, i++)
        {
            std::memset(buf.pData_, 0x0, buf.size_);
            switch(i)
            {
            //Fixme:There are known roundoff error while typecasting the number to int64_t
            //for example Duration (double to int64_t)
            case MovieHeaderVersion:
                if(xmpData_["Xmp.video.MovieHeaderVersion"].count() > 0)
                {
                    int64_t movieHeaderVersion  = (int64_t)xmpData_["Xmp.video.MovieHeaderVersion"].toLong();
                    buf = returnBuf(movieHeaderVersion,1);
                    io_->write(&buf.pData_[0],1);
                    io_->seek(3,BasicIo::cur);
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case CreateDate:
                if(xmpData_["Xmp.video.DateUTC"].count() > 0)
                {
                    uint64_t dateUTC  = (uint64_t)xmpData_["Xmp.video.DateUTC"].toLong();
                    buf = returnUnsignedBuf(dateUTC);
                    io_->write(buf.pData_,4);
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case ModifyDate:
                if(xmpData_["Xmp.video.ModificationDate"].count() > 0)
                {
                    uint64_t modificationDate  = (uint64_t)xmpData_["Xmp.video.ModificationDate"].toLong();
                    buf = returnUnsignedBuf(modificationDate);
                    io_->write(buf.pData_,4);
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case TimeScale:
                if(xmpData_["Xmp.video.TimeScale"].count() > 0)
                {
                    io_->read(buf.pData_,4);
                    timeScale_ = returnBufValue(buf);
                    io_->seek(-4,BasicIo::cur);
                    int64_t tmpTimeScale  = (int64_t)xmpData_["Xmp.video.TimeScale"].toLong();
                    buf = returnBuf(tmpTimeScale);
                    io_->write(buf.pData_,4);
                }
                else
                {
                    io_->read(buf.pData_,4);
                    timeScale_ = returnBufValue(buf);
                }
                break;
            case Duration:
                if(xmpData_["Xmp.video.Duration"].count() > 0)
                {
                    int64_t duration  = (int64_t)((double)xmpData_["Xmp.video.Duration"].
                            toLong()*(double)timeScale_/(double)1000);
                    buf = returnBuf(duration);
                    io_->write(buf.pData_,4);
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case PreferredRate:
                if(xmpData_["Xmp.video.PreferredRate"].count() > 0)
                {
                    buf = returnBuf((int64_t)xmpData_["Xmp.video.PreferredRate"].toLong()
                            *(256*256),4);
                    io_->write(buf.pData_,4);
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case PreferredVolume:
                if(xmpData_["Xmp.video.PreferredVolume"].count() > 0)
                {
                    uint64_t preferredVolume  = (uint64_t)(xmpData_["Xmp.video.PreferredVolume"]
                            .toLong()*2);//128 = 100%
                    buf = returnBuf(preferredVolume,2);
                    io_->write(buf.pData_,2);
                    io_->seek(2,BasicIo::cur);
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case PreviewTime:
                if(xmpData_["Xmp.video.PreviewTime"].count() > 0)
                {
                    int64_t previewTime  = (int64_t)((double)xmpData_["Xmp.video.PreviewTime"].toLong()
                            *(double)timeScale_/(double)1000);//1024=  1 second
                    buf = returnBuf(previewTime);
                    io_->write(buf.pData_,4);
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case PreviewDuration:
                if(xmpData_["Xmp.video.PreviewDuration"].count() > 0)
                {
                    int64_t previewDuration  = (int64_t)((double)xmpData_["Xmp.video.PreviewDuration"]
                            .toLong()*(double)timeScale_/(double)1000);
                    buf = returnBuf(previewDuration);
                    io_->write(buf.pData_,4);
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case PosterTime:
                if(xmpData_["Xmp.video.PosterTime"].count() > 0)
                {
                    int64_t posterTime  = (int64_t)xmpData_["Xmp.video.PosterTime"].toLong();
                    buf = returnBuf(posterTime);
                    io_->write(buf.pData_,4);
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case SelectionTime:
                if(xmpData_["Xmp.video.SelectionTime"].count() > 0)
                {
                    int64_t selectionTime  = (int64_t)xmpData_["Xmp.video.SelectionTime"].toLong();
                    buf = returnBuf(selectionTime);
                    io_->write(buf.pData_,4);
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case SelectionDuration:
                if(xmpData_["Xmp.video.SelectionDuration"].count() > 0)
                {
                    int64_t selectionDuration  = (int64_t)((double)xmpData_["Xmp.video.SelectionDuration"]
                            .toLong()*(double)timeScale_/(double)1000);
                    buf = returnBuf(selectionDuration);
                    io_->write(buf.pData_,4);
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case CurrentTime:
                if(xmpData_["Xmp.video.CurrentTime"].count() > 0)
                {
                    int64_t currentTime  = (int64_t)xmpData_["Xmp.video.CurrentTime"].toLong();
                    buf = returnBuf(currentTime);
                    io_->write(buf.pData_,4);
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            case NextTrackID:
                if(xmpData_["Xmp.video.NextTrackID"].count() > 0)
                {
                    int64_t nextTrackID  = (int64_t)xmpData_["Xmp.video.NextTrackID"].toLong();
                    buf = returnBuf(nextTrackID);
                    io_->write(buf.pData_,4);
                }
                else
                {
                    io_->seek(4,BasicIo::cur);
                }
                break;
            default:
                io_->seek(4,BasicIo::cur);
                break;
            }
        }
        io_->seek(size%4,BasicIo::cur);
    }
    return;
}// QuickTimeVideo::movieHeaderDecoder

void QuickTimeVideo::aspectRatio()
{
    //TODO - Make a better unified method to handle all cases of Aspect Ratio

    double aspectRatio = (double)width_ / (double)height_;
    aspectRatio = floor(aspectRatio*10) / 10;
    xmpData_["Xmp.video.AspectRatio"] = aspectRatio;

    int32_t aR = (int32_t) ((aspectRatio*10.0)+0.1);

    switch  (aR) {
    case 13 : xmpData_["Xmp.video.AspectRatio"] = "4:3"		; break;
    case 17 : xmpData_["Xmp.video.AspectRatio"] = "16:9"	; break;
    case 10 : xmpData_["Xmp.video.AspectRatio"] = "1:1"		; break;
    case 16 : xmpData_["Xmp.video.AspectRatio"] = "16:10"	; break;
    case 22 : xmpData_["Xmp.video.AspectRatio"] = "2.21:1"  ; break;
    case 23 : xmpData_["Xmp.video.AspectRatio"] = "2.35:1"  ; break;
    case 12 : xmpData_["Xmp.video.AspectRatio"] = "5:4"     ; break;
    default : xmpData_["Xmp.video.AspectRatio"] = aspectRatio;break;
    }
    return;
} // QuickTimeVideo::aspectRatio

void QuickTimeVideo::reverseTagDetails(const TagDetails inputTagVocabulary[],
                                       RevTagDetails  outputTagVocabulary[] ,int32_t size)
{
    int32_t i;
    for (i=0; i<size ;i++)
    {
        outputTagVocabulary[i].label_ = inputTagVocabulary[i].label_;
        outputTagVocabulary[i].val_ = inputTagVocabulary[i].val_;
    }
    return;
} // QuickTimeVideo::reverseTagDetails

Image::AutoPtr newQTimeInstance(BasicIo::AutoPtr io, bool /*create*/)
{
    Image::AutoPtr image(new QuickTimeVideo(io));
    if (!image->good())
    {
        image.reset();
    }
    return image;
}

bool isQTimeType(BasicIo& iIo, bool advance)
{
    const int32_t len = 4;
    byte buf[len];
    iIo.read(buf, len);
    iIo.read(buf, len);

    if (iIo.error() || iIo.eof())
    {
        return false;
    }

    bool matched = isQuickTimeType(buf[0], buf[1], buf[2], buf[3]);
    if (!advance || !matched)
    {
        iIo.seek(static_cast<int32_t>(0), BasicIo::beg);
    }

    return matched;
}

}                                       // namespace Exiv2

