DISTS="trusty xenial yakkety"
STRRELEASE=$(cat version.h |grep DEEMBED_VERSION)
RELEASE="${STRRELEASE##* }"
git clone . ../DeEmbed-$RELEASE
cd ..
cd DeEmbed-$RELEASE
rm -rf .git
cd ..
tar -zcvf DeEmbed_$RELEASE.orig.tar.gz DeEmbed-$RELEASE
cd DeEmbed-$RELEASE
COUNT=0
for DIST in ${DISTS} ; do
	COUNT=$(($COUNT-1))
	
	dch -D $DIST -m -v $RELEASE$COUNT -b
	
	debuild -S -k8AD5905E
	
	
done
