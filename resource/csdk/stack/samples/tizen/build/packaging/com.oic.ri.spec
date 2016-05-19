%define PREFIX /usr/apps/com.oic.ri
%define ROOTDIR  %{_builddir}/%{name}-%{version}
%define DEST_INC_DIR  %{buildroot}/%{_includedir}/OICHeaders
%define DEST_LIB_DIR  %{buildroot}/%{_libdir}

Name: com-oic-ri
Version:    0.1
Release:    1
Summary: Tizen oicri application
URL: http://slp-source.sec.samsung.net
Source: %{name}-%{version}.tar.gz
License: Apache-2.0
Group: Applications/OIC
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(capi-network-wifi)
BuildRequires: pkgconfig(capi-network-bluetooth)
BuildRequires: pkgconfig(uuid)
BuildRequires: pkgconfig(sqlite3)
BuildRequires: boost-devel
BuildRequires: boost-thread
BuildRequires: boost-system
BuildRequires: boost-filesystem
BuildRequires: scons


%description
SLP oicri application

%prep

%setup -q

%build

echo %{ROOTDIR}

scons TARGET_OS=tizen -c
scons TARGET_OS=tizen TARGET_TRANSPORT=%{TARGET_TRANSPORT} SECURED=%{SECURED} RELEASE=%{RELEASE} LOGGING=%{LOGGING} ROUTING=%{ROUTING}

%install
mkdir -p %{DEST_INC_DIR}
mkdir -p %{DEST_LIB_DIR}/pkgconfig

cp -f %{ROOTDIR}/resource/csdk/liboctbstack.so %{buildroot}/%{_libdir}
cp -f %{ROOTDIR}/resource/c_common/libc_common.a %{buildroot}/%{_libdir}
cp -f %{ROOTDIR}/resource/csdk/security/libocsrm.a %{buildroot}/%{_libdir}
cp -f %{ROOTDIR}/resource/csdk/connectivity/src/libconnectivity_abstraction.so %{buildroot}/%{_libdir}
cp -f %{ROOTDIR}/resource/csdk/connectivity/lib/libcoap-4.1.1/libcoap.a %{buildroot}/%{_libdir}
cp /usr/lib/libuuid.so.1 %{buildroot}%{_libdir}
if echo %{SECURED}|grep -qi '1'; then
	cp -f %{ROOTDIR}/extlibs/tinydtls/libtinydtls.a %{buildroot}/%{_libdir}
fi

cp -rf %{ROOTDIR}/resource/csdk/stack/include/ocstack.h* %{DEST_INC_DIR}/
cp -rf %{ROOTDIR}/resource/csdk/stack/include/ocstackconfig.h* %{DEST_INC_DIR}/
cp -rf %{ROOTDIR}/resource/csdk/stack/include/octypes.h* %{DEST_INC_DIR}/
cp -rf %{ROOTDIR}/resource/csdk/logger/include/logger.h* %{DEST_INC_DIR}/
cp -rf %{ROOTDIR}/resource/csdk/logger/include/logger_types.h* %{DEST_INC_DIR}/
cp resource/oc_logger/include/oc_logger.hpp %{DEST_INC_DIR}/
cp resource/oc_logger/include/oc_log_stream.hpp %{DEST_INC_DIR}/
cp resource/oc_logger/include/oc_logger.h %{DEST_INC_DIR}/
cp resource/oc_logger/include/oc_logger_types.h %{DEST_INC_DIR}/
cp resource/oc_logger/include/targets/oc_console_logger.h %{DEST_INC_DIR}
cp resource/oc_logger/include/targets/oc_ostream_logger.h %{DEST_INC_DIR}
cp resource/csdk/stack/include/ocpresence.h %{DEST_INC_DIR}
cp resource/csdk/stack/include/ocpayload.h %{DEST_INC_DIR}
cp resource/c_common/platform_features.h %{DEST_INC_DIR}
cp resource/csdk/stack/include/payload_logging.h %{DEST_INC_DIR}
cp resource/csdk/stack/include/rdpayload.h %{DEST_INC_DIR}
cp extlibs/tinycbor/tinycbor/src/cbor.h %{DEST_INC_DIR}
cp extlibs/cjson/cJSON.h %{DEST_INC_DIR}
cp -rf %{ROOTDIR}/com.oic.ri.pc %{DEST_LIB_DIR}/pkgconfig/

%files
%manifest com.oic.ri.manifest
%defattr(-,root,root,-)
%{_libdir}/lib*.so*
%{_libdir}/lib*.a*
%{_includedir}/OICHeaders/*
%{_libdir}/pkgconfig/*.pc

