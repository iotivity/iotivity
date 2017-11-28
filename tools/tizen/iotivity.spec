Name: iotivity
Version: %{!?version:1.3.1}%{?version}
Release: %{!?release:0}%{?release}
Summary: IoT Connectivity sponsored by the OCF
Group: Network & Connectivity / IoT Connectivity
License: Apache-2.0 and BSD-2-Clause and (MIT or BSL-1.0) and MIT
URL: https://www.iotivity.org/
Source0: http://mirrors.kernel.org/%{name}/%{version}/%{name}-%{version}.tar.gz

%if 0%{?manifest:1}
Source1001: %{name}.manifest
Source1002: %{name}-test.manifest
%endif

%if 0%{?tizen:1}
%{!?TARGET_OS: %define TARGET_OS tizen}
#TODO: align to default
%define LOGGING 1
%endif

%if "%{tizen}" == "2.3"
%define TARGET_TRANSPORT IP,BT
%endif

%if "%{tizen}" == "3.0"
%define OIC_SUPPORT_TIZEN_TRACE True
%endif

%if "%{profile}" == "ivi"
%define TARGET_TRANSPORT IP
%endif

%if "%{TARGET_OS}" == "linux"
%define TARGET_TRANSPORT IP
%endif


# default is RELEASE mode.
# If DEBUG mode is needed, please use tizen_build_devel_mode
%define RELEASE True
# For Example
%if %{RELEASE} == "True"
%define build_mode release
%else
%define build_mode debug
%endif

%if 3 <= 0%{?tizen_version_major}
%ifarch armv7l armv7hl armv7nhl armv7tnhl armv7thl
BuildRequires: python-accel-armv7l-cross-arm
%endif
%ifarch aarch64
BuildRequires: python-accel-aarch64-cross-aarch64
%endif
%endif

%if ! 0%{?license:0}
%define license %doc
%endif

# Default values to be eventually overiden BEFORE or as gbs params:
%{!?ES_TARGET_ENROLLEE: %define ES_TARGET_ENROLLEE %{TARGET_OS}}
%{!?LOGGING: %define LOGGING 0}
%{!?RD_MODE: %define RD_MODE CLIENT}
%{!?RELEASE: %define RELEASE 1}
%{!?ROUTING: %define ROUTING EP}
%{!?SECURED: %define SECURED 1}
%{!?TARGET_OS: %define TARGET_OS linux}
%{!?TARGET_TRANSPORT: %define TARGET_TRANSPORT ALL}
%{!?VERBOSE: %define VERBOSE 1}
%{!?WITH_CLOUD: %define WITH_CLOUD False}
%{!?WITH_MQ: %define WITH_MQ OFF}
%{!?WITH_PROXY: %define WITH_PROXY 1}
%{!?WITH_TCP: %define WITH_TCP 0}
%{!?MULTIPLE_OWNER: %define MULTIPLE_OWNER 1}
%{!?OIC_SUPPORT_TIZEN_TRACE: %define OIC_SUPPORT_TIZEN_TRACE False}
%define BUILD_DIR out/%{TARGET_OS}/%{TARGET_ARCH}/%{build_mode}/

%define SCONSFLAGS \\\
--prefix=%{_prefix} \\\
%{?_smp_mflags} \\\
\\\
ES_TARGET_ENROLLEE=%{ES_TARGET_ENROLLEE} \\\
LIB_INSTALL_DIR=%{_libdir} \\\
LOGGING=%{LOGGING} \\\
RD_MODE=%{RD_MODE} \\\
RELEASE=%{RELEASE} \\\
ROUTING=%{ROUTING} \\\
SECURED=%{SECURED} \\\
TARGET_OS=%{TARGET_OS} \\\
TARGET_TRANSPORT=%{TARGET_TRANSPORT} \\\
VERBOSE=%{VERBOSE} \\\
WITH_CLOUD=%{WITH_CLOUD} \\\
WITH_MQ=%{WITH_MQ} \\\
WITH_PROXY=%{WITH_PROXY} \\\
WITH_TCP=%{WITH_TCP} \\\
MULTIPLE_OWNER=%{MULTIPLE_OWNER} \\\
OIC_SUPPORT_TIZEN_TRACE=%{OIC_SUPPORT_TIZEN_TRACE} \\\
%{?EXTRA_RPM_SCONSFLAGS}

BuildRequires:  autoconf
BuildRequires:  automake
BuildRequires:  libtool
BuildRequires:  chrpath
BuildRequires:  expat-devel
BuildRequires:  python
BuildRequires:  scons
BuildRequires:  openssl-devel
BuildRequires:  boost-devel
BuildRequires:  boost-thread
BuildRequires:  boost-system
BuildRequires:  boost-filesystem
BuildRequires:  pkgconfig(uuid)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(libcurl)
BuildRequires:  pkgconfig(sqlite3)
%if "%{TARGET_OS}" == "tizen"
BuildRequires:  gettext-tools
BuildRequires:  pkgconfig(dlog)
%if "%{OIC_SUPPORT_TIZEN_TRACE}" == "True"
BuildRequires:  pkgconfig(ttrace)
%endif
BuildRequires:  pkgconfig(capi-network-connection)
BuildRequires:  pkgconfig(capi-network-bluetooth) >= 0.1.52
%if 0%{?profile:1} && 3 <= 0%{?tizen_version_major}
BuildRequires:  bluetooth-tools-profile_%{profile}
%else
BuildRequires:  bluetooth-tools
%endif
%endif
Requires(postun): /sbin/ldconfig
Requires(post): /sbin/ldconfig

%description
An open source reference implementation of the OIC standard specifications
IoTivity Base Libraries are included.


%package service
Summary: Development files for %{name}
Group: Network & Connectivity/Service
Requires: %{name} = %{version}-%{release}

%description service
The %{name}-service package contains service libraries files for
developing applications that use %{name}-service.

%package test
Summary: Development files for %{name}
Group: Network & Connectivity/Testing
Requires: %{name} = %{version}-%{release}

%description test
The %{name}-test package contains example files to show
how the iotivity works using %{name}-test

%package devel
Summary: Development files for %{name}
Group: Network & Connectivity/Development
Requires: %{name} = %{version}-%{release}
Requires: pkgconfig
Requires: boost-devel

%description devel
The %{name}-devel package contains libraries and header files for
developing applications that use %{name}.

%prep
%setup -q
chmod g-w %_sourcedir/*

find . \
     -iname "LICEN*E*"  \
     -o -name "*BSD*" \
     -o -name "*COPYING*" \
     -o -name "*GPL*" \
     -o -name "*MIT*" \
     | sort | uniq \
     | grep -v 'libcoap-4.1.1/LICENSE.GPL'  \
     | while read file ; do \
          dir=$(dirname -- "$file")
          echo "Files: ${dir}/*"
          echo "License: ${file}"
          sed 's/^/ /' "${file}"
          echo ""
          echo ""
     done > tmp.tmp && mv tmp.tmp LICENSE

cat LICENSE

[ -r 'config.md' ] || cat<<EOF | sed -e 's|%{buildroot}|(...BUILDROOT...)|g' | tee config.md
# Build configuration info #
SCONSFLAGS: %{SCONSFLAGS}
$(scons --version)
EOF

%if 0%{?manifest:1}
cp %{SOURCE1001} .
%if 0%{?tizen_version_major} < 3
cp %{SOURCE1002} .
%else
cp %{SOURCE1001} ./%{name}-test.manifest
%endif
%endif

%build
SCONSFLAGS="%{SCONSFLAGS}" ; export SCONSFLAGS;
scons

%install
rm -rf %{buildroot}
CFLAGS="${CFLAGS:-%optflags}" ; export CFLAGS ;
SCONSFLAGS="%{SCONSFLAGS}" ; export SCONSFLAGS;
scons --install-sandbox=%{buildroot} install


find "%{buildroot}" -type f -perm u+x -exec chrpath -d "{}" \;
find "%{buildroot}" -type f -iname "lib*.so" -exec chrpath -d "{}" \;

# Support legacy path (transitional, use pkg-config)
ln -fs iotivity/resource %{buildroot}%{_includedir}/
ln -fs iotivity/service %{buildroot}%{_includedir}/
ln -fs iotivity/c_common %{buildroot}%{_includedir}/

rm -rfv out %{buildroot}/out %{buildroot}/${HOME} ||:


%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%if 0%{?manifest:1}
%manifest %{name}.manifest
%endif
%defattr(-,root,root,-)
%license LICENSE
%{_libdir}/liboc.so
%{_libdir}/liboc_logger.so
%{_libdir}/liboc_logger_core.so
%{_libdir}/liboctbstack.so
%{_libdir}/libconnectivity_abstraction.so
%{_libdir}/libresource_directory.so
%if 0%{?SECURED} == 1
%{_libdir}/libocpmapi.so
%{_libdir}/libocprovision.so
%endif

%files service
%if 0%{?manifest:1}
%manifest %{name}.manifest
%endif
%defattr(-,root,root,-)
%license LICENSE
%{_libdir}/libBMISensorBundle.so
%{_libdir}/libDISensorBundle.so
%{_libdir}/libHueBundle.so
%{_libdir}/librcs_client.so
%{_libdir}/librcs_common.so
%{_libdir}/librcs_container.so
%{_libdir}/librcs_server.so
%{_libdir}/libESEnrolleeSDK.so
%{_libdir}/libESMediatorRich.so
%{_libdir}/libnotification*.so
%if 0%{?WITH_PROXY} == 1
%{_libdir}/libcoap_http_proxy.so
%endif
%if "%{TARGET_OS}" == "linux"
%{_libdir}/lib*plugin.so
%{_libdir}/lib*ipca.so
%endif

%files test
%if 0%{?manifest:1}
%manifest %{name}-test.manifest
%endif
%defattr(-,root,root,-)
%license LICENSE
%{_libdir}/%{name}/*

%files devel
%defattr(-,root,root,-)
%license LICENSE
%doc *.md
%{_libdir}/lib*.a
%{_libdir}/pkgconfig/%{name}.pc
%{_includedir}/*

