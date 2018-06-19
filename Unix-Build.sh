# Copyright (C) 2014 Benny Bobaganoosh
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

BUILT_TARGET="Release"

#First parameter, if any, is used as build target.
#If no parameter is given, then default to Release
if [ "$#" == 0 ]; then
	BUILD_TARGET="Release"
else
	BUILD_TARGET="$1"
fi

#All remaining parameters are used as parameters to CMake

cd build
cmake -DCMAKE_BUILD_TYPE="$BUILD_TARGET" "${@:1}" ../
make -j 4 && if [ -d ../res/ ]; then
	mv ../res/ ./
fi
