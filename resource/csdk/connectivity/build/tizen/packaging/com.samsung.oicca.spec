%define PREFIX /usr/apps/com.samsung.oicca
%define ROOTDIR  %{_builddir}/%{name}-%{version}
%define USR_INC_DIR  /usr/include
%define DEST_INC_DIR  %{buildroot}%{USR_INC_DIR}/interfaceHeaders
%define DEST_LIB_DIR  %{buildroot}/usr/lib

Name: com.samsung.oicca
Version:    0.1
Release:    1
Summary: Tizen oicca application
URL: http://slp-source.sec.samsung.net
Source: %{name}-%{version}.tar.gz
License: SAMSUNG
Group: Samsung/Application
#BuildRequires: cmake
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(capi-network-wifi)
BuildRequires: pkgconfig(capi-network-bluetooth)
BuildRequires: boost-devel
BuildRequires: boost-thread
BuildRequires: boost-system
BuildRequires: boost-filesystem
#BuildRequires: oic-core

%description
SLP oicca application

%prep
%setup -q

%build

make clean
make

#make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr/share/license
mkdir -p %{buildroot}/usr/share/packages
mkdir -p %{buildroot}/etc/smack/accesses2.d
mkdir -p %{buildroot}/usr/apps/com.samsung.oicca/bin/
mkdir -p %{buildroot}/usr/lib
mkdir -p %{DEST_INC_DIR}
mkdir -p %{DEST_LIB_DIR}/pkgconfig

cp -rf %{ROOTDIR}/liboicinterface.a %{buildroot}/usr/lib
cp -rf %{ROOTDIR}/cacommon.h* %{DEST_INC_DIR}/
cp -rf %{ROOTDIR}/caadapterinterface.h* %{DEST_INC_DIR}/
cp -rf %{ROOTDIR}/uthreadpool.h* %{DEST_INC_DIR}/
cp -rf %{ROOTDIR}/cawifiadapter.h* %{DEST_INC_DIR}/
cp -rf %{ROOTDIR}/caethernetadapter.h* %{DEST_INC_DIR}/
cp -rf %{ROOTDIR}/caedradapter.h* %{DEST_INC_DIR}/
cp -rf %{ROOTDIR}/caleadapter.h* %{DEST_INC_DIR}/
cp -rf %{ROOTDIR}/cainterface.h* %{DEST_INC_DIR}/
cp -rf %{ROOTDIR}/com.samsung.oicca.pc %{DEST_LIB_DIR}/pkgconfig/


%files
%defattr(-,root,root,-)
%attr(-,inhouse,inhouse)
%{_libdir}/lib*.a*
%{_includedir}/interfaceHeaders/*
%{_libdir}/pkgconfig/*.pc
