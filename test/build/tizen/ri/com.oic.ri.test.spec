%define PREFIX /usr/apps/com.oic.iori
%define ROOTDIR  %{_builddir}/%{name}-%{version}

Name: com-oic-ri-test
Version: 1.2.0
Release: 4
License: To be filled
Summary: RI TEST
Packager: Masud Parves <m.parves@samsung.com>
Group: Applications/OICSample
Source: %{name}-%{version}.tar.gz


BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: boost-devel
BuildRequires: boost-thread
BuildRequires: boost-system
BuildRequires: boost-filesystem
BuildRequires: pkgconfig(capi-network-wifi)
BuildRequires: scons
BuildRequires: iotivity
BuildRequires: expat-devel

%{!?TARGET_SERVICE: %define TARGET_SERVICE ricsdk}
%{!?TARGET_TEST: %define TARGET_TEST tc}

%description
Platform Project

%prep
%setup -q

%build
scons TARGET_TEST=%{TARGET_TEST} TARGET_SERVICE=%{TARGET_SERVICE}

%install
#make install DESTDIR=$RPM_BUILD_ROOT/usr
mkdir -p %{buildroot}%{_datadir}/packages
mkdir -p %{buildroot}/%{_sysconfdir}/smack/accesses2.d
mkdir -p %{buildroot}/usr/apps/com.oic.ri.test/bin/
mkdir -p %{buildroot}/usr/apps/com.oic.ri.test/bin/internal

cp -rf %{ROOTDIR}/com.oic.ri.test.xml %{buildroot}/%{_datadir}/packages
cp -rf %{ROOTDIR}/iotivity_ri_*_test %{buildroot}/usr/apps/com.oic.ri.test/bin/

%clean
#make clean

%files
%manifest com.oic.ri.test.manifest
%defattr(-,root,root,-)
/usr/apps/com.oic.ri.test/bin/iotivity_ri_*_test

/%{_datadir}/packages/com.oic.ri.test.xml

#%defattr(644, root, root)
#%attr(755, root, root) /usr/bin

