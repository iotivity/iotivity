%define PREFIX /usr/apps/com.oic.ca.sample
%define ROOTDIR  %{_builddir}/%{name}-%{version}

Name: com.oic.ca.sample
Version:    0.1
Release:    1
Summary: Tizen adapter interfacesample application
URL: http://slp-source.sec.samsung.net
Source: %{name}-%{version}.tar.gz
License: Apache License, Version 2.0
Group: OIC/Application
#BuildRequires: cmake
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: boost-devel
BuildRequires: boost-thread
BuildRequires: boost-system
BuildRequires: boost-filesystem
#BuildRequires: oic-core
BuildRequires: pkgconfig(capi-network-wifi)
BuildRequires: pkgconfig(capi-network-bluetooth)
BuildRequires: scons
BuildRequires: com.oic.ca
#BuildRequires: com.samsung.oicca-devel

%description
SLP interfacesample application

%prep
%setup -q

%build

scons TARGET_OS=tizen -c
scons TARGET_OS=tizen TARGET_TRANSPORT=%{TARGET_TRANSPORT}

#make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
# remove later
#%make_install
mkdir -p %{buildroot}/usr/share/license
mkdir -p %{buildroot}/usr/share/license
mkdir -p %{buildroot}/usr/share/packages
mkdir -p %{buildroot}/etc/smack/accesses2.d
mkdir -p %{buildroot}/usr/apps/com.oic.ca.sample/bin/
mkdir -p %{buildroot}/usr/apps/com.oic.ca.sample/bin/internal
mkdir -p %{buildroot}/usr/lib

cp -rf %{ROOTDIR}/com.oic.ca.sample.xml %{buildroot}/usr/share/packages
cp -rf %{ROOTDIR}/com.oic.ca.sample.rule %{buildroot}/etc/smack/accesses2.d
cp -rf %{ROOTDIR}/scons/interface_sample %{buildroot}/usr/apps/com.oic.ca.sample/bin/internal
cp -rf %{ROOTDIR}/scons/ca_sample %{buildroot}/usr/apps/com.oic.ca.sample/bin/

%files
%manifest com.oic.ca.sample.manifest
/etc/smack/accesses2.d/com.oic.ca.sample.rule
%defattr(-,root,root,-)
%attr(-,inhouse,inhouse)
/usr/apps/com.oic.ca.sample/bin/internal/interface_sample
/usr/apps/com.oic.ca.sample/bin/ca_sample
#%{_libdir}/lib*.a*
/usr/share/packages/com.oic.ca.sample.xml
#/opt/share/process-info/cmsample.ini
#/usr/share/license/%{name}
