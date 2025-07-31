#!/bin/sh -e
## **************************************************************************
## For copyright and licensing terms, see the file named COPYING.
## **************************************************************************
# vim: set filetype=sh:

command1_lists="../package/commands1 ../package/extra-manpages1"
command8_lists="../package/commands8 ../package/extra-manpages8"

install -d "slashdoc"

(

cat ../package/commands1 ../package/commands8

echo getty getty-noreset
echo ${command1_lists} ${command8_lists}

cat ${command1_lists} | 
while read -r i
do 
	echo "$i.1" slashdoc/"$i.html"
	printf >> "$3" '%s.1\n' "$i"
done
cat ${command8_lists} | 
while read -r i
do 
	echo "$i.8" slashdoc/"$i.html"
	printf >> "$3" "$i.8\n"
done
for section in 3 4 5 7
do
	echo ../package/extra-manpages${section}
	cat ../package/extra-manpages${section} | 
	while read -r i
	do 
		echo "$i.${section}" slashdoc/"$i.html"
		printf >> "$3" '%s.%s\n' "$i" "${section}"
	done
done

) | 
xargs -r redo-ifchange --
