Name: iotivity
Version: 0.9
Release: 0
Summary: IoTivity Base Stack & IoTivity Services
Group: System Environment/Libraries
License: Apache-2.0
URL: https://www.iotivity.org/
Source0: %{name}-%{version}.tar.bz2

BuildRequires:	gettext, expat-devel
BuildRequires:	python, libcurl-devel
BuildRequires:	scons
BuildRequires:	openssl-devel
BuildRequires:  boost-devel
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

%prep
%setup -q -n %{name}-%{version}

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

scons -j 4 TARGET_ARCH=$RPM_ARCH

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}%{_includedir}
mkdir -p %{buildroot}%{_libdir}
mkdir -p %{buildroot}%{_sbindir}

cp out/linux/*/release/lib*.so %{buildroot}%{_libdir}
cp out/linux/*/release/lib*.a %{buildroot}%{_libdir}

cp resource/csdk/stack/include/ocstack.h %{buildroot}%{_includedir}
cp resource/include/*.h %{buildroot}%{_includedir}

cp service/things-manager/sdk/inc/*.h %{buildroot}%{_includedir}
cp service/soft-sensor-manager/SDK/cpp/include/*.h %{buildroot}%{_includedir}


%clean
rm -rf %{buildroot}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/lib*.so
%{_libdir}/lib*.a

%files devel
%defattr(-,root,root,-)
%{_includedir}/*.h
