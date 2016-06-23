Name: IoTivity_TC
Version: 0.0.1
Release: 1
License: To be filled
Summary: IoTivity UTC ITC build for Tizen
Packager: Jisan Mahmud <jisan.m@samsung.com>
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
BuildRequires: pkgconfig(efl-assist)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(uuid)
BuildRequires: pkgconfig(capi-network-wifi)
BuildRequires: pkgconfig(capi-network-bluetooth)
BuildRequires: pkgconfig(capi-appfw-app-common)
BuildRequires: scons

%description
Platform Project

%prep
%setup -q

%build
scons

%install
#---------------------PPM------------------------------
mkdir -p $RPM_BUILD_ROOT/usr/bin/ppm
mkdir -p $RPM_BUILD_ROOT/usr/lib
mkdir -p $RPM_BUILD_ROOT/opt/usr/apps/org.iotivity.service.ppm.ppmsampleapp/lib/
cp iotivity_ppm_itc $RPM_BUILD_ROOT/usr/bin/ppm/
cp iotivity_ppm_utc $RPM_BUILD_ROOT/usr/bin/ppm/
cp Normal_SamplePlugin.so $RPM_BUILD_ROOT/usr/bin/ppm/
cp pluginmanager.xml $RPM_BUILD_ROOT/usr/bin/ppm/
cp pluginmanager.xml $RPM_BUILD_ROOT/opt/usr/apps/org.iotivity.service.ppm.ppmsampleapp/lib/
cp IotivityOrgSource/target/iotivity/out/tizen/armv7l/release/libpmimpl.so $RPM_BUILD_ROOT/usr/bin/ppm/
cp IotivityOrgSource/target/iotivity/out/tizen/armv7l/release/*.so $RPM_BUILD_ROOT/usr/lib/

#---------------------SSM------------------------------
mkdir -p $RPM_BUILD_ROOT/usr/bin/ssm
cp iotivity_ssm_itc $RPM_BUILD_ROOT/usr/bin/ssm/
cp iotivity_ssm_utc $RPM_BUILD_ROOT/usr/bin/ssm/
cp libSSMTestPlugin.so $RPM_BUILD_ROOT/usr/bin/ssm/
cp SSMTestSensor $RPM_BUILD_ROOT/usr/bin/ssm/
cp SoftSensorDescription.xml $RPM_BUILD_ROOT/usr/bin/ssm/

#---------------------NM------------------------------
mkdir -p $RPM_BUILD_ROOT/usr/bin/nm
cp nm_manager $RPM_BUILD_ROOT/usr/bin/nm/
cp nm_consumer $RPM_BUILD_ROOT/usr/bin/nm/
cp nm_provider $RPM_BUILD_ROOT/usr/bin/nm/
cp nm_provider2 $RPM_BUILD_ROOT/usr/bin/nm/
cp nm_non_hostable_provider $RPM_BUILD_ROOT/usr/bin/nm/
cp iotivity_nm_itc $RPM_BUILD_ROOT/usr/bin/nm/
cp iotivity_nm_utc $RPM_BUILD_ROOT/usr/bin/nm/

#---------------------TM------------------------------
mkdir -p $RPM_BUILD_ROOT/usr/bin/tm
cp iotivity_tm_itc $RPM_BUILD_ROOT/usr/bin/tm
cp iotivity_tm_utc $RPM_BUILD_ROOT/usr/bin/tm
cp testserver $RPM_BUILD_ROOT/usr/bin/tm
cp unsupportivetestserver $RPM_BUILD_ROOT/usr/bin/tm
cp bootstrapserver $RPM_BUILD_ROOT/usr/bin/tm
cp testcollection $RPM_BUILD_ROOT/usr/bin/tm

%clean
scons -c

%files
%defattr(644, root, root)
%attr(755, root, root) 
/usr/bin
/usr/lib
/opt/usr/apps/org.iotivity.service.ppm.ppmsampleapp/lib

%changelog
* Tue Jun 23 2015 Author jisan.m@samsung.com
 - initial release
