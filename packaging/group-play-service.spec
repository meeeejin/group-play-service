## Basic Descriptions of this package
Name:       group-play-service
Summary:    Group Play Service
Version:		1.0
Release:    1
Group:      Framework/system
License:    Apache License, Version 2.0
Source0:    %{name}-%{version}.tar.gz
Source1:    group-play-service.service

# Required packages
# Pkgconfig tool helps to find libraries that have already been installed
BuildRequires:  cmake
BuildRequires:  libattr-devel
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(capi-system-info)
BuildRequires:  pkgconfig(capi-network-wifi)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(aul)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(heynoti)
BuildRequires:  pkgconfig(ecore-x)
BuildRequires:  pkgconfig(ecore-evas)
BuildRequires:  pkgconfig(capi-appfw-app-manager)
BuildRequires:  pkgconfig(capi-appfw-application)
BuildRequires:  pkgconfig(osp-appfw)

## Description string that this package's human users can understand
%description
Group Play Service


## Preprocess script
%prep
# setup: to unpack the original sources / -q: quiet
# patch: to apply patches to the original sources
%setup -q

## Build script
%build
# 'cmake' does all of setting the Makefile, then 'make' builds the binary.
cmake . -DCMAKE_INSTALL_PREFIX=/usr
make %{?jobs:-j%jobs}

## Install script
%install
# make_install: equivalent to... make install DESTDIR="%(?buildroot)"
%make_install

# install license file
mkdir -p %{buildroot}/usr/share/license
cp LICENSE %{buildroot}/usr/share/license/%{name}

# install systemd service
mkdir -p %{buildroot}%{_libdir}/systemd/system/graphical.target.wants
install -m 0644 %SOURCE1 %{buildroot}%{_libdir}/systemd/system/
ln -s ../group-play-service.service %{buildroot}%{_libdir}/systemd/system/graphical.target.wants/group-play-service.service

mkdir -p %{buildroot}/opt/etc/smack/accesses.d
install -m 644 group-play-service.rule %{buildroot}/opt/etc/smack/accesses.d

## Postprocess script
%post 

## Binary Package: File list
%files
%manifest group-play-service.manifest
%{_bindir}/group-play-svc
%{_libdir}/systemd/system/group-play-service.service
%{_libdir}/systemd/system/graphical.target.wants/group-play-service.service
/usr/share/license/%{name}
/opt/etc/smack/accesses.d/group-play-service.rule
