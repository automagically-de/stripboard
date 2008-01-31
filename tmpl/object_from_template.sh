#!/bin/sh

if [ -z "$1" ]; then
	echo "usage: $0 <type> [<Type> [<TYPE>]]" >&2
	exit 1
fi

g_type=$1
g_Type=`echo $1 | awk '{ sub(".", substr(toupper($1),1,1) , $1); print}'`
g_TYPE=`echo $1 | tr 'a-z' 'A-Z'`

out_c="../src/object_$g_type.c"
out_h="../src/object_$g_type.h"

if [ -f "$out_c" -o -f "$out_h" ]; then
	echo "$out_c or $out_h already exist, aborting." >&2
	exit 1
fi

EXPR1="s/%template%/$g_type/g"
EXPR2="s/%Template%/$g_Type/g"
EXPR3="s/%TEMPLATE%/$g_TYPE/g"

sed -e $EXPR1 -e $EXPR2 -e $EXPR3 < object_template.h > $out_h
sed -e $EXPR1 -e $EXPR2           < object_template.c > $out_c

