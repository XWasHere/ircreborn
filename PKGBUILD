pkgname=ircreborn
pkgver=1.2.2
pkgrel=1
arch=('x86_64')
source=('git://github.com/xwashere/ircreborn.git')
license=('GPL')
cksums=('SKIP')
url="https://github.com/xwashere/ircreborn"
depends=(libx11 libxcb)
makedepends=(gcc texinfo)

build() {
    cd $srcdir/ircreborn
    make binit
    make
}

package() {
    cd $srcdir/ircreborn
    mkdir -p $pkgdir/usr/bin
    mkdir -p $pkgdir/usr/share/info
    make install PREFIX="$pkgdir/"
}