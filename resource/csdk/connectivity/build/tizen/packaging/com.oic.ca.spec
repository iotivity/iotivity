%define PREFIX /usr/apps/com.oic.ca
%define ROOTDIR  %{_builddir}/%{name}-%{version}
%define USR_INC_DIR  /usr/include
%define DEST_INC_DIR  %{buildroot}%{USR_INC_DIR}/interfaceHeaders
%define DEST_LIB_DIR  %{buildroot}/usr/lib

Name: com.oic.ca
Version:    0.1
Release:    1
Summary: Tizen oicca application
URL: http://slp-source.sec.samsung.net
Source: %{name}-%{version}.tar.gz
License: Apache License, Version 2.0
Group: OIC/Application
#BuildRequires: cmake
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(capi-network-wifi)
BuildRequires: pkgconfig(capi-network-bluetooth)
BuildRequires: boost-devel
BuildRequires: boost-thread
BuildRequires: boost-system
BuildRequires: boost-filesystem
BuildRequires: scons
#BuildRequires: oic-core

%description
SLP oicca application

%prep

%setup -q

%build

echo %{ROOTDIR}

scons TARGET_OS=tizen -c
scons TARGET_OS=tizen TARGET_TRANSPORT=%{TARGET_TRANSPORT}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr/share/license
mkdir -p %{buildroot}/usr/share/packages
mkdir -p %{buildroot}/etc/smack/accesses2.d
mkdir -p %{buildroot}/usr/apps/com.oic.ca/bin/
mkdir -p %{buildroot}/usr/lib
mkdir -p %{DEST_INC_DIR}
mkdir -p %{DEST_LIB_DIR}/pkgconfig

cp -rf %{ROOTDIR}/con/libconnectivity_abstraction.a %{buildroot}/usr/lib
cp -rf %{ROOTDIR}/con/lib/libcoap-4.1.1/libcoap.a %{buildroot}/usr/lib
cp -rf %{ROOTDIR}/con/api/cacommon.h* %{DEST_INC_DIR}/
cp -rf %{ROOTDIR}/con/inc/caadapterinterface.h* %{DEST_INC_DIR}/
cp -rf %{ROOTDIR}/con/common/inc/uthreadpool.h* %{DEST_INC_DIR}/
cp -rf %{ROOTDIR}/con/inc/cawifiadapter.h* %{DEST_INC_DIR}/
cp -rf %{ROOTDIR}/con/inc/caethernetadapter.h* %{DEST_INC_DIR}/
cp -rf %{ROOTDIR}/con/inc/caedradapter.h* %{DEST_INC_DIR}/
cp -rf %{ROOTDIR}/con/inc/caleadapter.h* %{DEST_INC_DIR}/
cp -rf %{ROOTDIR}/con/api/cainterface.h* %{DEST_INC_DIR}/
cp -rf %{ROOTDIR}/com.oic.ca.pc %{DEST_LIB_DIR}/pkgconfig/


%files
%defattr(-,root,root,-)
%attr(-,inhouse,inhouse)
%{_libdir}/lib*.a*
%{_includedir}/interfaceHeaders/*
%{_libdir}/pkgconfig/*.pc
