#!/bin/sh

##
# buildAll.sh - rebuild everything


##
# useful little function for giving us properties of a library
props() {
	if [ ! -e $* ]; then
		echo $* does not exist
		exit 1
	fi
	ls    -alt  $*
	lipo  -info $*
	otool -L    $*
	echo ----------------------
}

##
# clean everthing
for target in console console+library console+dylib console+framework; do
	for configuration in Debug Release; do
		xcodebuild -project *.xcodeproj -configuration $configuration -target $target clean
		if [ $? != 0 ]; then
			echo something wrong!
			exit 1
		fi
	done
done

##
# rebuild everthing
for target in console console+library console+dylib console+framework; do
	echo ''
	echo building target $target
	echo ''
	for configuration in Debug Release; do
		xcodebuild -project *.xcodeproj -configuration $configuration -target $target
		if [ $? != 0 ]; then
			echo ''
			echo something wrong!
			echo ''
			exit 1
		fi
	done
done

##
# report the properties of everything we have built
for configuration in Debug Release; do
	for blob in console console+dylib console+framework console+library libmylib.a myDYLIB.dylib myframework.framework/Versions/A/myframework ;do 
		
		props build/$configuration/$blob
		
		if [ $? != 0 ]; then
			echo ''
			echo something wrong!
			echo ''
			exit 1
		fi
	done
done

##
# now test that everything executes OK
for configuration in Debug Release; do
	for blob in console console+dylib console+framework console+library ;do 
		echo  build/$configuration/$blob
		pushd build/$configuration >> /dev/null
			./$blob
			result=$?
		popd >> /dev/null
		echo '---'
		if [ $result != 0 ]; then
			echo ''
			echo something wrong!
			echo ''
			exit $result
		fi
	done
done

# That's all Folks!
##
