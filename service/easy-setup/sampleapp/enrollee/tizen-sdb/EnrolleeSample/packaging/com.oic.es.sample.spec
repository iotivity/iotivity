%define PREFIX /usr/apps/com.oic.es.sample
%define ROOTDIR  %{_builddir}/%{name}-%{version}
%{!?VERBOSE: %define VERBOSE 1}

Name: com-oic-es-sample
Version:    1.1.1
Release:    0
Summary: Tizen adapter interfacesample application
URL: http://slp-source.sec.samsung.net
Source: %{name}-%{version}.tar.gz
License: Apache-2.0
Group: Applications/OICSample
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: boost-devel
BuildRequires: boost-thread
BuildRequires: boost-system
BuildRequires: boost-filesystem
BuildRequires: pkgconfig(capi-network-wifi)
BuildRequires: pkgconfig(capi-network-bluetooth)
BuildRequires: scons
BuildRequires: iotivity
BuildRequires: iotivity-devel
BuildRequires: iotivity-service


%description
EasySetup Sample application

%prep
%setup -q

%build

scons VERBOSE=%{VERBOSE} TARGET_OS=tizen LOGGING=True TARGET_TRANSPORT=%{TARGET_TRANSPORT} SECURED=%{SECURED} RELEASE=%{RELEASE} ROUTING=%{ROUTING} ES_ROLE=%{ES_ROLE} ES_TARGET_ENROLLEE=%{ES_TARGET_ENROLLEE} ES_SOFTAP_MODE=%{ES_SOFTAP_MODE}

%install

mkdir -p %{buildroot}%{_datadir}/packages
mkdir -p %{buildroot}/%{_sysconfdir}/smack/accesses2.d
mkdir -p %{buildroot}/usr/apps/com.oic.es.sample/bin/
mkdir -p %{buildroot}/usr/apps/com.oic.es.sample/bin/internal

cp -rf %{ROOTDIR}/com.oic.es.sample.xml %{buildroot}/%{_datadir}/packages
cp -rf %{ROOTDIR}/scons/enrollee_wifi %{buildroot}/usr/apps/com.oic.es.sample/bin/

%files
%manifest com.oic.es.sample.manifest
%defattr(-,root,root,-)
/usr/apps/com.oic.es.sample/bin/enrollee_wifi
/%{_datadir}/packages/com.oic.es.sample.xml


