#!/bin/echo docker build . -f
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

FROM fedora:24
MAINTAINER Philippe Coval (philippe.coval@osg.samsung.com)

ENV project iotivity
ARG version
ENV version ${version:-1.3.1}
ENV package ${project}-${version}
ARG user
ENV user ${user:-abuild}
ARG release
ENV release ${release:-0~${user}0+${DISTTAG}}
ENV EMAIL ${EMAIL:-nobody@localhost}
ENV NAME ${NAME:-Nobody}

RUN echo "#log: ${project}: Preparing system (DISTTAG=${DISTTAG})" \
  && dnf update -y \
  && dnf install -y \
   git \
   make \
   sudo \
   rpm-build \
\
   autoconf \
   automake \
   boost-devel \
   bzip2 \
   chrpath \
   expat-devel \
   findutils \
   gcc-c++\
   gettext-devel \
   git \
   glib2-devel \
   libcurl-devel \
   libtool \
   libuuid-devel \
   openssl-devel \
   scons \
   sqlite-devel \
   unzip \
   wget \
   xz \
 && dnf clean -y all \
 && sync

ARG workdir
ENV workdir ${workdir:-${HOME}/usr/local/src/${project}}
WORKDIR ${workdir}

COPY . ${WORKDIR}

RUN echo "#log: ${project}: Preparing sources" \
  && uname -a \
  && cat /etc/os-release \
  && scons --version \
  && gcc --version \
  && g++ --version \
  && [ ! -x prep.sh ] || EXEC_MODE=true ./prep.sh \
  && tar cvfz ../${package}.tar.gz \
--transform "s|^./|${package}/|" \
--exclude 'debian' --exclude-vcs \
. \
  && sync

RUN echo "#log: ${project}: Setup rpm-build (${WORKDIR})" \
  && mkdir -p "/root/rpmbuild/SOURCES/" \
  && ln -vfs "${workdir}/../${package}.tar.gz"  "/root/rpmbuild/SOURCES/" \
  && ln -vfs "${workdir}/tools/tizen/"* "/root/rpmbuild/SOURCES/" \
  && sync

RUN echo "#log: ${project}: Building RPMs (${project}-${version}.${release})" \
  && cd "/root/rpmbuild/SOURCES/" \
  && ls -l \
  && time rpmbuild -ba "${project}.spec" \
   --define "version ${version}" \
   --define "release ${release}" \
  && find /root/rpmbuild/ -iname "*.*rpm" \
  && sync

RUN echo "#log: ${project}: Installing RPMs (${release})" \
  && rpm -v -i /root/rpmbuild/RPMS/*/*.rpm \
  && rpm -ql ${project} \
  && rpm -ql ${project}-service \
  && rpm -ql ${project}-devel \
  && rpm -ql ${project}-test \
  && rpm -ql ${project}-debuginfo \
  && sync
