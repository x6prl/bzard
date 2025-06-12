Name:       iq-notifier
Version:    0.4.0
Release:    1%{?dist}
Summary:    Fancy and flexible notification daemon

License:    GPLv3
URL:        https://github.com/RussianBruteForce/iq-notifier
Source0:    iq-notifier-%{version}.tar.gz
Source1:    iq-notifier.desktop

BuildRequires:  cmake
BuildRequires:  libqtxdg-devel
BuildRequires:  qt5-qtbase-devel
BuildRequires:  desktop-file-utils
BuildRequires:  qt5-qtdeclarative-devel

%description
IQ Notifier is fancy and flexible notification daemon.


%prep
%setup -q


%build
%cmake
make %{?_smp_mflags}


%install
%make_install
install -m 644 -D %{SOURCE1} %{buildroot}/%{_datadir}/applications/iq-notifier.desktop


%files
%doc COPYING README.md
%{_bindir}/iq-notifier
%{_datadir}/iq-notifier
%{_datadir}/applications/iq-notifier.desktop


%changelog
* Mon Jul 24 2017 Yaroslav Sidlovsky <zawertun@gmail.com> - 0.4.0-1
- first spec version

