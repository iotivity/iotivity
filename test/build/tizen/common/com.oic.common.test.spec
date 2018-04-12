%define ROOTDIR  %{_builddir}/%{name}-%{version}
%{!?VERBOSE: %define VERBOSE 1}

Name: com-oic-common-test
Version: w.x.y
Release: z
Summary: Iotivity Test
URL: http://slp-source.sec.samsung.net
Source: %{name}-%{version}.tar.gz
License: Apache-2.0
Group: Applications/OICSample

BuildRequires: boost-devel
BuildRequires: boost-thread
BuildRequires: boost-system
BuildRequires: boost-filesystem
BuildRequires: libcurl-devel
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(uuid)
BuildRequires: pkgconfig(appcore-efl)
BuildRequires: pkgconfig(ecore-x)
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(capi-network-wifi)
BuildRequires: pkgconfig(capi-network-bluetooth)
BuildRequires: scons

#%if "%{MODULE}" == "ca"
#BuildRequires: com-oic-ri
#%endif

%if "%{MODULE}" == "needed-common-libs"
BuildRequires: iotivity
BuildRequires: iotivity-devel
BuildRequires: iotivity-service
%endif

BuildRequires: expat-devel

%description
Platform Project

%prep
%setup -q

%build
scons TEST=%{TEST} SDK=%{SDK} TARGET_TRANSPORT=%{TARGET_TRANSPORT} MODULE=%{MODULE} TLS_MODE=%{TLS_MODE} SECURED=%{SECURED}

%install
mkdir -p %{buildroot}%{_datadir}/packages
mkdir -p $RPM_BUILD_ROOT/usr/lib
mkdir -p %{buildroot}/%{_sysconfdir}/smack/accesses2.d
mkdir -p %{buildroot}/usr/apps/com.oic.%{MODULE}.test/bin/
mkdir -p %{buildroot}/usr/apps/com.oic.%{MODULE}.test/bin/internal

cp -rf %{ROOTDIR}/com.oic.%{MODULE}.test.xml %{buildroot}/%{_datadir}/packages
cp -rf %{ROOTDIR}/iotivity_%{MODULE}_* %{buildroot}/usr/apps/com.oic.%{MODULE}.test/bin/

%files
%manifest com.oic.%{MODULE}.test.manifest
%defattr(-,root,root,-)
/usr/apps/com.oic.%{MODULE}.test/bin/iotivity_%{MODULE}_*
/%{_datadir}/packages/com.oic.%{MODULE}.test.xml

