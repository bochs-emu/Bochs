#!/usr/bin/perl

#
#
#	Copyright (C) 1991-2002 and beyond by Bungie Studios, Inc.
#	and the "Aleph One" developers.
#
#	This program is free software; you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation; either version 2 of the License, or
#	(at your option) any later version.
#
#	This program is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	This license is contained in the file "COPYING",
#	which is included with this source code; it is available online at
#	http://www.gnu.org/licenses/gpl.html
#
#

my $fullfolderPath = shift(@ARGV);
die "err: No folder specified" unless defined($fullfolderPath) && length($fullfolderPath);

$fullfolderPath =~ s{/$}{};
$fullfolderPath =~ m{([^/]+)$};

local $folderName   = $1;
local $folderSize   = undef;
local $imageName    = "'$fullfolderPath.dmg'";
local $imageSectors = undef;
local $imageTemp    = "'$fullfolderPath-tmp.dmg'";

die "err: $folderName is not a directory\n" if(!-d $fullfolderPath);

# Know a better way to get the first value from du? 
($folderSize) = split(m/ /, `du -s "$fullfolderPath"`);
die "err: du failed with $?\n" if($?);

# Inflate $folderSize for disk image overhead. Minimum 5 MB disk
local $fiveMBImage=10240;

$imageSectors = $folderSize + int($folderSize * 0.08);
if($imageSectors < $fiveMBImage)
{
	$imageSectors = $fiveMBImage;
}

# Create image, overwriting prior version
`hdiutil create -ov -sectors $imageSectors $imageTemp`;
die "err: hdiutil create failed with $?\n" if($?);

# Initialize the image
local $hdid_info=`hdid -nomount $imageTemp`;
die "err: hdid -nomount failed with $?\n" if($?);

$hdid_info =~ s/( |\t|\n)+/~!/g;
local (@hdid_info) = split(m/~!/, $hdid_info);

local ($disk_dev, $hfs_dev);

$disk_dev = $hdid_info[0];
$hfs_dev = $hdid_info[4];

$disk_dev =~ s{/dev/}{};
$hfs_dev =~ s/disk/rdisk/;

`newfs_hfs -v "$folderName" $hfs_dev`;
if($?)
{
	local $err = $?;
	`hdiutil eject $disk_dev`;
	die "err: newfs_hfs failed with $err\n";
}

`hdiutil eject $disk_dev`;

# Fill the image
$hdid_info = `hdid $imageTemp`;
die "err: hdid failed with $?\n" if($?);

$hdid_info =~ s/( |\t|\n)+/~!/g;
@hdid_info = split(m/~!/, $hdid_info);

$disk_dev = $hdid_info[0];
$disk_dev =~ s{/dev/}{};

`ditto -rsrcFork "$fullfolderPath" "/Volumes/$folderName"`;
if($?)
{
	local $err = $?;
	`hdiutil eject $disk_dev`;
	die "err: ditto failed with $err\n";
}
`hdiutil eject $disk_dev`;

# Create the compressed image
`hdiutil convert $imageTemp -format UDCO -o $imageName`;
die "err: hdiutil convert failed with $?\n" if($?);
