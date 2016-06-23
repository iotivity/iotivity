Name: org.tizen.gtest
Version: 0.0.1
Release: 1
License: To be filled
Summary: gtest
Packager: Jisan Mahmud <jisan.m>
Group: Application
Source: %{name}-%{version}.tar.gz
%description
Platform Project

%prep
%setup -q

%build
make gtest_tizen

%install
make install DESTDIR=$RPM_BUILD_ROOT/usr

%clean
make clean

%files
%defattr(644, root, root)
%attr(755, root, root) /usr/bin

%changelog
* Sat Mar 24 2012 Author <E-mail>
 - initial release
