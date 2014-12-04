%define MODULE_NAME liboic
%define MODULE_LIBNAME liboic
Name:       core-%{MODULE_NAME}-tests
Summary:    Core API unit TC (%{name})
Version:    0.1
Release:    0
Group:      Development/Tools
License:    Apache License, Version 2.0, Samsung Properietary
Source0:    %{name}-%{version}.tar.gz
#BuildRequires:  pkgconfig(%{MODULE_LIBNAME})
BuildRequires: pkgconfig(capi-network-wifi)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(capi-network-bluetooth)
BuildRequires:  pkgconfig(dlog)
#BuildRequires:  pkgconfig(oic-core)
BuildRequires:  pkgconfig(com.samsung.oicca)
BuildRequires:  cmake

%description
Core API unit TC (%{name})

%prep
%setup -q

%build

%define PREFIX "%{_libdir}/%{name}"

export LDFLAGS+="-Wl,--rpath=%{PREFIX} -Wl,--as-needed"

cmake . -DMODULE="%{MODULE_NAME}" -DCMAKE_INSTALL_PREFIX=%{_prefix}

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install
mkdir -p %{buildroot}/opt/usr/share/license
cp LICENSE %{buildroot}/opt/usr/share/license/%{name}
mkdir -p %{buildroot}/tmp/
cp scripts/add_all_smack_rule.sh %{buildroot}/tmp/
cp scripts/all_smack.rule %{buildroot}/tmp/
#mkdir -p %{buildroot}/opt/usr/bin/tct-liboic-core
#cp src/liboic/libcapi-network-wifi.so.0 %{buildroot}/opt/usr/bin/tct-liboic-core
#cp src/liboic/libcapi-network-wifi.so.0.1.20 %{buildroot}/opt/usr/bin/tct-liboic-core
#cp src/liboic/libcapi-network-wifi.so.1 %{buildroot}/opt/usr/bin/tct-liboic-core
%post

%postun


%files
/opt/usr/bin/*
#/usr/$(BIN_DIR)/lib*.so*
/opt/usr/share/license/%{name}
/tmp/add_all_smack_rule.sh
/tmp/all_smack.rule
