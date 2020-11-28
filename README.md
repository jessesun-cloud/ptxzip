# ptxzip
A tool to reduce ptx file size

ABOUT
================================================================================
ptxzip is an application to shrink ptx file size by using following tricks.

1) reduce position precision
scanned data likely have precision above 1mm, it is not necessary to keep
more than 3 digit after dot. for example:
12.032272 -2.613480 -2.036392
can store as
12.032 -2.613 -2.036

2) reduce intensity precision
intensity normally save as 6 digit precision float. sometime less precision is acceptable.
for example
0.156863
can store as
0.157

3) subsample
for dense scan we maybe don't need all the detail data, therefor can apply subsample
to reduce disk space

using trick 1) and 2), the disk space saving is about 25%.

usage:
ptxzip inputfilename outputfilename -p PRECISION -s SAMPLERATE -i INTENSITY_PRECISION

PRECISION, default be 3, is the precision we want to store position.
INTENSITY_PRECISION, default be 3, is the precision we want to store intensity.
SAMPLERATE, is the sub sample rate we want to apply.
when value is 1 we don't apply subsample.
when value is N, output will keep 1 point for every N X N points of original point cloud

for example
ptxzip in.ptx out.ptx -p 4 -i 4
use 4 digit to store position and intensity, don't do subsample.

ptxzip in.ptx out.ptx -s 2
use 3 digit to store position and intensity, apply 2 x subsample.

PTX FILE Format
================================================================================
Ptx format is a point cloud text format by Leica Geosystems LLC.
Ptx format reference:
http://paulbourke.net/dataformats/ptx/

