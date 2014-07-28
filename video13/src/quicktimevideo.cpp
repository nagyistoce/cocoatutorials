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

#ifndef   _MSC_VER
#define stricmp strcasecmp
#endif
// *****************************************************************************
// class member definitions
namespace Exiv2
{
    namespace Internal
    {

    //! File type information.
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

    //! hadler Class Information.
    extern const TagVocabulary handlerClassTags[] =
    {
        {   "dhlr", "Data Handler" },
        {   "mhlr", "Media Handler" }
    };

    //! Handler Type Information.
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

    //! Vendor Id information.
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

    //! Camera Information.
    extern const TagVocabulary cameraByteOrderTags[] =
    {
        {   "II", "Little-endian (Intel, II)" },
        {   "MM", "Big-endian (Motorola, MM)" }
    };

    //! Graphics related Information.
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

    //! Media Language Codes.
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

    //! User Date related Information.
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

    //! User Data related Information.
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

    //! Nikon Tags Information.
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

    //! Nikon Color space Information.
    extern const TagDetails NikonColorSpace[] =
    {
        {       1, "sRGB" },
        {       2, "Adobe RGB" },
    };

    //! Nikon Tags.
    extern const TagVocabulary NikonGPS_Latitude_Longitude_ImgDirection_Reference[] =
    {
        {   "N", "North" },
        {   "S", "South" },
        {   "E", "East" },
        {   "W", "West" },
        {   "M", "Magnetic North" },
        {   "T", "True North" },
    };

    //! Nikon Tags
    extern const TagDetails NikonGPSAltitudeRef[] =
    {
        {   0, "Above Sea Level" },
        {   1, "Below Sea Level" },
    };

    //! Nikon Tags.
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

    //! Nikon Tags.
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

    //! picture tags.
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

    //! Filter Effects.
    extern const TagDetails FilterEffect[] =
    {
        {   0x80, "Off" },
        {   0x81, "Yellow"    },
        {   0x82, "Orange"   },
        {   0x83, "Red"   },
        {   0x84, "Green"   },
        {   0xff, "n/a"   },
    };

    //! Toning Effect.
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

    //! White balance Information.
    extern const TagDetails whiteBalance[] =
    {
        {   0, "Auto" },
        {   1, "Daylight" },
        {   2, "Shade" },
        {   3, "Fluorescent" },
        {   4, "Tungsten" },
        {   5, "Manual" },
    };

    //! To select specific bytes with in a chunk.
    enum movieHeaderTags
    {
        MovieHeaderVersion, CreateDate, ModifyDate, TimeScale, Duration, PreferredRate, PreferredVolume,
        PreviewTime = 18, PreviewDuration,PosterTime, SelectionTime, SelectionDuration, CurrentTime, NextTrackID
    };

    //! To select specific bytes with in a chunk.
    enum trackHeaderTags
    {
        TrackHeaderVersion, TrackCreateDate, TrackModifyDate, TrackID, TrackDuration = 5, TrackLayer = 8,
        TrackVolume, ImageWidth = 19, ImageHeight
    };

    //! To select specific bytes with in a chunk.
    enum mediaHeaderTags
    {
        MediaHeaderVersion, MediaCreateDate, MediaModifyDate, MediaTimeScale, MediaDuration, MediaLanguageCode
    };

    //! To select specific bytes with in a chunk.
    enum handlerTags
    {
        HandlerClass = 1, HandlerType, HandlerVendorID
    };

    //! To select specific bytes with in a chunk.
    enum videoHeaderTags
    {
        GraphicsMode = 2, OpColor
    };

    //! To select specific bytes with in a chunk.
    enum stream
    {
        Video, Audio, Hint, Null, GenMediaHeader
    };

    //! To select specific bytes with in a chunk.
    enum imageDescTags
    {
        codec, VendorID = 4, SourceImageWidth_Height = 7,  XResolution,
        YResolution, CompressorName = 10, BitDepth
    };

    //! To select specific bytes with in a chunk.
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
        bool    result = true ;
        for(int32_t i = 0; result && i < 4; ++i)
            if(tolower(buf.pData_[i]) != tolower(str[i]))
                result = false;
        return result;
    }

    /*!
     * \brief equalsQTimeTag overloaded function to handle tag Array
     *                      It will compare all the tags in a array.
     * \param buf Data buffer that will contain Tag to compare.
     * \param arr Contains multiple tags to compare from.
     * \param arraysize Number of tags in arr.
     * \return true if buf matches any one tag in arr.
     */
    bool equalsQTimeTag(Exiv2::DataBuf& buf,const char arr[][5],int32_t arraysize)
    {
        bool  result = false;
        for ( int32_t i=0; !result && i< arraysize; i++)
            result  = (bool)(stricmp((const char*)buf.pData_,arr[i+1])==0);
        return result;
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
            Exiv2::byte tmpValue[4] = {};
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
            Exiv2::byte tmpValue[4]={};
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
      @param n
      @return Returns a signed 64-bit integer
     */
    int64_t returnBufValue(Exiv2::DataBuf& buf, int32_t n = 4)
    {
        int64_t temp = 0;
        int32_t i;
        for(i = n - 1; i >= 0; i--)
#ifdef _MSC_VER
            temp = temp + static_cast<int64_t>(buf.pData_[i]*(pow(256.0, n-i-1)));
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
            buf.pData_[i] = (Exiv2::byte)(int16_t) ((int32_t)(intValue / (int32_t)pow(256.0,n-i-1)) % 256);
        }
        return buf;
    }
    /*!
      @brief Function used to convert buffer data into 64-bit
          unsigned integer, information stored in Big Endian format
      @param buf Data buffer that will contain data to be converted
      @param n
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

    /*!
 * \brief returnUnsignedBuf converts integer value to byte value
 *                          which can be written to the file.
 * \param intValue
 * \param n
 * \return
 */
    DataBuf returnUnsignedBuf(uint64_t intValue,int32_t n=4)
    {
        DataBuf buf((uint32_t)(n+1));
        buf.pData_[n] = '\0';
        for(int32_t i = n - 1; i >= 0; i--)
        {
            buf.pData_[i] = (Exiv2::byte)(uint16_t) ((int32_t)(intValue / (int32_t)pow(256.0,n-i-1)) % 256);
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

class QuickTimeVideo::Private
{
public:
    Private()
    {
        timeScale_ = 0;
        currentStream_ = 0;
        continueTraversing_ = true;
        height_ = 1;
        width_ = 1;
        m_modifyMetadata = false;
        m_decodeMetadata = true;
    }

public:
    //! Variable which stores Time Scale unit, used to calculate time.
    uint64_t timeScale_;
    //! Variable which stores current stream being processsed.
    int currentStream_;
    //! Variable to check the end of metadata traversing.
    bool continueTraversing_;
    //! Variable to store height and width of a video frame.
    uint64_t height_, width_;
    //! Variable to decide write(True)/read(False) metadata
    bool m_modifyMetadata;
    //! Variable to decide decode/skip metadata
    bool m_decodeMetadata;

    /*!
         * \brief m_QuickSkeleton
         * A container which hold the entire information about all premitive atoms inside quicktime
         * structure
         */
    std::vector<QuickAtom*> m_QuickSkeleton;
};

QuickTimeVideo::QuickTimeVideo(BasicIo::AutoPtr io)
    : Image(ImageType::qtime, mdNone, io), d(new Private)
{
    d->m_decodeMetadata = true;
    d->m_modifyMetadata = false;
} // QuickTimeVideo::QuickTimeVideo

QuickTimeVideo::~QuickTimeVideo()
{
    delete d;
}

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
    for (uint32_t i=0; i < d->m_QuickSkeleton.size();i++)
    {
        io_->seek((d->m_QuickSkeleton[i]->m_AtomLocation - 4),BasicIo::beg);
        io_->read(hdrId.pData_,4);
        if(equalsQTimeTag(hdrId,atomId))
        {
            atomsDetails.push_back(make_pair((uint32_t)io_->tell(),
                                             (uint32_t)(d->m_QuickSkeleton[i]->m_AtomSize)));
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

    d->m_decodeMetadata = false;
    d->m_modifyMetadata = true;

    std::vector<std::string> atomFlags;

    //Add new tags to the beginning of the container prmitiveFlags and handle conditions to the end of case statements.

    atomFlags.push_back("stsd");
    atomFlags.push_back("keys");
    atomFlags.push_back("tapt");
    atomFlags.push_back("pnot");
    atomFlags.push_back("udta");
    atomFlags.push_back("vmhd");
    atomFlags.push_back("hdlr");
    atomFlags.push_back("mdhd");
    atomFlags.push_back("tkhd");
    atomFlags.push_back("mvhd");
    atomFlags.push_back("ftyp");

    int32_t iMaxCount, i, j;
    iMaxCount = (uint32_t) atomFlags.size();
    std::vector< pair<uint32_t,uint32_t> > trakPositions = findAtomPositions("trak");

    for (j=0; j<iMaxCount; j++)
    {
        std::vector<pair<uint32_t,uint32_t> > atomPositions = findAtomPositions(atomFlags.back().c_str());
        atomFlags.pop_back();

        for (i=0; i<(uint32_t)atomPositions.size(); i++)
        {
            if(j == 2 || j == 3 || j == 4 || j == 10)
            {
                io_->seek(trakPositions[i].first,BasicIo::beg);
                setMediaStream();
            }
            io_->seek(atomPositions[i].first,BasicIo::beg);

            u_int32_t uiAtomPosition = (u_int32_t)atomPositions[i].second;
            switch (j)
            {
            case 0: fileTypeDecoder(uiAtomPosition); break;
            case 1: movieHeaderDecoder(uiAtomPosition); break;
            case 2: trackHeaderDecoder(uiAtomPosition); break;
            case 3: mediaHeaderDecoder(uiAtomPosition); break;
            case 4: break;
                //        Fixme:Editing dandler data is known to corrupt file data
                //        handlerDecoder(hdlrPositions[i].second);
            case 5: videoHeaderDecoder(uiAtomPosition); break;
            case 6: userDataDecoder(uiAtomPosition); break;
            case 7: previewTagDecoder(uiAtomPosition); break;
            case 8: trackApertureTagDecoder(uiAtomPosition); break;
            case 9:keysTagDecoder(uiAtomPosition); break;
            case 10: sampleDesc(uiAtomPosition); break;
            default:  break;
            }
        }
        atomPositions.clear();
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
    d->continueTraversing_ = true;
    d->height_ = d->width_ = 1;

    xmpData_["Xmp.video.FileSize"] = (double)io_->size()/(double)1048576;
    xmpData_["Xmp.video.FileName"] = io_->path();
    xmpData_["Xmp.video.MimeType"] = mimeType();

    while (d->continueTraversing_)
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
        d->continueTraversing_ = false;
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
        QuickAtom* tmpAtom = new QuickAtom();
        tmpAtom->m_AtomLocation = io_->tell();
        tmpAtom->m_AtomSize = size;
        memcpy((Exiv2::byte* )tmpAtom->m_AtomId,(const Exiv2::byte*)buf.pData_,5);
        d->m_QuickSkeleton.push_back(tmpAtom);
    }

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
        if(!d->m_modifyMetadata)
        {
            io_->read(buf.pData_, size);
            if (d->currentStream_ == Video)
                xmpData_["Xmp.video.URL"] = Exiv2::toString(buf.pData_);
            else if (d->currentStream_ == Audio)
                xmpData_["Xmp.audio.URL"] = Exiv2::toString(buf.pData_);
        }
        else
        {
            if(xmpData_["Xmp.video.URL"].count() > 0)
                writeStringData(xmpData_["Xmp.video.URL"],size);
            else if ((d->currentStream_ == Audio) && (xmpData_["Xmp.audio.URL"].count() >0))
                writeStringData(xmpData_["Xmp.audio.URL"],size);
            else
                io_->seek(size,BasicIo::cur);
        }
    }

    else if (equalsQTimeTag(buf, "urn "))
    {
        if(!d->m_modifyMetadata)
        {
            io_->read(buf.pData_, size);
            if (d->currentStream_ == Video)
                xmpData_["Xmp.video.URN"] = Exiv2::toString(buf.pData_);
            else if (d->currentStream_ == Audio)
                xmpData_["Xmp.audio.URN"] = Exiv2::toString(buf.pData_);
        }
        else
        {
            if ((d->currentStream_ == Video) && (xmpData_["Xmp.video.URN"].count() >0))
                writeStringData(xmpData_["Xmp.video.URN"],size);
            else if ((d->currentStream_ == Audio) && (xmpData_["Xmp.audio.URN"].count() >0))
                writeStringData(xmpData_["Xmp.audio.URN"],size);
            else
                io_->seek(size,BasicIo::cur);
        }
    }

    else if (equalsQTimeTag(buf, "dcom"))
    {
        if(!d->m_modifyMetadata)
        {
            io_->read(buf.pData_, size);
            xmpData_["Xmp.video.Compressor"] = Exiv2::toString(buf.pData_);
        }
        else
        {
            writeStringData(xmpData_["Xmp.video.Compressor"],size);
        }
    }

    else if (equalsQTimeTag(buf, "smhd"))
    {
        io_->seek(4,BasicIo::cur);
        if(!d->m_modifyMetadata)
        {
            io_->read(buf.pData_, 4);
            xmpData_["Xmp.audio.Balance"] = returnBufValue(buf, 2);
        }
        else
        {
            DataBuf tmpBuf = returnBuf(xmpData_["Xmp.audio.Balance"].toLong());
            io_->write(tmpBuf.pData_,2);
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
    if(!d->m_modifyMetadata)
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
        writeLongData(xmpData_["Xmp.video.PreviewDate"]);
        writeShortData(xmpData_["Xmp.video.PreviewVersion"]);

        //Variation
        if(xmpData_["Xmp.video.PreviewAtomType"].count() >0)
        {
            Exiv2::byte rawPreviewAtomType[4] = {'P','I','C','T'};
            const std::string previewAtomType = xmpData_["Xmp.video.PreviewAtomType"].toString();
            if(!(previewAtomType.compare("QuickDraw Picture") == 0))
                for(int32_t j=0; j<4; j++)
                    rawPreviewAtomType[j] = previewAtomType[j];

            writeMultibyte(rawPreviewAtomType, 4);
        }
        io_->seek(cur_pos + size, BasicIo::beg);
    }
    return;
} // QuickTimeVideo::previewTagDecoder

void QuickTimeVideo::keysTagDecoder(uint32_t size)
{
    DataBuf buf(4);
    uint64_t cur_pos = io_->tell();

    if(!d->m_modifyMetadata)
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
        writeLongData(xmpData_["Xmp.video.PreviewDate"]);
        writeShortData(xmpData_["Xmp.video.PreviewVersion"]);

        //Variation
        if(xmpData_["Xmp.video.PreviewAtomType"].count() >0)
        {
            Exiv2::byte rawPreviewAtomType[4] = {'P','I','C','T'};
            const std::string previewAtomType = xmpData_["Xmp.video.PreviewAtomType"].toString();
            if(!(previewAtomType.compare("QuickDraw Picture") == 0))
                for(int32_t j=0; j<4; j++)
                    rawPreviewAtomType[j] = previewAtomType[j];

            writeMultibyte(rawPreviewAtomType, 4);
        }
        io_->seek(cur_pos + size, BasicIo::beg);
    }
    return;
} // QuickTimeVideo::keysTagDecoder

void QuickTimeVideo::trackApertureTagDecoder(uint32_t size)
{
    DataBuf buf(4), buf2(2);
    uint64_t cur_pos = io_->tell();
    if(!d->m_modifyMetadata)
    {
        Exiv2::byte n = 3;
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
        Exiv2::byte n = 3;
        while(n--)
        {
            io_->seek(static_cast<int32_t>(4), BasicIo::cur); io_->read(buf.pData_, 4);

            if(equalsQTimeTag(buf, "clef"))
            {
                io_->seek(static_cast<int32_t>(4), BasicIo::cur);
                writeApertureData(xmpData_["Xmp.video.CleanApertureWidth"],4);
                writeApertureData(xmpData_["Xmp.video.CleanApertureHeight"],4);
            }
            else if(equalsQTimeTag(buf, "prof"))
            {
                io_->seek(static_cast<int32_t>(4), BasicIo::cur);
                writeApertureData(xmpData_["Xmp.video.ProductionApertureWidth"],4);
                writeApertureData(xmpData_["Xmp.video.ProductionApertureHeight"],4);
            }
            else if(equalsQTimeTag(buf, "enof"))
            {
                io_->seek(static_cast<int32_t>(4), BasicIo::cur);
                writeApertureData(xmpData_["Xmp.video.EncodedPixelsWidth"],4);
                writeApertureData(xmpData_["Xmp.video.EncodedPixelsHeight"],4);
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
        if(!d->m_modifyMetadata)
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
            RevTagDetails revTagDetails[(sizeof(whiteBalance)/sizeof(whiteBalance[0]))];
            reverseTagDetails(whiteBalance,revTagDetails,((sizeof(whiteBalance)/sizeof(whiteBalance[0]))));

            writeShortData(xmpData_["Xmp.video.Make"],24);
            writeShortData(xmpData_["Xmp.video.Model"],24);

            //Variation
            if(xmpData_["Xmp.video.ExposureTime"].count() >0)
            {
                const std::vector<int32_t> d_exposureTime =
                        getLongFromString(xmpData_["Xmp.video.ExposureTime"].toString(),'/');
                const int32_t tmpValue = (d_exposureTime.at(1)*10);
                io_->write((Exiv2::byte*)&tmpValue,4);
            }
            else
                io_->seek(4,BasicIo::cur);

            //Variation
            if(xmpData_["Xmp.video.FNumber"].count() >0)
            {
                io_->seek(4,BasicIo::cur);
                io_->read(buf2.pData_,4);
                io_->seek(-8,BasicIo::cur);
                const int32_t fNumber =(int32_t)
                        ((double)xmpData_["Xmp.video.FNumber"].toLong()
                        *(double)getULong(buf2.pData_, littleEndian));
                writeMultibyte((Exiv2::byte*)&fNumber, 4, 4);
            }
            else
                io_->seek(8,BasicIo::cur);

            if(xmpData_["Xmp.video.ExposureCompensation"].count() >0)
            {
                io_->seek(4,BasicIo::cur);
                io_->read(buf2.pData_,4);
                io_->seek(-8,BasicIo::cur);
                const int32_t exposureCompensation =(int32_t)
                        ((double)xmpData_["Xmp.video.ExposureCompensation"].toLong()
                        *(double)getULong(buf2.pData_, littleEndian));
                writeMultibyte((Exiv2::byte*)&exposureCompensation, 4, 14);
            }
            else
                io_->seek(18,BasicIo::cur);

            if(xmpData_["Xmp.video.WhiteBalance"].count() >0)
            {
                rtd = find(revTagDetails,xmpData_["Xmp.video.WhiteBalance"].toString());
                if(rtd)
                {
                    const int32_t sWhiteBalance = (int32_t)rtd->val_;
                    writeMultibyte((Exiv2::byte*)&sWhiteBalance,4);
                }
                else
                    io_->seek(4,BasicIo::cur);
            }
            else
                io_->seek(4,BasicIo::cur);

            if(xmpData_["Xmp.video.FocalLength"].count() >0)
            {
                io_->seek(4,BasicIo::cur);
                io_->read(buf2.pData_,4);
                io_->seek(-8,BasicIo::cur);
                const int32_t focalLength =(int32_t)((double)xmpData_["Xmp.video.FocalLength"].toLong()
                        *(double)getULong(buf2.pData_, littleEndian));
                writeMultibyte((Exiv2::byte*)&focalLength, 4, 99);
            }
            else
                io_->seek(103,BasicIo::cur);

            writeStringData(xmpData_["Xmp.video.Software"],48);
            writeLongData(xmpData_["Xmp.video.ISO"]);
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
    uint32_t             size = 0;
    uint32_t             size_internal = size_external;

    std::memset(buf.pData_, 0x0, buf.size_);

    if(!d->m_modifyMetadata)
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

                writeShortData(xmpData_[exvGettext(tv->label_)],(size-8));

            else if(equalsQTimeTag(buf, "CMbo") || equalsQTimeTag(buf, "Cmbo"))
                io_->seek(2,BasicIo::cur);

            else if(tv)
                writeShortData(xmpData_[exvGettext(tv->label_)],(size-8));
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

    if(!d->m_modifyMetadata)
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

                io_->read(buf.pData_, 2);
                xmpData_["Xmp.video.TimeZone"] = Exiv2::getShort(buf.pData_, bigEndian);
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
        bool bDataWriten = false;
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
                writeShortData(xmpData_["Xmp.video.PictureControlVersion"],4);
                writeShortData(xmpData_["Xmp.video.PictureControlName"],20);
                writeShortData(xmpData_["Xmp.video.PictureControlBase"],20);
                io_->seek(4,BasicIo::cur);
                std::memset(buf.pData_, 0x0, buf.size_);

                if(xmpData_["Xmp.video.PictureControlAdjust"].count() >0)
                {
                    RevTagDetails revTagDetails[(sizeof(PictureControlAdjust)/sizeof(PictureControlAdjust[0]))];
                    reverseTagDetails(PictureControlAdjust,revTagDetails,((sizeof(PictureControlAdjust)/sizeof(PictureControlAdjust[0]))));

                    rtd2 = find(revTagDetails,xmpData_["Xmp.video.PictureControlAdjust"].toString());
                    if(rtd2)
                    {
                        Exiv2::byte rawPictureControlAdjust = (Exiv2::byte)rtd2->val_;
                        bDataWriten = writeMultibyte(&rawPictureControlAdjust, 1);
                    }
                }
                if(!bDataWriten)
                    io_->seek(1,BasicIo::cur);
                bDataWriten = false;

                if(xmpData_["Xmp.video.PictureControlQuickAdjust"].count() >0)
                {
                    RevTagDetails revTagDetails[(sizeof(NormalSoftHard)/sizeof(NormalSoftHard[0]))];
                    reverseTagDetails(NormalSoftHard,revTagDetails,((sizeof(NormalSoftHard)/sizeof(NormalSoftHard[0]))));

                    rtd2 = find(revTagDetails, xmpData_["Xmp.video.PictureControlQuickAdjust"].toString() );
                    if(rtd2)
                    {
                        Exiv2::byte rawPictureControlQuickAdjust = (Exiv2::byte)rtd2->val_;
                        bDataWriten = writeMultibyte(&rawPictureControlQuickAdjust,1);
                    }
                }
                if(!bDataWriten)
                    io_->seek(1,BasicIo::cur);
                bDataWriten = false;

                if(xmpData_["Xmp.video.Sharpness"].count() >0)
                {
                    RevTagDetails revTagDetails[(sizeof(NormalSoftHard)/sizeof(NormalSoftHard[0]))];
                    reverseTagDetails(NormalSoftHard,revTagDetails,((sizeof(NormalSoftHard)/sizeof(NormalSoftHard[0]))));

                    rtd2 = find(revTagDetails, xmpData_["Xmp.video.Sharpness"].toString() );
                    if(rtd2)
                    {
                        Exiv2::byte rawSharpness = (Exiv2::byte)rtd2->val_;
                        bDataWriten = writeMultibyte(&rawSharpness,1);
                    }
                }
                if(!bDataWriten)
                    io_->seek(1,BasicIo::cur);
                bDataWriten = false;

                if(xmpData_["Xmp.video.Contrast"].count() >0)
                {
                    RevTagDetails revTagDetails[(sizeof(NormalSoftHard)/sizeof(NormalSoftHard[0]))];
                    reverseTagDetails(NormalSoftHard,revTagDetails,((sizeof(NormalSoftHard)/sizeof(NormalSoftHard[0]))));

                    rtd2 = find(revTagDetails, xmpData_["Xmp.video.Contrast"].toString() );
                    if(rtd2)
                    {
                        Exiv2::byte rawContrast = (Exiv2::byte)rtd2->val_;
                        bDataWriten = writeMultibyte(&rawContrast,1);
                    }
                }
                if(!bDataWriten)
                    io_->seek(1,BasicIo::cur);
                bDataWriten = false;

                if(xmpData_["Xmp.video.Brightness"].count() >0)
                {
                    RevTagDetails revTagDetails[(sizeof(NormalSoftHard)/sizeof(NormalSoftHard[0]))];
                    reverseTagDetails(NormalSoftHard,revTagDetails,((sizeof(NormalSoftHard)/sizeof(NormalSoftHard[0]))));

                    rtd2 = find(revTagDetails, xmpData_["Xmp.video.Brightness"].toString() );
                    if(rtd2)
                    {
                        Exiv2::byte rawBrightness = (Exiv2::byte)rtd2->val_;
                        bDataWriten = writeMultibyte(&rawBrightness,1);
                    }
                }
                if(!bDataWriten)
                    io_->seek(1,BasicIo::cur);
                bDataWriten = false;

                if(xmpData_["Xmp.video.Saturation"].count() >0)
                {
                    RevTagDetails revTagDetails[(sizeof(Saturation)/sizeof(Saturation[0]))];
                    reverseTagDetails(Saturation,revTagDetails,((sizeof(Saturation)/sizeof(Saturation[0]))));

                    rtd2 = find(revTagDetails, xmpData_["Xmp.video.Saturation"].toString() );
                    if(rtd2)
                    {
                        Exiv2::byte rawSaturation = (Exiv2::byte)rtd2->val_;
                        bDataWriten = writeMultibyte(&rawSaturation,1);
                    }
                }
                if(!bDataWriten)
                    io_->seek(1,BasicIo::cur);
                bDataWriten = false;

                if(xmpData_["Xmp.video.HueAdjustment"].count() >0)
                {
                    Exiv2::byte rawHueAdjustment = (Exiv2::byte)xmpData_["Xmp.video.HueAdjustment"].toString()[0];
                    bDataWriten = writeMultibyte(&rawHueAdjustment,1);
                }
                if(!bDataWriten)
                    io_->seek(1,BasicIo::cur);
                bDataWriten = false;

                if(xmpData_["Xmp.video.FilterEffect"].count() >0)
                {
                    RevTagDetails revTagDetails[(sizeof(FilterEffect)/sizeof(FilterEffect[0]))];
                    reverseTagDetails(FilterEffect,revTagDetails,((sizeof(FilterEffect)/sizeof(FilterEffect[0]))));

                    rtd2 = find(revTagDetails, xmpData_["Xmp.video.FilterEffect"].toString() );
                    if(rtd2)
                    {
                        Exiv2::byte rawFilterEffect = (Exiv2::byte)rtd2->val_;
                        bDataWriten = writeMultibyte(&rawFilterEffect,1);
                    }
                }
                if(!bDataWriten)
                    io_->seek(1,BasicIo::cur);
                bDataWriten = false;

                if(xmpData_["Xmp.video.ToningEffect"].count() >0)
                {
                    RevTagDetails revTagDetails[(sizeof(ToningEffect)/sizeof(ToningEffect[0]))];
                    reverseTagDetails(ToningEffect,revTagDetails,((sizeof(ToningEffect)/sizeof(ToningEffect[0]))));

                    rtd2 = find(revTagDetails, xmpData_["Xmp.video.ToningEffect"].toString() );
                    if(rtd2)
                    {
                        Exiv2::byte rawToningEffect = (Exiv2::byte)rtd2->val_;
                        bDataWriten = writeMultibyte(&rawToningEffect,1);
                    }
                }
                if(!bDataWriten)
                    io_->seek(1,BasicIo::cur);
                bDataWriten = false;

                if(xmpData_["Xmp.video.ToningSaturation"].count() >0)
                {
                    Exiv2::byte rawToningSaturation = (Exiv2::byte)xmpData_["Xmp.video.ToningSaturation"].toString()[0];
                    bDataWriten = writeMultibyte(&rawToningSaturation,1);
                }
                io_->seek(local_pos + dataLength, BasicIo::beg);
            }

            else if(TagID == 0x2000024)
            {
                uint64_t local_pos = io_->tell();
                dataLength = Exiv2::getUShort(buf.pData_, bigEndian);
                std::memset(buf.pData_, 0x0, buf.size_);

                writeShortData(xmpData_["Xmp.video.TimeZone"]);

                if(xmpData_["Xmp.video.DayLightSavings"].count() >0)
                {
                    RevTagDetails revTagDetails[(sizeof(YesNo)/sizeof(YesNo[0]))];
                    reverseTagDetails(YesNo,revTagDetails,((sizeof(YesNo)/sizeof(YesNo[0]))));

                    rtd2 = find(revTagDetails, xmpData_["Xmp.video.DayLightSavings"].toString() );
                    if(rtd2)
                    {
                        Exiv2::byte rawDayLightSavings = (Exiv2::byte)rtd2->val_;
                        bDataWriten = writeMultibyte(&rawDayLightSavings,1);
                    }
                }
                if(!bDataWriten)
                    io_->seek(1,BasicIo::cur);
                bDataWriten = false;

                if(xmpData_["Xmp.video.DateDisplayFormat"].count() >0)
                {
                    RevTagDetails revTagDetails[(sizeof(DateDisplayFormat)/sizeof(DateDisplayFormat[0]))];
                    reverseTagDetails(DateDisplayFormat,revTagDetails,((sizeof(DateDisplayFormat)/sizeof(DateDisplayFormat[0]))));

                    rtd2 = find(revTagDetails, xmpData_["Xmp.video.DateDisplayFormat"].toString() );
                    if(rtd2)
                    {
                        Exiv2::byte rawDateDisplayFormat = (Exiv2::byte)rtd2->val_;
                        bDataWriten = writeMultibyte(&rawDateDisplayFormat,1);
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
                        writeStringData(xmpData_[exvGettext(td->label_)],dataLength);
                    else
                        io_->seek(dataLength,BasicIo::cur);
                }
            }
            else if(dataType == 4) // what is 4?  would it be better to have an enum with a name for each dataType?
            {
                dataLength = Exiv2::getUShort(buf.pData_, bigEndian) * 4;
                if(td && (xmpData_[exvGettext(td->label_)].count() >0) && (dataLength < 200))
                {
                    writeLongData(xmpData_[exvGettext(td->label_)],4,dataLength-4);
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
                    writeShortData(xmpData_[exvGettext(td->label_)],2,dataLength-2);
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
                    io_->write((Exiv2::byte*)&tagData,4);
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
                    io_->write((Exiv2::byte*)&dataEight[0],2);
                    io_->write((Exiv2::byte*)&dataEight[1],2);
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
                d->currentStream_ = Video;
            else if(equalsQTimeTag(buf, "soun"))
                d->currentStream_ = Audio;
            else if (equalsQTimeTag(buf, "hint"))
                d->currentStream_ = Hint;
            else
                d->currentStream_ = GenMediaHeader;
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
    if(!d->m_modifyMetadata)
    {
        if (d->currentStream_ == Video)
            xmpData_["Xmp.video.FrameRate"] = (double)totalframes * (double)d->timeScale_ / (double)timeOfFrames;
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
        if (d->currentStream_ == Video)
            imageDescDecoder();
        else if (d->currentStream_ == Audio)
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

    if(!d->m_modifyMetadata)
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
            bool bDataWriten = false;
            switch(i)
            {
            case AudioFormat:
                if(xmpData_["Xmp.audio.Compressor"].count() > 0)
                {
                    Exiv2::byte rawCompressor[4];
                    td = find(revTagVocabulary,xmpData_["Xmp.audio.Compressor"].toString());
                    if(td)
                    {
                        const char* compressor = td->label_;
                        for(int32_t j=0; j<4; j++)
                            rawCompressor[j] = compressor[j];
                        bDataWriten = writeMultibyte(rawCompressor, 4);
                    }
                }
                break;

            case AudioVendorID:
                if(xmpData_["Xmp.audio.VendorID"].count() >0)
                {
                    Exiv2::byte rawVendorID[4];
                    td = find(revVendorIDTags,xmpData_["Xmp.audio.VendorID"].toString());
                    if(td)
                    {
                        const char* vendorID = td->label_;
                        for(int32_t j=0; j<4; j++)
                            rawVendorID[j] = vendorID[j];
                        bDataWriten = writeMultibyte(rawVendorID, 4);
                    }
                }
                break;

            case AudioChannels:
                if(xmpData_["Xmp.audio.ChannelType"].count() >0)
                {
                    DataBuf rawBuf(2);
                    rawBuf = returnBuf((int64_t)xmpData_["Xmp.audio.ChannelType"].toLong(),2);
                    bDataWriten = writeMultibyte(rawBuf.pData_, 2);
                }
                if(!bDataWriten)
                    io_->seek(2, BasicIo::cur);
                bDataWriten = false;

                if(xmpData_["Xmp.audio.BitsPerSample"].count() >0)
                {
                    DataBuf rawBuf(2);
                    rawBuf = returnBuf((int64_t)xmpData_["Xmp.audio.BitsPerSample"].toLong(),2);
                    bDataWriten = writeMultibyte(rawBuf.pData_,2);
                }
                if(!bDataWriten)
                    io_->seek(2, BasicIo::cur);
                bDataWriten = true;
                break;

            case AudioSampleRate:
                if(xmpData_["Xmp.audio.SampleRate"].count() >0)
                {
                    DataBuf rawSampleRate(5);
                    rawSampleRate.pData_[4] = '\0';
                    const int64_t sampleRate = (int64_t)xmpData_["Xmp.audio.SampleRate"].toLong();
                    rawSampleRate = returnBuf(sampleRate,4);

                    bDataWriten = writeMultibyte(rawSampleRate.pData_+2,2);
                    bDataWriten = writeMultibyte(rawSampleRate.pData_,2);
                }
                break;

            default:
                io_->seek(4,BasicIo::cur);
                bDataWriten = true;
                break;
            }
            if(!bDataWriten)
                io_->seek(4, BasicIo::cur);
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

    if(!d->m_modifyMetadata)
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
            bool bDataWritten = false;
            switch(i)
            {
            case codec:
                if(xmpData_["Xmp.video.Codec"].count() >0)
                {
                    std::string codecName = xmpData_["Xmp.video.Codec"].toString();
                    td = find(revTagVocabulary, codecName);
                    Exiv2::byte rawCodec[4];
                    if(td)
                    {
                        const char* codecNameTag = td->label_;
                        for(int32_t j=0; j<4 ;j++)
                            rawCodec[j] = (Exiv2::byte)codecNameTag[j];
                    }
                    else
                        for(int32_t j=0; j<4 ;j++)
                            rawCodec[j] = (Exiv2::byte)codecName[j];
                    bDataWritten = writeMultibyte(rawCodec, 4);
                }
                break;

            case VendorID:
                if(xmpData_["Xmp.video.VendorID"].count() >0)
                {
                    td = find(revVendorIDTags, xmpData_["Xmp.video.VendorID"].toString());
                    if(td)
                    {
                        Exiv2::byte rawVendorID[4];
                        const char* vendorID = td->label_;
                        for(int32_t j=0; j<4 ;j++)
                            rawVendorID[j] = vendorID[j];
                        bDataWritten = writeMultibyte(rawVendorID,4);
                    }
                }
                break;

            case SourceImageWidth_Height:
                if(xmpData_["Xmp.video.SourceImageWidth"].count() >0)
                {
                    DataBuf imageWidth = returnBuf((int64_t)xmpData_["Xmp.video.SourceImageWidth"].toLong(),2);
                    bDataWritten = writeMultibyte(imageWidth.pData_,2);
                }
                if(!bDataWritten)
                    io_->seek(2,BasicIo::cur);
                bDataWritten = false;

                if(xmpData_["Xmp.video.SourceImageHeight"].count() >0)
                {
                    DataBuf imageHeight = returnBuf(((int64_t)(xmpData_["Xmp.video.SourceImageHeight"].toLong() -
                                                     xmpData_["Xmp.video.SourceImageHeight"].toLong()/65536)*65536));
                    bDataWritten = writeMultibyte(imageHeight.pData_,2);
                }
                if(!bDataWritten)
                    io_->seek(2,BasicIo::cur);
                bDataWritten = true;
                break;

            case XResolution:
                //This field should not be edited
                break;

            case YResolution:
                io_->seek(7,BasicIo::cur);
                size -= 3;
                bDataWritten = true;
                break;

            case CompressorName:
                if(xmpData_["Xmp.video.Compressor"].count() >0)
                {
                    Exiv2::byte rawCompressor[32] = {};
                    const std::string compressor = xmpData_["Xmp.video.Compressor"].toString();
                    for(int32_t j=0;j<min((int32_t)32,(int32_t)compressor.size()); j++)
                        rawCompressor[j] = (Exiv2::byte)compressor[j];
                    io_->seek(4,BasicIo::cur);
                    bDataWritten = writeMultibyte(rawCompressor,32);
                }
                else
                    io_->seek(36,BasicIo::cur);
                size -= 32;
                bDataWritten = true;
                break;

            default:
                break;
            }
            if(!bDataWritten)
                io_->seek(4,BasicIo::cur);
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
    d->currentStream_ = Video;

    const RevTagDetails* rtd;
    const TagDetails* td;

    if(!d->m_modifyMetadata)
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

    if(!d->m_modifyMetadata)
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
                    if (d->currentStream_ == Video)
                        xmpData_["Xmp.video.HandlerClass"] = exvGettext(tv->label_);
                    else if (d->currentStream_ == Audio)
                        xmpData_["Xmp.audio.HandlerClass"] = exvGettext(tv->label_);
                }
                break;
            case HandlerType:
                tv = find(handlerTypeTags, Exiv2::toString( buf.pData_));
                if(tv)
                {
                    if (d->currentStream_ == Video)
                        xmpData_["Xmp.video.HandlerType"] = exvGettext(tv->label_);
                    else if (d->currentStream_ == Audio)
                        xmpData_["Xmp.audio.HandlerType"] = exvGettext(tv->label_);
                }
                break;
            case HandlerVendorID:
                tv = find(vendorIDTags, Exiv2::toString( buf.pData_));
                if(tv)
                {
                    if (d->currentStream_ == Video)
                        xmpData_["Xmp.video.HandlerVendorID"] = exvGettext(tv->label_);
                    else if (d->currentStream_ == Audio)
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
                if (d->currentStream_ == Video)
                {
                    if(xmpData_["Xmp.video.HandlerClass"].count() >0)
                    {
                        Exiv2::byte rawHandlerClass[4];
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
                else if (d->currentStream_ == Audio)
                {
                    if(xmpData_["Xmp.audio.HandlerClass"].count() >0)
                    {
                        Exiv2::byte rawHandlerClass[4];
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
                if (d->currentStream_ == Video)
                {
                    if(xmpData_["Xmp.video.HandlerType"].count() >0)
                    {
                        Exiv2::byte rawHandlerType[4];
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
                else if (d->currentStream_ == Audio)
                {
                    if(xmpData_["Xmp.audio.HandlerType"].count() >0)
                    {
                        Exiv2::byte rawHandlerType[4];
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
                if (d->currentStream_ == Video)
                {
                    if(xmpData_["Xmp.video.HandlerVendorID"].count() >0)
                    {
                        Exiv2::byte rawHandlerVendorID[4];
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
                else if (d->currentStream_ == Audio)
                {
                    if(xmpData_["Xmp.audio.HandlerVendorID"].count() >0)
                    {
                        Exiv2::byte rawHandlerVendorID[4];
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
    DataBuf     buf(5);
    buf.pData_[4] = '\0';
    std::memset(buf.pData_, 0x0, buf.size_);
    Exiv2::Value::AutoPtr v = Exiv2::Value::create(Exiv2::xmpSeq);
    const TagVocabulary* td;

    if(!d->m_modifyMetadata)
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
                    Exiv2::byte rawMajorBrand[4];
                    td = find(revTagVocabulary, xmpData_["Xmp.video.MajorBrand"].toString());
                    const char* majorBrandVoc = td->label_;
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
                    Exiv2::byte rawCompatibleBrand[4];
                    td = find(revTagVocabulary, xmpData_["Xmp.video.CompatibleBrands"].toString());
                    if(td)
                    {
                        const char* compatibleBrandVoc = td->label_;
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
    const TagDetails* td;
    const RevTagDetails* rtd;

    if(!d->m_modifyMetadata)
    {
        int32_t i;
        for (i = 0; size/4 != 0 ; size -=4, i++)
        {
            io_->read(buf.pData_, 4);

            switch(i)
            {
            case MediaHeaderVersion:
                if(d->currentStream_ == Video)
                    xmpData_["Xmp.video.MediaHeaderVersion"] = returnBufValue(buf,1);
                else if (d->currentStream_ == Audio)
                    xmpData_["Xmp.audio.MediaHeaderVersion"] = returnBufValue(buf,1);
                break;
            case MediaCreateDate:
                //A 32-bit integer that specifies (in seconds since midnight, January 1, 1904) when the movie atom was created.
                if(d->currentStream_ == Video)
                    xmpData_["Xmp.video.MediaCreateDate"] = returnUnsignedBufValue(buf);
                else if (d->currentStream_ == Audio)
                    xmpData_["Xmp.audio.MediaCreateDate"] = returnUnsignedBufValue(buf);
                break;
            case MediaModifyDate:
                //A 32-bit integer that specifies (in seconds since midnight, January 1, 1904) when the movie atom was created.
                if(d->currentStream_ == Video)
                    xmpData_["Xmp.video.MediaModifyDate"] = returnUnsignedBufValue(buf);
                else if (d->currentStream_ == Audio)
                    xmpData_["Xmp.audio.MediaModifyDate"] = returnUnsignedBufValue(buf);
                break;
            case MediaTimeScale:
                if(d->currentStream_ == Video)
                    xmpData_["Xmp.video.MediaTimeScale"] = returnBufValue(buf);
                else if (d->currentStream_ == Audio)
                    xmpData_["Xmp.audio.MediaTimeScale"] = returnBufValue(buf);
                time_scale = returnBufValue(buf);
                break;
            case MediaDuration:
                if(d->currentStream_ == Video)
                    xmpData_["Xmp.video.MediaDuration"] = returnBufValue(buf)/time_scale;
                else if (d->currentStream_ == Audio)
                    xmpData_["Xmp.audio.MediaDuration"] = returnBufValue(buf)/time_scale;
                break;
            case MediaLanguageCode:
                if(d->currentStream_ == Video)
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
                else if (d->currentStream_ == Audio)
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
                if(d->currentStream_ == Video)
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
                else if (d->currentStream_ == Audio)
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
                if(d->currentStream_ == Video)
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
                else if (d->currentStream_ == Audio)
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
                if(d->currentStream_ == Video)
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
                else if (d->currentStream_ == Audio)
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
                if(d->currentStream_ == Video)
                {
                    if(xmpData_["Xmp.video.MediaTimeScale"].count() > 0)
                    {
                        buf = returnBuf((int64_t)xmpData_["Xmp.video.MediaTimeScale"].toLong());
                        io_->write(buf.pData_,4);
                        io_->seek(-4,BasicIo::cur);
                    }
                }
                else if (d->currentStream_ == Audio)
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
                if(d->currentStream_ == Video)
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
                else if (d->currentStream_ == Audio)
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
                if(d->currentStream_ == Video)
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
                else if (d->currentStream_ == Audio)
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

    if(!d->m_modifyMetadata)
    {
        int32_t i;
        for (i = 0; size/4 != 0  ; size -=4, i++)
        {
            io_->read(buf.pData_, 4);

            switch(i)
            {
            case TrackHeaderVersion:
                if(d->currentStream_ == Video)
                    xmpData_["Xmp.video.TrackHeaderVersion"] = returnBufValue(buf,1);
                else if(d->currentStream_ == Audio)
                    xmpData_["Xmp.audio.TrackHeaderVersion"] = returnBufValue(buf,1);
                break;
            case TrackCreateDate:
                //A 32-bit integer that specifies (in seconds since midnight, January 1, 1904) when the movie atom was created.
                if(d->currentStream_ == Video)
                    xmpData_["Xmp.video.TrackCreateDate"] = returnUnsignedBufValue(buf);
                else if(d->currentStream_ == Audio)
                    xmpData_["Xmp.audio.TrackCreateDate"] = returnUnsignedBufValue(buf);
                break;
            case TrackModifyDate:
                //A 32-bit integer that specifies (in seconds since midnight, January 1, 1904) when the movie atom was created.
                if(d->currentStream_ == Video)
                    xmpData_["Xmp.video.TrackModifyDate"] = returnUnsignedBufValue(buf);
                else if(d->currentStream_ == Audio)
                    xmpData_["Xmp.audio.TrackModifyDate"] = returnUnsignedBufValue(buf);
                break;
            case TrackID:
                if(d->currentStream_ == Video)
                    xmpData_["Xmp.video.TrackID"] = returnBufValue(buf);
                else if(d->currentStream_ == Audio)
                    xmpData_["Xmp.audio.TrackID"] = returnBufValue(buf);
                break;
            case TrackDuration:
                if(d->currentStream_ == Video)
                    xmpData_["Xmp.video.TrackDuration"] = (int64_t)((double)returnBufValue(buf)
                                                                    *1000/(double)d->timeScale_);
                else if(d->currentStream_ == Audio)
                    xmpData_["Xmp.audio.TrackDuration"] = (int64_t)((double)returnBufValue(buf)
                                                                    *1000/(double)d->timeScale_);
                break;
            case TrackLayer:
                if(d->currentStream_ == Video)
                    xmpData_["Xmp.video.TrackLayer"] = returnBufValue(buf, 2);
                else if(d->currentStream_ == Audio)
                    xmpData_["Xmp.audio.TrackLayer"] = returnBufValue(buf, 2);
                break;
            case TrackVolume:
                if(d->currentStream_ == Video)
                    xmpData_["Xmp.video.TrackVolume"] = (returnBufValue(buf, 2)/2);//128=100%;
                else if(d->currentStream_ == Audio)
                    xmpData_["Xmp.audio.TrackVolume"] = (returnBufValue(buf, 2)/2);//;
                break;
            case ImageWidth:
                if(d->currentStream_ == Video)
                {
                    temp = (returnBufValue(buf, 4)/(256*256));
                    xmpData_["Xmp.video.Width"] = temp;
                    d->width_ = temp;
                }
                break;
            case ImageHeight:
                if(d->currentStream_ == Video)
                {
                    temp = (returnBufValue(buf, 4)/(256*256));
                    xmpData_["Xmp.video.Height"] = temp;
                    d->height_ = temp;
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
                if(d->currentStream_ == Video)
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
                else if(d->currentStream_ == Audio)
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
                if(d->currentStream_ == Video)
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
                else if(d->currentStream_ == Audio)
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
                if(d->currentStream_ == Video)
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
                else if(d->currentStream_ == Audio)
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
                if(d->currentStream_ == Video)
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
                else if(d->currentStream_ == Audio)
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
                if(d->currentStream_ == Video)
                {
                    if(xmpData_["Xmp.video.TrackDuration"].count() >0)
                    {
                        int64_t trackDuration= (int64_t)((double)xmpData_["Xmp.video.TrackDuration"].
                                toLong()*(double)d->timeScale_/1000);
                        buf = returnBuf(trackDuration);
                        io_->write(buf.pData_,4);
                    }
                    else
                    {
                        io_->seek(4,BasicIo::cur);
                    }
                }
                else if(d->currentStream_ == Audio)
                {
                    if(xmpData_["Xmp.audio.TrackDuration"].count() >0)
                    {
                        int64_t trackDuration= (int64_t)((double)xmpData_["Xmp.audio.TrackDuration"].
                                toLong()*(double)d->timeScale_/1000);
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
                if(d->currentStream_ == Video)
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
                else if(d->currentStream_ == Audio)
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
                if(d->currentStream_ == Video)
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
                else if(d->currentStream_ == Audio)
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
                if(d->currentStream_ == Video)
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
                if(d->currentStream_ == Video)
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

    if(!d->m_modifyMetadata)
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
                d->timeScale_ = returnBufValue(buf); break;
            case Duration:
                xmpData_["Xmp.video.Duration"] = (int64_t)((double)returnBufValue(buf)*
                                                           (double)1000/ (double)d->timeScale_); break;
            case PreferredRate:
                xmpData_["Xmp.video.PreferredRate"] = returnBufValue(buf, 4)/(256*256); break;
            case PreferredVolume:
                xmpData_["Xmp.video.PreferredVolume"] = (returnUnsignedBufValue(buf, 2)/2); break;
            case PreviewTime:
                xmpData_["Xmp.video.PreviewTime"] = (int64_t)((double)returnBufValue(buf)*
                                                              (double)1000/ (double)d->timeScale_); break;
            case PreviewDuration:
                xmpData_["Xmp.video.PreviewDuration"] = (int64_t)((double)returnBufValue(buf)
                                                                  *(double)1000/ (double)d->timeScale_); break;
            case PosterTime:
                xmpData_["Xmp.video.PosterTime"] = returnBufValue(buf); break;
            case SelectionTime:
                xmpData_["Xmp.video.SelectionTime"] = returnBufValue(buf); break;
            case SelectionDuration:
                xmpData_["Xmp.video.SelectionDuration"] = returnBufValue(buf)*1000/ d->timeScale_; break;
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
                    d->timeScale_ = returnBufValue(buf);
                    io_->seek(-4,BasicIo::cur);
                    int64_t tmpTimeScale  = (int64_t)xmpData_["Xmp.video.TimeScale"].toLong();
                    buf = returnBuf(tmpTimeScale);
                    io_->write(buf.pData_,4);
                }
                else
                {
                    io_->read(buf.pData_,4);
                    d->timeScale_ = returnBufValue(buf);
                }
                break;
            case Duration:
                if(xmpData_["Xmp.video.Duration"].count() > 0)
                {
                    int64_t duration  = (int64_t)((double)xmpData_["Xmp.video.Duration"].
                            toLong()*(double)d->timeScale_/(double)1000);
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
                            *(double)d->timeScale_/(double)1000);//1024=  1 second
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
                            .toLong()*(double)d->timeScale_/(double)1000);
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
                            .toLong()*(double)d->timeScale_/(double)1000);
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

    double aspectRatio = (double)d->width_ / (double)d->height_;
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

void QuickTimeVideo::writeStringData(Exiv2::Xmpdatum xmpStringData, int32_t size, int32_t skipOffset)
{
    if(xmpStringData.count() > 0)
    {
        std::string dataString = xmpStringData.toString();
        Exiv2::byte* rawData = new byte[(uint8_t)size];
        int32_t newSize = (int32_t)dataString.size();
        for(int32_t i=0; i<min(newSize,size); i++)
        {
            rawData[i] = (Exiv2::byte)dataString[i];
        }
        if(size > newSize)
        {
            for(int32_t i=newSize; i<size; i++)
            {
                rawData[i] = (Exiv2::byte)0;
            }
        }
        io_->write(rawData,size);
        io_->seek((int)skipOffset,BasicIo::cur);
        delete[] rawData;
    }
    else
    {
        io_->seek((size+skipOffset),BasicIo::cur);
    }
}

void QuickTimeVideo::writeLongData(Exiv2::Xmpdatum xmpIntData, int32_t size, int32_t skipOffset)
{
    if(xmpIntData.count() > 0)
    {
        int32_t  rawIntData = (int32_t)xmpIntData.toLong();
        io_->write((Exiv2::byte*)&rawIntData,size);
        io_->seek(skipOffset,BasicIo::cur);
    }
    else
    {
        io_->seek((size+skipOffset),BasicIo::cur);
    }
}

void QuickTimeVideo::writeShortData(Exiv2::Xmpdatum xmpIntData, int16_t size, int32_t skipOffset)
{
    if(xmpIntData.count() > 0)
    {
        int16_t  rawIntData = (int16_t)xmpIntData.toLong();
        io_->write((Exiv2::byte*)&rawIntData,size);
        io_->seek(skipOffset,BasicIo::cur);
    }
    else
    {
        io_->seek((size+skipOffset),BasicIo::cur);
    }
}

//
bool QuickTimeVideo::writeMultibyte(Exiv2::byte * bRawData ,int32_t iSize, int32_t iOffset)
{
    int32_t iCurrPos = io_->tell();
    if (iSize != 0 && bRawData != NULL)
    {
        if ((io_->write(bRawData, iSize) > 0))
            io_->seek(iOffset , BasicIo::cur);
        else
            io_->seek(iCurrPos + iSize + iOffset, BasicIo::beg);
    }
    else
        io_->seek(iCurrPos + iOffset, BasicIo::beg);

    //Return value is just for assignment to a variable at calling location.
    return true;
}

void QuickTimeVideo::writeApertureData(Exiv2::Xmpdatum xmpApertureData, int16_t size, int32_t skipOffset)
{
    if(xmpApertureData.count() >0)
    {
        Exiv2::byte rawApertureData[4];
        Exiv2::byte rawApertureDataTmp[2];
        const std::string apertureData =  xmpApertureData.toString();
        vector<uint16_t> apertureDataValue = getNumberFromString(apertureData,'.');
        memcpy(rawApertureDataTmp,&apertureDataValue.at(0),2);
        rawApertureData[0] = rawApertureDataTmp[0];
        rawApertureData[1] = rawApertureDataTmp[1];
        memcpy(rawApertureDataTmp,&apertureDataValue.at(1),2);
        rawApertureData[2] = rawApertureDataTmp[0];
        rawApertureData[3] = rawApertureDataTmp[1];
        io_->write(rawApertureData,size);
        io_->seek(skipOffset,BasicIo::cur);
    }
    else
    {
        io_->seek((size+skipOffset),BasicIo::cur);
    }
}

bool isQTimeType(BasicIo& iIo, bool advance)
{
    const int32_t len = 4;
    Exiv2::byte* buf = new byte[len];
    iIo.read(buf, len);
    iIo.read(buf, len);

    if (iIo.error() || iIo.eof())
    {
        delete[] buf;
        return false;
    }

    bool matched = isQuickTimeType(buf[0], buf[1], buf[2], buf[3]);
    if (!advance || !matched)
    {
        iIo.seek(static_cast<int32_t>(0), BasicIo::beg);
    }
    delete[] buf;
    return matched;
}

}                                       // namespace Exiv2

