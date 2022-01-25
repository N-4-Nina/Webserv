# !/bin/bash
S_FOLDER=srcs

echo "SRCS = \\" > srcs.mk
find $S_FOLDER -type f  | xargs -0 | awk -F"/" '{ print $NF "\134"}' >> srcs.mk
