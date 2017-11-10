Name: googletest
Version: 1.7.0
Release: 0
Summary: Lib for unit testing
Source: %{name}-%{version}.tar.gz
License: BSD-3-Clause
BuildRequires: make

%description
Lib for unit testing

%prep

%setup -q -n %{name}-%{version}

%build

%reconfigure
%__make

echo "TODO: make_install to preserve static lib"
rpm --eval '%make_install'
install -d "%{buildroot}/%{_includedir}/" "%{buildroot}/%{_libdir}/"
cp -rfa include/gtest "%{buildroot}/%{_includedir}/"
cp -a lib/.libs/lib*.a "%{buildroot}/%{_libdir}/"
touch debugsources.list

%clean

%files
%license LICENSE
%{_includedir}/*
%{_libdir}/*
