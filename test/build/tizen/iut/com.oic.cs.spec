%define PREFIX /usr/apps/com.oic.cs
%define ROOTDIR  %{_builddir}/%{name}-%{version}

Name: com-oic-cs
Version: 0.0.1
Release: 1
License: To be filled
Summary: Conformance Simulator
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

%description
Platform Project

%prep
%setup -q

%build
scons TARGET_TRANSPORT=IP SECURED=1

%install
#make install DESTDIR=$RPM_BUILD_ROOT/usr
mkdir -p %{buildroot}%{_datadir}/packages
mkdir -p %{buildroot}/%{_sysconfdir}/smack/accesses2.d
mkdir -p %{buildroot}/usr/apps/com.oic.cs/bin/
mkdir -p %{buildroot}/usr/apps/com.oic.cs/bin/internal

cp -rf %{ROOTDIR}/com.oic.cs.xml %{buildroot}/%{_datadir}/packages
cp -rf %{ROOTDIR}/ConformanceSimulator %{buildroot}/usr/apps/com.oic.cs/bin/

%clean
#make clean

%files
%manifest com.oic.cs.manifest
%defattr(-,root,root,-)
/usr/apps/com.oic.cs/bin/ConformanceSimulator

#/usr/apps/com.oic.ca.sim/bin/sampletc
/%{_datadir}/packages/com.oic.cs.xml

#%defattr(644, root, root)
#%attr(755, root, root) /usr/bin

