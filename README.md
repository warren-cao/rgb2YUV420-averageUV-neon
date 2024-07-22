# rgb2YUV420-averageUV-neon
Current support:
RGB  --> NV12/NV21   YV12/YU12/I420

ARGB --> NV12/NV21   YV12/YU12/I420

The four Y matrices in YUV420 share a set of UV values.

The usual RGB to YUV420 algorithm will sample one of the 4 pixels and calculate the UV value, and then use this set of UV values to the Y matrix, in some cases will lead to some image color loss, so here provides a method to calculate the average value of 4 pixels corresponding to UV and then fill, to avoid color loss, recorded here.

Currently supported languages: C&ARM NEON
