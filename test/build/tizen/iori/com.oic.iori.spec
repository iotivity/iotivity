%define PREFIX /usr/apps/com.oic.iori
%define ROOTDIR  %{_builddir}/%{name}-%{version}

Name: com-oic-iori
Version: 0.0.1
Release: 1
License: To be filled
Summary: InterOpAppRI
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
scons

%install
#make install DESTDIR=$RPM_BUILD_ROOT/usr
mkdir -p %{buildroot}%{_datadir}/packages
mkdir -p %{buildroot}/%{_sysconfdir}/smack/accesses2.d
mkdir -p %{buildroot}/usr/apps/com.oic.iori/bin/
mkdir -p %{buildroot}/usr/apps/com.oic.iori/bin/internal

cp -rf %{ROOTDIR}/com.oic.iori.xml %{buildroot}/%{_datadir}/packages
cp -rf %{ROOTDIR}/InterOpAppRI %{buildroot}/usr/apps/com.oic.iori/bin/

%clean
#make clean

%files
%manifest com.oic.iori.manifest
%defattr(-,root,root,-)
/usr/apps/com.oic.iori/bin/InterOpAppRI

/%{_datadir}/packages/com.oic.iori.xml

#%defattr(644, root, root)
#%attr(755, root, root) /usr/bin

