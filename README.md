# Vkontakte digiKam Plugin

This is the legacy code to export contents to VKontakte Webservice.

# Dependencies

## Common components

- Cmake 3.x.                        <https://cmake.org/>
- Qt6                               <https://www.qt.io/>

## From KDE framework

- ECM                               <https://invent.kde.org/frameworks/extra-cmake-modules>
- KF5::KIO                          <https://invent.kde.org/frameworks/kio>

## From digiKam

- digiKam >= 7.2 plugin interface   <https://www.digikam.org>

# Compiling and Installing

- mdkir build
- cd build
- cmake . -DCMAKE_INSTALL_PREFIX=/usr
- make
- make install/fast
