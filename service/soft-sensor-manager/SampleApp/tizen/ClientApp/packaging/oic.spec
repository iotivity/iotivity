Name:       oic
Summary:    Open Interconnect Consortium
Version: 	0.0.1
Release:    0
Group:      Network & Connectivity/Service
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1:    %{name}.service
Source1001: 	%{name}.manifest
BuildRequires: cmake
BuildRequires: boost-devel
#BuildRequires: pkgconfig(glib-2.0)
Buildrequires: gettext-tools
BuildRequires: pkgconfig(appcore-efl)
BuildRequires: pkgconfig(ecore-x)
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(efl-assist)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(capi-network-wifi)

%description
Tizen Open Interconnect Consortium Service.


%prep
%setup -q
cp %{SOURCE1001} .


%build
%cmake . -DBIN_INSTALL_DIR:PATH=%{_bindir}


%install
rm -rf %{buildroot}/BUILD/oic*
#%make_install

%post


%postun


%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_bindir}/simple*
/usr/apps/com.samsung.oicapp-test/*
/usr/share/*
/etc/smack/accesses2.d/com.samsung.oicapp-test.rule
#%%license LICENSE.APLv2
