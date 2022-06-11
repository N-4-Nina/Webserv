# !/bin/sh
S_FOLDER=./srcs

echo "SRCS = \\" > srcs.mk
find $S_FOLDER -type f | sed 's/.\/srcs//g' | cut -c2- | sed 's/$/\\/' >> srcs.mk
find ./srcs -type d | sed 's/.\/srcs//g' | sed -r '/^\s*$/d' | sed 's/^/.\/obj/' > subdirs.mk