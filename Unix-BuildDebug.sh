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

#This version of the build script is hardcoded to the debug version so
#it can be used by the Makefile
cd build
cmake -DCMAKE_BUILD_TYPE="Debug" ../
make -j 4 && if [ -d ../res/ ]; then
	mv ../res/ ./ 2>/dev/null
fi
