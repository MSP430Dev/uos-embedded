#!/bin/sh
#
# Delete temporary files.
#
find . -name '*~' -print0 | xargs -0 -t rm -r
find . -name '*.tmp' -print0 | xargs -0 -t rm -r
rm -f z.pch
