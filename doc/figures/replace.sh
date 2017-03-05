#!/bin/sh

for FILE in *.tex
do
    sed -i 's/\\\$/\$/g' $FILE
    for PORT1 in {0..6}
    do
        for PORT2 in {0..6}
        do
        #replace \$ with $
            
        #replace \_0$ with _{0}$
            sed -i 's/\\_'"$PORT1"'\$/_\{'"$PORT1"'\}\$/g' $FILE
            sed -i 's/\\_'"$PORT1$PORT2"'\$/_\{'"$PORT1$PORT2"'\}\$/g' $FILE
        #replace 
        done
    done
    sed -ri 's/\\ensuremath\{\\backslash\}/\\/g' $FILE
    sed -ri 's/\\_M\$/_\{M\}\$/g' $FILE

done

sed -ri 's/\\begin\{tikzpicture\}/\\begin\{tikzpicture\}\[scale=0.75\]/g' ThreePortModel.tex
