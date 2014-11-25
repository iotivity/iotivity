%define PREFIX /usr/apps/com.samsung.interfacesample
%define ROOTDIR  %{_builddir}/%{name}-%{version}

Name: com.samsung.interfacesample
Version:    0.1
Release:    1
Summary: Tizen interfacesample application
URL: http://slp-source.sec.samsung.net
Source: %{name}-%{version}.tar.gz
License: SAMSUNG
Group: Samsung/Application
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
BuildRequires: com.samsung.oicca
#BuildRequires: com.samsung.oicca-devel

%description
SLP interfacesample application

%prep
%setup -q

%build

make clean
make PLATFORM=linux

#make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
# remove later
#%make_install
mkdir -p %{buildroot}/usr/share/license
mkdir -p %{buildroot}/usr/share/license
mkdir -p %{buildroot}/usr/share/packages
mkdir -p %{buildroot}/etc/smack/accesses2.d
mkdir -p %{buildroot}/usr/apps/com.samsung.interfacesample/bin/
mkdir -p %{buildroot}/usr/lib
#cp -rf LICENSE %{buildroot}/usr/share/license/%{name}
cp -rf %{ROOTDIR}/com.samsung.interfacesample.xml %{buildroot}/usr/share/packages
cp -rf %{ROOTDIR}/com.samsung.interfacesample.rule %{buildroot}/etc/smack/accesses2.d
cp -rf %{ROOTDIR}/interface_sample %{buildroot}/usr/apps/com.samsung.interfacesample/bin/
cp -rf %{ROOTDIR}/ca_sample %{buildroot}/usr/apps/com.samsung.interfacesample/bin/
#cp -rf %{ROOTDIR}/liboicinterface.a %{buildroot}/usr/lib

%files
%manifest com.samsung.interfacesample.manifest
/etc/smack/accesses2.d/com.samsung.interfacesample.rule
%defattr(-,root,root,-)
%attr(-,inhouse,inhouse)
/usr/apps/com.samsung.interfacesample/bin/interface_sample
/usr/apps/com.samsung.interfacesample/bin/ca_sample
#%{_libdir}/lib*.a*
/usr/share/packages/com.samsung.interfacesample.xml
#/opt/share/process-info/cmsample.ini
#/usr/share/license/%{name}
