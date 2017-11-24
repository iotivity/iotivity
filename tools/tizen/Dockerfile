#!/bin/echo docker build . -f
#{
# Copyright 2017 Samsung Electronics France SAS
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#}

FROM ubuntu:16.04
MAINTAINER Philippe Coval (philippe.coval@osg.samsung.com)
ENV project iotivity

ARG SCONSFLAGS
ENV SCONSFLAGS ${SCONSFLAGS:-"VERBOSE=1 ERROR_ON_WARN=False"}

ARG prefix
ENV prefix ${prefix:-/usr/}
ARG destdir
ENV destdir ${destdir:-/usr/local/opt/${project}}

ENV DEBIAN_FRONTEND noninteractive
ENV LC_ALL en_US.UTF-8
ENV LANG ${LC_ALL}

RUN echo "#log: Configuring locales" \
  && apt-get update \
  && apt-get install -y locales \
  && echo "${LC_ALL} UTF-8" | tee /etc/locale.gen \
  && locale-gen ${LC_ALL} \
  && dpkg-reconfigure locales \
  && sync

RUN echo "#log: ${project}: Preparing system" \
  && apt-get update -y \
  && apt-get install -y \
  software-properties-common \
  lsb-release \
  scons \
  wget \
  unzip \
  \
  && apt-get clean \
  && sync

RUN echo "#log: ${project}: Install Tizen tools" \
  && distro=$(lsb_release -i -r -s -i | head -n2 | tr '\n' '_' | sed 's/.$//') \
  && url="http://download.tizen.org/tools/latest-release/$distro" \
  && add-apt-repository "deb $url /" \
  && apt-get update -y \
  && apt-get install -y --allow-unauthenticated gbs \
  && apt-get clean -y \
  && sync

ADD . /usr/local/src/${project}
WORKDIR /usr/local/src/${project}
RUN echo "#log: ${project}: Preparing sources" \
  && git config --global user.name "Nobody" \
  && git config --global user.email "nobody@localhost" \
  && git init \
  && git add -f . \
  && git commit -am 'WIP: Import ${project}' \
  && [ ! -x prep.sh ] || EXEC_MODE=true ./prep.sh \
  && sync

RUN echo "#log: ${project}: Building sources" \
  && gbs \
     -c tools/tizen/.gbs.conf \
     build \
     --include-all \
     -P "profile.tizen" --arch "armv7l" \
  && ./auto_build.py tizen_secured \
  && ./auto_build.py tizen_unsecured \
  && find ~/GBS-ROOT*/local/repos/*/*/RPMS -iname "*.rpm" -exec rpm -qlp "{}" \; \
  && sync

RUN echo "#log: ${project}: Cleaning objects" \
  && sync
