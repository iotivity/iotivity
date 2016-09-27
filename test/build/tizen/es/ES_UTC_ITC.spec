Name: org.tizen.ES_UTC_ITC
Version: 0.0.1
Release: 1
License: To be filled
Summary: Easy Setup Enrolle Test Case
Packager: Razib Chandra Deb <razib.deb@samsung.com>
Group: Test Application
Source: %{name}-%{version}.tar.gz

BuildRequires: boost-devel
BuildRequires: boost-thread
BuildRequires: boost-system
BuildRequires: boost-filesystem
BuildRequires: pkgconfig(glib-2.0)
Buildrequires: gettext-tools
Buildrequires: expat-devel
BuildRequires: pkgconfig(appcore-efl)
BuildRequires: pkgconfig(ecore-x)
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(uuid)
BuildRequires: pkgconfig(capi-network-wifi)
BuildRequires: pkgconfig(capi-network-bluetooth)
BuildRequires: scons
BuildRequires: iotivity
BuildRequires: iotivity-service

%description
Platform Project

%prep
%setup -q

%build
scons

%install
mkdir -p $RPM_BUILD_ROOT/usr/bin/es
mkdir -p $RPM_BUILD_ROOT/usr/lib
cp iotivity_es_test $RPM_BUILD_ROOT/usr/bin/es



%clean
scons -c

%files
%defattr(644, root, root)
%attr(755, root, root) 
/usr/bin/es
/usr/lib

%changelog
* Sat Mar 24 2012 Author <E-mail>
 - initial release
