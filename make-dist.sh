#!/bin/bash

#
# This script creates the dist packages and uploads them
#
# Unless you're a maintainer of dbus-cxx, you probably don't care about this.

#
# Release Process:
#
# 1. Mark the debian version as released
# 2. Ensure that the debian version and the version in CMakeLists.txt agree with each other
# 3. Commit changes(if needed).
# 4. Create a tag of the release version
# 5. Run this script
# 6. Go to sourceforge and set the default download to be the newly uploaded file

TAG_NAME=

checkout_tag() {
	git clone git@github.com:dbus-cxx/dbus-cxx.git dbus-cxx-release
	cd dbus-cxx-release
	git checkout tags/"$TAG_NAME"
}

build_docs() {
	mkdir build
	cd build
	cmake .. -DBUILD_SITE=ON
	make
	cd ..
}

build_source_package() {
	cd build
	make package_source
	cd ..
}

upload_files_to_sourceforge() {
	cd build
	mkdir "$TAG_NAME"
	cp *.tar.gz "$TAG_NAME"
	scp -Crp "$TAG_NAME" rm5248,dbus-cxx@frs.sourceforge.net:/home/frs/project/d/db/dbus-cxx/dbus-cxx/
	cd ..
}

read -p "Please enter tag name: " TAG_NAME

checkout_tag
build_docs
build_source_package
upload_files_to_sourceforge
