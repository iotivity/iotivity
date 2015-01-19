Name: iotivity
Version: 0.9.0
Release: 0
Summary: IoTivity Base Stack & IoTivity Services
Group: System Environment/Libraries
License: Apache-2.0
URL: https://www.iotivity.org/
Source0: %{name}-%{version}.tar.bz2
Source10: cereal.tar.bz2
BuildRequires: gettext-tools
BuildRequires: expat-devel
BuildRequires:	python, libcurl-devel
BuildRequires:	scons
BuildRequires:	openssl-devel
BuildRequires:  boost-devel, boost-program-options
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
%setup -q -n %{name}-%{version} -a 10

%build
%ifarch %arm
export RPM_ARCH=arm
%else
%ifarch aarch64
export RPM_ARCH=arm64
%else
%ifarch i586 i686 %{ix86}
export RPM_ARCH=x86
%else
export RPM_ARCH=%{_arch}
%endif
%endif
%endif

find . -iname "*.h*" -exec chmod -v a-x "{}" \;

scons -j 4 TARGET_ARCH=$RPM_ARCH

%__make \
    -C examples/OICMiddle \
    TARGET_ARCH=$RPM_ARCH

touch resource/deps resource/applyDepPatches

%__make \
    -C resource \
    DEPEND_DIR=$(pwd)/extlibs/

%__make \
    -C resource/csdk \
    DEPEND_DIR=$(pwd)/extlibs/


%install
rm -rf %{buildroot}

%__make \
    -C resource \
    DEPEND_DIR=$(pwd)/extlibs/ \
    DEST_LIB_DIR=%{buildroot}%{_libdir}/%{name}/ \
    install

%__make \
    -C resource/csdk \
    DEPEND_DIR=$(pwd)/extlibs/ \
    DESTDIR=%{buildroot} \
    install

%__make \
    -C resource/oc_logger \
    DEPEND_DIR=$(pwd)/extlibs/ \
    DESTDIR=%{buildroot} \
    install


install -d %{buildroot}%{_sbindir}

install -d %{buildroot}%{_libdir}

find . -iname "lib*.a" -exec install "{}" %{buildroot}%{_libdir}/ \;
find . -iname "lib*.so" -exec install "{}" %{buildroot}%{_libdir}/ \;


install -d %{buildroot}%{_includedir}
install -d %{buildroot}%{_includedir}/%{name}/
install resource/include/*.h %{buildroot}%{_includedir}/%{name}/

install service/things-manager/sdk/inc/*.h %{buildroot}%{_includedir}/%{name}/
install service/soft-sensor-manager/SDK/cpp/include/*.h %{buildroot}%{_includedir}/%{name}/

install -d %{buildroot}%{_bindir}
install examples/OICMiddle/debug/OICMiddle %{buildroot}%{_bindir}

rm -fv %{buildroot}%{_libdir}/libcoap.a
rm -fv %{buildroot}%{_libdir}/liboc.a
rm -fv %{buildroot}%{_libdir}/liboc_logger.a
rm -fv %{buildroot}%{_libdir}/libmosquitto.a

%clean
rm -rf %{buildroot}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/lib*.so

%files devel
%defattr(644,root,root,755)
%{_includedir}/*/*.h*
%{_includedir}/*/*/*.h*
%{_libdir}/lib*.a

%files examples
%defattr(-,root,root,-)
%{_bindir}/OICMiddle
%{_libdir}/%{name}/examples/*client*
%{_libdir}/%{name}/examples/*server*
%{_libdir}/%{name}/examples/*sample*
