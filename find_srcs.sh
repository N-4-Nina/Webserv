# !/bin/sh
S_FOLDER=srcs

echo "SRCS = \\" > srcs.mk
find $S_FOLDER -type f  | xargs -0 | awk -F"s/" '{ print $2 $3 "\134"}' >> srcs.mk
