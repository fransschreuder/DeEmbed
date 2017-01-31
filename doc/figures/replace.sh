#!/bin/sh

for FILE in *.tex
do
#replace \$ with $
    sed -i 's/\\\$/\$/g' $FILE
#replace \_0$ with _{0}$
    sed -i 's/\\_0\$/_\{0\}\$/g' $FILE
    sed -i 's/\\_1\$/_\{1\}\$/g' $FILE
    sed -i 's/\\_2\$/_\{2\}\$/g' $FILE
    sed -i 's/\\_3\$/_\{3\}\$/g' $FILE
#replace 
    sed -i 's/\\_00\$/_\{00\}\$/g' $FILE
    sed -i 's/\\_01\$/_\{01\}\$/g' $FILE
    sed -i 's/\\_10\$/_\{10\}\$/g' $FILE
    sed -i 's/\\_11\$/_\{11\}\$/g' $FILE
    sed -i 's/\\_21\$/_\{21\}\$/g' $FILE
    sed -i 's/\\_12\$/_\{12\}\$/g' $FILE
    sed -i 's/\\_22\$/_\{22\}\$/g' $FILE
    sed -i 's/\\_23\$/_\{23\}\$/g' $FILE
    sed -i 's/\\_32\$/_\{32\}\$/g' $FILE
    sed -i 's/\\_33\$/_\{33\}\$/g' $FILE
    sed -i 's/\\_03\$/_\{03\}\$/g' $FILE
    sed -i 's/\\_30\$/_\{30\}\$/g' $FILE
    sed -ri 's/\\ensuremath\{\\backslash\}/\\/g' $FILE
    sed -i 's/\\_M\$/_\{M\}\$/g' $FILE
done
