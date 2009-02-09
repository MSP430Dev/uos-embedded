#!/bin/sh
#
# Delete temporary files.
#
find . -name '*~' -print0 | xargs -0 -r --verbose rm -r
