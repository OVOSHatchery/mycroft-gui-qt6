# Installation Guide: mycroft-gui-qt6

**Binary packages and installation methods for all major Linux distributions**

---

## Quick Install (Pre-built Binaries)

### Ubuntu/Debian (PPA - Coming Soon)

```bash
# Add OpenVoiceOS PPA
sudo add-apt-repository ppa:openvoceos/ppa
sudo apt update

# Install from PPA
sudo apt install mycroft-gui-qt6
```

### Fedora/RHEL (Copr - Coming Soon)

```bash
# Enable OpenVoiceOS Copr repository
sudo dnf copr enable openvoceos/openvoceos

# Install
sudo dnf install mycroft-gui-qt6
```

### Arch Linux (AUR - Coming Soon)

```bash
# Using yay (AUR helper)
yay -S mycroft-gui-qt6

# Or using git + makepkg
git clone https://aur.archlinux.org/mycroft-gui-qt6.git
cd mycroft-gui-qt6
makepkg -si
```

---

## Build and Install from Source

### All Distributions: Build from Source

Follow the complete instructions in [BUILD.md](BUILD.md), then install:

```bash
# After successful build
cd build
sudo make install

# Verify installation
pkg-config --modversion mycroft-gui-qt6
```

---

## Distribution-Specific Installation

### Ubuntu 22.04 LTS / Debian 12

#### Prerequisites

```bash
sudo apt update
sudo apt install -y \
    build-essential cmake git pkg-config \
    qt6-base-dev qt6-declarative-dev qt6-multimedia-dev \
    libqt6websockets6-dev \
    extra-cmake-modules libkf6kirigami-dev libkf6coreaddons-dev
```

#### Build and Install

```bash
# Clone and build
git clone https://github.com/OpenVoiceOS/mycroft-gui-qt6.git
cd mycroft-gui-qt6
mkdir build && cd build

cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install to system
sudo make install

# Update library cache
sudo ldconfig

# Verify
pkg-config --modversion mycroft-gui-qt6
```

#### Create Local Debian Package (Optional)

For easier distribution:

```bash
cd mycroft-gui-qt6
mkdir -p debian/mycroft-gui-qt6/DEBIAN

cat > debian/mycroft-gui-qt6/DEBIAN/control << 'EOF'
Package: mycroft-gui-qt6
Version: 1.0.0
Architecture: amd64
Maintainer: OpenVoiceOS Team
Depends: libqt6core6, libqt6gui6, libqt6qml6, libqt6websockets6, libkf6kirigami6
Description: Qt6 GUI client for OpenVoiceOS
 A modern Qt6-based graphical user interface for the OpenVoiceOS voice assistant.
EOF

# Build Debian package
dpkg-deb --build debian/mycroft-gui-qt6 mycroft-gui-qt6_1.0.0_amd64.deb

# Install
sudo dpkg -i mycroft-gui-qt6_1.0.0_amd64.deb
```

---

### Fedora 38+ / RHEL 9 / CentOS Stream

#### Prerequisites

```bash
sudo dnf groupinstall -y "Development Tools"
sudo dnf install -y \
    cmake git pkg-config \
    qt6-qtbase-devel qt6-qtdeclarative-devel qt6-qtmultimedia-devel \
    qt6-qtwebsockets-devel \
    extra-cmake-modules kf6-kirigami-devel kf6-kcoreaddons-devel
```

#### Build and Install

```bash
# Clone and build
git clone https://github.com/OpenVoiceOS/mycroft-gui-qt6.git
cd mycroft-gui-qt6
mkdir build && cd build

cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install to system
sudo make install

# Update library cache
sudo ldconfig

# Verify
pkg-config --modversion mycroft-gui-qt6
```

#### Create RPM Package (Optional)

```bash
# Create RPM spec file
cat > mycroft-gui-qt6.spec << 'EOF'
Name:           mycroft-gui-qt6
Version:        1.0.0
Release:        1%{?dist}
Summary:        Qt6 GUI client for OpenVoiceOS

License:        Apache-2.0
URL:            https://github.com/OpenVoiceOS/mycroft-gui-qt6
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  cmake >= 3.24
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qtdeclarative-devel
BuildRequires:  qt6-qtwebsockets-devel
BuildRequires:  extra-cmake-modules
BuildRequires:  kf6-kirigami-devel
BuildRequires:  kf6-kcoreaddons-devel

Requires:       libqt6core6
Requires:       libqt6gui6
Requires:       libqt6qml6
Requires:       libqt6websockets6
Requires:       libkf6kirigami6

%description
A modern Qt6-based graphical user interface for the OpenVoiceOS voice assistant.

%prep
%setup -q

%build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

%install
cd build
make install DESTDIR=%{buildroot}

%files
%{_libdir}/libmycroft-gui-qt6.so*
%{_libdir}/qt6/qml/Mycroft/
%{_includedir}/mycroft-gui-qt6/
%{_datadir}/doc/mycroft-gui-qt6/

%changelog
* $(date +%a\ %b\ %d\ %Y) OpenVoiceOS <info@openvoceos.com> - 1.0.0-1
- Initial release
EOF

# Build RPM
rpmbuild -ba mycroft-gui-qt6.spec

# Install from built RPM
sudo dnf install ~/rpmbuild/RPMS/x86_64/mycroft-gui-qt6-1.0.0-1.fc38.x86_64.rpm
```

---

### Arch Linux / Manjaro

#### Prerequisites

```bash
sudo pacman -S \
    base-devel cmake git pkg-config \
    qt6-base qt6-declarative qt6-multimedia qt6-websockets \
    extra-cmake-modules kf6-kirigami kf6-coreaddons
```

#### Build and Install

```bash
# Clone and build
git clone https://github.com/OpenVoiceOS/mycroft-gui-qt6.git
cd mycroft-gui-qt6
mkdir build && cd build

cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install to system
sudo make install

# Update library cache
sudo ldconfig

# Verify
pkg-config --modversion mycroft-gui-qt6
```

#### Create AUR Package (PKGBUILD)

```bash
# Create PKGBUILD file
cat > PKGBUILD << 'EOF'
pkgname=mycroft-gui-qt6
pkgver=1.0.0
pkgrel=1
pkgdesc="Qt6 GUI client for OpenVoiceOS"
arch=('x86_64' 'aarch64' 'armv7h')
url="https://github.com/OpenVoiceOS/mycroft-gui-qt6"
license=('Apache-2.0')
depends=('qt6-base' 'qt6-declarative' 'qt6-multimedia' 'qt6-websockets' 'kf6-kirigami' 'kf6-coreaddons')
makedepends=('cmake' 'extra-cmake-modules')
source=("${pkgname}-${pkgver}.tar.gz::https://github.com/OpenVoiceOS/mycroft-gui-qt6/archive/v${pkgver}.tar.gz")
sha256sums=('SKIP')

build() {
    cd "${pkgname}-${pkgver}"
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
    make
}

package() {
    cd "${pkgname}-${pkgver}/build"
    make DESTDIR="${pkgdir}" install
}
EOF

# Build and install
makepkg -si
```

---

### Alpine Linux 3.15+

#### Prerequisites

```bash
sudo apk add \
    build-base cmake git pkgconfig \
    qt6-qtbase-dev qt6-qtdeclarative-dev qt6-qtmultimedia-dev \
    qt6-qtwebsockets-dev \
    extra-cmake-modules kf6-kirigami-dev kf6-coreaddons-dev
```

#### Build and Install

```bash
# Clone and build
git clone https://github.com/OpenVoiceOS/mycroft-gui-qt6.git
cd mycroft-gui-qt6
mkdir build && cd build

cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install to system
sudo make install

# Update library cache
sudo ldconfig

# Verify
pkg-config --modversion mycroft-gui-qt6
```

---

## Install to Custom Location

For development or containerized environments:

```bash
# Build with custom prefix
mkdir build && cd build
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$HOME/.local

make -j$(nproc)
make install

# Use the library
export LD_LIBRARY_PATH=$HOME/.local/lib:$LD_LIBRARY_PATH
export QML_IMPORT_PATH=$HOME/.local/lib/qt6/qml:$QML_IMPORT_PATH
export PKG_CONFIG_PATH=$HOME/.local/lib/pkgconfig:$PKG_CONFIG_PATH
```

---

## Verify Installation

### Check Library Installation

```bash
# Find installed library
find /usr -name "libmycroft-gui-qt6.so*" 2>/dev/null

# Expected output:
# /usr/local/lib/libmycroft-gui-qt6.so
# /usr/local/lib/libmycroft-gui-qt6.so.1
```

### Check QML Plugin

```bash
# List QML plugin files
ls -la /usr/local/lib/qt6/qml/Mycroft/

# Expected files:
# qmldir
# libmycroft.so
# *.qml (system templates and components)
```

### Test with pkg-config

```bash
# Check pkg-config can find it
pkg-config --modversion mycroft-gui-qt6
pkg-config --cflags --libs mycroft-gui-qt6

# Expected output:
# /usr/local/include/mycroft-gui-qt6
# -L/usr/local/lib -lmycroft-gui-qt6 -lQt6Core ...
```

### Runtime Tests

```bash
# Test QML can load module
qmlimportscanner -rootPath /usr/local/lib/qt6/qml | grep Mycroft

# Test library can be loaded
ldd /usr/local/lib/libmycroft-gui-qt6.so

# All Qt6 and KF6 libraries should be found
```

---

## Integration with OpenVoiceOS

### Install with ovos-core

After installing mycroft-gui-qt6, configure OpenVoiceOS to use it:

```bash
# Edit OpenVoiceOS configuration
nano ~/.config/mycroft/mycroft.conf

# Add GUI configuration:
{
    "gui_server": {
        "host": "localhost",
        "port": 18181,
        "ssl": false
    },
    "gui_plugins": [
        "mycroft.gui.qt6"
    ]
}
```

### Link to ovos-shell

If using ovos-shell, verify it can find the library:

```bash
# Check ovos-shell can find library
ldd /usr/bin/ovos-shell | grep mycroft-gui

# If not found, verify QML_IMPORT_PATH
echo $QML_IMPORT_PATH
# Should include: /usr/local/lib/qt6/qml
```

---

## Uninstall

### System-Wide Installation

```bash
# Using make
cd <build-directory>
sudo make uninstall

# Or manually
sudo rm /usr/local/lib/libmycroft-gui-qt6.so*
sudo rm -rf /usr/local/lib/qt6/qml/Mycroft/
sudo rm -rf /usr/local/include/mycroft-gui-qt6/
sudo rm -rf /usr/local/share/doc/mycroft-gui-qt6/
sudo ldconfig
```

### Debian/Ubuntu Package

```bash
# If installed via APT
sudo apt remove mycroft-gui-qt6
sudo apt autoremove

# Or from built package
sudo dpkg -r mycroft-gui-qt6
```

### Fedora/RHEL/CentOS

```bash
# If installed via DNF
sudo dnf remove mycroft-gui-qt6

# Or from built package
sudo dnf remove mycroft-gui-qt6-1.0.0-1.fc38.x86_64.rpm
```

### Arch Linux

```bash
# If installed via pacman/AUR
sudo pacman -Rs mycroft-gui-qt6
```

### Alpine Linux

```bash
# If installed via apk
sudo apk del mycroft-gui-qt6
```

---

## Troubleshooting Installation

### Library Not Found at Runtime

```bash
# Problem: "libmycroft-gui-qt6.so: cannot open shared object"

# Solution 1: Update library cache
sudo ldconfig

# Solution 2: Check library path
ldconfig -p | grep mycroft-gui

# Solution 3: Add to LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```

### QML Module Not Found

```bash
# Problem: "Cannot import module 'Mycroft'"

# Solution 1: Check QML path
ls -la /usr/local/lib/qt6/qml/Mycroft/

# Solution 2: Set QML_IMPORT_PATH
export QML_IMPORT_PATH=/usr/local/lib/qt6/qml:$QML_IMPORT_PATH

# Solution 3: Add to shell profile (~/.bashrc)
echo "export QML_IMPORT_PATH=/usr/local/lib/qt6/qml:\$QML_IMPORT_PATH" >> ~/.bashrc
```

### pkg-config Not Finding Package

```bash
# Problem: "Package mycroft-gui-qt6 not found"

# Solution 1: Verify .pc file exists
ls -la /usr/local/lib/pkgconfig/mycroft-gui-qt6.pc

# Solution 2: Update pkg-config path
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH

# Solution 3: Check pkg-config can find it
pkg-config --list-all | grep mycroft-gui
```

### Dependency Issues

```bash
# If missing Qt6 libraries
sudo apt install libqt6core6 libqt6gui6 libqt6qml6 libqt6websockets6  # Ubuntu

# If missing KF6 libraries
sudo apt install libkf6kirigami6 libkf6coreaddons6  # Ubuntu
```

---

## Next Steps

- [Build Guide](BUILD.md) — Advanced build options, cross-compilation
- [Architecture](docs/ARCHITECTURE.md) — System design and component interactions
- [Code Guide](docs/CODE_GUIDE.md) — Understanding the codebase
- [Components Reference](docs/COMPONENTS.md) — API and template documentation

---

**Installation complete?** See [README.md](README.md) for usage instructions.
