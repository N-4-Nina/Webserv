# !/bin/bash
S_FOLDER=srcs

echo "SRCS = \\" > srcs.mk
find $S_FOLDER -type f | awk -F "$S_FOLDER/" '{ print $2 $3 "\134"}' >> srcs.mk