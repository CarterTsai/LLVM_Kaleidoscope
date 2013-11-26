LLVM-Kaleidoscope
-----------------


Reference : http://llvm.org/apt/

### Ubuntu
Precise (12.04) - Last update : Wed, 06 Nov 2013 19:38:07 UTC / Revision: 194151
deb http://llvm.org/apt/precise/ llvm-toolchain-precise main
deb-src http://llvm.org/apt/precise/ llvm-toolchain-precise main
Quantal (12.10) - Last update : Sun, 10 Nov 2013 21:52:41 UTC / Revision: 194355
deb http://llvm.org/apt/quantal/ llvm-toolchain-quantal main
deb-src http://llvm.org/apt/quantal/ llvm-toolchain-quantal main
Raring (13.04) - Last update : Sun, 10 Nov 2013 20:39:14 UTC / Revision: 194355
deb http://llvm.org/apt/raring/ llvm-toolchain-raring main
deb-src http://llvm.org/apt/raring/ llvm-toolchain-raring main
Saucy (13.10) - Last update : Mon, 25 Nov 2013 21:27:04 UTC / Revision: 195668
deb http://llvm.org/apt/saucy/ llvm-toolchain-saucy main
deb-src http://llvm.org/apt/saucy/ llvm-toolchain-saucy main

### Install
To retrieve the archive signature:
`wget -O - http://llvm.org/apt/llvm-snapshot.gpg.key|sudo apt-key add -`

To install just clang and lldb:
`apt-get install clang-3.5 lldb-3.5`

To install all packages:
`apt-get install clang-3.5 clang-3.5-doc libclang-common-3.5-dev libclang-3.5-dev libclang1-3.5 libclang1-3.5-dbg libllvm-3.5-ocaml-dev libllvm3.5 libllvm3.5-dbg lldb-3.5 llvm-3.5 llvm-3.5-dev llvm-3.5-doc llvm-3.5-examples llvm-3.5-runtime clang-modernize-3.5 clang-format-3.5 python-clang-3.5 lldb-3.5-dev`
