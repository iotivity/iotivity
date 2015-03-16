Name:       ssm
Summary:    Open Interconnect Consortium
Version: 	0.0.1
Release:    0
Group:      Network & Connectivity/Service
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1:    libDiscomfortIndexSensor.so
Source2:    SoftSensorDescription.xml
Source1001: 	%{name}.manifest
BuildRequires: cmake
#BuildRequires: boost-devel
#BuildRequires: pkgconfig(glib-2.0)
#Buildrequires: gettext-tools
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
rm -rf %{buildroot}/BUILD/ssm*
#%make_install
cp -af %{SOURCE1} %{buildroot}/usr/apps/com.samsung.ssmtester/bin/
cp -af %{SOURCE2} %{buildroot}/usr/apps/com.samsung.ssmtester/bin/

%post


%postun


%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
/usr/apps/com.samsung.ssmtester/*
/usr/share/*
/etc/smack/accesses2.d/com.samsung.ssmtester.rule
#%%license LICENSE.APLv2

