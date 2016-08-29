%define PREFIX /usr/apps/com.oic.ca.sim
%define ROOTDIR  %{_builddir}/%{name}-%{version}

Name: com-oic-ca-sim
Version: 0.0.1
Release: 1
License: To be filled
Summary: Connectivity Abstruction Test
Packager: Razib Chandra Deb <razib.deb@samsung.com>
Group: Applications/CATest
Source: %{name}-%{version}.tar.gz

BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: boost-devel
BuildRequires: boost-thread
BuildRequires: boost-system
BuildRequires: boost-filesystem
BuildRequires: pkgconfig(capi-network-wifi)
BuildRequires: pkgconfig(capi-network-bluetooth)
BuildRequires: scons
BuildRequires: com-oic-ri
BuildRequires: expat-devel

%description
Platform Project

%prep
%setup -q

%build
scons TARGET_TRANSPORT=%{TARGET_TRANSPORT} SECURED=%{SECURED}

%install
mkdir -p %{buildroot}%{_datadir}/packages
mkdir -p %{buildroot}/%{_sysconfdir}/smack/accesses2.d
mkdir -p %{buildroot}/usr/apps/com.oic.ca.sim/bin/
mkdir -p %{buildroot}/usr/apps/com.oic.ca.sim/bin/internal

cp -rf %{ROOTDIR}/com.oic.ca.sim.xml %{buildroot}/%{_datadir}/packages
cp -rf %{ROOTDIR}/iotivity_ca_simulator %{buildroot}/usr/apps/com.oic.ca.sim/bin/
cp -rf %{ROOTDIR}/iotivity_ca_*test %{buildroot}/usr/apps/com.oic.ca.sim/bin/

%clean
#make clean

%files
%manifest com.oic.ca.sim.manifest
%defattr(-,root,root,-)
/usr/apps/com.oic.ca.sim/bin/iotivity_ca_simulator
/usr/apps/com.oic.ca.sim/bin/iotivity_ca_*test

/%{_datadir}/packages/com.oic.ca.sim.xml

