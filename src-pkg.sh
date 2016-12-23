DISTS="trusty xenial yakkety"
STRRELEASE=$(cat version.h |grep DEEMBED_VERSION)
RELEASE="${STRRELEASE##* }"
git clone . ../deembed-$RELEASE
cd ..
cd deembed-$RELEASE
rm -rf .git
cd ..
tar -zcvf deembed_$RELEASE.orig.tar.gz deembed-$RELEASE
cd deembed-$RELEASE
COUNT=0
for DIST in ${DISTS} ; do
	COUNT=$(($COUNT-1))
	
	dch -D $DIST -m -v $RELEASE$COUNT -b
	
	debuild -S -k8AD5905E
	
	
done
