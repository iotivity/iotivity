Name: iotivity
Version: 1.0.0
Release: 0
Summary: IoTivity Base Stack & IoTivity Services
Group: System Environment/Libraries
License: Apache-2.0
URL: https://www.iotivity.org/
Source0: %{name}-%{version}.tar.bz2
Source10: cereal.tar.bz2
Source100: tinycbor.tar.bz2
Source101: gtest-1.7.0.zip
# https://github.com/dascandy/hippomocks/archive/2f40aa11e31499432283b67f9d3449a3cd7b9c4d.zip
Source102: 2f40aa11e31499432283b67f9d3449a3cd7b9c4d.zip
BuildRequires: gettext-tools
BuildRequires: expat-devel
BuildRequires:	python, libcurl-devel
BuildRequires:	scons
BuildRequires:  unzip
BuildRequires:	openssl-devel
BuildRequires:  boost-devel, boost-program-options, boost-thread
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(uuid)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(capi-network-wifi)
BuildRequires:  pkgconfig(capi-network-bluetooth)
Requires(postun): /sbin/ldconfig
Requires(post): /sbin/ldconfig

%description
IoTivity Base (RICH & LITE) Stack & IoTivity Services

%package devel
Summary: Development files for %{name}
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}
Requires: pkgconfig

%description devel
The %{name}-devel package contains libraries and header files for
developing applications that use %{name}.

%package examples
Summary: Examples files for %{name}
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}
Requires: pkgconfig

%description examples
Contains samples applications that use %{name}.

%prep
%setup -q -n %{name}-%{version} -a 10 -a 100 -a 101 -a 102

%define release_mode false
%define build_mode debug
%define secure_mode 0
%define RPM_ARCH %{_arch}
%define target_os linux

# overide to prevent issues
%define _smp_mflags -j4

# overide arch if needed
%ifarch %arm
%if "%{tizen}" == "2.4"
%define RPM_ARCH arm-v7a
%endif
%else
%ifarch aarch64
%define RPM_ARCH arm64
%else
%ifarch i586 i686 %{ix86}
%define RPM_ARCH x86
%endif
%endif
%endif

%build

cp -rfv hippomocks-2f40aa11e31499432283b67f9d3449a3cd7b9c4d  extlibs/hippomocks-master
ln -fs ../../gtest-1.7.0  extlibs/gtest/gtest-1.7.0

find . -iname "*.h*" -exec chmod -v a-x "{}" \;

scons %{?_smp_mflags} \
    RELEASE=%{release_mode} \
    SECURED=%{secure_mode} \
    TARGET_ARCH=%{RPM_ARCH} \
    TARGET_OS=%{target_os} \
    TARGET_TRANSPORT=IP

%__make -C examples/OICSensorBoard/ BUILDTYPE=debug CLIENTARCH=%{RPM_ARCH}

%install
rm -rf %{buildroot}

echo %__make \
    -C resource \
    DEPEND_DIR=$(pwd)/extlibs/ \
    DEST_LIB_DIR=%{buildroot}%{_libdir}/%{name}/ \
    install

echo %__make \
    -C resource/csdk \
    DEPEND_DIR=$(pwd)/extlibs/ \
    DESTDIR=%{buildroot} \
    install

echo %__make \
    -C resource/oc_logger \
    DEPEND_DIR=$(pwd)/extlibs/ \
    DESTDIR=%{buildroot} \
    install


install -d %{buildroot}%{_sbindir}

install -d %{buildroot}%{_libdir}

find . -iname "lib*.a" -exec install "{}" %{buildroot}%{_libdir}/ \;
find . -iname "lib*.so" -exec install "{}" %{buildroot}%{_libdir}/ \;

find resource service -iname "include" -o -iname 'inc' -a -type d\
    | grep -v example | grep -v csdk | while read include ; do \
    dirname=$(dirname -- "$include") ; \
    install -d %{buildroot}%{_includedir}/%{name}/${dirname} ; \
    install $include/*.* %{buildroot}%{_includedir}/%{name}/${dirname}/ ; \
done

cd resource/csdk
find . -iname "include" -o -iname 'inc' -a -type d\
    | while read include ; do \
    dirname=$(dirname -- "$include") ; \
    install -d %{buildroot}%{_includedir}/%{name}/resource/${dirname} ; \
    install $include/*.* %{buildroot}%{_includedir}/%{name}/resource/${dirname}/ ; \
done
cd -

install -d %{buildroot}%{_includedir}/%{name}/resource/oc_logger/targets/
install ./resource/oc_logger/include/targets/*.* %{buildroot}%{_includedir}/%{name}/resource/oc_logger/targets/

install -d %{buildroot}%{_includedir}/%{name}/resource/c_common
install ./resource/c_common/*.h %{buildroot}%{_includedir}/%{name}/resource/c_common/

install -d %{buildroot}%{_libdir}/%{name}/examples/
install out/%{target_os}/%{RPM_ARCH}/%{build_mode}/resource/examples/*client %{buildroot}%{_libdir}/%{name}/examples/
install out/%{target_os}/%{RPM_ARCH}/%{build_mode}/resource/examples/*server %{buildroot}%{_libdir}/%{name}/examples/

rm -fv %{buildroot}%{_libdir}/libcoap.a
rm -fv %{buildroot}%{_libdir}/liboc.a
rm -fv %{buildroot}%{_libdir}/liboc_logger.a
rm -fv %{buildroot}%{_libdir}/libmosquitto.a

%__make -C examples/OICSensorBoard/ install \
 BUILDTYPE=debug \
 CLIENTARCH=%{RPM_ARCH} \
 install_dir=%{buildroot}/%{_libdir}/%{name}/examples/OICSensorBoard/

%clean
rm -rf %{buildroot}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/lib*.so

%files devel
%defattr(644,root,root,755)
%{_includedir}/%{name}/
%{_libdir}/lib*.a

%files examples
%defattr(-,root,root,-)
%{_libdir}/%{name}/examples/

